#ifndef __SOCKET_H_
#define __SOCKET_H_

namespace muduo
{
    class InetAddress;

    class Socket
    {
    public:
        explicit Socket(int sockfd);
        Socket(const Socket &fd) = delete;
        ~Socket();
        int fd() const
        {
            return fd_;
        }

        void bindAddress(const InetAddress &localaddr);

        void listen();

        int accept(InetAddress *peeraddr);

        void setReuseAddr(bool on);

    private:
        const int fd_;
    };
}

#endif