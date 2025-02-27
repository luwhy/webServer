#include "TcpServer.h"
#include "base/Logging.h"
#include "sylar/log.h"
#include "Acceptor.h"
#include "SocketsOps.h"
#include <functional>
#include <stdio.h>
#include "Logging.h"
namespace webs
{

    TcpServer::TcpServer(EventLoop *loop, const InetAddress &listenAddr) : loop_(CHECK_NOTNULL(loop)),
                                                                           name_(listenAddr.toHostPort()),
                                                                           acceptor_(new Acceptor(loop, listenAddr)),
                                                                           started_(false),
                                                                           nextConnId_(1)

    {
        acceptor_->setNewConnectionCallback(std::bind(&TcpServer::newConnection, this, std::placeholders::_1, std::placeholders::_2));
    }
    TcpServer::~TcpServer()
    {
    }
    void TcpServer::start()
    {
        if (!started_)
        {
            started_ = true;
        }
        if (!acceptor_->listenning())
        {
            // bind里面这个对象，不能施展智能指针
            loop_->runInloop(std::bind(&Acceptor::listen, acceptor_.get()));
        }
    }
    void TcpServer::newConnection(int sockfd, const InetAddress &peerAddr)
    {
        loop_->assertInLoopThread();
        char buf[32];
        snprintf(buf, sizeof buf, "#%d", nextConnId_);
        ++nextConnId_;
        std::string connName = name_ + buf;
        SYLAR_LOG_INFO(g_logger_src) << "TcpServer::newConnection [" << name_
                                     << "] - new connection [" << connName
                                     << "] from " << peerAddr.toHostPort();

        InetAddress localAddr(sockets::getLocalAddr(sockfd));

        TcpConnectionPtr conn(new TcpConnection(loop_, connName, sockfd, localAddr, peerAddr));
        connections_[connName] = conn;
        conn->setConnectionCallback(this->connectionCallback_);
        conn->setMessageCallback(this->messageCallbacak_);
        conn->connectEstablished();
    }
}