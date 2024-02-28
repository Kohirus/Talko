#pragma once

#include <arpa/inet.h>
#include <cassert>
#include <endian.h>
#include <net/callbacks.h>
#include <optional>
#include <string>

namespace talko::net::common {

/** 将网络序转换为主机序 */
template <typename T>
T hostToNetwork(T host) {
    switch (sizeof(host)) {
    case 8:
        return ::htobe64(host);
    case 4:
        return ::htobe32(host);
    case 2:
        return ::htobe16(host);
    default:
        assert(0);
    }
}

/** 将主机序转换为网络序 */
template <typename T>
T networkToHost(T network) {
    switch (sizeof(network)) {
    case 8:
        return ::be64toh(network);
    case 4:
        return ::be32toh(network);
    case 2:
        return ::be16toh(network);
    default:
        assert(0);
    }
}

/** 设置网络地址 */
void setSocketAddr(sockaddr_in& addr, std::string_view ip, uint16_t port);

/** 获取本地地址 */
sockaddr_in getLocalAddr(int sockfd);

/** 获取对端地址 */
sockaddr_in getPeerAddr(int sockfd);

/** 获取套接字错误 */
int getSocketError(int sockfd);

/** 将网络地址结构转换为IP地址的字符串形式 */
std::string toIp(const sockaddr_in& addr);

/** 将网络地址结构转换为IP地址和端口号的字符串形式 */
std::string toIpPort(const sockaddr_in& addr);

/** 关闭套接字描述符 */
void close(int sockfd);

/** 创建非阻塞套接字 */
int createNonblockingSocket(sa_family_t family);

/** 创建通知事件文件描述符 */
int createEventFd();

/** 创建定时器文件描述符 */
int createTimerFd();

/**
 * @brief 重置定时器超时时刻
 *
 * @param timer_fd 定时器文件描述符
 * @param expiration 定时器超时时刻
 */
void resetTimerFd(int timer_fd, TimePoint expiration);

/**
 * @brief 读取定时器文件描述符
 *
 * @param timer_fd 定时器文件描述符
 * @param now 当前时间点
 */
void readTimerFd(int timer_fd, TimePoint now);

/** 创建epoll模型 */
int createEpoll();

/** 绑定网络地址结构 */
void bind(int sockfd, const sockaddr* addr);

/** 监听客户端连接到来 */
void listen(int sockfd);

/** 接收客户端连接并返回相应的套接字描述符 */
int accept(int sockfd, sockaddr* addr);

/** 连接服务器 */
int connect(int sockfd, const sockaddr* addr);

/**
 * @brief 开启套接字选项
 *
 * @param sockfd 套接字描述符
 * @param level 套接字等级
 * @param optname 选项名称
 * @param enabled 是否开启
 */
void enableSocketOption(int sockfd, int level, int optname, bool enabled);

/** 关闭写通道 */
void shutdownWrite(int sockfd);

/**
 * @brief 向套接字描述符中写入数据
 *
 * @param sockfd 套接字描述符
 * @param buf 要写入的数据
 * @param count 数据大小
 * @return ssize_t 返回实际写入的数据大小
 */
ssize_t write(int sockfd, const void* buf, size_t count);

/**
 * @brief 读取套接字上的数据
 *
 * @param sockfd 套接字描述符
 * @param buf 读取数据的缓冲区
 * @param count 缓冲区大小
 * @return ssize_t 实际读取的字节数
 */
ssize_t read(int sockfd, void* buf, size_t count);

/**
 * @brief 分散读取套接字上的数据
 *
 * @param sockfd 套接字描述符
 * @param iov 多缓冲区结构体
 * @param iovcnt 缓冲区数目
 * @return ssize_t 返回实际读取的字节数
 */
ssize_t readv(int sockfd, const iovec* iov, int iovcnt);

/** 是否为自连接 */
bool isSelfConnection(int sockfd);

/** 将事件类型转化为字符串形式用于打印 */
std::string eventsToString(int events);

/** 将操作类型转换为字符串形式用于打印 */
std::string operationToString(int operation);
} // namespace talko::net::common