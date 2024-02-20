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
    logger->setPattern("[%c] [%C] [%l] [%k] %v");
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
    log::TerminalFont font(log::Color::Green, log::Color::Default, true, true, true);
    log::ConsoleAppender<log::RealMutex>::setLevelFont(log::LogLevel::info, font);
    log::info("info log...");
}

void registerLogger() {
    auto logger = log::createConsoleLoggerSt("test");
    logger->setLevel(log::LogLevel::debug);
    log::registerLogger(logger);
    std::cout << "Exist: " << log::exist("test") << std::endl;
    log::get("test")->debug("debug info");
    log::get("test")->info("info log");
    LOGGER_ERROR("test", "error log");
}

int main() {
    // mulitiThreadTest();
    singleThreadTest();
    // colorTest();
    // registerLogger();
    return 0;
}