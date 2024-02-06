#pragma once

#include <functional>
#include <log/log_info.h>
#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace talko {
namespace log {
/**
 * @brief 抽象标志 纯虚基类
 *
 */
class AbstractFlag {
public:
    AbstractFlag()          = default;
    virtual ~AbstractFlag() = default;

    /**
     * @brief 格式化文本内容
     *
     * @param info 日志信息
     * @param buffer 缓冲区
     */
    virtual void format(const LogInfo& info, std::string& buffer) = 0;
};

/**
 * @brief 简短工作日标志(Mon)
 *
 */
class ShortWeekdayFlag final : public AbstractFlag {
public:
    ShortWeekdayFlag() = default;

    void format(const LogInfo& info, std::string& buffer) override;
};

/**
 * @brief 工作日标志(Monday)
 *
 */
class WeekdayFlag final : public AbstractFlag {
public:
    WeekdayFlag() = default;

    void format(const LogInfo& info, std::string& buffer) override;
};

/**
 * @brief 简短月份字符串标志(Jan)
 *
 */
class ShortMonthStringFlag final : public AbstractFlag {
public:
    ShortMonthStringFlag() = default;

    void format(const LogInfo& info, std::string& buffer) override;
};

/**
 * @brief 月份字符串标志(January)
 *
 */
class MonthStringFlag final : public AbstractFlag {
public:
    MonthStringFlag() = default;

    void format(const LogInfo& info, std::string& buffer) override;
};

/**
 * @brief 标准日期时间标志(Tue Aug 31 10:15:42 2021)
 *
 */
class StandardDateTimeFlag final : public AbstractFlag {
public:
    StandardDateTimeFlag() = default;

    void format(const LogInfo& info, std::string& buffer) override;
};

/**
 * @brief 日志名称标志
 *
 */
class NameFlag final : public AbstractFlag {
public:
    NameFlag() = default;

    void format(const LogInfo& info, std::string& buffer) override;
};

/**
 * @brief 简短日期标志(12/25/20)
 *
 */
class ShortDateFlag final : public AbstractFlag {
public:
    ShortDateFlag() = default;

    void format(const LogInfo& info, std::string& buffer) override;
};

/**
 * @brief 十进制表示的每月的第几天标志(01-31)
 *
 */
class DayFlag final : public AbstractFlag {
public:
    DayFlag() = default;

    void format(const LogInfo& info, std::string& buffer) override;
};

/**
 * @brief 十进制表示的毫秒数标志(001-999)
 *
 */
class MillisecondsFlag final : public AbstractFlag {
public:
    MillisecondsFlag() = default;

    void format(const LogInfo& info, std::string& buffer) override;
};

/**
 * @brief 线程号标志
 *
 */
class ThreadIdFlag final : public AbstractFlag {
public:
    ThreadIdFlag() = default;

    void format(const LogInfo& info, std::string& buffer) override;
};

/**
 * @brief 十进制表示的微秒数标志(000001-999999)
 *
 */
class MicrosecondsFlag final : public AbstractFlag {
public:
    MicrosecondsFlag() = default;

    void format(const LogInfo& info, std::string& buffer) override;
};

/**
 * @brief 日期标志(2020-12-25)
 *
 */
class DateFlag final : public AbstractFlag {
public:
    DateFlag() = default;

    void format(const LogInfo& info, std::string& buffer) override;
};

/**
 * @brief 自纪元以来的秒数标志
 *
 */
class SecondsSinceEpochFlag final : public AbstractFlag {
public:
    SecondsSinceEpochFlag() = default;

    void format(const LogInfo& info, std::string& buffer) override;
};

/**
 * @brief 源文件名称标志(main.cpp)
 *
 */
class SourceFileNameFlag final : public AbstractFlag {
public:
    SourceFileNameFlag() = default;

    void format(const LogInfo& info, std::string& buffer) override;
};

/**
 * @brief 24小时制的时钟标志(01-24)
 *
 */
class HourFlag final : public AbstractFlag {
public:
    HourFlag() = default;

    void format(const LogInfo& info, std::string& buffer) override;
};

/**
 * @brief 12小时制的时钟标志(01-12)
 *
 */
class Hour12Flag final : public AbstractFlag {
public:
    Hour12Flag() = default;

    void format(const LogInfo& info, std::string& buffer) override;
};

/**
 * @brief 源文件完整或相对路径标志(/path/main.cpp)
 *
 */
class SourceFilePathFlag final : public AbstractFlag {
public:
    SourceFilePathFlag() = default;

    void format(const LogInfo& info, std::string& buffer) override;
};

/**
 * @brief 源文件所在行数标志
 *
 */
class SourceFileLineFlag final : public AbstractFlag {
public:
    SourceFileLineFlag() = default;

    void format(const LogInfo& info, std::string& buffer) override;
};

/**
 * @brief 十进制表示的每年的第几天标志(001-366)
 *
 */
class DayOfYearFlag final : public AbstractFlag {
public:
    DayOfYearFlag() = default;

    void format(const LogInfo& info, std::string& buffer) override;
};

/**
 * @brief 源文件所在函数名
 *
 */
class SourceFileFuncNameFlag final : public AbstractFlag {
public:
    SourceFileFuncNameFlag() = default;

    void format(const LogInfo& info, std::string& buffer) override;
};

/**
 * @brief 源文件名称及所在行数标志(main.cpp:245)
 *
 */
class SourceFileLocationFlag final : public AbstractFlag {
public:
    SourceFileLocationFlag() = default;

    void format(const LogInfo& info, std::string& buffer) override;
};

/**
 * @brief 完整源文件位置及所在行数标志(/path/main.cpp:245)
 *
 */
class FullSourceFileLocationFlag final : public AbstractFlag {
public:
    FullSourceFileLocationFlag() = default;

    void format(const LogInfo& info, std::string& buffer) override;
};

/**
 * @brief 日志级别简写标志(I)
 *
 */
class ShortLogLevelFlag final : public AbstractFlag {
public:
    ShortLogLevelFlag() = default;

    void format(const LogInfo& info, std::string& buffer) override;
};

/**
 * @brief 日志级别标志(Info)
 *
 */
class LogLevelFlag final : public AbstractFlag {
public:
    LogLevelFlag() = default;

    void format(const LogInfo& info, std::string& buffer) override;
};

/**
 * @brief 十进制表示的月份标志(01-12)
 *
 */
class MonthFlag final : public AbstractFlag {
public:
    MonthFlag() = default;

    void format(const LogInfo& info, std::string& buffer) override;
};

/**
 * @brief 十进制表示的分钟数标志(00-59)
 *
 */
class MinuteFlag final : public AbstractFlag {
public:
    MinuteFlag() = default;

    void format(const LogInfo& info, std::string& buffer) override;
};

/**
 * @brief 换行符标志(\n)
 *
 */
class NewLineFlag final : public AbstractFlag {
public:
    NewLineFlag() = default;

    void format(const LogInfo& info, std::string& buffer) override;
};

/**
 * @brief 十进制表示的纳秒数标志(000000001-999999999)
 *
 */
class NanosecondsFlag final : public AbstractFlag {
public:
    NanosecondsFlag() = default;

    void format(const LogInfo& info, std::string& buffer) override;
};

/**
 * @brief AM/PM标志
 *
 */
class AmPmFlag final : public AbstractFlag {
public:
    AmPmFlag() = default;

    void format(const LogInfo& info, std::string& buffer) override;
};

/**
 * @brief 进程号标志
 *
 */
class ProcessIdFlag final : public AbstractFlag {
public:
    ProcessIdFlag() = default;

    void format(const LogInfo& info, std::string& buffer) override;
};

/**
 * @brief 12小时制的时间标志(08:30:45 AM)
 *
 */
class Time12Flag final : public AbstractFlag {
public:
    Time12Flag() = default;

    void format(const LogInfo& info, std::string& buffer) override;
};

/**
 * @brief 小时和分钟标志(08:30)
 *
 */
class HourAndMinuteFlag final : public AbstractFlag {
public:
    HourAndMinuteFlag() = default;

    void format(const LogInfo& info, std::string& buffer) override;
};

/**
 * @brief 十进制表示的秒数标志(00-59)
 *
 */
class SecondsFlag final : public AbstractFlag {
public:
    SecondsFlag() = default;

    void format(const LogInfo& info, std::string& buffer) override;
};

/**
 * @brief 水平制表符标志(\t)
 *
 */
class TabFlag final : public AbstractFlag {
public:
    TabFlag() = default;

    void format(const LogInfo& info, std::string& buffer) override;
};

/**
 * @brief 时间标志(22:45:34)
 *
 */
class TimeFlag final : public AbstractFlag {
public:
    TimeFlag() = default;

    void format(const LogInfo& info, std::string& buffer) override;
};

/**
 * @brief 日志消息标志
 *
 */
class LogMessageFlag final : public AbstractFlag {
public:
    LogMessageFlag() = default;

    void format(const LogInfo& info, std::string& buffer) override;
};

/**
 * @brief 不带世纪的十进制年份标志(22)
 *
 */
class ShortYearFlag final : public AbstractFlag {
public:
    ShortYearFlag() = default;

    void format(const LogInfo& info, std::string& buffer) override;
};

/**
 * @brief 带世纪的十进制年份标志(2022)
 *
 */
class YearFlag final : public AbstractFlag {
public:
    YearFlag() = default;

    void format(const LogInfo& info, std::string& buffer) override;
};

/**
 * @brief 时区标志(CST)
 *
 */
class ZoneFlag final : public AbstractFlag {
public:
    ZoneFlag() = default;

    void format(const LogInfo& info, std::string& buffer) override;
};

/**
 * @brief 百分号标志(%)
 *
 */
class PercentFlag final : public AbstractFlag {
public:
    PercentFlag() = default;

    void format(const LogInfo& info, std::string& buffer) override;
};

/**
 * @brief 默认形式标志([2020-12-20 14:32:45.867] [logger_name] [info] [130546] xxxxx)
 *
 */
class DefaultFlag final : public AbstractFlag {
public:
    DefaultFlag() = default;

    void format(const LogInfo& info, std::string& buffer) override;
};

/**
 * @brief 文本内容标志
 *
 */
class ContentFlag final : public AbstractFlag {
public:
    ContentFlag() = default;

    void format(const LogInfo& info, std::string& buffer) override;

    /** 追加字符 */
    void append(char ch);

private:
    std::string content_ { "" };
};

/**
 * @brief 日志格式化
 * @details 可用格式化如下：
 * %a - 工作日简写(Sun)
 * %A - 工作日全称(Sunday)
 * %b - 月份简写(Dec)
 * %B - 月份全称(December)
 * %c - 标准的日期字符串(Tue Aug 31 10:15:42 2021)
 * %C - 日志名称
 * %d - 月/天/年(12/25/20)
 * %D - 十进制表示的每月第几天
 * %e - 十进制表示的毫秒
 * %E - 线程号
 * %f - 十进制表示的微秒
 * %F - 年-月-日(2020-12-25)
 * %g - 自纪元时间以来的秒数
 * %G - 源文件名称
 * %h - 24小时制的小时
 * %H - 12小时制的小时
 * %i - 源文件的完整或相对路径
 * %I - 源文件所在行
 * %j - 十进制表示的每年的第几天
 * %J - 源文件所在函数
 * %k - 源文件:所在行(main.cpp:145)
 * %K - 完整源文件:所在行(/dir/main.cpp:145)
 * %l - 日志级别简写
 * %L - 日志级别全称
 * %m - 十进制表示的分钟数
 * %M - 十进制表示的月份
 * %n - 换行符
 * %N - 十进制表示的纳秒
 * %p - 本地的AM或PM的等价显示
 * %P - 进程号
 * %r - 12小时的时间(08:30:45 AM)
 * %R - 小时和分钟(14:30)
 * %s - 十进制表示的秒数
 * %t - 水平制表符
 * %T - 时分秒(14:30:45)
 * %v - 日志内容
 * %y - 不带世纪的十进制年份
 * %Y - 带世纪部分的十进制年份
 * %z - 时区简写
 * %% - 百分号
 * %# - 默认形式([2020-12-20 14:32:45.867] [logger_name] [info] [334523] xxxxx)
 */
class LogFormatter {
public:
    using AbstractFlagPtr = std::unique_ptr<AbstractFlag>;
    using FlagSequence    = std::vector<AbstractFlagPtr>;
    using FlagHash        = std::unordered_map<char, std::function<AbstractFlagPtr()>>;

    LogFormatter() = default;

    /**
     * @brief Construct a new Log Formatter object
     * 
     * @param pattern 模式串
     */
    explicit LogFormatter(std::string pattern);

    /**
     * @brief 格式化日志信息到缓冲区中
     *
     * @param info 日志信息
     * @param buffer 缓冲区
     */
    void format(const LogInfo& info, std::string& buffer);

    /** 设置模式串 */
    void setPattern(std::string pattern);

    /**
     * @brief 添加自定义格式化标志
     * 
     * @param flag 格式化标志
     * @param fun 回调函数，返回指向父类的指针
     * @return 若映射表中没有该格式化标志则返回true，否则返回false
     */
    bool addCustomFlag(char flag, std::function<AbstractFlagPtr()> fun);

    /** 移除格式化标志 */
    void removeFlag(char flag);

private:
    /** 解析模式串 */
    void parse();

    /** 处理格式化标志 */
    void handleFlag(char flag);

private:
    std::string  pattern_ { "%#" }; ///< 模式串
    FlagSequence flags_ {};         ///< 格式化标志序列

    static FlagHash hash_; ///< 格式化映射表
};
} // namespace log
} // namespace talko