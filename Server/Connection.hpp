//
// Created by Sungjun Park on 2022/12/11.
//

#ifndef CONNECTION_HPP
#define CONNECTION_HPP

#include "HTTPDefinitions.hpp"
#include "Response.hpp"
#include "Request.hpp"
#include "Server.hpp"
#include <queue>

// HTTP 1.1 은 기본적으로 Pipelining 을 지원해야하지만, 실제로는 그렇지 않은 서버가 많다.
// 해당 서버도 Pipelining을 지원하지 않을 것이다.

namespace WS
{
  // connection must be deleted after closeConnection()
  class Connection
  {
  private:
      FileDescriptor m_socketFD;
      HTTP::Request m_request;
      WS::Storage m_receiveStorage;
      WS::Storage m_readFileStorage;
      WS::Storage m_writeFileStorage;
      size_t m_threadNO;
      Server& m_server;
  public:
      void parseRequestFromStorage();
      void setThreadNO(size_t thredNO);
      void closeConnection(); // close connection and delete jobs from thread Queue
      void attachEvent(struct kevent& event) const;
      size_t getThreadNO() const;
      HTTP::Request& getRequest() const;
      WS::Storage& getReceiveStorage() const;
      WS::Storage& getReadFileStorage() const;
      WS::Storage& getWriteFileStorage() const;
      Connection();
      ~Connection();
  };
}

#endif //CONNECTION_HPP
