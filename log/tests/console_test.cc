#include <log/console_appender.h>
#include <log/log.h>
#include <thread>
using namespace talko;

/** 多线程测试 */
void mulitiThreadTest() {
    auto func = []() {
        int a = 10;
        log::trace("trace log");
        log::debug("debug log: {}", a);
        log::info("info log");
        log::warn("warn log: {}, {}", a, a);
        log::error("error log");
        log::fatal("fatal log");
    };

    std::thread th1(func);
    std::thread th2(func);
    std::thread th3(func);

    th1.join();
    th2.join();
    th3.join();
}

/** 单线程测试 */
void singleThreadTest() {
    auto logger = log::createConsoleLoggerSt("default");
    logger->setLevel(log::LogLevel::trace);
    log::setDefaultLogger(logger);
    int a = 10, b = 20;
    LOG_TRACE("trace log: {}, {}", a, b);
    LOG_DEBUG("debug log: {}", a);
    LOG_INFO("info log");
    LOG_WARN("warn log: {}, {}", a, b);
    LOG_ERROR("error log");
    LOG_FATAL("fatal log");
}

/** 颜色更改测试 */
void colorTest() {
}

int main() {
    // mulitiThreadTest();
    singleThreadTest();
    return 0;
}