#pragma once

#include <fmt/core.h>
#include <fmt/format.h>
#include <log/log_info.h>
#include <log/types.h>
#include <string>

namespace talko::log {
/**
 * @brief 日志器
 *
 */
class Logger {
public:
    /**
     * @brief Construct a new Logger object
     *
     * @param logger_name 日志名称
     */
    explicit Logger(const std::string& logger_name);

    /**
     * @brief Construct a new Logger object
     *
     * @tparam It 迭代器
     * @param logger_name 日志名称
     * @param begin 起始迭代器
     * @param end 结束迭代器
     */
    template <typename It>
    Logger(const std::string& logger_name, It begin, It end)
        : name_(std::move(logger_name))
        , appenders_(begin, end) { }

    /**
     * @brief Construct a new Logger object
     *
     * @param logger_name 日志名称
     * @param appender 日志输出器
     */
    Logger(const std::string& logger_name, AppenderPtr appender);

    /**
     * @brief Construct a new Logger object
     *
     * @param logger_name 日志名称
     * @param list 日志输出器列表
     */
    Logger(const std::string& logger_name, AppenderInitList list);

    ~Logger() = default;

    Logger(const Logger& other);
    Logger& operator=(const Logger& other);

    Logger(Logger&& other) noexcept;
    Logger& operator=(Logger&& other) noexcept;

    /** 与日志器进行交换 */
    void swap(Logger& other) noexcept;

    /** 获取日志器名称 */
    inline const std::string& name() const { return name_; }

    /**
     * @brief 设置模式串
     *
     * @param pattern 模式串
     * @param use_utc 是否使用UTC时间，默认采用本地时间
     */
    void setPattern(std::string pattern, bool use_utc = false);

    /** 设置格式化器 */
    void setFormatter(FormatterPtr formatter);

    /** 设置日志等级 */
    void setLevel(LogLevel lvl);

    /** 获取日志等级 */
    LogLevel level() const;

    /** 是否应该输出日志 */
    bool shouldLog(LogLevel lvl) const;

    /** 获取所有日志输出器 */
    inline const AppenderList& appenders() const { return appenders_; }

    /** 获取所有日志输出器 */
    inline AppenderList& appenders() { return appenders_; }

    /** 复制日志器 */
    LoggerPtr clone(const std::string& logger_name);

    /**
     * @brief 输出日志
     *
     * @tparam Args 格式化参数类型
     * @param loc 日志位置
     * @param lvl 日志等级
     * @param fmt 格式化字符串
     * @param args 格式化参数
     */
    template <typename... Args>
    void log(LogLocation loc, LogLevel lvl, fmt::format_string<Args...> fmt, Args&&... args) {
        if (!shouldLog(lvl)) return;
        std::string buffer;
        fmt::vformat_to(std::back_inserter(buffer), fmt, fmt::make_format_args(args...));
        LogInfo info(loc, name_, lvl, buffer);
        log_(info);
    }

    /**
     * @brief 输出日志
     *
     * @tparam Args 格式化参数类型
     * @param lvl 日志等级
     * @param fmt 格式化字符串
     * @param args 格式化参数
     */
    template <typename... Args>
    void log(LogLevel lvl, fmt::format_string<Args...> fmt, Args&&... args) {
        log(LogLocation {}, lvl, fmt, std::forward<Args>(args)...);
    }

    /**
     * @brief 输出日志
     *
     * @tparam T 日志消息类型
     * @param lvl 日志等级
     * @param msg 日志消息
     */
    template <typename T>
    void log(LogLevel lvl, const T& msg) {
        log(LogLocation {}, lvl, msg);
    }

    /**
     * @brief 输出日志
     *
     * @param time_point 日志时间
     * @param loc 日志位置
     * @param lvl 日志等级
     * @param msg 日志消息
     */
    void log(utils::TimePoint time_point, LogLocation loc, LogLevel lvl, const std::string& msg);

    /**
     * @brief 输出日志
     *
     * @param loc 日志位置
     * @param lvl 日志等级
     * @param msg 日志消息
     */
    void log(LogLocation loc, LogLevel lvl, const std::string& msg);

    /**
     * @brief 输出trace级别日志
     *
     * @tparam Args 格式化参数类型
     * @param fmt 格式化字符串
     * @param args 格式化参数
     */
    template <typename... Args>
    void trace(fmt::format_string<Args...> fmt, Args&&... args) {
        log(LogLevel::trace, fmt, std::forward<Args>(args)...);
    }

    /**
     * @brief 输出debug级别日志
     *
     * @tparam Args 格式化参数类型
     * @param fmt 格式化字符串
     * @param args 格式化参数
     */
    template <typename... Args>
    void debug(fmt::format_string<Args...> fmt, Args&&... args) {
        log(LogLevel::debug, fmt, std::forward<Args>(args)...);
    }

    /**
     * @brief 输出info级别日志
     *
     * @tparam Args 格式化参数类型
     * @param fmt 格式化字符串
     * @param args 格式化参数
     */
    template <typename... Args>
    void info(fmt::format_string<Args...> fmt, Args&&... args) {
        log(LogLevel::info, fmt, std::forward<Args>(args)...);
    }

    /**
     * @brief 输出warn级别日志
     *
     * @tparam Args 格式化参数类型
     * @param fmt 格式化字符串
     * @param args 格式化参数
     */
    template <typename... Args>
    void warn(fmt::format_string<Args...> fmt, Args&&... args) {
        log(LogLevel::warn, fmt, std::forward<Args>(args)...);
    }

    /**
     * @brief 输出error级别日志
     *
     * @tparam Args 格式化参数类型
     * @param fmt 格式化字符串
     * @param args 格式化参数
     */
    template <typename... Args>
    void error(fmt::format_string<Args...> fmt, Args&&... args) {
        log(LogLevel::error, fmt, std::forward<Args>(args)...);
    }

    /**
     * @brief 输出fatal级别日志
     *
     * @tparam Args 格式化参数类型
     * @param fmt 格式化字符串
     * @param args 格式化参数
     */
    template <typename... Args>
    void fatal(fmt::format_string<Args...> fmt, Args&&... args) {
        log(LogLevel::fatal, fmt, std::forward<Args>(args)...);
    }

    /**
     * @brief 输出trace级别的日志消息
     *
     * @tparam T 日志消息参数类型
     * @param msg 日志消息
     */
    template <typename T>
    void trace(const T& msg) {
        log(LogLevel::trace, msg);
    }

    /**
     * @brief 输出debug级别的日志消息
     *
     * @tparam T 日志消息参数类型
     * @param msg 日志消息
     */
    template <typename T>
    void debug(const T& msg) {
        log(LogLevel::debug, msg);
    }

    /**
     * @brief 输出info级别的日志消息
     *
     * @tparam T 日志消息参数类型
     * @param msg 日志消息
     */
    template <typename T>
    void info(const T& msg) {
        log(LogLevel::info, msg);
    }

    /**
     * @brief 输出warn级别的日志消息
     *
     * @tparam T 日志消息参数类型
     * @param msg 日志消息
     */
    template <typename T>
    void warn(const T& msg) {
        log(LogLevel::warn, msg);
    }

    /**
     * @brief 输出error级别的日志消息
     *
     * @tparam T 日志消息参数类型
     * @param msg 日志消息
     */
    template <typename T>
    void error(const T& msg) {
        log(LogLevel::error, msg);
    }

    /**
     * @brief 输出fatal级别的日志消息
     *
     * @tparam T 日志消息参数类型
     * @param msg 日志消息
     */
    template <typename T>
    void fatal(const T& msg) {
        log(LogLevel::fatal, msg);
    }

private:
    /** 输出日志消息 */
    void log_(const LogInfo& info);

private:
    std::string  name_ { "" };              ///< 日志名称
    AppenderList appenders_ {};             ///< 日志输出器
    AtomicLevel  level_ { LogLevel::info }; ///< 日志等级
    ErrorHandler err_handler_ {};           ///< 错误消息句柄
};
} // namespace talko::log