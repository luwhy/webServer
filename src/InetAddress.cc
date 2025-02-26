#include "InetAddress.h"
#include "SocketsOps.h"
#include <strings.h>

#include <boost/static_assert.hpp>
namespace webs
{
    static const in_addr_t kInaddrAny = INADDR_ANY;

    BOOST_STATIC_ASSERT(sizeof(InetAddress) == sizeof(struct sockaddr_in));
    InetAddress::InetAddress(uint16_t port)
    {
        bzero(&addr_, sizeof(addr_));
        addr_.sin_family = AF_INET;
        addr_.sin_port = sockets::hostToNetwork16(port);
        addr_.sin_addr.s_addr = sockets::hostToNetwork32(kInaddrAny);
    }
    InetAddress::InetAddress(const std::string &ip, uint16_t port)
    {
        bzero(&addr_, sizeof(addr_));
        sockets::fromHostPort(ip.c_str(), port, &addr_);
        // ::inet_pton(AF_INET, ip.c_str(), &addr_);
        // addr_.sin_port = htobe16(port);
        // addr_.sin_family = AF_INET;
    }

    InetAddress::InetAddress(const struct sockaddr_in &addr) : addr_(addr)
    {
    }
    std::string InetAddress::toHostPort() const
    {
        char buf[32];
        sockets::toHostPort(buf, sizeof buf, addr_);
        // char host[INET_ADDRSTRLEN] = "INVALID";
        // ::inet_ntop(AF_INET, &addr_, host, sizeof(addr_));

        // uint16_t port = be16toh(addr_.sin_port);

        // snprintf(buf, sizeof buf, "%s:%u", host, port);
        return buf;
    }
}