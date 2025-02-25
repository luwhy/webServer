#include "TimerQueue.h"
#include "Timer.h"
#include "TimerId.h"
#include <sys/timerfd.h>
#include "sylar/log.h"
namespace webs
{
    sylar::Logger::ptr g_logger_g = SYLAR_LOG_NAME("system");
    namespace detail
    {
        int createTimerfd()
        {
            // 返回一个文件描述符，到期时产生文件描述符上的可读事件
            int timerfd = ::timerfd_create(CLOCK_MONOTONIC,
                                           TFD_NONBLOCK | TFD_CLOEXEC);
            if (timerfd < 0)
            {
                SYLAR_LOG_ERROR(g_logger_g) << "Failed in timerfd_create";
            }
            return timerfd;
        }
        struct timespec howMuchTimeFromNow(Timestamp when)
        {
            int64_t microseconds = when.microSecondsSinceEpoch() - Timestamp::now().microSecondsSinceEpoch();
            if (microseconds < 100)
            {
                microseconds = 100;
            }
            struct timespec ts;
            ts.tv_sec = static_cast<time_t>(
                microseconds / Timestamp::kMicroSecondsPerSecond);
            ts.tv_nsec = static_cast<long>(
                (microseconds % Timestamp::kMicroSecondsPerSecond) * 1000);
            return ts;
        }
        // 重置timerfd
        void resetTimerfd(int timerfd, Timestamp expiration)
        {
            // structure for timer start values and intervals.
            struct itimerspec newValue;
            struct itimerspec oldValue;
            bzero(&newValue, sizeof newValue);
            bzero(&oldValue, sizeof oldValue);
            newValue.it_value = howMuchTimeFromNow(expiration);
        }
    }
    TimerQueue::TimerQueue(EventLoop *loop) : loop_(loop),
                                              timerfd_(detail::createTimerfd()),
                                              timerfdChannel_(loop, timerfd_),
                                              timers_()
    {
    }
    TimerQueue::~TimerQueue()
    {
        ::close(timerfd_);
        for (auto it = timers_.begin(); it != timers_.end(); ++it)
        {

            delete it->second;
        }
    }
    TimerId TimerQueue::addTimer(const TimerCallback &cb, Timestamp when, double interval)
    {

        Timer *timer = new Timer(cb, when, interval);
        this->loop_->runInloop(std::bind(&TimerQueue::addTimerInLoop, this, timer));
        return TimerId(timer);
    }
    void TimerQueue::addTimerInLoop(Timer *timer)
    {
        loop_->assertInLoopThread();
        bool earliestChanged = insert(timer);

        if (earliestChanged)
        {
            detail::resetTimerfd(timerfd_, timer->expiration());
        }
    }
    void TimerQueue::handleRead()
    {
        loop_->assertInLoopThread();
        Timestamp now(Timestamp::now());
        detail::resetTimerfd(timerfd_, now);
        std::vector<Entry> expired = getExpired(now);
        for (auto it = expired.begin(); it != expired.end(); it++)
        {
            it->second->run();
        }
        reset(expired, now);
    }

    std::vector<TimerQueue::Entry> TimerQueue::getExpired(Timestamp now)
    {
        std::vector<Entry> expired;
        // UINTPTR_MAX 该宏表示能够保存对象指针的无符号整数的最大值。
        // 然后这里的Timer* 由于未定义，所以往往不会调用
        Entry sentry = std::make_pair(now, reinterpret_cast<Timer *>(UINTPTR_MAX));
        // 二分查找一个有序数列，返回第一个大于等于x的数，如果没找到，返回末尾的迭代器位置
        TimerList::iterator it = timers_.lower_bound(sentry);
        // 返回的是第一个
        assert(it == timers_.end() || now < it->first);
        // 从timers_.begin()到it，然后倒序插入expired
        // 小的是曾经的内容，是过去式，也就是说过期了
        std::copy(timers_.begin(), it, back_inserter(expired));
        // 从timers中移除
        timers_.erase(timers_.begin(), it);
        return expired;
    }

    void TimerQueue::reset(const std::vector<Entry> &expired, Timestamp now)
    {
        Timestamp nextExpire;
        for (std::vector<Entry>::const_iterator it = expired.begin(); it != expired.end(); ++it)
        {
            // 是否重复操作
            if (it->second->repeat())
            {
                it->second->restart(now);
                insert(it->second);
            }
            else
            {
                delete it->second;
            }
        }
        if (!timers_.empty())
        {
            nextExpire = timers_.begin()->second->expiration();
        }
        if (nextExpire.valid())
        {
            detail::resetTimerfd(timerfd_, nextExpire);
        }
    }
    bool TimerQueue::insert(Timer *timer)
    {
        bool earliestChanged = false;
        Timestamp when = timer->expiration();
        TimerList::iterator it = timers_.begin();
        // 如果为空，或者when小于first
        if (it == timers_.end() || when < it->first)
        {
            // 当前是最早变化
            earliestChanged = true;
        }
        std::pair<TimerList::iterator, bool> result = timers_.insert(std::make_pair(when, timer));
        assert(result.second);
        return earliestChanged;
    }
}