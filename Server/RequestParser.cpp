//
// Created by Sungjun Park on 2022/12/15.
//

#include "RequestParser.hpp"
#include <sstream>

#define DELIMITER "\r\n\r\n"

static bool isChunked(HTTP::Request* req)
{
  auto& headers = req->getHeaders();
  auto transferEncoding = headers.find("Transfer-encoding");

  return (transferEncoding != headers.end()
          && transferEncoding->second.find("chunked") != std::string::npos);
}

HTTP::Request* HTTP::RequestParser::parse(WS::Storage& storage, bool isForce)
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

      if (CONTENT_SIZE == 0)
      {
        m_parseStatus = REQ_PARSE_END;
      }
      else if (isChunked(m_request))
      {
        parseChunkedBody(storage);
      }
      else // not chunked
      {
        parseBody(storage);
      }
    }
  }
  catch (std::exception& e)
  {
    m_parseStatus = REQ_PARSE_ERROR;
  }

  if (isForce || m_parseStatus == REQ_PARSE_END || m_parseStatus == REQ_PARSE_ERROR)
  {
    HTTP::Request* ret = m_request;
    m_request = nullptr;
    return (ret);
  }
  return (nullptr);
}

void HTTP::RequestParser::parseStartLine(WS::Storage& storage)
{
  WS::Storage lineBuf(storage.subStorage(storage.getCursor(), storage.find("\r\n")));
              lineBuf.append((unsigned char*)"\0", 1);
  std::string lineStr((char*)lineBuf.data());
  std::stringstream stream(lineStr);
  std::string method, url, version;

  stream >> method >> url >> version;
  if (method.empty() || url.empty() || version.empty())
    throw (std::logic_error("start line parsing failed\n"));
  m_request->setMethod(method);
  m_request->setUrl(url);
  m_request->setVersion(version);
  storage.setCursor(lineBuf.size() + 1); // - '\0' + "\r\n"
}

void HTTP::RequestParser::parseHeader(WS::Storage& storage)
{
  WS::Storage headerBuf(storage.subStorage(storage.getCursor(), storage.find(DELIMITER)));
              headerBuf.append((unsigned char*)"\0", 1);
  std::string headerStr((char*)headerBuf.data());
  const char DELIM = ':';
  size_t start = 0;
  size_t end;

  while (start < headerStr.size())
  {
    std::string key, value;
    // get key
    end = headerStr.find(DELIM, start);
    if (end == std::string::npos)
      throw (std::logic_error("header parsing failed\n"));
    key = headerStr.substr(start, end);
    start = end + 1;
    // get value
    end = headerStr.find("\r\n", start);
    if (end == std::string::npos)
      throw (std::logic_error("header parsing failed\n"));
    value = headerStr.substr(start, end);
    start = end + 2;
    // add header
    m_request->addHeader(key, value);
  }
  // set content length
  auto content_length = m_request->getHeaders().find("Content-Length");
  if (content_length == m_request->getHeaders().end())
    m_request->setContentLength(0);
  else
    m_request->setContentLength(std::stoi(content_length->second));
}

void HTTP::RequestParser::parseBody(WS::Storage& storage)
{
  const size_t CONTENT_LENGTH = m_request->getContentLength();
  auto body = m_request->getBody();
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
  auto body = m_request->getBody();
       body.reserve(BUFFER_SIZE);
  size_t start = storage.getCursor();
  size_t end;

  while (start < storage.size())
  {
    end = storage.find(DELIMITER, start);
    std::string sizeStr((const char*) (storage.subStorage(start, end - start).data()));
    size_t size = std::stoi(sizeStr);
    start = end + 2;
    // read..
    body.append(&storage.data()[start], size);
    start += size + 4; // + delim size..
  }
  storage.setCursor(start);
  storage.pop(storage.getCursor() + 1);
}
