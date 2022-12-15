//
// Created by Sungjun Park on 2022/12/11.
//

#include "Connection.hpp"
#include "Handlers.hpp"
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>

WS::Connection::Connection() :
        m_socketFD(-1), m_readFD(-1), m_writeFD(-1),  m_request(nullptr), m_closed(false),
        m_socketRecvEvent(EV_TYPE_RECEIVE_SOCKET, this, WS::handleSocketReceive),
        m_socketSendEvent(EV_TYPE_SEND_SOCKET, this, WS::handleSocketSend),
        m_fileReadEvent(EV_TYPE_READ_FILE, this, WS::handleFileReadToSend),
        m_fileWriteEvent(EV_TYPE_WRITE_FILE, this, WS::handleFileWrite)
{
}

WS::Connection::~Connection()
{
  closeConnection();
}

void WS::Connection::parseRequestFromStorage(bool isForceParse)
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
    {
      ::close(m_socketFD);
      m_socketFD = -1;
    }
    if (m_readFD > 0)
    {
      ::close(m_readFD);
      m_readFD = -1;
    }
    if (m_writeFD > 0)
    {
      ::close(m_writeFD);
      m_writeFD = -1;
    }
    delete (m_request);
    m_request = nullptr;
현    std::cerr << "Disconnect : " << getClientIP() << std::endl;
  }
}

WS::ARequest* WS::Connection::getRequest()
{
  return (m_request);
}

WS::Storage& WS::Connection::getSocketReceiveStorage()
{
  return (m_socketRecvStorage);
}

WS::Storage& WS::Connection::getSocketSendStorage()
{
  return (m_socketSendStorage);
}

WS::Storage& WS::Connection::getFileReadStorage()
{
  return (m_fileReadStorage);
}

WS::Storage& WS::Connection::getFileWriteStorage()
{
  return (m_fileWriteStorage);
}


std::string WS::Connection::getClientIP() const
{
  auto pV4Addr = (struct sockaddr_in*)&m_socketIn;
  struct in_addr ipAddr = pV4Addr->sin_addr;
  char str[INET_ADDRSTRLEN];

  ::inet_ntop( AF_INET, &ipAddr, str, INET_ADDRSTRLEN );
  return {str};
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
