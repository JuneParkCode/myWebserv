//
// Created by Sungjun Park on 2022/12/12.
//

#ifndef VIRTUAL_SERVER_HPP
#define VIRTUAL_SERVER_HPP

#include "Event.hpp"
#include "Location.hpp"
#include <string>

#define BACKLOG 1024

typedef int FileDescriptor;

namespace WS
{
  class VirtualServer
  {
  private:
    std::string m_serverName;
    std::string m_listenIP;
    std::string m_listenPort;
    std::vector<Location> m_locations;
    Event m_listenEvent;
    FileDescriptor m_serverFD;
  public:
    int getServerFd() const;
    void setServerFd(int mServerFd);
    Event& getListenEvent();
    void setListenEvent(const Event& mListenEvent);
    const std::string& getServerName() const;
    void setServerName(const std::string& serverName);
    const std::string& getListenIp() const;
    void setListenIp(const std::string& listenIp);
    const std::string& getListenPort() const;
    void setListenPort(const std::string& listenPort);
    const std::vector<Location>& getLocations() const;
    void setLocations(const std::vector<Location>& locations);
    void listen();

  public:
    ~VirtualServer() = default;
    VirtualServer(std::string  mServerName, std::string  mListenIp, std::string  mListenPort, std::vector<Location>  mLocations);
  };
}

#endif //VIRTUAL_SERVER_HPP
