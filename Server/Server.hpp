//
// Created by Sungjun Park on 2022/12/12.
//

#ifndef SERVER_HPP
#define SERVER_HPP

#include "Job.hpp"
#include "VirtualServer.hpp"
#include "ThreadPool.hpp"
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
    ThreadPool m_threadPool;
  private:
    void listenVirtualServers();
  public:
    void attachEvent(struct kevent& event) const;
    void attachEvent(ssize_t ident, size_t filter, size_t flags, size_t fflags, void* udata) const;
    void attachNewJob(const Job& job);
    void run();
    Server();
    ~Server() = default;
  };
}

#endif //SERVER_HPP
