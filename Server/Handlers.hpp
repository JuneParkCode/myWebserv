#ifndef HANDLERS_HPP
#define HANDLERS_HPP

#include "Event.hpp"
#include <sys/event.h>

namespace WS
{
    void handleEvent(struct kevent& event);
    void handleSocketReceive(struct kevent& event);
    void handleSocketSend(struct kevent& event);
    void handleFileRead(struct kevent& event);
    void handleFileWrite(struct kevent& event);
    void handleAcceptConnection(struct kevent& event);
}

#endif //HANDLERS_HPP
