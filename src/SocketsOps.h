#ifndef __SOCKETS_OPS__H__
#define __SOCKETS_OPS__H__

#include <arpa/inet.h>
#include <endian.h>

namespace sockets
{
    /**
     * @brief 64位主机字节序到大端字节序(网络)
     *
     * @param host64
     * @return uint64_t
     */
    inline uint64_t hostToNetwork64(uint64_t host64)
    {

        return htobe64(host64);
    }
    /**
     * @brief 32位主机字节序到大端字节序(网络)
     *
     * @param host32
     * @return uint32_t
     */
    inline uint32_t hostToNetwork32(uint32_t host32)
    {
        return htobe32(host32);
    }

    /**
     * @brief 32位主机字节序到大端字节序(网络)
     *
     * @param host16
     * @return uint16_t
     */
    inline uint16_t hostToNetwork16(uint16_t host16)
    {
        return htobe16(host16);
    }
    /**
     * @brief 大端64转主机字节序
     *
     * @param net64
     * @return uint64_t
     */
    inline uint64_t networkToHost64(uint64_t net64)
    {
        return be64toh(net64);
    }
    /**
     * @brief 大端32转主机字节序
     *
     * @param net32
     * @return uint32_t
     */
    inline uint32_t networkToHost32(uint32_t net32)
    {
        return be32toh(net32);
    }
    /**
     * @brief 大端16字节转主机字节序
     *
     * @param net16
     * @return uint16_t
     */
    inline uint16_t networkToHost16(uint16_t net16)
    {
        return be16toh(net16);
    }

    int createNonBlockingOrDie();

    void bindOrDie(int sockfd, const struct sockaddr_in &addr);

    void listenOrDie(int sockfd);

    int accept(int sockfd, struct sockaddr_in *addr);

    void close(int sockfd);

    void toHostPort(char *buf, size_t size, const struct sockaddr_in &addr);

    void fromHostPort(const char *ip, uint16_t port, struct sockaddr_in *addr);

    struct sockaddr_in getLocalAddr(int sockfd);

    int getSocketError(int sockfd);
}

#endif
