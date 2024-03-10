#pragma once

#include <pool/connection_info.h>
#include <string>

namespace talko::pool {
/**
 * @brief 基础连接类 不可实例化
 *
 */
class BaseConnection {
public:
    BaseConnection();
    virtual ~BaseConnection() = default;

    BaseConnection(const BaseConnection&)            = delete;
    BaseConnection& operator=(const BaseConnection&) = delete;

    /** 检查与服务器之间的连接是否正常 */
    virtual bool ping() = 0;

    /**
     * @brief 连接数据库
     *
     * @param info 数据库信息
     * @param timeout 超时时间
     * @return 连接成功则返回true，否则返回false
     */
    virtual bool connect(DatabaseInfo info, Duration timeout) = 0;

    /** 获取错误信息 */
    virtual std::string errorMessage() const = 0;

    /** 获取空闲时长 */
    Duration getIdleDuration() const;

    /** 重置空闲时间 */
    void resetIdleTime();

protected:
    using TimePoint = std::chrono::high_resolution_clock::time_point;

    TimePoint start_idle_time_; ///< 起始空闲时间
};
} // namespace talko::pool