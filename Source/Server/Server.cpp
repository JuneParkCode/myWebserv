//
// Created by Sungjun Park on 2022/12/12.
//

#include "ThreadPool.hpp"
#include "Server.hpp"
#include "Handlers.hpp"
#include "Event.hpp"
#include <iostream>

WS::Server* G_SERVER; // 해당 변수에 외부 사용자는 접근할 수 없게 만들 수 없을까?

void WS::Server::attachEvent(struct kevent& event) const
{
  if (kevent(m_kqueue, &event, 1, nullptr, 0, nullptr) < 0)
  {
    auto* ev = reinterpret_cast<Event*>(event.udata);
    std::cerr << PRINT_MAGENTA + "event attach failed" + std::to_string(event.ident) << std::endl;
    std::cerr << "TYPE : " << (int)ev->type << std::endl;
    std::cerr << ::strerror(errno) << std::endl;
    // 별도의 처리가 필요?
  }
}

void WS::Server::attachEvent(ssize_t ident, size_t filter, size_t flags, size_t fflags, void* udata) const
{
  struct kevent event;
  EV_SET(&event, ident, filter, flags, fflags, 0, udata);

  if (kevent(m_kqueue, &event, 1, nullptr, 0, nullptr) < 0)
  {
    auto* ev = reinterpret_cast<Event*>(udata);
    std::cerr << PRINT_MAGENTA + "event attach failed" + std::to_string(ident) << std::endl;
    std::cerr << "TYPE : " << (int)ev->type << std::endl;
    std::cerr << ::strerror(errno);
    // 별도의 처리가 필요?
  }
}

void WS::Server::run()
{
  m_kqueue = kqueue();
  // listen ports
  try
  {
    listenVirtualServers();
  }
  catch (std::exception& e)
  {
    std::cerr << e.what();
    return ;
  }
  struct kevent event;
  while (true)
  {
    // 서버 시작. 새 이벤트(Req)가 발생할 때 까지 무한루프. (감지하는 kevent)
    int newEventCount = ::kevent(m_kqueue, nullptr, 0, &event, 1, nullptr);

    if (newEventCount > 0)
    {
      auto ev = reinterpret_cast<Event*>(event.udata);
      if (ev->type != EV_TYPE_ACCEPT_CONNECTION)
        attachEvent(event.ident, event.filter, EV_DISABLE, event.fflags, event.udata);
      if (THREAD_MODE)
        m_threadPool.enqueueIOJob(event);
      else
        WS::handleEvent(event);
    }
    else if (newEventCount == 0) // timeout in kq
    {
    }
    else
    {
      // timeout 처리 -> thread가 도는 상황.. 생각해봐야함..
    }
  }
}

WS::Server::Server():
        m_threadPool(std::thread::hardware_concurrency())
{
  std::cout << "SERVER THREADS : " << std::thread::hardware_concurrency() << std::endl;
  G_SERVER = this;
}

WS::Server::Server(size_t numberOfThreads):
  m_threadPool(numberOfThreads)
{
  std::cout << "SERVER THREADS : " << numberOfThreads << std::endl;
  G_SERVER = this;
}

void WS::Server::attachNewJob(const WS::Job& job)
{
  m_threadPool.enqueueNormalJob(job);
}

WS::VirtualServer& WS::Server::listen(const std::string& hostname, const std::string& IP, const std::string& port)
{
  m_virtualServers.emplace_back(hostname, IP, port, this);
  return (*m_virtualServers.rbegin());
}

void WS::Server::listenVirtualServers()
{
  for (auto& server : m_virtualServers)
  {
    server.listen();
  }
}
