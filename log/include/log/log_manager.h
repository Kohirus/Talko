#pragma once

#include <log/types.h>
#include <mutex>
#include <unordered_map>

namespace talko {
namespace log {
/**
 * @brief 日志管理器，单例模式实现
 *
 */
class LogManager {
public:
    using LoggerMap = std::unordered_map<std::string, LoggerPtr>;

    LogManager(const LogManager&)            = delete;
    LogManager& operator=(const LogManager&) = delete;

    LogManager(LogManager&&)            = delete;
    LogManager& operator=(LogManager&&) = delete;

    /** 返回全局唯一实例对象 */
    static LogManager& instance();

    /** 注册日志器，注册成功返回true，否则返回false */
    bool registerLogger(LoggerPtr logger);

    /** 获取指定名称的日志器 */
    LoggerPtr get(const std::string& logger_name);

    /** 指定名称的日志器是否已经存在 */
    bool exist(const std::string& logger_name);

    /** 获取默认日志器 */
    LoggerPtr defaultLogger();

    /** 设置默认的日志器 */
    void setDefaultLogger(LoggerPtr logger);

    /** 删除指定名称的日志器 */
    void remove(const std::string& logger_name);

    /** 清空所有日志器 */
    void clear();

    /** 设置格式化器 */
    void setFormatter(FormatterPtr formatter);

    /** 设置全局日志等级 */
    void setGlobalLevel(LogLevel lvl);

    /** 获取全局日志等级 */
    LogLevel globalLevel();

private:
    LogManager();
    ~LogManager() = default;

private:
    AtomicLevel  global_level_ { LogLevel::info }; ///< 全局日志等级
    LoggerMap    loggers_ {};                      ///< 日志器映射表
    LoggerPtr    default_logger_ { nullptr };      ///< 默认日志器
    FormatterPtr formatter_ { nullptr };           ///< 格式化器
    std::mutex   map_mtx_;                         ///< 保护日志器映射表的线程安全
};
} // namespace log
} // namespace talko