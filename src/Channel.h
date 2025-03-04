#ifndef __CHANNEL_H
#define __CHANNEL_H
#include <functional>
#include "sylar/log.h"
#include "EventLoop.h"
#include "base/Timestamp.h"
namespace muduo
{
    class Channel : public std::enable_shared_from_this<Channel>
    {
    public:
        typedef std::function<void()> EventCallback;
        typedef std::function<void(muduo::Timestamp)> ReadEventCallback;
        Channel(EventLoop *loop, int fd);
        Channel(Channel &c) = delete;
        ~Channel();

        void handleEvent(muduo::Timestamp receiveTime);
        void setReadCallback(const ReadEventCallback &cb)
        {
            this->readCallback_ = cb;
        }
        void setWriteCallback(const EventCallback &cb) { this->writeCallback_ = cb; }
        void setErrorCallback(const EventCallback &cb) { this->errorCallback_ = cb; }
        void setCloseCallback(const EventCallback &cb) { this->closeCallback_ = cb; }

        int fd() const { return fd_; }
        int events() const { return events_; }
        int index() const { return index_; }
        // 实际发生的事件
        void set_revents(int revt) { this->revents_ = revt; }
        void set_index(int index)
        {
            this->index_ = index;
        }
        bool isNoneEvent() const { return events_ == kNoneEvent; }
        void enableReading()
        {
            events_ |= kReadEvent;
            update();
        }
        EventLoop *ownerLoop()
        {
            return loop_;
        }
        void disableAll()
        {
            events_ = kNoneEvent;
            update();
        }

    private:
        void update();
        static const int kNoneEvent;
        static const int kReadEvent;
        static const int kWriteEvent;

        EventLoop *loop_;
        const int fd_;
        int events_;
        int revents_;
        int index_; // used by poller,记录在pollfds_数组中的下标
        bool eventHandling_;
        ReadEventCallback readCallback_;
        EventCallback writeCallback_;
        EventCallback errorCallback_;
        EventCallback closeCallback_;
    };
}
#endif