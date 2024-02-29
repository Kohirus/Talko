#pragma once

#include <optional>
#include <set>
#include <shared_mutex>
#include <unordered_map>

namespace talko::registry {
/**
 * @brief 服务管理器
 * 
 */
class ServiceManager {
public:
    using ServiceInfo = std::pair<std::string, uint16_t>;

    ServiceManager()  = default;
    ~ServiceManager() = default;

    ServiceManager(const ServiceManager&)            = delete;
    ServiceManager& operator=(const ServiceManager&) = delete;

    /**
     * @brief 添加新的服务
     *
     * @param service_name 服务名称
     * @param ip 服务所属的IP地址
     * @param port 服务所属的端口号
     * @return 添加成功则返回true，否则返回false
     */
    bool addService(const std::string& service_name, const std::string& ip, uint16_t port);

    /**
     * @brief 添加新的方法
     *
     * @param service_name 已存在的服务名称
     * @param method_name 方法名称
     * @return 添加成功则返回true，否则返回false
     */
    bool addMethod(const std::string& service_name, const std::string& method_name);

    /** 指定服务是否存在 */
    bool serviceExist(const std::string& service_name);

    /** 指定方法是否存在 */
    bool methodExist(const std::string& service_name, const std::string& method_name);

    /** 移除指定的服务 */
    void removeService(const std::string& service_name);

    /** 移除指定的方法 */
    void removeMethod(const std::string& service_name, const std::string& method_name);

    /** 查询指定的服务名称和方法名称所属的IP地址和端口号 */
    std::optional<ServiceInfo> find(const std::string& service_name, const std::string& method_name);

private:
    using MethodSet = std::set<std::string>;

    /** 节点信息 */
    struct NodeInfo {
        NodeInfo(const std::string& ip_in, uint16_t port_in)
            : ip(ip_in)
            , port(port_in) { }
        std::string ip;      ///< IP地址
        uint16_t    port;    ///< 端口号
        MethodSet   methods; ///< 方法
    };

    using ServiceMap = std::unordered_map<std::string, NodeInfo>;

    ServiceMap        services_; ///< 服务映射表
    std::shared_mutex mtx_;      ///< 保护服务映射表的线程安全
};
} // namespace talko::registry