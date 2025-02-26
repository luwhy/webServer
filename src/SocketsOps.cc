#include "SocketsOps.h"
#include "sylar/log.h"
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <strings.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/types.h>
#include <boost/implicit_cast.hpp>
namespace sockets
{
    typedef struct sockaddr SA;
    sylar::Logger::ptr s_logger_so = SYLAR_LOG_NAME("system");

    const SA *sockaddr_cast(const struct sockaddr_in *addr)
    {
        return static_cast<const SA *>(boost::implicit_cast<const void *>(addr));
    }

    SA *sockaddr_cast(struct sockaddr_in *addr)
    {
        return static_cast<SA *>(boost::implicit_cast<void *>(addr));
    }
    /**
     * @brief 这段代码的作用是将给定的套接字设置为非阻塞模式，并确保在执行 exec 系列函数时自动关闭该套接字。
     * 这通常用于防止子进程继承父进程的文件描述符，避免资源泄漏或意外的文件描述符共享。
     *
     * @param sockfd
     */
    void setNonBlockAndCloseOnExec(int sockfd)
    {
        int flags = ::fcntl(sockfd, F_GETFL, 0);
        flags |= O_NONBLOCK;
        int ret = ::fcntl(sockfd, F_SETFL, flags);

        // 获得close-on-exec旗标,F_GETFD
        flags = ::fcntl(sockfd, F_GETFD, 0);
        flags |= FD_CLOEXEC;
        // 设置close-on-exec旗标F_SETFD
        ret = ::fcntl(sockfd, F_SETFD, flags);
    }

    int createNonBlockingOrDie()
    {
#if VALGRIND
        int sockfd = ::socket(AF_INET, SOCK_STREAM, IPPORT_TCP);
        if (sockfd < 0)
        {
            SYLAR_LOG_ERROR(s_logger_so) << "sockets::createNonblockingOrDie";
        }
#else
        int sockfd = ::socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, IPPROTO_TCP);
        if (sockfd < 0)
        {
            SYLAR_LOG_ERROR(s_logger_so) << "sockets::createNonblockingOrDie";
        }
#endif
        return sockfd;
    }
    void bindOrDie(int sockfd, const struct sockaddr_in &addr)
    {
        int ret = ::bind(sockfd, sockaddr_cast(&addr), sizeof(addr));
        if (ret < 0)
        {
            SYLAR_LOG_ERROR(s_logger_so) << "sockets::bindOrDie";
        }
    }
    void listenOrDie(int sockfd)
    {
        int ret = ::listen(sockfd, SOMAXCONN);
        if (ret < 0)
        {
            SYLAR_LOG_ERROR(s_logger_so) << "sockets:: listenOrDie";
        }
    }
    /**
     * @brief accept 返回新的套接字
     *
     * @param sockfd
     * @param addr
     * @return int
     */
    int accept(int sockfd, sockaddr_in *addr)
    {
        socklen_t addrlen = sizeof(*addr);
#if VALGRIND
        int connfd = ::accept(sockfd, sockaddr_cast(addr), &addrlen);
        setNonBlockAndCloseOnExec(connfd);
#else
        int connfd = ::accept4(sockfd, sockaddr_cast(addr), &addrlen, SOCK_NONBLOCK | SOCK_CLOEXEC);
#endif
        if (connfd < 0)
        {
            int savedErrno = errno;
            switch (savedErrno)
            {
            case EAGAIN:
            case ECONNABORTED:
            case EINTR:
            case EPROTO: // ???
            case EPERM:
            case EMFILE: // per-process lmit of open file desctiptor ???
                // expected errors
                errno = savedErrno;
                break;
            case EBADF:
            case EFAULT:
            case EINVAL:
            case ENFILE:
            case ENOBUFS:
            case ENOMEM:
            case ENOTSOCK:
            case EOPNOTSUPP:
                SYLAR_LOG_ERROR(s_logger_so) << " unexpected error of ::accept " << savedErrno;
                break;
            default:
                SYLAR_LOG_ERROR(s_logger_so) << "unknown error of ::accept " << savedErrno;
                break;
            }
        }
        return connfd;
    }
    void close(int sockfd)
    {
        int ret = ::close(sockfd);
        if (ret < 0)
        {
            SYLAR_LOG_ERROR(s_logger_so) << " socket close error";
        }
    }
    /**
     * @brief 将网络字节序转化为字符ip
     *
     * @param buf
     * @param size
     * @param addr
     */
    void toHostPort(char *buf, size_t size, const sockaddr_in &addr)
    {
        char host[INET_ADDRSTRLEN] = "INVALID";
        ::inet_ntop(AF_INET, &addr.sin_addr, host, sizeof host);
        uint16_t port = networkToHost16(addr.sin_port);
        snprintf(buf, size, "%s:%u", host, port);
    }

    /**
     * @brief inet_pton 将 IP 地址从文本表示形式转换为网络字节序的二进制形式。"presentation to network"
     *          网络端是大端序
     * @param ip
     * @param port
     * @param addr
     */
    void fromHostPort(const char *ip, uint16_t port, sockaddr_in *addr)
    {
        addr->sin_family = AF_INET;
        addr->sin_port = hostToNework16(port);

        if (::inet_pton(AF_INET, ip, &addr->sin_addr) <= 0)
        {
            SYLAR_LOG_ERROR(s_logger_so) << "sockets::fromHostPort";
        }
    }
}