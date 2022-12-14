//
// Created by Sungjun Park on 2022/12/11.
//

#ifndef CONNECTION_HPP
#define CONNECTION_HPP

#include "HTTPDefinitions.hpp"
#include "Response.hpp"
#include "ARequest.hpp"
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
      FileDescriptor m_readFD;
      FileDescriptor m_writeFD;
      WS::ARequest* m_request;
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
      ARequest* getRequest();
      WS::Storage& getReceiveStorage();
      WS::Storage& getReadFileStorage();
      WS::Storage& getWriteFileStorage();
      std::string getClientIP() const;
      FileDescriptor getSocketFd() const;
      void setSocketFd(FileDescriptor socketFd);
      FileDescriptor getReadFd() const;
      void setReadFd(FileDescriptor readFd);
      FileDescriptor getWriteFd() const;
      void setWriteFd(FileDescriptor writeFd);
      Connection();
      ~Connection();
  };
}

#endif //CONNECTION_HPP
