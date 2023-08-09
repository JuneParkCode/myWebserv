//
// Created by Sungjun Park on 2022/12/11.
//

#include "Connection.hpp"
#include "Server.hpp"
#include "Handlers.hpp"
#include "RequestProcessor.hpp"
#include <sys/event.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>

extern WS::Server* G_SERVER;

#define DEFAULT_BUFFER_SIZE 80 * 1000 * 1000

WS::Connection::Connection(WS::VirtualServer* server) :
        m_socketFD(-1), m_readFD(-1), m_writeFD(-1),  m_request(nullptr), m_closed(false),
        m_socketRecvEvent(EV_TYPE_RECEIVE_SOCKET, this, server, WS::handleSocketReceive),
        m_socketSendEvent(EV_TYPE_SEND_SOCKET, this, server, WS::handleSocketSend),
        m_fileReadEvent(EV_TYPE_READ_FILE, this, server, WS::handleFileReadToSend),
        m_fileWriteEvent(EV_TYPE_WRITE_FILE, this, server, WS::handleFileWrite),
        m_server(server),
        m_reqeustParser(this),
        m_fileReadStorage(DEFAULT_BUFFER_SIZE),
        m_fileWriteStorage(DEFAULT_BUFFER_SIZE),
        m_socketRecvStorage(DEFAULT_BUFFER_SIZE),
        m_socketSendStorage(DEFAULT_BUFFER_SIZE)
{
}

WS::Connection::~Connection()
{
  closeConnection();
  delete (m_request);
  m_request = nullptr;
}

// FIXME : 현재는 HTTP만 받지만 다른 프로토콜도 받을 수 있게 만들어보자.
void WS::Connection::parseRequestFromStorage(struct kevent event)
{
  auto* request = m_reqeustParser.parse(event, m_socketRecvStorage);
  if (request != nullptr)
  {
    m_request = request;
    auto* response = HTTP::RequestProcessor::createResponse(request, this);
    request->setResponse(response);
    m_server->response(request, response);
  }
}

void WS::Connection::setSocketFD(FileDescriptor fd)
{
  m_socketFD = fd;
}

void WS::Connection::closeConnection()
{
  if (m_socketFD > 0)
  {
    ::shutdown(m_socketFD, SHUT_RDWR);
    if (::close(m_socketFD) < 0)
      std::cout << PRINT_RED + "CLOSE ERR" << std::endl;
    m_socketFD = -1;
  }
  if (m_readFD > 0)
  {
    if (::close(m_readFD) < 0)
      std::cout << PRINT_RED + "CLOSE ERR" << std::endl;
    m_readFD = -1;
  }
  if (m_writeFD > 0)
  {
    if (::close(m_writeFD) < 0)
      std::cout << PRINT_RED + "CLOSE ERR" << std::endl;
    m_writeFD = -1;
  }
  std::cerr << PRINT_RESET +  "Disconnect : " +  getClientIP() << std::endl;
}

WS::VirtualServer& WS::Connection::getServer()
{
  return (*m_server);
}

HTTP::Request* WS::Connection::getRequest()
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

void WS::Connection::setRequest(HTTP::Request* request)
{
  m_request = request;
}
