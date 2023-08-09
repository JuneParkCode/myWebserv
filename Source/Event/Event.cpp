//
// Created by Sungjun Park on 2022/12/11.
//

#include "Event.hpp"
#include "Connection.hpp"
#include "VirtualServer.hpp"

WS::Event::Event(WS::EventType type_, WS::Connection* connection_, WS::VirtualServer* server_, std::function<void(struct kevent&)> handler_):
  type(type_), connection(connection_), server(server_), handler(handler_)
{
}
