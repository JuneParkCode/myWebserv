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

const WS::Storage& HTTP::Request::getBody() const
{
  return m_body;
}

void HTTP::Request::setBody(const WS::Storage& mBody)
{
  m_body = mBody;
}

ssize_t HTTP::Request::getContentLength() const
{
  return m_contentLength;
}

void HTTP::Request::setContentLength(ssize_t mContentLength)
{
  m_contentLength = mContentLength;
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

bool HTTP::Request::isTimeout()
{
  return false;
}

bool HTTP::Request::isFinished()
{
  return false;
}

HTTP::Request::Request()
{

}
