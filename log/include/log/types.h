#pragma once

#include <atomic>
#include <functional>
#include <initializer_list>
#include <log/log_level.h>
#include <memory>
#include <vector>
#include <mutex>

namespace talko {
namespace log {
class Logger;
class LogFormatter;
class LogAppender;

using LoggerPtr        = std::shared_ptr<Logger>;
using AppenderPtr      = std::shared_ptr<LogAppender>;
using FormatterPtr     = std::shared_ptr<LogFormatter>;
using AppenderList     = std::vector<AppenderPtr>;
using AppenderInitList = std::initializer_list<AppenderPtr>;
using AtomicLevel      = std::atomic<LogLevel>;

/**
 * @brief 虚假互斥锁
 * 
 */
struct FakeMutex {
    void lock() const {}
    void unlock() const {}
};

using RealMutex = std::mutex;
} // namespace log
} // namespace talko