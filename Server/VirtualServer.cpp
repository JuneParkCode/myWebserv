//
// Created by Sungjun Park on 2022/12/12.
//

#include "VirtualServer.hpp"
#include <utility>
#include <sys/socket.h>
#include <fcntl.h>
#include <arpa//inet.h>

// 이번에 새로 배운 형식
// 이렇게 작성하면 copy 가 여러번 일어날 수 있는 문제를 해결할 수 있다.
// 그런데, string 이 짧아서 실질적으로 효용은 없을 것 같다.
WS::VirtualServer::VirtualServer(std::string  mServerName, std::string  mListenIp, std::string  mListenPort,
                                 std::vector<Location>  mLocations) :
        m_serverName(std::move(mServerName)),
        m_listenIP(std::move(mListenIp)),
        m_listenPort(std::move(mListenPort)),
        m_locations(std::move(mLocations))
{
}


const std::string& WS::VirtualServer::getServerName() const
{
  return m_serverName;
}

void WS::VirtualServer::setServerName(const std::string& serverName)
{
  m_serverName = serverName;
}

const std::string& WS::VirtualServer::getListenIp() const
{
  return m_listenIP;
}

void WS::VirtualServer::setListenIp(const std::string& listenIp)
{
  m_listenIP = listenIp;
}

const std::string& WS::VirtualServer::getListenPort() const
{
  return m_listenPort;
}

void WS::VirtualServer::setListenPort(const std::string& listenPort)
{
  VirtualServer::m_listenPort = listenPort;
}

const std::vector<WS::Location>& WS::VirtualServer::getLocations() const
{
  return m_locations;
}

void WS::VirtualServer::setLocations(const std::vector<Location>& locations)
{
  VirtualServer::m_locations = locations;
}

static struct sockaddr_in setSocketAddr(const std::string& IP, const std::string& PORT)
{
  struct sockaddr_in sockIN;

  ::inet_pton(AF_INET, IP.c_str(), &sockIN.sin_addr);
  sockIN.sin_port = ntohs(std::stoi(PORT));
  sockIN.sin_family = AF_INET;
  return (sockIN);
}

void WS::VirtualServer::listen()
{
  struct sockaddr_in sockIN = setSocketAddr(m_listenIP, m_listenPort);

  int opt = 1;
  if ((this->m_serverFD = ::socket(AF_INET, SOCK_STREAM, 0)) < 0)
  {
    throw (std::runtime_error("Create Socket failed\n"));
  }
  if (::setsockopt(this->m_serverFD, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
  {
    throw (std::runtime_error("Socket set option failed\n"));
  }
  if (::fcntl(this->m_serverFD, F_SETFL, O_NONBLOCK) < 0)
  {
    throw (std::runtime_error("fcntl non-block failed\n"));
  }
  if (::bind(this->m_serverFD, reinterpret_cast<const sockaddr*>(&sockIN), sizeof(sockIN)) < 0)
  {
    throw (std::runtime_error("Bind Socket failed\n"));
  }
  if (::listen(this->m_serverFD, BACKLOG) < 0)
  {
    throw (std::runtime_error("Listen Socket failed\n"));
  }
}

WS::Event& WS::VirtualServer::getListenEvent()
{
  return (m_listenEvent);
}

void WS::VirtualServer::setListenEvent(const WS::Event& mListenEvent)
{
  m_listenEvent = mListenEvent;
}

int WS::VirtualServer::getServerFd() const
{
  return m_serverFD;
}

void WS::VirtualServer::setServerFd(int mServerFd)
{
  m_serverFD = mServerFd;
}
