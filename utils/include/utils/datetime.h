#pragma once

#include <chrono>
#include <ctime>
#include <optional>
#include <string>

namespace talko::utils {
using TimePoint = std::chrono::high_resolution_clock::time_point;

/** 时间字符串类型 */
enum class StringType {
    Date,          ///< 仅日期
    Time,          ///< 仅时间
    DateTime,      ///< 日期时间
    DateTimeMilli, ///< 日期时间.毫秒
    DateTimeMicro, ///< 日期时间.微秒
    DateTimeNano   ///< 日期时间.纳秒
};

/**
 * @brief 时间日期类，默认采用本地时间
 *
 */
class DateTime {
public:
    /**
     * @brief Construct a new Date Time object
     *
     * @param time_point 时间点
     */
    DateTime(TimePoint time_point);

    /**
     * @brief Construct a new Date Time object
     *
     * @param ctime C-Style时间戳
     */
    DateTime(std::time_t ctime);

    DateTime(const DateTime&)            = default;
    DateTime& operator=(const DateTime&) = default;

    ~DateTime() = default;

    /** 获取当前时间 */
    static DateTime now();

    /** 返回无效时间 */
    static std::optional<DateTime> invalid();

    /** 获取时间点 */
    inline TimePoint timePoint() const { return time_point_; }

    /** 获取自纪元时间以来的秒数 */
    long secondsSinceEpoch() const;

    /** 获取年份 */
    int year() const;

    /** 获取月份 */
    int month() const;

    /** 获取天数 */
    int day() const;

    /** 获取年天数 */
    int dayOfYear() const;

    /** 获取工作日 */
    int weekday() const;

    /** 获取24小时制的小时 */
    int hour() const;

    /** 获取12小时制的小时 */
    int hour12() const;

    /** 是否为AM */
    bool isAm() const;

    /** 获取分钟数 */
    int minute() const;

    /** 获取秒数 */
    int seconds() const;

    /** 获取毫秒数 */
    int milliseconds() const;

    /** 获取微秒数 */
    long microseconds() const;

    /** 获取纳秒数 */
    long nanoseconds() const;

    /** 获取时区缩写 */
    std::string zone() const;

    /** 转化为本地时间 */
    DateTime& toLocalTime();

    /** 转化为UTC时间 */
    DateTime& toUtcTime();

    /** 转换为字符串形式 */
    std::string toString(StringType type = StringType::DateTime) const;

    /** 将时间点转换为日期时间的字符串形式 */
    static std::string toString(TimePoint time, StringType type = StringType::DateTime);

private:
    /** 获取std::tm对象 */
    std::tm* getTm() const;

private:
    TimePoint time_point_; ///< 时间点
    bool      local_time_; ///< 是否使用本地时间
};
} // namespace talko::utils