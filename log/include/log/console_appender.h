#pragma once

#include <log/factory.h>
#include <log/log_appender.h>
#include <log/log_formatter.h>
#include <unistd.h>
#include <unordered_map>
#include <utility>

namespace talko::log {
/** 终端颜色 */
enum class Color {
    Black = 0, ///< 黑色
    Red,       ///< 红色
    Green,     ///< 绿色
    Yellow,    ///< 黄色
    Blue,      ///< 蓝色
    Purple,    ///< 紫色
    Cyan,      ///< 青色
    White,     ///< 白色
    Default    ///< 默认
};

/** 将字符串转为颜色枚举变量 */
inline Color fromString(const std::string& str) {
    if (str == "black") {
        return Color::Black;
    } else if (str == "red") {
        return Color::Red;
    } else if (str == "green") {
        return Color::Green;
    } else if (str == "yellow") {
        return Color::Yellow;
    } else if (str == "blue") {
        return Color::Blue;
    } else if (str == "purple") {
        return Color::Purple;
    } else if (str == "cyan") {
        return Color::Cyan;
    } else if (str == "white") {
        return Color::White;
    } else {
        return Color::Default;
    }
}

/**
 * @brief 终端字体属性
 *
 */
struct TerminalFont {
    TerminalFont() = default;

    /**
     * @brief Construct a new Terminal Font object
     *
     * @param fore 前景色
     * @param back 背景色
     * @param b 粗体
     * @param i 斜体
     * @param u 下划线
     */
    TerminalFont(Color fore, Color back, bool b, bool i, bool u)
        : foreground(fore)
        , background(back)
        , bold(b)
        , italic(i)
        , underline(u) { }

    TerminalFont(Color fore, Color back)
        : TerminalFont(fore, back, false, false, false) { }

    TerminalFont(Color fore)
        : TerminalFont(fore, Color::Default) { }

    Color foreground { Color::Default }; ///< 前景色
    Color background { Color::Default }; ///< 背景色
    bool  bold { false };                ///< 粗体
    bool  italic { false };              ///< 斜体
    bool  underline { false };           ///< 下划线
};

using LevelFont = std::unordered_map<LogLevel, TerminalFont>;

/**
 * @brief 终端输出器
 *
 * @tparam MutexType 互斥器类型
 */
template <typename MutexType>
class ConsoleAppender : public LogAppender {
public:
    ConsoleAppender(bool async = false)
        : LogAppender(async)
        , formatter_(std::make_unique<LogFormatter>("%#")) {
        output_.setHandler(stdout);
    }
    ~ConsoleAppender() = default;

    ConsoleAppender(const ConsoleAppender&)            = delete;
    ConsoleAppender& operator=(const ConsoleAppender&) = delete;

    /** 输出日志 */
    void log(const LogInfo& info) override {
        std::lock_guard<MutexType> lock(mtx_);

        // 格式化日志消息
        std::string formatted, out_str;
        formatter_->format(info, formatted);

        // 获取终端输出的前缀和后缀 输出颜色
        auto [prefix, suffix] = compileColor(info.level);
        out_str.append(prefix);
        out_str.append(formatted);
        if (out_str.back() == '\n') {
            out_str.pop_back();
            out_str.append(suffix);
            out_str.push_back('\n');
        } else {
            out_str.append(suffix);
        }

        // 输出日志
        output_.output(out_str);
        output_.flush();
    }

    /** 设置格式化串 */
    void setPattern(const std::string& pattern) override {
        std::lock_guard<MutexType> lock(mtx_);
        formatter_ = std::make_unique<LogFormatter>(pattern);
    }

    /** 设置格式化器 */
    void setFormatter(FormatterPtr formatter) override {
        std::lock_guard<MutexType> lock(mtx_);
        formatter_ = std::move(formatter);
    }

    /** 设置指定等级的字体属性 */
    static void setLevelFont(LogLevel lvl, const TerminalFont& propetry) {
        propetry_[lvl] = propetry;
    }

private:
    /** 编译指定等级的颜色，返回终端输出所需的前缀和后缀 */
    static std::pair<std::string, std::string> compileColor(LogLevel lvl) {
        std::string front = "\033[";

        auto& prop = propetry_[lvl];
        if (prop.bold) front.append("1;");
        if (prop.italic) front.append("3;");
        if (prop.underline) front.append("4;");
        if (prop.foreground != Color::Default) {
            front.append(std::to_string(static_cast<int>(prop.foreground) + 30));
            front.push_back(';');
        }
        if (prop.background != Color::Default) {
            front.append(std::to_string(static_cast<int>(prop.background) + 40));
            front.push_back(';');
        }

        front[front.size() - 1] = 'm';

        return { front, "\033[0m" };
    }

private:
    MutexType        mtx_;       ///< 互斥锁
    FormatterPtr     formatter_; ///< 格式化器
    static LevelFont propetry_;  ///< 终端字体属性映射表
};

template <typename MutexType>
LevelFont ConsoleAppender<MutexType>::propetry_ = {
    { LogLevel::trace, TerminalFont {} },
    { LogLevel::debug, TerminalFont(Color::Blue) },
    { LogLevel::info, TerminalFont(Color::Green) },
    { LogLevel::warn, TerminalFont(Color::Yellow, Color::Default, false, false, true) },
    { LogLevel::error, TerminalFont(Color::Red, Color::Default, true, false, false) },
    { LogLevel::fatal, TerminalFont(Color::Red, Color::Default, true, true, true) },
    { LogLevel::off, TerminalFont {} },
};

using ConsoleAppenderSt = ConsoleAppender<FakeMutex>;
using ConsoleAppenderMt = ConsoleAppender<RealMutex>;

/**
 * @brief 获取单线程控制台同步日志器
 *
 * @param logger_name 日志名称
 * @return std::shared_ptr<Logger> 返回控制台日志器
 */
inline std::shared_ptr<Logger> createConsoleLoggerSt(const std::string& logger_name) {
    return Factory::create<ConsoleAppenderSt>(logger_name);
}

/**
 * @brief 获取多线程控制台同步日志器
 *
 * @param logger_name 日志名称
 * @return std::shared_ptr<Logger> 返回控制台日志器
 */
inline std::shared_ptr<Logger> createConsoleLoggerMt(const std::string& logger_name) {
    return Factory::create<ConsoleAppenderMt>(logger_name);
}
} // namespace talko::log