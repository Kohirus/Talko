#include <chrono>
#include <iostream>
#include <log/log.h>
using namespace talko;

void mulitThreadTest(bool rewrite, size_t log_num, size_t thread_num) {
    auto logger = log::createFileLoggerMt("filelog", "./log/log.txt", rewrite);
    log::setDefaultLogger(logger);

    auto func = [log_num]() {
        for (size_t i = 0; i < log_num; ++i) {
            log::info("info log");
        }
    };

    std::vector<std::thread> threads;

    for (size_t i = 0; i < thread_num; ++i) {
        threads.emplace_back(std::thread(func));
    }

    for (auto& thread : threads) {
        thread.join();
    }
}

void singleThreadTest(bool rewrite) {
    auto logger = log::createFileLoggerSt("filelog", "./log/log.txt", rewrite);
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

void multiThreadSyncTest(bool rewrite, size_t log_num, size_t thread_num) {
    pool::setThreadPoolMode(pool::ThreadPoolMode::dynamic);
    pool::startThreadPool();

    auto logger = log::createFileLoggerMt("filelog", "./log/log.txt", rewrite, true);
    log::setDefaultLogger(logger);

    auto func = [log_num]() {
        for (size_t i = 0; i < log_num; ++i) {
            log::info("info log");
        }
    };

    std::vector<std::thread> threads;

    for (size_t i = 0; i < thread_num; ++i) {
        threads.emplace_back(std::thread(func));
    }

    for (auto& thread : threads) {
        thread.join();
    }
}

int main() {
    auto start = std::chrono::high_resolution_clock::now();

    // singleThreadTest(true);
    // mulitThreadTest(true, 5000, 10);
    multiThreadSyncTest(true, 5000, 10);

    auto end      = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << "Function execution time: " << duration.count() << " us" << std::endl;
    return 0;
}