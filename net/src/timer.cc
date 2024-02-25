#include <log/log.h>
#include <net/timer.h>
#include <utils/datetime.h>

namespace talko::net {
Timer::Timer(uint64_t sequence, TimerCallback cb, TimePoint when, Duration interval)
    : sequence_(sequence)
    , callback_(std::move(cb))
    , expiration_(when)
    , interval_(interval)
    , repeat_(interval.count() > 0) {
    LOGGER_TRACE("net", "Create new Timer[{}], Sequence: {}, Expiration: {}, Interval: {} ms",
        fmt::ptr(this), sequence_, utils::DateTime::toString(when, utils::StringType::DateTimeMilli),
        interval.count());
}

Timer::~Timer() {
    LOGGER_TRACE("net", "Destory Timer[{}]", fmt::ptr(this));
}

void Timer::run() const {
    callback_();
}

TimePoint Timer::expiration() const {
    return expiration_;
}

bool Timer::repeat() const {
    return repeat_;
}

void Timer::restart(TimePoint now) {
    if (repeat_) {
        expiration_ = now + interval_;
    }
}

uint64_t Timer::sequence() const {
    return sequence_;
}
} // namespace talko::net