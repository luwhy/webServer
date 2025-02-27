#include "../src/Acceptor.h"
#include "../src/EventLoop.h"
#include "../src/InetAddress.h"
#include "../src/SocketsOps.h"
#include <stdio.h>

void newConnection(int sockfd, const webs::InetAddress &peerAddr)
{
    printf("newConnection(): accepted a new connection from %s\n",
           peerAddr.toHostPort().c_str());
    ::write(sockfd, "How are you?\n", 13);
    sockets::close(sockfd);
}

int main()
{
    printf("main(): pid = %d\n", getpid());

    webs::InetAddress listenAddr(9980);
    webs::EventLoop loop;

    webs::Acceptor acceptor(&loop, listenAddr);

    acceptor.setNewConnectionCallback(newConnection);

    acceptor.listen();

    loop.loop();
}
