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
        Poller(std::shared_ptr<EventLoop> loop);
        Poller(Poller &poller) = delete;
        ~Poller();
        muduo::Timestamp poll(int timeoutMs, std::shared_ptr<ChannelList> activeChannels);

        void updateChannel(std::shared_ptr<Channel> channel);

        void assertInLoopThread() { ownerLoop_->assertInLoopThread(); }

    private:
        typedef std::vector<struct pollfd> PollFdList;
        typedef std::map<int, std::shared_ptr<Channel>> ChannelMap;
        void fillActivateChannels(int numEvents, std::shared_ptr<ChannelList> activeChannels) const;

        std::shared_ptr<EventLoop> ownerLoop_;
        PollFdList pollfds_;
        ChannelList channels_;
    };
}
#endif