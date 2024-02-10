#pragma once

#include <log/log_info.h>
#include <log/output_strategy.h>
#include <log/types.h>
#include <log/factory.h>

namespace talko::log {
/**
 * @brief 日志输出器
 *
 */
class LogAppender {
public:
    LogAppender()          = default;
    virtual ~LogAppender() = default;

    /** 输出日志信息 */
    virtual void log(const LogInfo& info) = 0;

    /** 设置模式串 */
    virtual void setPattern(const std::string& pattern) = 0;

    /** 设置格式化器 */
    virtual void setFormatter(FormatterPtr formatter) = 0;

    /** 设置日志等级 */
    void setLevel(LogLevel lvl);

    /** 获取日志等级 */
    LogLevel level() const;

    /** 是否应该输出日志 */
    bool shouldLog(LogLevel lvl) const;

protected:
    AtomicLevel level_ { LogLevel::trace }; ///< 日志等级
};
} // namespace talko::log