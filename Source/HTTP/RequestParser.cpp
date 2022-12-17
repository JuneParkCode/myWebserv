//
// Created by Sungjun Park on 2022/12/16.
//

#include "RequestParser.hpp"
#include "Server.hpp"
#include "Connection.hpp"
#include <sstream>
#include <iostream>

extern WS::Server* G_SERVER;

void HTTP::RequestParser::parseRequestHead(WS::Storage& buffer)
{
  // first, we have to check it has \r\n\r\n
  const size_t CURSOR_POS = buffer.getCursor();
  const size_t DELIM_POS = buffer.find("\r\n\r\n", CURSOR_POS);
  buffer.setCursor(DELIM_POS + 4); // + DELIM SIZE
  if (DELIM_POS == WS::Storage::NOT_FOUND)
    return ;
  char* bufferData = (char*)&(buffer.data()[CURSOR_POS]);
  std::istringstream stream(std::string(bufferData, DELIM_POS - CURSOR_POS));
  parseRequestLine(stream);
  parseHeader(stream);
}

void HTTP::RequestParser::parseRequestLine(std::istringstream& stream)
{
  std::string line, method, path, version;
  std::getline(stream, line);
  std::istringstream requestLine(line);

  requestLine >> method >> path >> version;
  if (line.empty() || method.empty() || path.empty() || version. empty())
    throw (std::logic_error("Request line parse failed\n"));
  if (version != "HTTP/1.1" || version == "HTTP/0.9") // check version
    throw (std::logic_error("Not supported version\n"));
  m_processingRequest->setRequestLine(method, path, version);
  m_parseStatus = REQ_PARSE_HEADER;
}

// 해당 지점에 올 때는 buffer에 \r\n\r\n 이 있는 것이 검증된 상태임.
void HTTP::RequestParser::parseHeader(std::istringstream& stream)
{
  std::string line;

  //[key]: [value]\r\n
  while (std::getline(stream, line) && line != "\r")
  {
    std::istringstream headerLine(line);
    std::string key, value;
    std::getline(headerLine, key, ':');
    std::getline(headerLine, value, '\r');
    value = value.substr(1); // it includes ' '
    if (key.empty() || value.empty())
      throw (std::logic_error("Header parse failed\n"));
    m_processingRequest->addHeader(key, value);
  }
  m_processingRequest->setContentLength();
  if (m_processingRequest->getContentLength() > 0)
  {
    m_processingRequest->getBody().reserve(m_processingRequest->getContentLength()); // reserve it...
  }
  m_parseStatus = REQ_PARSE_BODY;
}

void HTTP::RequestParser::parseChunkedBody(WS::Storage& buffer)
{
  auto& body = m_processingRequest->getBody();
        body.reserve(BUFFER_SIZE);
  auto start = buffer.getCursor();

  // get new chunk size
  if (m_chunkSize == 0)
  {
    // check delimiter exist
    auto delimPos = buffer.find("\r\n", start);
    if (delimPos == WS::Storage::NOT_FOUND) // wait for next receive...
      return ;
    // get chunk size string
    auto chunkSizeLength = delimPos - start;
    char chunkSizeStr[20] = {0};
    if (chunkSizeLength >= 20)
      throw (std::logic_error("invalid body chunk\n"));
    ::memcpy(chunkSizeStr, &buffer[start], chunkSizeLength);
    // check it is not hex string
    for (size_t i = 0; i < chunkSizeLength; ++i)
    {
      if (::strchr("0123456789abcdef", chunkSizeStr[i]) == nullptr)
        throw (std::logic_error("invalid body chunk\n"));
    }
    // size string to size (num)
    char *p; // dummy
    m_chunkSize = ::strtoll(chunkSizeStr, &p, 16);
    if (m_chunkSize == 0)
    {
      m_parseStatus = REQ_PARSE_END;
      return ;
    }
    buffer.setCursor(delimPos + 2);
  }
  // check storage remain size and process partial / all
  auto bufferRemainSize = buffer.size() - buffer.getCursor();
  if (m_chunkSize <= bufferRemainSize)
  {
    body.append(&(buffer.data())[buffer.getCursor()], m_chunkSize);
    buffer.setCursor(buffer.getCursor() + m_chunkSize + 2); // \r\n
    m_chunkSize = 0;
    bufferRemainSize = buffer.size() - buffer.getCursor();
    if (bufferRemainSize)
      parseChunkedBody(buffer);
  }
}
//4680440650
void HTTP::RequestParser::parseCommonBody(WS::Storage& buffer)
{
  const auto CURSOR_POS = buffer.getCursor();
  const size_t CONTENT_LENGTH = m_processingRequest->getContentLength();
  WS::Storage& body = m_processingRequest->getBody();
               body.reserve(CONTENT_LENGTH);

  if (CONTENT_LENGTH == 0)
    m_parseStatus = REQ_PARSE_END;
  else
  {
    auto data = &(buffer.data())[CURSOR_POS];
    auto dataSize = (CONTENT_LENGTH >= (buffer.size() - CURSOR_POS))
            ? (buffer.size() - CURSOR_POS)
            : (CONTENT_LENGTH - buffer.size());
    body.append(data, dataSize);
    buffer.setCursor(CURSOR_POS + dataSize);
  }
  if (CONTENT_LENGTH <= body.size())
    m_parseStatus = REQ_PARSE_END;
  buffer.pop(buffer.getCursor() + 1);
}

void HTTP::RequestParser::parseRequestBody(WS::Storage& buffer)
{
  if (m_processingRequest->isChunked())
    parseChunkedBody(buffer);
  else if (m_processingRequest->getContentLength() > 0)
    parseCommonBody(buffer);
}

// stream 을 사용할 경우, 이를 복사하는데 분명히 시간이 들어간다. body는 buffer를 이용하자.
WS::ARequest* HTTP::RequestParser::parse(struct kevent& event, WS::Storage& buffer)
{
  if (buffer.empty())
    return (nullptr);
  if (this->m_processingRequest == nullptr)
    this->m_processingRequest = new HTTP::Request();
  if (this->m_parseStatus == REQ_PARSE_START)
    parseRequestHead(buffer);
  if (this->m_parseStatus == REQ_PARSE_BODY)
  {
    parseRequestBody(buffer);
    if (m_parseStatus == REQ_PARSE_BODY) // recv again...
    {
      G_SERVER->attachEvent(event.ident, event.filter, EV_ENABLE, 0, event.udata);
    }
    else
      m_processingRequest->setContentLength();
  }
  if (this->m_parseStatus == REQ_PARSE_ERROR)
  {
    auto* ret = m_processingRequest;
    ret->setError();

    init();
    return (ret);
  }
  else if (this->m_parseStatus == REQ_PARSE_END)
  {
    auto* ret = m_processingRequest;

    init();
    buffer.pop(ret->getBody().size());
    std::cerr << buffer.size() << std::endl;
    return (ret);
  }
  return (nullptr);
}

void HTTP::RequestParser::init()
{
  m_parseStatus = REQ_PARSE_START;
  m_processingRequest = nullptr;
  m_chunkSize = 0;
}


HTTP::RequestParser::RequestParser(WS::Connection* connection):
  m_connection(connection)
{
  init();
}

HTTP::RequestParser::~RequestParser()
{
  if (m_processingRequest != nullptr)
    delete (m_processingRequest);
  init();
}
