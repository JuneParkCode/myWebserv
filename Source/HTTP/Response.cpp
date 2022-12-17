//
// Created by Sungjun Park on 2022/12/10.
//

#include "Response.hpp"

void HTTP::Response::setHeader(const std::string& key, const std::string& value)
{

}

void HTTP::Response::setResponseLine(const std::string& version, const std::string& statusCode, const std::string& statusMessage)
{

}

const std::string& HTTP::Response::getVersion() const
{
  return (m_version);
}

const std::string& HTTP::Response::getStatusCode() const
{
  return (m_statusCode);
}

const std::string& HTTP::Response::getStatusMesseage() const
{
  return (m_statusMessage);
}

const std::unordered_map<std::string, std::string>& HTTP::Response::getHeader() const
{
  return (m_headers);
}

void HTTP::Response::send()
{

}

HTTP::Response::Response(HTTP::Request* request, WS::Connection* connection):
  m_request(request), m_connection(connection), m_isDone(false)
{
}