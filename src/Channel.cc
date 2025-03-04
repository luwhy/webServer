#include "Channel.h"
#include "EventLoop.h"
#include <poll.h>
#include "sylar/log.h"
namespace webs
{
    sylar::Logger::ptr g_logger_c = SYLAR_LOG_NAME("system");
    const int Channel::kNoneEvent = 0;
    const int Channel::kReadEvent = POLLIN | POLLPRI;
    webs::Channel::Channel(EventLoop *loop, int fd) : loop_(loop),
                                                      fd_(fd),
                                                      events_(0),
                                                      revents_(0),
                                                      index_(-1),
                                                      eventHandling_(false)
    {
    }

    Channel::~Channel()
    {
        assert(!eventHandling_);
    }
    // 根据revents_的值分别调用不同回调
    void Channel::handleEvent(muduo::Timestamp receiveTime)
    {
        eventHandling_ = true;
        if (revents_ & POLLNVAL)
            SYLAR_LOG_WARN(g_logger_c) << "Channel::handle_event() POLLNVAL";
        if ((revents_ & POLLHUP) && !(revents_ & POLLIN))
        {
            SYLAR_LOG_WARN(g_logger_c) << "Channel::handle_event() POLLHUP";
            if (closeCallback_)
                closeCallback_();
        }
        if (revents_ & (POLLERR | POLLNVAL))
        {
            if (errorCallback_)
            {
                errorCallback_();
            }
        }
        if (revents_ & (POLLIN | POLLPRI | POLLRDHUP))
        {
            if (readCallback_)
            {
                readCallback_(receiveTime);
            }
        }
        if (revents_ & POLLOUT)
        {
            if (writeCallback_)
            {
                writeCallback_();
            }
        }
        eventHandling_ = false;
    }
    void Channel::update()
    {
        loop_->updateChannel(shared_from_this());
    }
}
