//
// Created by Sungjun Park on 2022/12/11.
//

#ifndef EVENT_HPP
#define EVENT_HPP

#include "Connection.hpp"
namespace WS
{
    enum EventType
    {
        EV_TYPE_READ_FILE,
        EV_TYPE_WRITE_FILE,
        EV_TYPE_RECEIVE_SOCKET,
        EV_TYPE_SEND_SOCKET
    };
    typedef struct Event
    {
        EventType type;
        Connection* connection;
        Event(EventType type_, Connection* connection_): type(type_), connection(connection_){};
    } Event;

    void handleEvent(const Event& event);
}

#endif //EVENT_HPP
