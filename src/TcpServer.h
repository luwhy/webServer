#ifndef __TCP__SERVER_H_
#define __TCP__SERVER_H_

#include "Callbacks.h"
#include "TcpConnection.h"
#include <map>
#include <memory>
namespace webs
{
    class EventLoop;
    class Acceptor;
    class TcpServer
    {

    public:
        TcpServer(EventLoop *loop, const InetAddress &listenAddr);
        ~TcpServer();
        TcpServer(const TcpServer &t) = delete;

        void start();

        /**
         * @brief 设置connection 回调，非线程安全
         *
         * @param cb
         */
        void setConnectionCallback(const ConnectionCallback &cb)
        {
            connectionCallback_ = cb;
        }

        void setMessageCallback(const MessageCallback &cb)
        {
            messageCallbacak_ = cb;
        }

    private:
        void newConnection(int sockfd, const InetAddress &peerAddr);

        void removeConnection(const TcpConnectionPtr &conn);
        typedef std::map<std::string, TcpConnectionPtr> ConnectionMap;

        // acceptor loop
        EventLoop *loop_;
        const std::string name_;
        std::unique_ptr<Acceptor> acceptor_;

        ConnectionCallback connectionCallback_;

        MessageCallback messageCallbacak_;

        bool started_;

        int nextConnId_;

        ConnectionMap connections_;
    };

};

#endif