#include "EventLoop.h"
#include "base/Logging.h"
#include "Channel.h"
#include <poll.h>
#include <iostream>
#include "Poller.h"
#include "TimerQueue.h"
namespace webs
{
    __thread EventLoop *t_loopInThisThread = nullptr;
    sylar::Logger::ptr g_logger = SYLAR_LOG_NAME("system");
    const int kPollTimeMs = 10000;
    EventLoop::EventLoop() : threadId_(muduo::CurrentThread::tid()), poller_(new Poller(this)), timerQueue_(new TimerQueue(this))
    {
        SYLAR_LOG_INFO(g_logger) << "EventLoop created " << this << " in thread " << threadId_;
        this->looping_.store(false);
        this->quit_.store(false);

        if (t_loopInThisThread)
        {
            SYLAR_LOG_ERROR(g_logger) << "Another EventLoop " << t_loopInThisThread << " exists in this thread " << threadId_;
        }
        else
        {
            // 只是指针指向该地址
            t_loopInThisThread = this;
        }
    }

    EventLoop::~EventLoop()
    {
        assert(!looping_);
        t_loopInThisThread = nullptr;
    }

    // 调用poller::poll()获得当前活动事件的Channel列表，然后依次调用每个Channel的handleEvent()函数
    void EventLoop::loop()
    {
        assert(!looping_);
        assertInLoopThread();
        looping_.store(true);
        quit_.store(false);
        while (!quit_.load())
        {
            // 清空activChannels_
            activChannels_.clear();
            // poll获取当前活跃事件，填充入activChannels_
            poller_->poll(kPollTimeMs, &activChannels_);
            // 处理事件
            for (ChannelList::iterator it = activChannels_.begin(); it != activChannels_.end(); ++it)
            {
                it->get()->handleEvent();
            }
        }

        SYLAR_LOG_INFO(g_logger) << "EventLoop " << this << " stop stopping";
        looping_ = false;
    }

    void EventLoop::quit()
    {
        quit_.store(true);
    }

    TimerId EventLoop::runAt(const Timestamp &time, const TimerCallback &cb)
    {
        return timerQueue_->addTimer(cb, time, 0.0);
    }

    TimerId EventLoop::runAfter(double delay, const TimerCallback &cb)
    {
        Timestamp time(addTime(Timestamp::now(), delay));
        return runAt(time, cb);
    }

    TimerId EventLoop::runEvery(double interval, const TimerCallback &cb)
    {
        Timestamp time(addTime(Timestamp::now(), interval));
        return timerQueue_->addTimer(cb, time, interval);
    }

    void EventLoop::assertInLoopThread()
    {
        if (!isInLoopThread())
        {
            abortNotInLoopThread();
        }
    }

    void EventLoop::updateChannel(std::shared_ptr<Channel> channel)
    {
        assert(channel->ownerLoop() == this);
        assertInLoopThread();
        poller_->updateChannel(channel);
    }

    void EventLoop::abortNotInLoopThread()
    {
    }
    EventLoop *EventLoop::getEventLoopOfCurrentThread()
    {
        return t_loopInThisThread;
    }
}