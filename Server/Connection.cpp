//
// Created by Sungjun Park on 2022/12/11.
//

#include "Connection.hpp"
#include <arpa/inet.h>
#include <unistd.h>

void WS::Connection::parseRequestFromStorage()
{
  // parse...
  std::cout << m_receiveStorage;
}

void WS::Connection::setSocketFD(FileDescriptor fd)
{
  m_socketFD = fd;
}

void WS::Connection::closeConnection()
{
  if (!m_closed)
  {
    std::cerr << "connection closed\n";
    ::close(m_socketFD);
    ::close(m_request.getReadFd());
    ::close(m_request.getWriteFd());
    m_closed = true;
    ::free (this);
  }
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
  return {str};
}

WS::Connection::Connection() : m_closed(false)
{

}

WS::Connection::~Connection()
{

}
