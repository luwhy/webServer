#ifndef EVENT_LOOP_H
#define EVENT_LOOP_H

#include <pthread.h>
#include <atomic>
#include "base/CurrentThread.h"
#include "sylar/log.h"
#include <memory>
#include "TimerId.h"
namespace webs
{
    class Channel;
    class Poller;
    class TimerQueue;
    class EventLoop
    {
    public:
        EventLoop();

        EventLoop(EventLoop &e) = delete;

        static EventLoop *getEventLoopOfCurrentThread();

        ~EventLoop();

        void loop();

        void quit();

        Timestamp pollReturnTime() const { return pollReturnTime_; }

        /**
         * @brief 在某个时间运行cb
         *
         * @param time
         * @param cb
         * @return TimerId
         */
        TimerId runAt(const Timestamp &time, const TimerCallback &cb);

        /**
         * @brief 在某个延迟时间运行cb
         *
         * @param delay
         * @param cb
         * @return TimerId
         */
        TimerId runAfter(double delay, const TimerCallback &cb);

        /**
         * @brief 在每个间隔后运行
         *
         * @param interval
         * @param cb
         * @return TimerId
         */
        TimerId runEvery(double interval, const TimerCallback &cb);

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
        std::unique_ptr<TimerQueue> timerQueue_;
        Timestamp pollReturnTime_;
    };
}

#endif