#ifndef __POLLER_H
#define __POLLER_H
#include <memory>
#include <vector>
#include "EventLoop.h"
#include "base/Timestamp.h"
#include <map>
struct pollfd;
namespace webs
{
    class Channel;
    class EventLoop;

    class Poller
    {
    public:
        typedef std::vector<std::shared_ptr<Channel>> ChannelList;
        Poller(EventLoop *loop);
        Poller(Poller &poller) = delete;
        ~Poller();
        muduo::Timestamp poll(int timeoutMs, ChannelList *activeChannels);

        void updateChannel(std::shared_ptr<Channel> channel);

        void assertInLoopThread() { ownerLoop_->assertInLoopThread(); }

    private:
        // pollfd有一个fd,一个fd记录对应一个channel，一个channel又记录自己在pollfds_中的下标
        typedef std::vector<struct pollfd> PollFdList;
        typedef std::map<int, std::shared_ptr<Channel>> ChannelMap;
        void fillActivateChannels(int numEvents, ChannelList *activeChannels) const;

        EventLoop *ownerLoop_;
        PollFdList pollfds_;
        ChannelMap channels_;
    };
}
#endif