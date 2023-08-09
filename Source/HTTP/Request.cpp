//
// Created by Sungjun Park on 2022/12/16.
//

#include "Request.hpp"
#include "Response.hpp"

WS::Storage& HTTP::Request::getBody()
{
  return (m_body);
}

const std::unordered_map<std::string, std::string>& HTTP::Request::getHeaders() const
{
  return (m_headers);
}

const std::string& HTTP::Request::getMethod() const
{
  return (m_method);
}

const std::string& HTTP::Request::getPath() const
{
  return (m_path);
}

const std::string& HTTP::Request::getVersion() const
{
  return (m_version);
}

size_t HTTP::Request::getContentLength() const
{
  return (m_contentLength);
}

void HTTP::Request::setContentLength()
{
  if (m_contentLength == 0 && isChunked())
    m_contentLength = m_body.size();
  else // common body
  {
    auto it = m_headers.find("Content-Length");
    if (it == m_headers.end())
      m_contentLength = 0;
    else
      m_contentLength = std::stoll(it->second);
  }
}

void HTTP::Request::setContentLength(size_t len)
{
  m_contentLength = len;
}

void HTTP::Request::setError(HTTP::StatusCode statusCode)
{
  m_isError = true;
  m_errorCode = statusCode;
}

void HTTP::Request::setRequestLine(const std::string& method, const std::string& path, const std::string& version)
{
  m_method = method;
  m_path = path;
  m_version = version;
}

void HTTP::Request::response()
{

}

bool HTTP::Request::isChunked() const
{
  auto it = m_headers.find("Transfer-Encoding");

  if (it == m_headers.end() || it->second.find("chunked") == std::string::npos)
    return (false);
  return (true);
}

void HTTP::Request::addHeader(const std::string& key, const std::string& value)
{
  auto pair = std::pair<std::string, std::string>(key, value);

  m_headers[key] = value;
}

HTTP::Request::Request() noexcept:
  m_contentLength(0),
  m_isError(false),
  m_baseTime(clock()),
  m_errorCode(ST_ERROR)
{
}

#include <iostream>
void HTTP::Request::display() const
{
  std::cout << m_method << " " << m_path << " " << m_version << std::endl;
  for (auto it : m_headers)
  {
    std::cout << "(" << it.first << ") : (" << it.second << ")" << std::endl;
  }
  std::cout << "Content-Length : " << m_contentLength << std::endl;
  std::cout << "through put : " << getThroughPut() << "mb/s" << std::endl;
}

double HTTP::Request::getThroughPut() const
{
  clock_t now = clock();
  auto runTime = (double)(now - m_baseTime);

  return ((m_body.size() / (runTime / CLOCKS_PER_SEC) / (1000 * 1000))); // by MB
}

bool HTTP::Request::isErrorRequest() const
{
  return (m_isError);
}

void HTTP::Request::setResponse(HTTP::Response* response)
{
  m_response = response;
}

HTTP::Request::~Request()
{
  delete (m_response);
}
