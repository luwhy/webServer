#ifndef __CHANNEL_H
#define __CHANNEL_H
#include <functional>
#include "sylar/log.h"
#include "EventLoop.h"

namespace webs
{
    class Channel
    {
    public:
        typedef std::function<void()> EventCallback;
        Channel(EventLoop *loop, int fd);
        Channel(Channel &c) = delete;

        void handleEvent();
        void setReadCallback(const EventCallback &cb) { this->readCallback_ = cb; }
        void setWriteCallback(const EventCallback &cb) { this->writeCallback_ = cb; }
        void setErrorCallback(const EventCallback &cb) { this->errorCallback_ = cb; }

        int fd() const { return fd_; }
        int events() const { return events_; }
        void set_revents(int revt) { this->revents_ = revt; }
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

    private:
        void update();
        static const int kNoneEvent;
        static const int kReadEvent;
        static const int kWriteEvent;

        EventLoop *loop_;
        const int fd_;
        int events_;
        int revents_;
        int index_; // used by poller

        EventCallback readCallback_;
        EventCallback writeCallback_;
        EventCallback errorCallback_;
        EventCallback closeCallback_;
    };
}
#endif