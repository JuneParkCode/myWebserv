//
// Created by Sungjun Park on 2022/12/11.
//

#ifndef CONNECTION_HPP
#define CONNECTION_HPP

#include "HTTPDefinitions.hpp"
#include "Response.hpp"
#include "Request.hpp"
#include <queue>
#include <netinet/in.h>

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
      bool m_closed;
  public:
      struct sockaddr_in m_socketIn;
  public:
      void parseRequestFromStorage();
      void setSocketFD(FileDescriptor fd);
      void closeConnection(); // close connection and delete jobs from thread Queue
      HTTP::Request& getRequest();
      WS::Storage& getReceiveStorage();
      WS::Storage& getReadFileStorage();
      WS::Storage& getWriteFileStorage();
      std::string getClientIP() const;
      Connection();
      ~Connection();
  };
}

#endif //CONNECTION_HPP
