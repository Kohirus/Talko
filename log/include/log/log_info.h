#pragma once

#include <log/log_level.h>
#include <string_view>
#include <utils/datetime.h>
#include <utils/os.h>

namespace talko {
namespace log {
struct LogLocation {
    LogLocation() = default;

    /**
     * @brief Construct a new Log Location object
     *
     * @param filename_in 文件名
     * @param line_in 行号
     * @param funcname_in 函数名
     */
    LogLocation(std::string filename_in, int line_in, std::string funcname_in)
        : filename(std::move(filename_in))
        , line(line_in)
        , funcname(std::move(funcname_in)) {}

    int         line { 0 };      ///< 行号
    std::string filename { "" }; ///< 文件名
    std::string funcname { "" }; ///< 函数名
};

struct LogInfo {
    LogInfo() = default;

    /**
     * @brief Construct a new Log Info object
     *
     * @param time 日志时间
     * @param loc 日志位置
     * @param name 日志名称
     * @param lvl 日志等级
     * @param msg 日志内容
     */
    LogInfo(utils::TimePoint time, LogLocation loc, std::string_view name, LogLevel lvl, std::string_view msg)
        : logger_name(name)
        , level(lvl)
        , location(loc)
        , thread_id(utils::os::threadId())
        , time_point(time)
        , content(msg) {
    }

    /**
     * @brief Construct a new Log Info object
     *
     * @param loc 日志位置
     * @param name 日志名称
     * @param lvl 日志等级
     * @param msg 日志内容
     */
    LogInfo(LogLocation loc, std::string_view name, LogLevel lvl, std::string_view msg)
        : LogInfo(utils::DateTime::now().timePoint(), loc, name, lvl, msg) {
    }

    /**
     * @brief Construct a new Log Info object
     *
     * @param name 日志名称
     * @param lvl 日志等级
     * @param msg 日志内容
     */
    LogInfo(std::string_view name, LogLevel lvl, std::string_view msg)
        : LogInfo(LogLocation(), name, lvl, msg) {
    }

    std::string_view logger_name { "Logger" }; ///< 日志名称
    LogLevel         level { LogLevel::info }; ///< 日志等级
    LogLocation      location {};              ///< 日志位置
    std::string_view content { "" };           ///< 日志内容

    long thread_id { utils::os::threadId() }; ///< 线程号

    utils::TimePoint time_point { utils::DateTime::now().timePoint() }; ///< 日志时间
};
} // namespace log
} // namespace talko