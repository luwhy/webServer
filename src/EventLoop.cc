#include "EventLoop.h"
#include "base/Logging.h"
#include "Channel.h"
#include <poll.h>
#include <iostream>
#include "Poller.h"
#include "TimerQueue.h"
#include <sys/eventfd.h>

#include "Logging.h"
namespace webs
{
    static int createEventfd()
    {
        // eventfd()函数可以创建一个被用户空间应用程序作为“等待/通知”机制使用的eventfd对象，或被内核用于通知用户空间应用程序事件消息。
        // 可以用于select/epoll监听以达到异步的目的，避免在没有事件时发生阻塞
        int evtfd = ::eventfd(0, 0);
        if (evtfd < 0)
        {
            abort();
        }
        return evtfd;
    }

    __thread EventLoop *t_loopInThisThread = nullptr;

    const int kPollTimeMs = 1000;
    EventLoop::EventLoop() : threadId_(muduo::CurrentThread::tid()),
                             poller_(new Poller(this)),
                             timerQueue_(new TimerQueue(this)),
                             wakeupFd_(createEventfd()),
                             wakeupChannel_(new Channel(this, wakeupFd_))
    {
        SYLAR_LOG_INFO(g_logger_src) << "EventLoop created " << this << " in thread " << threadId_;
        this->looping_.store(false);
        this->quit_.store(false);
        this->callingPendingFunctors_.store(false);
        if (t_loopInThisThread)
        {
            SYLAR_LOG_ERROR(g_logger_src) << "Another EventLoop " << t_loopInThisThread << " exists in this thread " << threadId_;
        }
        else
        {
            // 只是指针指向该地址
            t_loopInThisThread = this;
        }
        wakeupChannel_->setReadCallback(std::bind(&EventLoop::handleRead, this));
        wakeupChannel_->enableReading();
    }

    EventLoop::~EventLoop()
    {
        assert(!looping_);
        ::close(wakeupFd_);
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
            pollReturnTime_ = poller_->poll(kPollTimeMs, &activChannels_);
            // 处理事件
            for (ChannelList::iterator it = activChannels_.begin(); it != activChannels_.end(); ++it)
            {
                it->get()->handleEvent();
            }
            dopendingFunctors();
        }

        SYLAR_LOG_INFO(g_logger_src) << "EventLoop " << this << " stop stopping";
        looping_ = false;
    }

    void EventLoop::quit()
    {
        quit_.store(true);
        if (!isInLoopThread())
        {
            wakeup();
        }
    }

    void EventLoop::runInloop(const Functor &cb)
    {
        if (isInLoopThread())
        {
            cb();
        }
        else
        {
            queueInLoop(cb);
        }
    }

    void EventLoop::queueInLoop(const Functor &cb)
    {
        {
            MutexLockGuard lock(mutex_);
            pendingFunctors_.push_back(cb);
        }
        // 不在此线程或者callingPendingFunctors_为true时，向eventfd写入1
        if (!isInLoopThread() || callingPendingFunctors_)
        {
            wakeup();
        }
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

    void EventLoop::wakeup()
    {
        uint64_t one = 1;
        ssize_t n = write(wakeupFd_, &one, sizeof one);
        if (n != sizeof one)
        {
            LOG_ERROR << "EventLoop::handleRead() writes " << n << " bytes instead of 8";
        }
    }

    void EventLoop::abortNotInLoopThread()
    {
        SYLAR_LOG_INFO(g_logger_src) << "EventLoop::abortNotInLoopThread - EventLoop " << this
                                     << " was created in threadId_ = " << threadId_
                                     << ", current thread id = " << CurrentThread::tid();
    }

    void EventLoop::handleRead()
    {
        uint64_t one = 1;
        ssize_t n = ::read(wakeupFd_, &one, sizeof one);
        if (n != sizeof one)
        {
            SYLAR_LOG_ERROR(g_logger_src) << "EventLoop::handleRead() reads " << n << " bytes instead of 8";
        }
    }

    void EventLoop::dopendingFunctors()
    {
        std::vector<Functor> functors;
        callingPendingFunctors_ = true;
        {
            // 互换内容，减少临界区长度
            MutexLockGuard lock(mutex_);
            functors.swap(pendingFunctors_);
        }
        for (size_t i = 0; i < functors.size(); ++i)
        {
            functors[i]();
        }
        callingPendingFunctors_ = false;
    }

    EventLoop *EventLoop::getEventLoopOfCurrentThread()
    {
        return t_loopInThisThread;
    }
}