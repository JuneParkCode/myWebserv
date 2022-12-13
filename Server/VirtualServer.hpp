//
// Created by Sungjun Park on 2022/12/12.
//

#ifndef VIRTUAL_SERVER_HPP
#define VIRTUAL_SERVER_HPP

#include "Location.hpp"
#include <string>

namespace WS
{
  class VirtualServer
  {
  private:
    std::string m_serverName;
    std::string m_listenIP;
    std::string m_listenPort;
    std::vector<Location> m_locations;
  public:
    const std::string& getServerName() const;
    void setServerName(const std::string& serverName);
    const std::string& getListenIp() const;
    void setListenIp(const std::string& listenIp);
    const std::string& getListenPort() const;
    void setListenPort(const std::string& listenPort);
    const std::vector<Location>& getLocations() const;
    void setLocations(const std::vector<Location>& locations);
  public:
    VirtualServer(const std::string& serverName, const std::string& listenIP, const std::string& listenPort, const std::vector<Location>& m_locations);
    ~VirtualServer();
  };
}

#endif //VIRTUAL_SERVER_HPP
