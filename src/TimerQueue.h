#ifndef TIMER__QUEUE__H
#define TIMER__QUEUE__H

#include "base/Timestamp.h"
#include "Callbacks.h"
#include "Channel.h"
#include <set>
#include <vector>
using namespace muduo;
namespace webs
{
    class Timer;
    class EventLoop;
    class TimerId;
    class TimerQueue
    {
        TimerQueue(EventLoop *loop);
        TimerQueue(const TimerQueue &t) = delete;
        ~TimerQueue();
        TimerId addTimer(const TimerCallback &cb, Timestamp when, double interval);

    private:
        // 一组pair<Timestamp, Timer *>
        typedef std::pair<Timestamp, Timer *> Entry;

        // entry的set
        typedef std::set<Entry> TimerList;

        void handleRead();

        // 获取过期的timer
        std::vector<Entry> getExpired(Timestamp now);

        void reset(const std::vector<Entry> &expired, Timestamp now);

        // 看看后续是否要unique_ptr
        bool insert(Timer *timer);

        EventLoop *loop_;

        const int timerfd_;

        Channel timerfdChannel_;

        TimerList timers_;
    };
}
#endif