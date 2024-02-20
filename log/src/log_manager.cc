#include <log/console_appender.h>
#include <log/log_formatter.h>
#include <log/log_manager.h>
#include <log/logger.h>

namespace talko {
namespace log {
LogManager& LogManager::instance() {
    static LogManager ins;
    return ins;
}

LogManager::LogManager()
    : formatter_(new LogFormatter("%#")) {
    auto cs_appender    = std::make_shared<ConsoleAppenderMt>();
    default_logger_     = std::make_shared<Logger>("default", std::move(cs_appender));
    loggers_["default"] = default_logger_;
}

bool LogManager::registerLogger(LoggerPtr logger) {
    std::lock_guard<std::mutex> lock(map_mtx_);

    auto it = loggers_.find(logger->name());
    if (it != loggers_.end()) {
        return false;
    }

    loggers_[logger->name()] = std::move(logger);
    return true;
}

LoggerPtr LogManager::get(const std::string& logger_name) {
    std::lock_guard<std::mutex> lock(map_mtx_);

    auto it = loggers_.find(logger_name);
    if (it == loggers_.end()) {
        return nullptr;
    }

    return it->second;
}

bool LogManager::exist(const std::string& logger_name) {
    std::lock_guard<std::mutex> lock(map_mtx_);

    auto it = loggers_.find(logger_name);
    return it != loggers_.end();
}

LoggerPtr LogManager::defaultLogger() {
    std::lock_guard<std::mutex> lock(map_mtx_);
    return default_logger_;
}

void LogManager::setDefaultLogger(LoggerPtr logger) {
    std::lock_guard<std::mutex> lock(map_mtx_);
    default_logger_ = std::move(logger);
    auto it         = loggers_.find("default");
    if (it != loggers_.end()) {
        it->second = default_logger_;
    } else {
        loggers_["default"] = default_logger_;
    }
}

void LogManager::remove(const std::string& logger_name) {
    std::lock_guard<std::mutex> lock(map_mtx_);

    auto it = loggers_.find(logger_name);
    if (it != loggers_.end()) {
        loggers_.erase(it);
    }
}

void LogManager::clear() {
    std::lock_guard<std::mutex> lock(map_mtx_);
    loggers_.clear();
    default_logger_.reset();
    default_logger_ = nullptr;
}

void LogManager::setFormatter(FormatterPtr formatter) {
    std::lock_guard<std::mutex> lock(map_mtx_);
    formatter_ = std::move(formatter);
    for (auto& logger : loggers_) {
        logger.second->setFormatter(formatter_->clone());
    }
}

void LogManager::setGlobalLevel(LogLevel lvl) {
    global_level_.store(lvl);
}

LogLevel LogManager::globalLevel() {
    return global_level_.load();
}
} // namespace log
} // namespace talko