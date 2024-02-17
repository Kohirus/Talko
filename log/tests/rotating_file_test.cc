#include <iostream>
#include <log/log.h>
using namespace talko;

void singleThreadTest(size_t log_num) {
    auto logger = log::createRotatingFileLoggerSt("rotating_file", "./rotating_log/log.txt", 1 * log::KB, 3);
    logger->setLevel(log::LogLevel::trace);
    log::setDefaultLogger(logger);

    for (size_t i = 0; i < log_num; ++i) {
        int a = 10, b = 20;
        LOG_TRACE("trace log: {}, {}", a, b);
        LOG_DEBUG("debug log: {}", a);
        LOG_INFO("info log");
        LOG_WARN("warn log: {}, {}", a, b);
        LOG_ERROR("error log");
        LOG_FATAL("fatal log");
    }
}

int main() {
    singleThreadTest(1000);
    return 0;
}