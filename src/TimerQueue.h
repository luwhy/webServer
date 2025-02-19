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
        typedef std::pair<Timestamp, Timer *> Entry;
        typedef std::set<Entry> TimerList;

        void handleRead();

        std::vector<Entry> getExpired(Timestamp now);

        void reset(const std::vector<Entry> &expired, Timestamp now);

        bool insert(Timer *timer);

        EventLoop *loop_;

        const int timerfd_;

        Channel timerfdChannel_;

        TimerList timers_;
    };
}
#endif