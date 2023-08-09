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
#include <string>

#define THREAD_MODE (true)

typedef int FileDescriptor;
// colors
#define PRINT_RED     std::string("\x1b[31m")
#define PRINT_GREEN   std::string("\x1b[32m")
#define PRINT_YELLOW  std::string("\x1b[33m")
#define PRINT_BLUE    std::string("\x1b[34m")
#define PRINT_MAGENTA std::string("\x1b[35m")
#define PRINT_CYAN    std::string("\x1b[36m")
#define PRINT_RESET   std::string("\x1b[0m")

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
    // 일반 유저가 접근 못하게 할 필요가 있음.
    void attachEvent(struct kevent& event) const;
    void attachEvent(ssize_t ident, size_t filter, size_t flags, size_t fflags, void* udata) const;
    void attachNewJob(const Job& job);
    // 서버 프로그래밍 할 때 사용
    VirtualServer& listen(const std::string& hostname, const std::string& IP, const std::string& port); // create virtual server and set listen IP:PORT
    void run(); // run server
    Server();
    Server(size_t numberOfThreads);
    ~Server() = default;
  };
}

#endif //SERVER_HPP
