#include "Request.hpp"

const std::string& HTTP::Request::getMethod() const
{
  return m_method;
}

void HTTP::Request::setMethod(const std::string& mMethod)
{
  m_method = mMethod;
}

const std::string& HTTP::Request::getUrl() const
{
  return m_URL;
}

void HTTP::Request::setUrl(const std::string& mUrl)
{
  m_URL = mUrl;
}

const std::string& HTTP::Request::getVersion() const
{
  return m_version;
}

void HTTP::Request::setVersion(const std::string& mVersion)
{
  m_version = mVersion;
}

const std::map<std::string, std::string>& HTTP::Request::getHeaders() const
{
  return m_headers;
}

WS::Storage& HTTP::Request::getBody()
{
  return (m_body);
}

void HTTP::Request::setBody(const WS::Storage& mBody)
{
  m_body = mBody;
}

size_t HTTP::Request::getContentLength() const
{
  return (m_contentLength);
}

void HTTP::Request::setContentLength(size_t len)
{
  m_contentLength = len;
}

const std::chrono::system_clock& HTTP::Request::getRequestTime() const
{
  return m_requestTime;
}

void HTTP::Request::setRequestTime(const std::chrono::system_clock& mRequestTime)
{
  m_requestTime = mRequestTime;
}

const HTTP::Response& HTTP::Request::getResponse() const
{
  return m_response;
}

void HTTP::Request::setResponse(const HTTP::Response& mResponse)
{
  m_response = mResponse;
}

void HTTP::Request::setParseError()
{
  m_parseError = true;
}

bool HTTP::Request::isTimeout()
{
  return false;
}

bool HTTP::Request::isFinished()
{
  return false;
}

HTTP::Request::Request() : m_contentLength(0), m_parseError(false)
{

}

void HTTP::Request::addHeader(const std::string& key, const std::string& value)
{
  std::pair<std::string, std::string> pair = {key, value};
  m_headers.insert(pair);
}

void HTTP::Request::response()
{

}

void HTTP::Request::display() const
{
  std::cerr << "display request..\n";
  std::cerr << m_method << " " << m_URL << " " << m_version << std::endl;
  for (auto it = m_headers.cbegin(); it != m_headers.cend(); ++it)
  {
    std::cerr << "----------------------------------------------\n";
    std::cerr  << "(" << it->first << "):(" << it->second << ")\n";
  }
  std::cerr << "CONTENT_LENGTH\n";
  std::cerr << m_contentLength << std::endl;
}
