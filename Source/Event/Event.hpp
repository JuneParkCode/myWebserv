//
// Created by Sungjun Park on 2022/12/11.
//

#ifndef EVENT_HPP
#define EVENT_HPP

#include <netinet/in.h>
#include <functional>
#include <sys/event.h>

namespace WS
{
    class Connection;
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
        std::function<void(struct kevent&)> handler;
        Event() = default;
        Event(EventType type_, Connection* connection_, std::function<void(struct kevent&)> handler_);
    } Event;
}

#endif //EVENT_HPP
