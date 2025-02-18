#include "Poller.h"
#include <poll.h>
#include <assert.h>
#include "Channel.h"
namespace webs
{
    Poller::Poller(std::shared_ptr<EventLoop> loop) : ownerLoop_(loop)
    {
    }
    Poller::~Poller()
    {
    }
    muduo::Timestamp Poller::poll(int timeoutMs, std::shared_ptr<ChannelList> activeChannels)
    {
        int numEvents = ::poll(&(*pollfds_.begin()), pollfds_.size(), timeoutMs);
    }
    void Poller::updateChannel(std::shared_ptr<Channel> channel)
    {
    }
    void Poller::fillActivateChannels(int numEvents, std::shared_ptr<ChannelList> activeChannels) const
    {
    }
}