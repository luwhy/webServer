#include "Socket.h"
#include "SocketsOps.h"
#include "InetAddress.h"
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <strings.h>
namespace webs
{
    Socket::Socket(int sockfd) : fd_(sockfd)
    {
    }
    Socket::~Socket()
    {
        sockets::close(fd_);
    }
    void Socket::bindAddress(const InetAddress &localaddr)
    {
        sockets::bindOrDie(fd_, localaddr.getSockAddrInet());
    }
    void Socket::listen()
    {
        sockets::listenOrDie(fd_);
    }
    int Socket::accept(InetAddress *peeraddr)
    {
        struct sockaddr_in addr;
        bzero(&addr, sizeof(addr));
        int connd = sockets::accept(fd_, &addr);
        if (connd > 0)
        {
            peeraddr->setSockAddrInet(addr);
        }
        return connd;
    }

    /**
     * @brief 设置是否重用addr
     *
     *
     * @param on
     */
    void Socket::setReuseAddr(bool on)
    {
        int optval = on ? 1 : 0;
        ::setsockopt(fd_, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof optval);
    }
}