#ifndef _INET_ADDRESS_H_
#define _INET_ADDRESS_H_
#include <netinet/in.h>
#include <string>
namespace webs
{
    class InetAddress
    {
    public:
        explicit InetAddress(uint16_t port);

        InetAddress(const std::string &ip, uint16_t port);

        InetAddress(const struct sockaddr_in &addr);

        std::string toHostPort() const;

        const struct sockaddr_in getSockAddrInet() const { return addr_; }

        void setSockAddrInet(const struct sockaddr_in &addr)
        {
            this->addr_ = addr;
        }

    private:
        struct sockaddr_in addr_;
    };
}

#endif