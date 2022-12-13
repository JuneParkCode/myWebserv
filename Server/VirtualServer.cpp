//
// Created by Sungjun Park on 2022/12/12.
//

#include "VirtualServer.hpp"

const std::string& WS::VirtualServer::getServerName() const
{
  return m_serverName;
}

void WS::VirtualServer::setServerName(const std::string& serverName)
{
  VirtualServer::m_serverName = serverName;
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
