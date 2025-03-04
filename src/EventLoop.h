#ifndef EVENT_LOOP_H
#define EVENT_LOOP_H

#include <pthread.h>
#include <atomic>
#include "base/CurrentThread.h"
#include "sylar/log.h"
#include <memory>
#include "TimerId.h"
#include "base/Thread.h"
#include "base/Mutex.h"

namespace muduo
{
    class Channel;
    class Poller;
    class TimerQueue;
    class EventLoop
    {
    public:
        typedef std::function<void()> Functor;
        EventLoop();

        EventLoop(EventLoop &e) = delete;

        static EventLoop *getEventLoopOfCurrentThread();

        ~EventLoop();

        void loop();

        void quit();

        Timestamp pollReturnTime() const { return pollReturnTime_; }

        /**
         * @brief 如果执行的是本io线程，则立刻执行cb，否则放入队列中
         *
         *
         * @param cb
         */
        void runInloop(const Functor &cb);

        /**
         * @brief 供多线程其他情况使用
         *
         * @param cb
         */
        void queueInLoop(const Functor &cb);

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

        /**
         * @brief threadId_是否和当前线程相等
         *
         * @return true
         * @return false
         */
        bool isInLoopThread() const { return threadId_ == CurrentThread::tid(); }

        void updateChannel(std::shared_ptr<Channel> channel);

        void removeChannel(std::shared_ptr<Channel> channel);

        void wakeup();

    private:
        void abortNotInLoopThread();

        /**
         * @brief 处理wakeup事件
         *
         */
        void handleRead();

        void dopendingFunctors();

        typedef std::vector<std::shared_ptr<Channel>> ChannelList;
        std::atomic<bool> looping_;
        std::atomic<bool> quit_;
        std::atomic<bool> callingPendingFunctors_;

        const pid_t threadId_;

        std::unique_ptr<Poller> poller_;
        std::unique_ptr<TimerQueue> timerQueue_;
        Timestamp pollReturnTime_;

        ChannelList activChannels_;
        // eventfd 对应的文件内容是一个 8 字节的数字，这个数字是 read/write 操作维护的计数。
        // 首先，write 的时候，累加计数，read 的时候读取计数，并且清零。
        // 返回值是读取或写入的字节数
        int wakeupFd_;
        std::shared_ptr<Channel> wakeupChannel_;
        MutexLock mutex_;
        std::vector<std::function<void()>> pendingFunctors_;
    };
}

#endif