#include "Handlers.hpp"
#include "Server.hpp"
#include "Job.hpp"
#include "Connection.hpp"
#include <unistd.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <iostream>
#include <signal.h>

#define BUFFER_SIZE (10 * 1000 * 1000)

extern WS::Server* G_SERVER;

void WS::handleEvent(struct kevent& event)
{
  auto ev = reinterpret_cast<Event*>(event.udata);

  if (event.flags & EV_ERROR) // EV_ERROR -> disconnect connection...
  {
    std::cerr << PRINT_RED + "EV_ERR\n";
//    delete (ev->connection);
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
          if (::close(readFD) < 0)
            std::cout << PRINT_RED + "CLOSE ERR" << std::endl;
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
          if (::close(writeFD) < 0)
            std::cout << PRINT_RED + "CLOSE ERR" << std::endl;
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
        std::cout << PRINT_YELLOW +  "SHUTDOWN SOCK RECV\n";
        ::shutdown(event.ident, SHUT_RD);
        ev->connection->m_closed = true;
        G_SERVER->attachEvent(ev->connection->getSocketFd(), EVFILT_READ, EV_ADD | EV_ENABLE, 0, &ev->connection->m_socketSendEvent);
        return ;
      }
    }
    case EV_TYPE_SEND_SOCKET:
    {
      if (event.flags & EV_EOF) // client closed own read socket -> can not response
      {
        std::cerr  << PRINT_RED + "CLOSE SOCK WRITE\n";
        delete (ev->connection);
        return ;
      }
      break;
    }
    case EV_TYPE_ACCEPT_CONNECTION:
    {
      handleAcceptConnection(event);
      G_SERVER->attachEvent(event.ident, event.filter, EV_ADD, event.fflags, event.udata);
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
  if (connection.getRequest() != nullptr)
    return ;
  auto& receiveBuffer = connection.getSocketReceiveStorage();
  auto readSize = receiveBuffer.read(event.ident);

  if (readSize == -1)
  {
    std::cerr << PRINT_RED + "recv failed\n";
//    delete (ev->connection);
  }
  else
  {
//    if (THREAD_MODE)
//    {
//      std::function<void()> jobHandler;
//      jobHandler = [&connection, event](){
//          connection.parseRequestFromStorage(event);
//      };
//      WS::Job job(jobHandler);
////      G_SERVER->attachNewJob(job);
//    }
//    else
      connection.parseRequestFromStorage(event);
  }
}

// send data from response send buffer
void WS::handleSocketSend(struct kevent& event)
{
//  std::cerr << "send handler\n";
  signal(SIGPIPE, SIG_IGN);
  auto ev = reinterpret_cast<Event*>(event.udata);
  auto& buffer = ev->connection->getSocketSendStorage();
  const size_t bufferCursor = buffer.getCursor();
  const size_t BUF_SIZE = (buffer.size() - bufferCursor);

  auto sendSize = ::send((FileDescriptor)event.ident, &buffer.data()[bufferCursor], BUF_SIZE, MSG_DONTWAIT);
  if (sendSize == -1)
  {
    std::cerr << PRINT_RED + "send failed\n";
//    delete (ev->connection);
  }
  else
  {
    if (buffer.size() <= bufferCursor + sendSize) // buffer send done
    {
      buffer.clear();
      if (ev->connection->m_closed)
      {
        std::cout << PRINT_BLUE + "send done, close connection" + std::to_string(ev->connection->getSocketFd()) << std::endl;
        delete (ev->connection);
      }
      else
      {
        G_SERVER->attachEvent(ev->connection->getSocketFd(), EVFILT_READ, EV_ADD | EV_ENABLE, 0, &ev->connection->m_socketRecvEvent);
        ev->connection->setRequest(nullptr);
      }
    }
    else // partial send...
    {
      buffer.setCursor(bufferCursor + sendSize);
      G_SERVER->attachEvent(event.ident, EVFILT_WRITE, EV_ADD | EV_ENABLE, event.fflags, &ev->connection->m_socketSendEvent);
    }
  }
}

void WS::handleFileReadToSend(struct kevent& event)
{
  std::cerr << "fread handler\n";
  auto ev = reinterpret_cast<Event*>(event.udata);
  auto& readBuffer = ev->connection->getSocketSendStorage();
  auto readSize = readBuffer.read(event.ident);

  if (readSize == -1)
  {
    std::cerr << PRINT_RED + "read failed\n";
    delete (ev->connection);
  }
  else if (event.data + readSize >= 0) // read done
  {
    if (::close(ev->connection->getReadFd()) < 0)
      std::cout << PRINT_RED + "CLOSE ERR" << std::endl;
    ev->connection->setReadFd(-1);
    G_SERVER->attachEvent(ev->connection->getSocketFd(), EVFILT_WRITE,  EV_ADD | EV_ENABLE, 0, &ev->connection->m_socketSendEvent);
  }
  else
  {
    // read  again.
    std::cerr << "read again\n";
    G_SERVER->attachEvent(event.ident, EVFILT_READ, EV_ADD | EV_ENABLE, 0, event.udata);
  }
}

void WS::handleFileWrite(struct kevent& event)
{
//  std::cerr << "write handler\n";
  auto ev = reinterpret_cast<Event*>(event.udata);
  auto& buffer = ev->connection->getFileWriteStorage();
  const size_t bufferCursor = buffer.getCursor();
  const size_t BUF_SIZE = (buffer.size() - bufferCursor);

  auto writeSize = ::write((FileDescriptor)event.ident, &buffer.data()[bufferCursor], BUF_SIZE);
  if (writeSize == -1)
  {
    std::cerr << PRINT_RED + "write failed\n";
    delete (ev->connection);
  }
  else
  {
    if (buffer.size() <= bufferCursor + writeSize) // write buffer to file done
    {
      buffer.clear();
      if (::close(ev->connection->getWriteFd()) < 0)
        std::cout << PRINT_RED + "CLOSE ERR" << std::endl;
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
    std::cerr << PRINT_RED + "accept failed : " << ::strerror(errno) << std::endl;
    std::cerr << "socket fd : " << event.ident << std::endl;
    return ;
  }
  else
  {
    auto* ev = reinterpret_cast<Event*>(event.udata);
    auto newConnection = new WS::Connection(ev->server);

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
    std::cout << PRINT_GREEN + "Connect : " << newConnection->getClientIP() + "  BACKLOG : " + std::to_string(event.data) + " FD : " + std::to_string(newSocket) << std::endl;
    G_SERVER->attachEvent(newSocket, EVFILT_READ, EV_ADD, 0, &newConnection->m_socketRecvEvent);
  }
}
