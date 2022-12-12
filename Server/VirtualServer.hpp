//
// Created by Sungjun Park on 2022/12/12.
//

#ifndef VIRTUAL_SERVER_HPP
#define VIRTUAL_SERVER_HPP

#include <string>

namespace WS
{
  class VirtualServer
  {
  private:
    std::string m_serverName;
    std::string m_listenIP;
    std::string m_listenPort;
  public:
    VirtualServer();
    ~VirtualServer();
  };
}

#endif //VIRTUAL_SERVER_HPP
