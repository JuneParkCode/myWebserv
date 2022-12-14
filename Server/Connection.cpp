//
// Created by Sungjun Park on 2022/12/11.
//

#include "Connection.hpp"
#include <arpa/inet.h>
#include <unistd.h>

void WS::Connection::parseRequestFromStorage()
{

}

void WS::Connection::setThreadNO(size_t threadNO)
{
  m_threadNO = threadNO;
}

void WS::Connection::setSocketFD(FileDescriptor fd)
{
  m_socketFD = fd;
}

void WS::Connection::closeConnection()
{
  if (!m_closed)
  {
    ::close(m_socketFD);
    ::close(m_request.getReadFd());
    ::close(m_request.getWriteFd());
    m_closed = true;
    delete (this);
  }
}

size_t WS::Connection::getThreadNO() const
{
  return (m_threadNO);
}

HTTP::Request& WS::Connection::getRequest()
{
  return (m_request);
}

WS::Storage& WS::Connection::getReceiveStorage()
{
  return (m_receiveStorage);
}

WS::Storage& WS::Connection::getReadFileStorage()
{
  return (m_readFileStorage);
}

WS::Storage& WS::Connection::getWriteFileStorage()
{
  return (m_writeFileStorage);
}

std::string WS::Connection::getClientIP() const
{
  auto pV4Addr = (struct sockaddr_in*)&m_socketIn;
  struct in_addr ipAddr = pV4Addr->sin_addr;
  char str[INET_ADDRSTRLEN];

  ::inet_ntop( AF_INET, &ipAddr, str, INET_ADDRSTRLEN );
  return std::string(str);
}

WS::Connection::Connection() : m_closed(false)
{

}

WS::Connection::~Connection()
{

}
