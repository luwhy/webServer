#include "Acceptor.h"
#include "EventLoop.h"
#include "InetAddress.h"
#include <functional>

namespace webs
{
    Acceptor::Acceptor(EventLoop *loop, const InetAddress &listenAddr) : loop_(loop)
    {
    }

    void Acceptor::hanleRead()
    {
    }
}