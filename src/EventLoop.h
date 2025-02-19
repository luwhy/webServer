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

        void quit();

        void assertInLoopThread();

        bool isInLoopThread() const { return true; }

        void updateChannel(std::shared_ptr<Channel> channel);

    private:
        void abortNotInLoopThread();

        typedef std::vector<std::shared_ptr<Channel>> ChannelList;
        std::atomic<bool> looping_;
        std::atomic<bool> quit_;

        const pid_t threadId_;

        std::unique_ptr<Poller> poller_;
        ChannelList activChannels_;
    };
}

#endif