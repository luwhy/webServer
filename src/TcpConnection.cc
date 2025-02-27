#include "TcpConnection.h"
#include "sylar/log.h"
#include "base/Logging.h"
#include "Channel.h"
#include "EventLoop.h"
#include "Socket.h"

#include <errno.h>
#include <stdio.h>

#include <functional>
#include "Logging.h"
namespace webs
{
    // sylar::Logger::ptr logger_t = SYLAR_LOG_NAME("system");
    TcpConnection::TcpConnection(EventLoop *loop, const std::string &name, int sockfd, const InetAddress &localAddr, const InetAddress &peerAddr) : loop_(CHECK_NOTNULL(loop)),
                                                                                                                                                    name_(name),
                                                                                                                                                    state_(kConnecting),
                                                                                                                                                    socket_(new Socket(sockfd)),
                                                                                                                                                    channel_(new Channel(loop, sockfd)),
                                                                                                                                                    localAddr_(localAddr),
                                                                                                                                                    peerAddr_(peerAddr)
    {
        SYLAR_LOG_DEBUG(g_logger_src) << "TcpConnection::ctor[" << name_ << "] at " << this
                                      << " fd=" << sockfd;
        channel_->setReadCallback(std::bind(&TcpConnection::handleRead, this));
    }

    TcpConnection::~TcpConnection()
    {
        SYLAR_LOG_DEBUG(g_logger_src) << "TcpConnection::dtor[" << name_ << "] at " << this
                                      << " fd=" << channel_->fd();
    }

    void TcpConnection::connectEstablished()
    {
        this->loop_->assertInLoopThread();
        assert(state_ == kConnecting);
        this->setState(kConnecting);
        connectionCallback_(shared_from_this());
    }

    void TcpConnection::connectDestoried()
    {
        this->loop_->assertInLoopThread();
        assert(state_ == kConnecting);
        this->setState(kDisconnected);
        channel_->disableAll();
        connectionCallback_(shared_from_this());
        loop_->removeChannel(channel_.get());
    }

    /**
     * @brief read回调
     *
     */
    void TcpConnection::handleRead()
    {
        char buf[UINT16_MAX];
        ssize_t n = ::read(channel_->fd(), buf, sizeof buf);
        messageCallback_(shared_from_this(), buf, n);
    }
}