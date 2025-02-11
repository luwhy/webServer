#include "EventLoop.h"
#include "Thread.h"
namespace webs
{
    __thread EventLoop *t_loopInThisThread = NULL;
    EventLoop::EventLoop() : m_threadID(CurrentThread::tid())
    {
        this->m_looping = false;
    }

    EventLoop::~EventLoop()
    {
    }

    void EventLoop::loop()
    {
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
}