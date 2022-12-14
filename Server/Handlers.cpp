#include "Handlers.hpp"
#include "Server.hpp"
#include "Job.hpp"
#include <unistd.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <iostream>

#define BUFFER_SIZE (16 * 1024)

extern WS::Server* G_SERVER;

// 같은 connection에서는 call이 작업에 순차적으로 발생하기 때문에 connection을 delete 하기만 하면 이후에 같은 connection에 대해서 작업을 하지 않음.

void WS::handleEvent(struct kevent& event)
{
  auto ev = reinterpret_cast<Event*>(event.udata);

  if (event.flags & EV_ERROR) // EV_ERROR -> disconnect connection...
  {
    std::cerr << "EV_ERR\n";
    delete (ev->connection);
    delete (ev);
    return ;
  }

  switch (ev->type)
  {
    case EV_TYPE_READ_FILE:
    {
      if (event.flags & EV_EOF) // file read done
      {
        delete (ev);
        return ;
      }
      handleFileRead(event);
      break ;
    }
    case EV_TYPE_WRITE_FILE:
    {
      if (event.flags & EV_EOF)
      {
        delete (ev);
        return ;
      }
      handleFileWrite(event);
      break ;
    }
    case EV_TYPE_RECEIVE_SOCKET:
    {
      if (event.flags & EV_EOF) // connection closed
      {
        delete (ev->connection);
        delete (ev);
        return ;
      }
      handleSocketReceive(event);
      // receive 의 경우 request 가 모두 종료되고 난 이후에 다시 받을 수 있도록 함..
      return ;
    }
    case EV_TYPE_SEND_SOCKET:
    {
      if (event.flags & EV_EOF) // connection closed
      {
        delete (ev->connection);
        delete (ev);
        return ;
      }
      handleSocketSend(event);
      break ;
    }
    case EV_TYPE_ACCEPT_CONNECTION:
    {
      handleAcceptConnection(event);
      break ;
    }
  }
  // enable event
  G_SERVER->attachEvent(event.ident, event.filter, EV_ENABLE, event.fflags, event.udata);
}

// receive data from socket and store at connection buffer
void WS::handleSocketReceive(struct kevent& event)
{
  std::cerr << "handle receive\n";
  unsigned char buffer[BUFFER_SIZE];
  auto ev = reinterpret_cast<Event*>(event.udata);
  auto readSize = read((FileDescriptor)event.ident, buffer, sizeof(buffer));
  auto& receiveBuffer = ev->connection->getReceiveStorage();
  auto& connection = *ev->connection;

  if (readSize == -1)
  {
    delete (ev->connection);
  }
  else
  {
    connection.getReceiveStorage().append(buffer, readSize);
    auto jobHandler = [&connection](){
      connection.parseRequestFromStorage();
    };
    WS::Job job(jobHandler);
    G_SERVER->attachNewJob(job);
  }
}

// send data from response send buffer
void WS::handleSocketSend(struct kevent& event)
{
  auto ev = reinterpret_cast<Event*>(event.udata);
  auto& buffer = ev->connection->getReadFileStorage();
  auto sendSize = ::send((FileDescriptor)event.ident, buffer.data(), buffer.size(), MSG_DONTWAIT);
  // FIXME: buffer.empty() || !buffer -> ERROR?
  if (sendSize == -1)
  {
    delete (ev->connection);
  }
  else
  {
    buffer.pop(sendSize);
  }
}

// read data from file and store in response send buffer
void WS::handleFileRead(struct kevent& event)
{
  auto ev = reinterpret_cast<Event*>(event.udata);
  unsigned char buffer[BUFFER_SIZE];
  ssize_t readSize = ::read((FileDescriptor)event.ident, buffer, sizeof(buffer));
  auto& readBuffer = ev->connection->getReadFileStorage();

  if (readSize == -1)
  {
    delete (ev->connection);
  }
  else
  {
    readBuffer.append(buffer, readSize);
  }
}

// write body buffer from request to fd
void WS::handleFileWrite(struct kevent& event)
{
  auto ev = reinterpret_cast<Event*>(event.udata);
  auto buffer = ev->connection->getWriteFileStorage();
  auto writeSize = ::write(event.ident, buffer.data(), buffer.size());
  // get body buffer from request

  if (writeSize == -1)
  {
    delete (ev->connection);
  }
  else
  {
    buffer.pop(writeSize);
  }
}

void WS::handleAcceptConnection(struct kevent& event)
{
  auto ev = reinterpret_cast<Event*>(event.udata);
  struct sockaddr_in sockIn;
  socklen_t len = sizeof(sockIn);
  FileDescriptor newSocket = ::accept((FileDescriptor)event.ident, reinterpret_cast<sockaddr*>(&sockIn), &len);

  if (newSocket < 0)
  {
    std::cerr << "accept failed\n";
    std::cerr << "fd " << event.ident << std::endl;
    std::cerr << ::strerror(errno);
    return ;
  }
  else
  {
    auto newConnection = new WS::Connection();

    newConnection->setSocketFD(newSocket);
    newConnection->m_socketIn = sockIn;
    struct linger optLinger = {1, 0};
    // set socket option
    if (::fcntl(newSocket, F_SETFL, O_NONBLOCK) < 0)
    {
      throw (std::runtime_error("fcntl non block failed\n"));
    }
    if (::setsockopt(newSocket, SOL_SOCKET, SO_LINGER, &optLinger, sizeof(optLinger)) < 0 )
    {
      throw (std::runtime_error("Socket opt failed\n"));
    }
    std::cerr << "Connect : " << newConnection->getClientIP() << std::endl;
    // new Event could be leak...
    G_SERVER->attachEvent(newSocket, EVFILT_READ, EV_ADD | EV_ENABLE, 0, new Event(EV_TYPE_RECEIVE_SOCKET, newConnection));
  }
}
