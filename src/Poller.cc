#include "Poller.h"
#include <poll.h>
#include <assert.h>
#include "Channel.h"
#include "sylar/log.h"
namespace webs
{
    sylar::Logger::ptr g_logger_p = SYLAR_LOG_NAME("system");
    Poller::Poller(EventLoop *loop) : ownerLoop_(loop)
    {
    }
    Poller::~Poller()
    {
    }
    // 调用poll获得当前活动的IO事件然后填入activatechannel中并返回时间
    muduo::Timestamp Poller::poll(int timeoutMs, ChannelList *activeChannels)
    {
        // 成功时，poll() 返回结构体中 revents 域不为 0 的文件描述符个数；如果在超时前没有任何事件发生，poll()返回 0；失败返回-1
        // fd：每一个 pollfd 结构体指定了一个被监视的文件描述符，可以传递多个结构体，指示 poll() 监视多个文件描述符。
        for (auto i : pollfds_)
        {
            SYLAR_LOG_DEBUG(g_logger_p) << i.fd;
        }
        int numEvents = ::poll(&(*pollfds_.begin()), pollfds_.size(), timeoutMs);
        // 也可以这样写，vector.data()获取首元素指针
        // int numEvents = ::poll(pollfds_.data(), pollfds_.size(), timeoutMs);
        muduo::Timestamp now(muduo::Timestamp::now());
        if (numEvents > 0)
        {
            SYLAR_LOG_INFO(g_logger_p) << numEvents << " events happended";
            fillActivateChannels(numEvents, activeChannels);
        }
        else if (numEvents == 0)
        {
            SYLAR_LOG_INFO(g_logger_p) << " nothing happend";
        }
        else
        {
            SYLAR_LOG_INFO(g_logger_p) << " poller::poll()";
        }
        return now;
    }
    void Poller::updateChannel(std::shared_ptr<Channel> channel)
    {
        assertInLoopThread();
        SYLAR_LOG_INFO(g_logger_p) << "fd= " << channel->fd() << " events= " << channel->events();
        if (channel->index() < 0)
        {
            // 这里不在channels_中,新增
            assert(channels_.find(channel->fd()) == channels_.end());
            struct pollfd pfd;
            pfd.fd = channel->fd();
            pfd.events = static_cast<short>(channel->events());
            // pfd.revents = 0;
            pollfds_.push_back(pfd);
            int idx = static_cast<int>(pollfds_.size()) - 1;
            channel->set_index(idx);
            channels_[pfd.fd] = channel;
        }
        else
        {
            // update
            assert(channels_.find(channel->fd()) != channels_.end());
            assert(channels_[channel->fd()] == channel);
            int idx = channel->index();
            assert(0 <= idx && idx < static_cast<int>(pollfds_.size()));
            struct pollfd &pfd = pollfds_[idx];
            assert(pfd.fd == channel->fd() || pfd.fd == -1);
            pfd.events = static_cast<short>(channel->events());
            // pfd.revents = 0;
            if (channel->isNoneEvent())
            {
                pfd.fd = -1;
            }
        }
    }
    // 遍历pollfds_,找出有活动的fd，把对应的channel填入activeChannels
    void Poller::fillActivateChannels(int numEvents, ChannelList *activeChannels) const
    {
        for (PollFdList::const_iterator pfd = pollfds_.begin(); pfd != pollfds_.end() && numEvents > 0; ++pfd)
        {
            if (pfd->revents > 0)
            {
                --numEvents;
                ChannelMap::const_iterator ch = channels_.find(pfd->fd);
                assert(ch != channels_.end());
                std::shared_ptr<Channel> channel = ch->second;
                assert(channel->fd() == pfd->fd);
                channel->set_revents(pfd->revents);
                activeChannels->push_back(channel);
            }
        }
    }
}