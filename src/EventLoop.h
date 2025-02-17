#ifndef EVENT_LOOP_H
#define EVENT_LOOP_H

#include <pthread.h>
#include <atomic>
#include "base/CurrentThread.h"
#include "sylar/log.h"
namespace webs
{
    class EventLoop
    {
    public:
        EventLoop();

        EventLoop(EventLoop &e) = delete;

        static EventLoop *getEventLoopOfCurrentThread();

        ~EventLoop();

        void loop();

        void assertInLoopThread();

        bool isInLoopThread() const { return true; }

    private:
        void abortNotInLoopThread();

        std::atomic<bool> m_looping;

        const pid_t m_threadID;
    };
}

#endif