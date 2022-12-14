//
// Created by Sungjun Park on 2022/12/11.
//

#include "Event.hpp"
#include "Connection.hpp"

WS::Event::Event(WS::EventType type_, WS::Connection* connection_): type(type_), connection(connection_)
{
}
