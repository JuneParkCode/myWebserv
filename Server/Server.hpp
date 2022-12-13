//
// Created by Sungjun Park on 2022/12/12.
//

#ifndef SERVER_HPP
#define SERVER_HPP

#include "VirtualServer.hpp"
#include <sys/event.h>
#include <vector>

typedef int FileDescriptor;

namespace WS
{
  class Server
  {
  private:
    FileDescriptor m_kqueue;
    std::vector<VirtualServer> m_virtualServers;
    void listenVirtualServers();
  public:
    void attachEvent(struct kevent& event);
    void run();
    Server();
    ~Server() = default;
  };
}

#endif //SERVER_HPP
