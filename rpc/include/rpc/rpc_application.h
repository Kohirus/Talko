#pragma once

#include <json/json.h>
#include <log/log.h>
#include <net/inet_address.h>
#include <rpc/rpc_channel.h>
#include <rpc/rpc_controller.h>
#include <rpc/rpc_provider.h>

namespace talko::rpc {
class RpcApplication {
public:
    static RpcApplication& instance();

    /**
     * @brief 初始化RPC应用程序
     *
     * @param argc 参数个数
     * @param argv 参数列表
     */
    void init(int argc, char* argv[]);

    /** 获取服务器名称 */
    inline const std::string& serverName() const { return name_; }

    /** 获取服务器IP地址 */
    inline const std::string& ip() const { return ip_; }

    /** 获取服务器端口号 */
    inline uint16_t port() const { return port_; }

    /** 是否复用端口号 */
    inline bool reusePort() const { return reuse_port_; }

    /** 获取子事件循环的数目 */
    inline size_t subloopSize() const { return subloop_num_; }

    /** 返回服务器网络地址 */
    net::InetAddress serverAddress() const;

    /** 获取注册器的IP地址 */
    inline const std::string& registerIp() const { return register_ip_; }

    /** 获取注册器的端口号 */
    inline uint16_t registerPort() const { return register_port_; }

    /** 获取注册器的网络地址 */
    net::InetAddress registerAddress() const;

private:
    using FuncQueue = std::queue<std::function<void()>>;

    RpcApplication()  = default;
    ~RpcApplication() = default;

    /** 线程池初始化 */
    void initThreadPool();

    /** 日志初始化 */
    void initLog();

    /** 网络初始化 */
    void initNetwork();

    /** 注册器初始化 */
    void initRegister();

    /**
     * @brief 配置日志器
     *
     * @param funcs 回调函数队列
     * @param node 日志器节点
     * @param default_name 默认日志器名称
     * @param global_level 全局日志器等级
     * @param global_pattern 全局日志格式
     */
    void configLogger(FuncQueue& funcs, const json::JsonNode& node, std::string_view default_name,
        const std::string& global_level, const std::string& global_pattern);

    /**
     * @brief 配置日志输出器
     *
     * @param funcs 回调函数队列
     * @param node 日志输出器节点
     * @param logger_level 日志器等级
     * @param logger_pattern 日志器格式
     * @return log::AppenderPtr 返回创建的日志输出器
     */
    log::AppenderPtr configAppender(FuncQueue& funcs, const json::JsonNode& node,
        const std::string& logger_level, const std::string& logger_pattern);

    /** 确保存在 net 和 rpc 日志器 */
    void ensureNetAndRpcLogger();

    /** 配置终端字体属性 */
    void configConsoleProperty();

private:
    json::JsonNode config_; ///< Json配置根节点

    std::string name_ { "server" };    ///< 服务器名称
    std::string ip_ { "0.0.0.0" };     ///< IP地址
    uint16_t    port_ { 8000 };        ///< 端口号
    bool        reuse_port_ { false }; ///< 是否复用端口
    size_t      subloop_num_ { 3 };    ///< 子事件循环数目

    std::string register_ip_ { "127.0.0.1" }; ///< 注册器IP
    uint16_t    register_port_ { 8888 };      ///< 注册器端口
};
} // namespace talko::rpc