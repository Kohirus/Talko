#pragma once

#include <log/log_manager.h>
#include <log/types.h>

namespace talko::log {
/**
 * @brief 工厂
 *
 */
struct Factory {
    /**
     * @brief 创建指定输出器类型的日志器
     *
     * @tparam Appender 输出器类型
     * @tparam AppenderArgs 输出器构造函数类型
     * @param logger_name 日志器名称
     * @param args 输出器构造函数参数
     * @return LoggerPtr 返回日志器
     */
    template <typename Appender, typename... AppenderArgs>
    static LoggerPtr create(std::string logger_name, AppenderArgs&&... args) {
        auto appender = std::make_shared<Appender>(std::forward<AppenderArgs>(args)...);
        auto logger   = std::make_shared<Logger>(std::move(logger_name), std::move(appender));
        LogManager::instance().registerLogger(logger);
        return logger;
    }
};
} // namespace talko::log