#include "../src/Acceptor.h"
#include "../src/EventLoop.h"
#include "../src/InetAddress.h"
#include "../src/SocketsOps.h"
#include <stdio.h>

void newConnection(int sockfd, const muduo::InetAddress &peerAddr)
{
    printf("newConnection(): accepted a new connection from %s\n",
           peerAddr.toHostPort().c_str());
    ::write(sockfd, "How are you?\n", 13);
    sockets::close(sockfd);
}

int main()
{
    printf("main(): pid = %d\n", getpid());

    muduo::InetAddress listenAddr(9980);
    muduo::EventLoop loop;

    muduo::Acceptor acceptor(&loop, listenAddr);

    acceptor.setNewConnectionCallback(newConnection);

    acceptor.listen();

    loop.loop();
}
