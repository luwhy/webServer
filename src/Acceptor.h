#ifndef _ACCEPTOR__H_
#define _ACCEPTOR__H_
#include <functional>
#include "Channel.h"
#include "Socket.h"
namespace muduo
{
    class EventLoop;
    class InetAddress;
    class Acceptor
    {
    public:
        typedef std::function<void(int sockfd, const InetAddress &)> NewConnnectionCallback;
        Acceptor(EventLoop *loop, const InetAddress &listenAddr);
        Acceptor(const Acceptor &a) = delete;

        /**
         * @brief 设置连接回调
         *
         * @param cb
         */
        void setNewConnectionCallback(const NewConnnectionCallback &cb)
        {
            newConnectionCallback_ = cb;
        }

        bool listenning() const { return listenning_; }

        void listen();

    private:
        void handleRead();

        EventLoop *loop_;

        Socket acceptSocket_;

        std::shared_ptr<Channel> acceptChannel_;

        NewConnnectionCallback newConnectionCallback_;

        bool listenning_;
    };

}
#endif