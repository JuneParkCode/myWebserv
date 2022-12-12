#include "Handlers.hpp"
#include <unistd.h>
#include <sys/socket.h>

#define BUFFER_SIZE (16 * 1024)

/*
 * after EV_EOF, jobs must be deleted....
 * Solution 01 : use kqueue per thread            -> 이번 webserv에서 활용한 바 있음. kqueue를 여러번 만드는게 조금 꺼림직 하다.
 *          02 : delete all jobs after EV_EOF     -> EVFILT_READ 와 같은 이벤트는 호출이 여러번 되기 떄문에 EV_CLEAR를 만들어야함.
 *          03 : get Event -> Disable Event
 *               -> handling event
 *               -> enable event                  -> event를 너무 자주 enable disable 시키는 감이 있음. 내부 로직에 의해서 속도에 끼치는게 아닐까?
 * 우선은 Solution 03을 기준 작성할 것.
 * */

void WS::handleEvent(struct kevent& event)
{
  auto ev = reinterpret_cast<Event*>(event.udata);

  if (event.flags & EV_ERROR) // EV_ERROR -> disconnect connection...
  {
    ev->connection->closeConnection();
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
        ev->connection->closeConnection();
        delete (ev->connection);
        delete (ev);
        return ;
      }
      handleSocketReceive(event);
      break ;
    }
    case EV_TYPE_SEND_SOCKET:
    {
      if (event.flags & EV_EOF) // connection closed
      {
        ev->connection->closeConnection();
        delete (ev->connection);
        delete (ev);
        return ;
      }
      handleSocketSend(event);
      break ;
    }
  }
  // enable event
  event.flags = event.flags | EV_ENABLE;
  ev->connection->attachEvent(event);
}

// receive data from socket and store at connection buffer
void WS::handleSocketReceive(struct kevent& event)
{
  unsigned char buffer[BUFFER_SIZE];
  auto ev = reinterpret_cast<Event*>(event.udata);
  auto readSize = read((FileDescriptor)event.ident, buffer, sizeof(buffer));
  auto& receiveBuffer = ev->connection->getReceiveStorage();

  if (readSize == -1)
  {
    ev->connection->closeConnection();
  }
  else
  {
    ev->connection->getReceiveStorage().append(buffer, readSize);
  }
}

// send data from response send buffer
void WS::handleSocketSend(struct kevent& event)
{
  auto ev = reinterpret_cast<Event*>(event.udata);
  auto& buffer = ev->connection->getReadFileStorage();
  auto sendSize = send((FileDescriptor)event.ident, buffer.data(), buffer.size(), MSG_DONTWAIT);
  // FIXME: buffer.empty() || !buffer -> ERROR?
  if (sendSize == -1)
  {
    ev->connection->closeConnection();
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
  ssize_t readSize = read((FileDescriptor)event.ident, buffer, sizeof(buffer));
  auto& readBuffer = ev->connection->getReadFileStorage();

  if (readSize == -1)
  {
    ev->connection->closeConnection();
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
  auto buffer = ev->connection->getRequest().getBody();
  auto writeSize = write(event.ident, buffer.data(), buffer.size());
  // get body buffer from request

  if (writeSize == -1)
  {
    ev->connection->closeConnection();
  }
  else
  {
    buffer.pop(writeSize);
  }
}
