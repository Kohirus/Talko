#include <log/log.h>
#include <net/event_loop.h>
using namespace talko;

void timerFuncOnce() {
    log::info("timer run at");
}

void timerFuncAfter() {
    log::info("timer run after");
}

void timerFuncEvery() {
    log::info("timer run every");
}

int main() {
    log::defaultLogger()->setLevel(log::LogLevel::debug);
    log::defaultLogger()->setPattern("[%T.%f] [%C] [%l] [%E] %v");

    net::EventLoop loop;

    net::TimePoint time = std::chrono::high_resolution_clock::now() +
        std::chrono::seconds(3);

    loop.runAt(time, timerFuncOnce);
    loop.runAfter(std::chrono::seconds(4), timerFuncAfter);
    loop.runEvery(std::chrono::seconds(1), timerFuncEvery);

    loop.loop();
}