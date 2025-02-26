#include "Acceptor.h"
#include "EventLoop.h"
#include "InetAddress.h"
#include "SocketsOps.h"
#include <functional>

namespace webs
{
    Acceptor::Acceptor(EventLoop *loop, const InetAddress &listenAddr) : loop_(loop),
                                                                         acceptSocket_(Socket(sockets::createNonBlockingOrDie())),
                                                                         acceptChannel_(loop_, acceptSocket_.fd()),
                                                                         listenning_(false)
    {
    }

    void Acceptor::listen()
    {
    }

    void Acceptor::hanleRead()
    {
    }
}