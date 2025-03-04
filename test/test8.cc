#include "../src/TcpServer.h"
#include "../src/EventLoop.h"
#include "../src/InetAddress.h"
#include <stdio.h>

void onConnection(const muduo::TcpConnectionPtr &conn)
{
    if (conn->connected())
    {
        printf("onConnection(): new connection [%s] from %s\n",
               conn->name().c_str(),
               conn->peerAddr().toHostPort().c_str());
    }
    else
    {
        printf("onConnection(): connection [%s] is down\n",
               conn->name().c_str());
    }
}

void onMessage(const muduo::TcpConnectionPtr &conn,
               muduo::Buffer *data,
               ssize_t len)
{
    printf("onMessage(): received %zd bytes from connection [%s]\n",
           len, conn->name().c_str());
}

int main()
{
    printf("main(): pid = %d\n", getpid());

    muduo::InetAddress listenAddr(9981);
    muduo::EventLoop loop;

    muduo::TcpServer server(&loop, listenAddr);
    server.setConnectionCallback(onConnection);
    server.setMessageCallback(onMessage);
    server.start();

    loop.loop();
}
