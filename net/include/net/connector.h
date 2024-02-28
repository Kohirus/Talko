#pragma once

#include <atomic>
#include <functional>
#include <memory>
#include <net/inet_address.h>

namespace talko::net {
class EventLoop;
class Channel;

/**
 * @brief 客户端连接器
 *
 */
class Connector : public std::enable_shared_from_this<Connector> {
public:
    using NewConnectionCallback = std::function<void(int)>;

    /**
     * @brief Construct a new Connector object
     *
     * @param loop 事件循环
     * @param server_addr 服务器地址
     */
    Connector(EventLoop* loop, const InetAddress& server_addr);
    ~Connector();

    Connector(const Connector&)            = delete;
    Connector& operator=(const Connector&) = delete;

    Connector(Connector&&)            = delete;
    Connector& operator=(Connector&&) = delete;

    /** 设置新连接的回调函数 */
    void setNewConnectionCallback(NewConnectionCallback cb);

    /** 启动连接器，尝试连接服务器 */
    void start();

    /**
     * @brief 重新启动连接器，重连服务器
     *
     * @warning 必须在事件循环的创建线程中调用
     */
    void restart();

    /** 停止连接器，与服务器断开连接 */
    void stop();

    /** 获取服务器地址 */
    const InetAddress& serverAddress() const;

private:
    /** 连接器状态 */
    enum class State {
        Disconnected, ///< 已断开连接
        Connecting,   ///< 正在连接
        Connected     ///< 已建立连接
    };

    /** 启动连接器 */
    void start_();

    /** 停止连接器 */
    void stop_();

    /** 尝试连接服务器 */
    void connect();

    /** 正在连接服务器 */
    void connecting(int sockfd);

    /** 重连服务器 */
    void retry(int sockfd);

    /** 移除并重置套接字通道 */
    int removeAndResetChannel();

    /** 重置套接字通道 */
    void resetChannel();

    /** 处理套接字上的可写事件 */
    void handleWrite();

    /** 处理套接字上的错误事件 */
    void handleError();

    /** 将连接状态转为字符串 */
    static std::string toString(State state);

private:
    static const int kMaxRetryDelay;     ///< 最大重连延迟时间
    static const int kMinInitRetryDelay; ///< 最小初始化重连延迟时间
    static const int kMaxInitRetryDelay; ///< 最大初始化重连延迟时间

    EventLoop*  loop_;        ///< 事件循环
    InetAddress server_addr_; ///< 服务器地址

    bool connect_ { false }; ///< 是否与服务器建立连接
    int  retry_delay_;       ///< 重连的延迟时间，单位毫秒

    std::atomic<State>       state_ { State::Disconnected }; ///< 连接状态
    std::unique_ptr<Channel> channel_ { nullptr };           ///< 套接字通道，仅用于非阻塞connect时的可读/错误事件

    NewConnectionCallback new_connection_cb_ {}; ///< 新连接回调函数
};
} // namespace talko::net