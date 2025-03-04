#ifndef EVENT_LOOP_THREAD_H
#define EVENT_LOOP_THREAD_H
#include "base/Thread.h"
#include "base/Condition.h"
#include "base/Mutex.h"

namespace muduo
{
    class EventLoop;
    class EventLoopThread
    {
    public:
        EventLoopThread();
        ~EventLoopThread();
        EventLoopThread(const EventLoopThread &e) = delete;
        EventLoop *startLoop();

    private:
        void threadFunc();

    private:
        EventLoop *loop_;
        bool exiting_;
        muduo::Thread thread_;
        muduo::MutexLock mutex_;
        muduo::Condition cond_;
    };
}
#endif