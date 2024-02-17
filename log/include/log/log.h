#pragma once

#include <fmt/core.h>
#include <log/console_appender.h>
#include <log/file_appender.h>
#include <log/log_formatter.h>
#include <log/log_manager.h>
#include <log/logger.h>
#include <log/rotating_file_appender.h>
#include <log/types.h>

namespace talko::log {
/** 获取指定名称的日志器 */
LoggerPtr get(const std::string& name);

/** 设置全局模式串和时间类型，默认采用本地时间 */
void setPattern(std::string pattern, bool use_utc = false);

/** 设置全局格式化器 */
void setFormatter(FormatterPtr formatter);

/** 获取全局日志等级 */
LogLevel globalLevel();

/** 注册日志器 */
bool registerLogger(LoggerPtr logger);

/** 移除指定名称的日志器 */
void remove(const std::string& name);

/** 指定名称的日志器是否存在 */
bool exist(const std::string& name);

/** 清空所有日志器 */
void clear();

/** 获取默认日志器 */
LoggerPtr defaultLogger();

/** 设置默认日志器 */
void setDefaultLogger(LoggerPtr logger);

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
    defaultLogger()->log(loc, lvl, fmt, std::forward<Args>(args)...);
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
    defaultLogger()->log(lvl, fmt, std::forward<Args>(args)...);
}

/**
 * @brief 输出日志
 *
 * @tparam T 日志消息参数类型
 * @param loc 日志位置
 * @param lvl 日志等级
 * @param msg 日志消息
 */
template <typename T>
void log(LogLocation loc, LogLevel lvl, const T& msg) {
    defaultLogger()->log(loc, lvl, msg);
}

/**
 * @brief 输出日志
 *
 * @tparam T 日志消息参数类型
 * @param lvl 日志等级
 * @param msg 日志消息
 */
template <typename T>
void log(LogLevel lvl, const T& msg) {
    defaultLogger()->log(lvl, msg);
}

/**
 * @brief 输出trace级别的日志
 *
 * @tparam Args 格式化参数类型
 * @param fmt 格式化字符串
 * @param args 格式化参数
 */
template <typename... Args>
void trace(fmt::format_string<Args...> fmt, Args&&... args) {
    defaultLogger()->trace(fmt, std::forward<Args>(args)...);
}

/**
 * @brief 输出debug级别的日志
 *
 * @tparam Args 格式化参数类型
 * @param fmt 格式化字符串
 * @param args 格式化参数
 */
template <typename... Args>
void debug(fmt::format_string<Args...> fmt, Args&&... args) {
    defaultLogger()->debug(fmt, std::forward<Args>(args)...);
}

/**
 * @brief 输出info级别的日志
 *
 * @tparam Args 格式化参数类型
 * @param fmt 格式化字符串
 * @param args 格式化参数
 */
template <typename... Args>
void info(fmt::format_string<Args...> fmt, Args&&... args) {
    defaultLogger()->info(fmt, std::forward<Args>(args)...);
}

/**
 * @brief 输出warn级别的日志
 *
 * @tparam Args 格式化参数类型
 * @param fmt 格式化字符串
 * @param args 格式化参数
 */
template <typename... Args>
void warn(fmt::format_string<Args...> fmt, Args&&... args) {
    defaultLogger()->warn(fmt, std::forward<Args>(args)...);
}

/**
 * @brief 输出error级别的日志
 *
 * @tparam Args 格式化参数类型
 * @param fmt 格式化字符串
 * @param args 格式化参数
 */
template <typename... Args>
void error(fmt::format_string<Args...> fmt, Args&&... args) {
    defaultLogger()->error(fmt, std::forward<Args>(args)...);
}

/**
 * @brief 输出fatal级别的日志
 *
 * @tparam Args 格式化参数类型
 * @param fmt 格式化字符串
 * @param args 格式化参数
 */
template <typename... Args>
void fatal(fmt::format_string<Args...> fmt, Args&&... args) {
    defaultLogger()->fatal(fmt, std::forward<Args>(args)...);
}

/**
 * @brief 输出trace级别的日志
 *
 * @tparam T 日志消息参数类型
 * @param msg 日志消息
 */
template <typename T>
void trace(const T& msg) {
    defaultLogger()->trace(msg);
}

/**
 * @brief 输出debug级别的日志
 *
 * @tparam T 日志消息参数类型
 * @param msg 日志消息
 */
template <typename T>
void debug(const T& msg) {
    defaultLogger()->debug(msg);
}

/**
 * @brief 输出info级别的日志
 *
 * @tparam T 日志消息参数类型
 * @param msg 日志消息
 */
template <typename T>
void info(const T& msg) {
    defaultLogger()->info(msg);
}

/**
 * @brief 输出warn级别的日志
 *
 * @tparam T 日志消息参数类型
 * @param msg 日志消息
 */
template <typename T>
void warn(const T& msg) {
    defaultLogger()->warn(msg);
}

/**
 * @brief 输出error级别的日志
 *
 * @tparam T 日志消息参数类型
 * @param msg 日志消息
 */
template <typename T>
void error(const T& msg) {
    defaultLogger()->error(msg);
}

/**
 * @brief 输出fatal级别的日志
 *
 * @tparam T 日志消息参数类型
 * @param msg 日志消息
 */
template <typename T>
void fatal(const T& msg) {
    defaultLogger()->fatal(msg);
}

} // namespace talko::log

#define LOG_CALL(logger, level, ...)    (logger)->log(log::LogLocation(__FILE__, __LINE__, __FUNCTION__), level, __VA_ARGS__)
#define DEFAULT_LOG(level, ...)         LOG_CALL(log::defaultLogger(), level, __VA_ARGS__)
#define SPECIFIED_LOG(name, level, ...) LOG_CALL(log::get(name), level, __VA_ARGS__)

#define LOG_TRACE(...) DEFAULT_LOG(log::LogLevel::trace, __VA_ARGS__)
#define LOG_DEBUG(...) DEFAULT_LOG(log::LogLevel::debug, __VA_ARGS__)
#define LOG_INFO(...)  DEFAULT_LOG(log::LogLevel::info, __VA_ARGS__)
#define LOG_WARN(...)  DEFAULT_LOG(log::LogLevel::warn, __VA_ARGS__)
#define LOG_ERROR(...) DEFAULT_LOG(log::LogLevel::error, __VA_ARGS__)
#define LOG_FATAL(...) DEFAULT_LOG(log::LogLevel::fatal, __VA_ARGS__)

#define LOGGER_TRACE(name, ...) SPECIFIED_LOG(name, log::LogLevel::trace, __VA_ARGS__)
#define LOGGER_DEBUG(name, ...) SPECIFIED_LOG(name, log::LogLevel::debug, __VA_ARGS__)
#define LOGGER_INFO(name, ...)  SPECIFIED_LOG(name, log::LogLevel::info, __VA_ARGS__)
#define LOGGER_WARN(name, ...)  SPECIFIED_LOG(name, log::LogLevel::warn, __VA_ARGS__)
#define LOGGER_ERROR(name, ...) SPECIFIED_LOG(name, log::LogLevel::error, __VA_ARGS__)
#define LOGGER_FATAL(name, ...) SPECIFIED_LOG(name, log::LogLevel::fatal, __VA_ARGS__)