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
    public:
        TimerQueue(EventLoop *loop);
        TimerQueue(const TimerQueue &t) = delete;
        ~TimerQueue();
        /**
         * @brief 如果interval大于0，则表示重复执行，只负责转发
         *
         *
         * @param cb
         * @param when
         * @param interval
         * @return TimerId
         */

        TimerId addTimer(const TimerCallback &cb, Timestamp when, double interval);

        /**
         * @brief负责执行修改定时器功能
         *
         * @param timer
         */
        void addTimerInLoop(Timer *timer);

    private:
        // 一组pair<Timestamp, Timer *>
        typedef std::pair<Timestamp, Timer *> Entry;

        // entry的set
        typedef std::set<Entry> TimerList;

        void handleRead();

        /**
         * @brief 获取过期entry
         *
         * @param now
         * @return std::vector<Entry>
         */
        std::vector<Entry> getExpired(Timestamp now);

        void reset(const std::vector<Entry> &expired, Timestamp now);

        // 看看后续是否要unique_ptr

        /**
         * @brief 插入timer
         *
         * @param timer
         * @return true
         * @return false
         */
        bool insert(Timer *timer);

        EventLoop *loop_;

        const int timerfd_;

        Channel timerfdChannel_;

        TimerList timers_;
    };
}
#endif