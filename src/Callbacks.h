#ifndef CALLBACKS__H
#define CALLBACKS__H
#include <functional>
#include <memory>
#include "sylar/log.h"
#include "base/Timestamp.h"
namespace webs
{
    class Buffer;
    class TcpConnection;
    typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;
    typedef std::function<void()> TimerCallback;
    typedef std::function<void(const TcpConnectionPtr &)> ConnectionCallback;
    typedef std::function<void(const TcpConnectionPtr &,
                               Buffer *buf,
                               muduo::Timestamp)>
        MessageCallback;

    typedef std::function<void(const TcpConnectionPtr &)> CloseCallback;
}
#endif