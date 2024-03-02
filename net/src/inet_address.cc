#include <log/log.h>
#include <net/common.h>
#include <net/inet_address.h>

namespace talko::net {
InetAddress::InetAddress(uint16_t port, bool lookback_only) {
    ::bzero(&addr_, sizeof(addr_));
    addr_.sin_family      = AF_INET;
    in_addr_t ip          = lookback_only ? INADDR_LOOPBACK : INADDR_ANY;
    addr_.sin_addr.s_addr = common::hostToNetwork(ip);
    addr_.sin_port        = common::hostToNetwork(port);
}

InetAddress::InetAddress(std::string_view ip, uint16_t port) {
    ::bzero(&addr_, sizeof(addr_));
    common::setSocketAddr(addr_, ip, port);
}

InetAddress::InetAddress(const sockaddr_in& addr)
    : addr_(addr) {
}

sa_family_t InetAddress::family() const {
    return addr_.sin_family;
}

std::string InetAddress::toIp() const {
    return common::toIp(addr_);
}

std::string InetAddress::toIpPort() const {
    return common::toIpPort(addr_);
}

uint16_t InetAddress::port() const {
    return common::networkToHost(addr_.sin_port);
}

const sockaddr* InetAddress::sockAddr() const {
    return reinterpret_cast<const sockaddr*>(&addr_);
}

void InetAddress::setSockAddr(const sockaddr_in& addr) {
    addr_ = addr;
}
} // namespace talko::net