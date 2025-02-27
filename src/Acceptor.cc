#include "Acceptor.h"
#include "EventLoop.h"
#include "InetAddress.h"
#include "SocketsOps.h"
#include <functional>

namespace webs
{
    Acceptor::Acceptor(EventLoop *loop, const InetAddress &listenAddr) : loop_(loop),
                                                                         acceptSocket_(sockets::createNonBlockingOrDie()),
                                                                         acceptChannel_(new Channel(loop_, acceptSocket_.fd())),
                                                                         listenning_(false)
    {
        acceptSocket_.setReuseAddr(true);
        acceptSocket_.bindAddress(listenAddr);
        acceptChannel_->setReadCallback(std::bind(&Acceptor::hanleRead, this));
    }

    void Acceptor::listen()
    {
        loop_->assertInLoopThread();
        listenning_ = true;
        acceptSocket_.listen();
        acceptChannel_->enableReading();
    }

    void Acceptor::hanleRead()
    {
        loop_->assertInLoopThread();
        InetAddress peerAddr(0);
        int connfd = acceptSocket_.accept(&peerAddr);
        if (connfd >= 0)
        {
            if (newConnectionCallback_)
            {
                newConnectionCallback_(connfd, peerAddr);
            }
            else
            {
                sockets::close(connfd);
            }
        }
    }
}