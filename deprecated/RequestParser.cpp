//
// Created by Sungjun Park on 2022/12/15.
//

#include "RequestParser.hpp"
#include "Server.hpp"
#include <sys/event.h>
#include <sstream>

#define DELIMITER "\r\n\r\n"

extern WS::Server* G_SERVER;

static bool isChunked(HTTP::Request* req)
{
  auto& headers = req->getHeaders();
  auto transferEncoding = headers.find("Transfer-Encoding");

  return (transferEncoding != headers.end()
          && transferEncoding->second.find("chunked") != std::string::npos);
}

HTTP::Request* HTTP::RequestParser::parse(struct kevent& event, WS::Storage& storage)
{
  if (m_request == nullptr)
    m_request = new HTTP::Request();

  // FIXME: Refactor 필요함
  try
  {
    size_t headerDelimPos;
    if (m_parseStatus == REQ_PARSE_START)
    {
      if ((headerDelimPos = storage.find(DELIMITER)) == WS::Storage::NOT_FOUND)
        return (nullptr);
      parseStartLine(storage);
      parseHeader(storage);
      storage.setCursor(headerDelimPos + 4); // bodyPos
      m_parseStatus = REQ_PARSE_BODY;
    }
    if (m_parseStatus == REQ_PARSE_BODY)
    {
      const size_t CONTENT_SIZE = m_request->getContentLength();

      if (isChunked(m_request))
      {
        while (m_parseStatus == REQ_PARSE_BODY && !storage.empty())
          parseChunkedBody(storage);
        if (m_parseStatus == REQ_PARSE_BODY && storage.empty()) // get socket recv again..
          G_SERVER->attachEvent(event.ident, EVFILT_READ, EV_ADD | EV_ENABLE, 0, event.udata);
      }
      else if (CONTENT_SIZE == 0)
      {
        m_parseStatus = REQ_PARSE_END;
        storage.pop(storage.getCursor());
      }
      else // not chunked
      {
        parseBody(storage);
        if (m_parseStatus == REQ_PARSE_BODY && storage.empty()) // get socket recv again..
          G_SERVER->attachEvent(event.ident, EVFILT_READ, EV_ADD | EV_ENABLE, 0, event.udata);
      }
    }
  }
  catch (std::exception& e)
  {
    std::cerr << e.what();
    m_parseStatus = REQ_PARSE_ERROR;
    m_request->setParseError();
  }
  if (m_parseStatus == REQ_PARSE_ERROR)
    std::cerr << "PARSE ERR\n";
  if (m_parseStatus == REQ_PARSE_END)
  {
    HTTP::Request* ret = m_request;
    m_request->setContentLength(m_request->getBody().size());
    m_request->display(); // std::cerr
    m_request = nullptr;
    // explicitly..
    G_SERVER->attachEvent(event.ident, EVFILT_READ, EV_DISABLE, 0, event.udata);
    return (ret);
  }
  return (nullptr);
}

bool HTTP::RequestParser::checkVersion(const std::string& version)
{
  if (::strncmp(version.c_str(), "HTTP", 4) != 0)
    return (false);
  auto httpVersion = std::stod(version.substr(5));
  std::cerr << "httpVersion : " << httpVersion << std::endl;
  return (0.9 <= httpVersion && httpVersion <= 1.1);
}

void HTTP::RequestParser::parseStartLine(WS::Storage& storage)
{
  size_t delimPos = storage.find("\r\n", storage.getCursor());
  WS::Storage lineBuf(storage.subStorage(0, delimPos));
              lineBuf.append((unsigned char*)"\0", 1);
  std::string lineStr((char*)lineBuf.data());
  std::stringstream stream(lineStr);
  std::string method, url, version;

  stream >> method >> url >> version;
  if (method.empty() || url.empty() || version.empty() || !checkVersion(version))
    throw (std::logic_error("start line parsing failed\n"));
  m_request->setMethod(method);
  m_request->setUrl(url);
  m_request->setVersion(version);
  storage.setCursor(lineBuf.size()); // - '\0' + "\r\n"
}

void HTTP::RequestParser::parseHeader(WS::Storage& storage)
{
  const size_t CURSOR = storage.getCursor();
  const size_t DELIMITER_POS = storage.find(DELIMITER, CURSOR);
  WS::Storage headerBuf(storage.subStorage(CURSOR, DELIMITER_POS - CURSOR + 2)); // + /r/n
              headerBuf.append((unsigned char*)"\0", 1);
  std::string headerStr((char*)headerBuf.data());
  const char DELIM = ':';
  size_t start = 0;
  size_t end;

  while (start < headerStr.size() - 4)
  {
    std::string key, value;
    // get key
    end = headerStr.find(DELIM, start);
    if (end == std::string::npos)
      throw (std::logic_error("header parsing failed\n"));
    key = headerStr.substr(start, end - start);
    start = end + 1;
    // get value
    end = headerStr.find("\r\n", start);
    if (end == std::string::npos)
      throw (std::logic_error("header parsing failed\n"));
    value = headerStr.substr(start + 1, end - start - 1);
    start = end + 2;
    // add header
    m_request->addHeader(key, value);
  }
  // set content length
  auto content_length = m_request->getHeaders().find("Content-Length");
  if (content_length == m_request->getHeaders().end())
    m_request->setContentLength(0);
  else
    m_request->setContentLength(std::stoll(content_length->second));
}

void HTTP::RequestParser::parseBody(WS::Storage& storage)
{
  const size_t CONTENT_LENGTH = m_request->getContentLength();
  WS::Storage& body = m_request->getBody();
       body.reserve(CONTENT_LENGTH);

  if (CONTENT_LENGTH == 0)
    m_parseStatus = REQ_PARSE_END;
  else
  {
    auto data = &(storage.data())[storage.getCursor()];
    size_t dataSize = (CONTENT_LENGTH >= (storage.size() - storage.getCursor()))
                    ? (storage.size() - storage.getCursor())
                    : (CONTENT_LENGTH - storage.size());
    body.append(data, dataSize);
    storage.setCursor(storage.getCursor() + dataSize);
  }
  if (CONTENT_LENGTH <= body.size())
    m_parseStatus = REQ_PARSE_END;
  storage.pop(storage.getCursor() + 1);
}

void HTTP::RequestParser::parseChunkedBody(WS::Storage& storage)
{
  auto& body = m_request->getBody();
        body.reserve(BUFFER_SIZE);
  size_t start = storage.getCursor();

  if (m_chunkSize == 0)
  {
    size_t delimPos = storage.find("\r\n", start);
    if (delimPos - start >= 20)
      throw (std::logic_error("invalid body chunk\n"));
    // get chunk size string
    char chunkSizeStr[20] = {0};
    for (size_t idx = start; (idx < (start + sizeof(chunkSizeStr) - 1) && idx < delimPos); ++idx)
      chunkSizeStr[idx - start] = (char)storage[idx];
    // string to size
    char *p;
    size_t chunkSize = ::strtol(chunkSizeStr, &p, 16);
    m_chunkSize = chunkSize;
    if (chunkSize == 0)
    {
      m_parseStatus = REQ_PARSE_END;
      return ;
    }
    storage.setCursor(delimPos + 2);
  }
  const size_t STORAGE_REMAIN_SIZE = storage.size() - storage.getCursor();
  if (m_chunkSize > STORAGE_REMAIN_SIZE) // use all storage data
  {
    body.append(&(storage.data())[storage.getCursor()], STORAGE_REMAIN_SIZE);
    storage.clear();
    m_chunkSize -= STORAGE_REMAIN_SIZE;
  }
  else // partial read
  {
    body.append(&(storage.data())[storage.getCursor()], m_chunkSize);
    storage.setCursor(storage.getCursor() + m_chunkSize + 2); // \r\n
    m_chunkSize = 0;
  }
}

HTTP::RequestParser::RequestParser(): m_request(nullptr), m_parseStatus(REQ_PARSE_START), m_chunkSize(0)
{

}

HTTP::RequestParser::~RequestParser()
{
  delete (m_request);
}
