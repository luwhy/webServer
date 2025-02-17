#include "EventLoop.h"
#include "base/Logging.h"
#include <poll.h>
#include <iostream>
namespace webs
{
    __thread EventLoop *t_loopInThisThread = nullptr;
    sylar::Logger::ptr g_logger = SYLAR_LOG_NAME("system");
    EventLoop::EventLoop() : m_threadID(muduo::CurrentThread::tid())
    {
        SYLAR_LOG_INFO(g_logger) << "EventLoop created " << this << " in thread " << m_threadID;
        this->m_looping = false;
        if (t_loopInThisThread)
        {
            SYLAR_LOG_ERROR(g_logger) << "Another EventLoop " << t_loopInThisThread << " exists in this thread " << m_threadID;
        }
        else
        {
            // 只是指针指向该地址
            t_loopInThisThread = this;
        }
    }

    EventLoop::~EventLoop()
    {
        assert(!m_looping);
        t_loopInThisThread = nullptr;
    }

    void EventLoop::loop()
    {
        assert(!m_looping);
        assertInLoopThread();
        m_looping = true;
        poll(NULL, 0, 5 * 1000);
        SYLAR_LOG_INFO(g_logger) << "EventLoop " << this << " stop stopping";
        m_looping = false;
    }

    void EventLoop::assertInLoopThread()
    {
        if (!isInLoopThread())
        {
            abortNotInLoopThread();
        }
    }

    void EventLoop::abortNotInLoopThread()
    {
    }
    EventLoop *EventLoop::getEventLoopOfCurrentThread()
    {
        return t_loopInThisThread;
    }
}