//
// Created by Sungjun Park on 2022/12/11.
//

#include "Connection.hpp"
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>

void WS::Connection::parseRequestFromStorage()
{
}

void WS::Connection::setSocketFD(FileDescriptor fd)
{
  m_socketFD = fd;
}

void WS::Connection::closeConnection()
{
  if (!m_closed)
  {
    m_closed = true;
    if (m_socketFD > 0)
      ::close(m_socketFD);
    if (m_readFD > 0)
      ::close(m_readFD);
    if (m_writeFD > 0)
      ::close(m_writeFD);
    delete (m_request);
    ::free (this);
    std::cerr << "connection closed\n";
  }
}

WS::ARequest* WS::Connection::getRequest()
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
  return {str};
}

WS::Connection::Connection() :
  m_socketFD(-1), m_readFD(-1), m_writeFD(-1),  m_request(nullptr), m_closed(false)
{
}

WS::Connection::~Connection()
{

}

FileDescriptor WS::Connection::getSocketFd() const
{
  return m_socketFD;
}

void WS::Connection::setSocketFd(FileDescriptor socketFd)
{
  m_socketFD = socketFd;
}

FileDescriptor WS::Connection::getReadFd() const
{
  return m_readFD;
}

void WS::Connection::setReadFd(FileDescriptor readFd)
{
  m_readFD = readFd;
}

FileDescriptor WS::Connection::getWriteFd() const
{
  return m_writeFD;
}

void WS::Connection::setWriteFd(FileDescriptor writeFd)
{
  m_writeFD = writeFd;
}
