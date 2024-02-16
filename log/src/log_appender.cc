#include <log/log_appender.h>
#include <log/log_formatter.h>

namespace talko::log {
LogAppender::LogAppender(bool async)
    : output_(nullptr, async) {
}

void LogAppender::setLevel(LogLevel lvl) {
    level_.store(lvl);
}

LogLevel LogAppender::level() const {
    return level_.load();
}

bool LogAppender::shouldLog(LogLevel lvl) const {
    return lvl >= level_.load();
}
} // namespace talko::log