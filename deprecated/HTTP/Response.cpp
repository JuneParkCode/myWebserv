//
// Created by Sungjun Park on 2022/12/10.
//

#include "Response.hpp"

void HTTP::Response::setHeader(const std::string& key, const std::string& value)
{

}

void HTTP::Response::setStartLine(const std::string& version, const std::string& statusCode, const std::string& statusMessage)
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

const std::string& HTTP::Response::getHeader(const std::string& key) const
{
  return (m_headers.at(key));
}

void HTTP::Response::send()
{

}

HTTP::Response::Response()
{

}

HTTP::Response::~Response()
{

}
