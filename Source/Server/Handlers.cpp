#include "Handlers.hpp"
#include "Server.hpp"
#include "Job.hpp"
#include "Connection.hpp"
#include <unistd.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <iostream>

#define BUFFER_SIZE (10 * 1000 * 1024)

extern WS::Server* G_SERVER;

// 같은 connection에서는 call이 작업에 순차적으로 발생하기 때문에 connection을 delete 하기만 하면 이후에 같은 connection에 대해서 작업을 하지 않음.

void WS::handleEvent(struct kevent& event)
{
  auto ev = reinterpret_cast<Event*>(event.udata);

  if (event.flags & EV_ERROR) // EV_ERROR -> disconnect connection...
  {
    std::cerr << "EV_ERR\n";
    delete (ev->connection);
    return ;
  }

  switch (ev->type)
  {
    case EV_TYPE_READ_FILE:
    {
      if (event.flags & EV_EOF) // file read done
      {
        FileDescriptor readFD = ev->connection->getReadFd();
        if (readFD > 0)
        {
          close(readFD);
          ev->connection->setReadFd(-1);
        }
        return ;
      }
      break;
    }
    case EV_TYPE_WRITE_FILE:
    {
      if (event.flags & EV_EOF)
      {
        FileDescriptor writeFD = ev->connection->getWriteFd();
        if (writeFD > 0)
        {
          close(writeFD);
          ev->connection->setWriteFd(-1);
        }
        return ;
      }
      break;
    }
    case EV_TYPE_RECEIVE_SOCKET:
    {
      if (event.flags & EV_EOF)  // client closed own write socket -> still can response
      {
        ::shutdown(event.ident, SHUT_RD);
        return ;
      }
    }
    case EV_TYPE_SEND_SOCKET:
    {
      if (event.flags & EV_EOF) // client closed own read socket -> can not response
      {
        delete (ev->connection);
        return ;
      }
      break;
    }
    case EV_TYPE_ACCEPT_CONNECTION:
    {
      handleAcceptConnection(event);
      G_SERVER->attachEvent(event.ident, event.filter, EV_ADD | EV_ENABLE, event.fflags, event.udata);
      return ;
    }
  }
  ev->handler(event);
}

// receive data from socket and store at connection buffer
void WS::handleSocketReceive(struct kevent& event)
{
//  std::cerr << "socket receive\n";
  auto ev = reinterpret_cast<Event*>(event.udata);
  auto& connection = *ev->connection;
  auto& receiveBuffer = connection.getSocketReceiveStorage();
  auto readSize = receiveBuffer.read(event.ident);
//  static size_t total;

  if (readSize == -1)
  {
    std::cerr << strerror(errno) << std::endl;
    delete (ev->connection);
  }
  else
  {
    // if client closed socket write, response by socket residue + stored data...
//    total += readSize;
//    std::cout << "socket received : " << (double)readSize / 1024 << "kb"<< std::endl;
//    std::cout << "total received : " << (double)total / (1000 * 1024) << "mb"<< std::endl;
    std::function<void()> jobHandler;
    jobHandler = [&connection, event](){
        connection.parseRequestFromStorage(event);
    };
    WS::Job job(jobHandler);
    G_SERVER->attachNewJob(job);
  }
}

// send data from response send buffer
void WS::handleSocketSend(struct kevent& event)
{
//  std::cerr << "send handler\n";
  auto ev = reinterpret_cast<Event*>(event.udata);
  auto& buffer = ev->connection->getSocketSendStorage();
  const size_t bufferCursor = buffer.getCursor();
  const size_t BUF_SIZE = (buffer.size() - bufferCursor);

  auto sendSize = ::send((FileDescriptor)event.ident, &buffer.data()[bufferCursor], BUF_SIZE, MSG_DONTWAIT);
  if (sendSize == -1)
  {
    delete (ev->connection);
  }
  else
  {
    if (buffer.size() <= bufferCursor + sendSize) // buffer send done
    {
      buffer.clear();
      // check if response is done -> readFileFD == -1  (no more read file and no more send..)
      if (ev->connection->getReadFd() < 0)
      {
        // enable socket recv
        G_SERVER->attachEvent(ev->connection->getSocketFd(), EVFILT_READ, EV_ADD, event.fflags, &ev->connection->m_socketRecvEvent);
      }
      else
      {
        // read more..
        G_SERVER->attachEvent(ev->connection->getReadFd(), EVFILT_READ, EV_ADD, event.fflags, &ev->connection->m_fileReadEvent);
      }
    }
    else // partial send...
    {
      buffer.setCursor(bufferCursor + sendSize);
      // send more..
      G_SERVER->attachEvent(event.ident, event.filter, EV_ADD, event.fflags, &ev->connection->m_socketSendEvent);
    }
  }
}

void WS::handleFileReadToSend(struct kevent& event)
{
  std::cerr << "fread handler\n";
  auto ev = reinterpret_cast<Event*>(event.udata);
  auto& readBuffer = ev->connection->getFileReadStorage();

  auto readSize = readBuffer.read(event.ident);
  if (readSize == -1)
  {
    delete (ev->connection);
  }
  else if (readSize == 0 || readSize < BUFFER_SIZE) // read done
  {
    close(ev->connection->getReadFd());
    ev->connection->setReadFd(-1);
  }

  if (readSize > 0) // send it!
  {
    G_SERVER->attachEvent(ev->connection->getSocketFd(), EVFILT_WRITE, EV_ADD, 0, &ev->connection->m_socketSendEvent);
  }
}

void WS::handleFileWrite(struct kevent& event)
{
  std::cerr << "write handler\n";
  auto ev = reinterpret_cast<Event*>(event.udata);
  auto& buffer = ev->connection->getFileWriteStorage();
  const size_t bufferCursor = buffer.getCursor();
  const size_t BUF_SIZE = (buffer.size() - bufferCursor);

  auto writeSize = ::write((FileDescriptor)event.ident, &buffer.data()[bufferCursor], BUF_SIZE);
  if (writeSize == -1)
  {
    delete (ev->connection);
  }
  else
  {
    if (buffer.size() <= bufferCursor + writeSize) // write buffer to file done
    {
      buffer.clear();
      close(ev->connection->getWriteFd());
      ev->connection->setWriteFd(-1);
    }
    else // partial write...
    {
      buffer.setCursor(bufferCursor + writeSize);
      // write more..
      G_SERVER->attachEvent(ev->connection->getWriteFd(), EVFILT_WRITE, EV_ADD, event.fflags, &ev->connection->m_fileWriteEvent);
    }
  }
}

void WS::handleAcceptConnection(struct kevent& event)
{
  struct sockaddr_in sockIn;
  socklen_t len = sizeof(sockIn);
  FileDescriptor newSocket = ::accept((FileDescriptor)event.ident, reinterpret_cast<sockaddr*>(&sockIn), &len);

  if (newSocket < 0)
  {
    std::cerr << "accept failed : " << ::strerror(errno) << std::endl;
    std::cerr << "socket fd : " << event.ident << std::endl;
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
    std::cout << "Connect : " << newConnection->getClientIP() << std::endl;
    G_SERVER->attachEvent(newSocket, EVFILT_READ, EV_ADD, 0, &newConnection->m_socketRecvEvent);
  }
}
