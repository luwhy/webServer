#ifndef CALLBACKS__H
#define CALLBACKS__H
#include <functional>
#include <memory>
#include "sylar/log.h"

namespace webs
{

    class TcpConnection;
    typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;
    typedef std::function<void()> TimerCallback;
    typedef std::function<void(const TcpConnectionPtr &)> ConnectionCallback;
    typedef std::function<void(const TcpConnectionPtr &,
                               const char *data,
                               ssize_t len)>
        MessageCallback;

    typedef std::function<void(const TcpConnectionPtr &)> CloseCallback;
}
#endif