#include <log/log.h>

namespace talko::log {
LoggerPtr get(const std::string& name) {
    return LogManager::instance().get(name);
}

void setPattern(std::string pattern, bool use_utc) {
    auto formatter = std::make_unique<LogFormatter>(std::move(pattern), use_utc);
    LogManager::instance().setFormatter(std::move(formatter));
}

void setFormatter(FormatterPtr formatter) {
    LogManager::instance().setFormatter(std::move(formatter));
}

LogLevel globalLevel() {
    return LogManager::instance().globalLevel();
}

void setGlobalLevel(LogLevel level) {
    return LogManager::instance().setGlobalLevel(level);
}

bool registerLogger(LoggerPtr logger) {
    return LogManager::instance().registerLogger(std::move(logger));
}

void remove(const std::string& name) {
    LogManager::instance().remove(name);
}

bool exist(const std::string& name) {
    return LogManager::instance().exist(name);
}

void clear() {
    LogManager::instance().clear();
}

LoggerPtr defaultLogger() {
    return LogManager::instance().defaultLogger();
}

void setDefaultLogger(LoggerPtr logger) {
    LogManager::instance().setDefaultLogger(std::move(logger));
}
} // namespace talko::log