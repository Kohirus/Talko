#pragma once

#include <string>

namespace talko {
namespace log {
/** 日志等级 */
enum class LogLevel {
    trace, ///< 追踪级别
    debug, ///< 调试级别
    info,  ///< 信息级别
    warn,  ///< 警告级别
    error, ///< 错误级别
    fatal, ///< 故障级别
    off    ///< 关闭日志
};

namespace level {
/** 将日志等级转化为字符串 */
std::string toString(LogLevel level);

/** 将日志等级转化为简短字符串 */
std::string toShortString(LogLevel level);

/** 将字符串转化为日志等级 */
LogLevel fromString(std::string_view str);
} // namespace level
} // namespace log
} // namespace talko