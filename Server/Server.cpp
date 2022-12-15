//
// Created by Sungjun Park on 2022/12/12.
//

#include "ThreadPool.hpp"
#include "Server.hpp"
#include "Event.hpp"
#include "Handlers.hpp"
#include <iostream>

void WS::Server::attachEvent(struct kevent& event) const
{
  if (kevent(m_kqueue, &event, 1, nullptr, 0, nullptr) < 0)
  {
    std::cerr << "event attach failed\n";
    std::cerr << ::strerror(errno);
    // 별도의 처리가 필요?
  }
}

void WS::Server::attachEvent(ssize_t ident, size_t filter, size_t flags, size_t fflags, void* udata) const
{
  struct kevent ev;
  EV_SET(&ev, ident, filter, flags, fflags, 0, udata);

  if (kevent(m_kqueue, &ev, 1, nullptr, 0, nullptr) < 0)
  {
    std::cerr << "event attach failed\n";
    std::cerr << ::strerror(errno);
    // 별도의 처리가 필요?
  }
}

void WS::Server::listenVirtualServers()
{
  for (auto& vServer : m_virtualServers)
  {
    vServer.listen();
    vServer.setListenEvent(Event(EV_TYPE_ACCEPT_CONNECTION, nullptr, WS::handleAcceptConnection));

    attachEvent(vServer.getServerFd(), EVFILT_READ, EV_ADD, 0, &vServer.getListenEvent());
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
      m_threadPool.enqueueIOJob(event);
      attachEvent(event.ident, event.filter, EV_DISABLE, event.fflags, event.udata);
    }
    else if (newEventCount == 0) // timeout in kq
    {
      // clear connections.
    }
    else
    {
      // timeout 처리 -> thread가 도는 상황.. 생각해봐야함..
    }
  }
}

WS::Server::Server():
        m_threadPool(20)
{
  // parse config file and set virtual servers

  // temporary
  Location loc;
  loc.setAutoIndex(false);
  loc.setRoot("../");
  std::vector<std::string> tmp;
  tmp.emplace_back("index.html");
  loc.setIndex(tmp);
  loc.setClientMaxBodySize(10000);
  tmp.clear();
  tmp.emplace_back("GET");
  loc.setAllowMethods(tmp);
  tmp.clear();
  loc.setAllowCGI(tmp);
  std::vector<Location> locs;
  m_virtualServers.emplace_back("webserv", "0.0.0.0", "42424", locs);
}

void WS::Server::attachNewJob(const WS::Job& job)
{
  m_threadPool.enqueueNormalJob(job);
}
