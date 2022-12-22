//
// Created by Sungjun Park on 2022/12/10.
//

#include "Response.hpp"
#include "Server.hpp"
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

extern WS::Server* G_SERVER;

size_t getFileSize(int fd)
{
  if (fd < 0)
    return 0;
  struct stat stat_buf;
  int rc = fstat(fd, &stat_buf);
  return rc == 0 ? stat_buf.st_size : -1;
}


void HTTP::Response::setHeader(const std::string& key, const std::string& value)
{
  m_headers[key] = value;
}

void HTTP::Response::setResponseLine(const std::string& version, const std::string& statusCode, const std::string& statusMessage)
{
  m_version = version;
  m_statusCode = statusCode;
  m_statusMessage = statusMessage;
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
  auto& buffer = m_connection->getSocketSendStorage();

  if (std::stoi(m_statusCode) >= 400)
  {
    setHeader("Connection", "close");
    m_connection->m_closed = true;
  }
  buffer += this->toString();
  G_SERVER->attachEvent(this->m_connection->getSocketFd(), EVFILT_WRITE, EV_ADD, 0, &m_connection->m_socketSendEvent);
}

void HTTP::Response::sendFile(const char* path) // must be absolute path
{
  int fd;

  if (::access(path, R_OK) == -1 ||  (fd = open(path, O_RDONLY)) == -1)
  {
    setResponseLine("HTTP/1.1", "404", "NOT FOUND");
    fd = ::open(m_connection->getServer().getErrorPage(), O_RDONLY);
  }
  if (fd == -1)
  {
    send();
    return ;
  }
  // set file size
  auto fileSize = getFileSize(fd);
  this->setHeader("Content-Length", std::to_string(fileSize));
  if (std::stoi(m_statusCode) >= 400)
  {
    setHeader("Connection", "close");
    m_connection->m_closed = true;
  }
  auto& buffer = m_connection->getSocketSendStorage();
  auto string = this->toString();
  buffer.append(reinterpret_cast<const WS::Byte*>((char*) string.c_str()), string.size()) ;
  this->m_connection->setReadFd(fd);
  G_SERVER->attachEvent(fd, EVFILT_READ, EV_ADD, 0, &m_connection->m_fileReadEvent);
}

HTTP::Response::Response(HTTP::Request* request, WS::Connection* connection):
  m_request(request), m_connection(connection), m_isDone(false)
{
}

std::string HTTP::Response::toString() const
{
  std::string result;

  result.reserve(500);
  result += m_version + " " + m_statusCode+ " " + m_statusMessage + "\r\n";
  for (const auto& it : m_headers)
  {
    result += (it.first + ": " + it.second + "\r\n");
  }
  result += "\r\n";
  return (result);
}
