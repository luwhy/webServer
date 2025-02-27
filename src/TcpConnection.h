#ifndef __TCP_CONNECTION__H__
#define __TCP_CONNECTION__H__
#include <memory>
#include <any>
#include "Callbacks.h"
#include "InetAddress.h"
namespace webs
{
    class Channel;
    class EventLoop;
    class Socket;
    class TcpConnection : public std::enable_shared_from_this<TcpConnection>
    {
    public:
        TcpConnection(EventLoop *loop, const std::string &name, int sockfd, const InetAddress &localAddr, const InetAddress &peerAddr);
        ~TcpConnection();
        TcpConnection(const TcpConnection &e) = delete;

        EventLoop *getLoop() const { return loop_; }

        const std::string &name() const { return name_; }

        const InetAddress &localAddr() { return localAddr_; }

        const InetAddress &peerAddr() { return peerAddr_; }

        bool connected() const { return state_ == kConnected; }

        void setConnectionCallback(const ConnectionCallback &cb)
        {
            connectionCallback_ = cb;
        }

        void setMessageCallback(const MessageCallback &cb)
        {
            messageCallback_ = cb;
        }

        /**
         * @brief 当tcp接收一个新的链接时，只应用一次
         *
         */
        void connectEstablished();

    private:
        enum StateE
        {
            kConnecting,
            kConnected
        };
        EventLoop *loop_;
        std::string name_;
        StateE state_;
        std::unique_ptr<Socket> socket_;
        std::shared_ptr<Channel> channel_;

        InetAddress localAddr_;
        InetAddress peerAddr_;

        ConnectionCallback connectionCallback_;
        MessageCallback messageCallback_;

    private:
        void setState(StateE s) { state_ = s; }

        void handleRead();
    };
}
#endif