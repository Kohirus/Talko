#pragma once

#include <chrono>
#include <string>

namespace talko::pool {
using Duration = std::chrono::duration<int, std::milli>;

struct DatabaseInfo {
    DatabaseInfo() = default;

    DatabaseInfo(const std::string ip_in, uint16_t port_in, const std::string password_in)
        : ip(ip_in)
        , port(port_in)
        , password(password_in) { }

    DatabaseInfo(uint16_t port_in, const std::string password_in)
        : port(port_in)
        , password(password_in) { }

    DatabaseInfo(uint16_t port_in, const std::string& username_in, const std::string& password_in, const std::string& database_in)
        : port(port_in)
        , username(username_in)
        , password(password_in)
        , database(database_in) { }

    std::string ip { "127.0.0.1" };  ///< IP地址
    uint16_t    port { 3306 };       ///< 端口号
    std::string username { "root" }; ///< 用户名
    std::string password { "" };     ///< 密码
    std::string database { "" };     ///< 数据库名
};

/**
 * @brief 连接信息
 *
 */
struct ConnectionInfo {
    ConnectionInfo() = default;

    DatabaseInfo database;        ///< 数据库信息
    size_t       init_size { 3 }; ///< 初始连接数量
    size_t       max_size { 50 }; ///< 最大连接数量

    Duration max_idle_interval { std::chrono::seconds(50) };      ///< 最大空闲时间
    Duration connect_timeout { std::chrono::milliseconds(1000) }; ///< 连接超时时间
};
} // namespace talko::pool