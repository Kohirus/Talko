#pragma once

#include <arpa/inet.h>
#include <string>

namespace talko::net {
/**
 * @brief 网络地址
 *
 */
class InetAddress {
public:
    /**
     * @brief Construct a new Inet Address object
     *
     * @param port 端口号
     * @param lookback_only 是否仅监听本地地址
     */
    explicit InetAddress(uint16_t port = 0, bool lookback_only = false);

    /**
     * @brief Construct a new Inet Address object
     *
     * @param ip IP地址
     * @param port 端口号
     */
    InetAddress(std::string_view ip, uint16_t port);

    /**
     * @brief Construct a new Inet Address object
     *
     * @param addr 网络地址结构
     */
    explicit InetAddress(const sockaddr_in& addr);

    ~InetAddress() = default;

    InetAddress(const InetAddress&)            = default;
    InetAddress& operator=(const InetAddress&) = default;

    /** 获取地址族 */
    sa_family_t family() const;

    /** 转换为IP地址字符串的形式 */
    std::string toIp() const;

    /** 转换为IP地址和端口号的字符串形式 */
    std::string toIpPort() const;

    /** 获取端口号 */
    uint16_t port() const;

    /** 获取通用网络地址结构 */
    const sockaddr* sockAddr() const;

    /** 设置网络地址结构 */
    void setSockAddr(const sockaddr_in& addr);

private:
    sockaddr_in addr_; ///< 网络地址结构
};
} // namespace talko::net