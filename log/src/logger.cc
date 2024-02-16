#include <log/log_appender.h>
#include <log/log_formatter.h>
#include <log/logger.h>

namespace talko::log {
Logger::Logger(std::string logger_name)
    : name_(std::move(logger_name)) {
}

Logger::Logger(std::string logger_name, AppenderPtr appender)
    : name_(std::move(logger_name))
    , appenders_({ appender }) {
}

Logger::Logger(std::string logger_name, AppenderInitList list)
    : Logger(std::move(logger_name), list.begin(), list.end()) {
}

Logger::Logger(const Logger& other)
    : name_(other.name_)
    , appenders_(other.appenders_)
    , level_(other.level_.load()) {
}

Logger& Logger::operator=(const Logger& other) {
    if (this != &other) {
        name_        = other.name_;
        appenders_   = other.appenders_;
        level_       = other.level_.load();
    }
    return *this;
}

Logger::Logger(Logger&& other) noexcept
    : name_(std::move(other.name_))
    , appenders_(std::move(other.appenders_))
    , level_(other.level_.load()) {
}

Logger& Logger::operator=(Logger&& other) noexcept {
    this->swap(other);
    return *this;
}

void Logger::swap(Logger& other) noexcept {
    name_.swap(other.name_);
    appenders_.swap(other.appenders_);
    auto other_lvl = other.level_.load();
    auto my_lvl    = level_.exchange(other_lvl);
    other.level_.store(my_lvl);
}

void Logger::setPattern(std::string pattern, bool use_utc) {
    auto formatter = std::make_unique<LogFormatter>(std::move(pattern), use_utc);
    setFormatter(std::move(formatter));
}

void Logger::setFormatter(FormatterPtr formatter) {
    auto end = appenders_.end();
    for (auto it = appenders_.begin(); it != end; ++it) {
        if (std::next(it) == end) {
            (*it)->setFormatter(std::move(formatter));
        } else {
            (*it)->setFormatter(formatter->clone());
        }
    }
}

void Logger::setLevel(LogLevel lvl) {
    level_.store(lvl);
}

LogLevel Logger::level() const {
    return level_.load();
}

bool Logger::shouldLog(LogLevel lvl) const {
    return lvl >= level_.load();
}

LoggerPtr Logger::clone(const std::string& logger_name) {
    auto cloned   = std::make_shared<Logger>(*this);
    cloned->name_ = logger_name;
    return cloned;
}

void Logger::log(utils::TimePoint time_point, LogLocation loc, LogLevel lvl, const std::string& msg) {
    if (!shouldLog(lvl)) return;
    LogInfo info(time_point, loc, name_, lvl, msg);
    log_(info);
}

void Logger::log(LogLocation loc, LogLevel lvl, const std::string& msg) {
    if (!shouldLog(lvl)) return;
    LogInfo info(loc, name_, lvl, msg);
    log_(info);
}

void Logger::log_(const LogInfo& info) {
    for (auto& appender : appenders_) {
        if (appender->shouldLog(info.level)) {
            appender->log(info);
        }
    }
}

} // namespace talko::log