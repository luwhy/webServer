#ifndef EVENT_LOOP_H
#define EVENT_LOOP_H

#include <pthread.h>
#include <atomic>
#include "base/CurrentThread.h"
#include "sylar/log.h"
#include <memory>
namespace webs
{
    class Channel;
    class Poller;
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

        void updateChannel(Channel *channel);

    private:
        void abortNotInLoopThread();

        std::atomic<bool> m_looping;

        const pid_t m_threadID;

        std::unique_ptr<Poller> poller_;
    };
}

#endif