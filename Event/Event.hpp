//
// Created by Sungjun Park on 2022/12/11.
//

#ifndef EVENT_HPP
#define EVENT_HPP

#include "Connection.hpp"
#include <netinet/in.h>

namespace WS
{
    enum EventType
    {
        EV_TYPE_READ_FILE,
        EV_TYPE_WRITE_FILE,
        EV_TYPE_RECEIVE_SOCKET,
        EV_TYPE_SEND_SOCKET,
        EV_TYPE_ACCEPT_CONNECTION
    };
    typedef struct Event
    {
        EventType type;
        WS::Connection* connection;
        size_t threadNO;
        Event() = default;
        Event(EventType type_, Connection* connection_): type(type_), connection(connection_){};
    } Event;
}

#endif //EVENT_HPP
