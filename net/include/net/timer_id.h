#pragma once

// #include <cstdint>
#include <net/callbacks.h>

namespace talko::net {
class Timer;

/**
 * @brief 定时器编号
 *
 */
class TimerId {
public:
    TimerId() = default;

    /**
     * @brief Construct a new Timer Id object
     *
     * @param timer 定时器对象
     * @param seq 定时器序号
     */
    // TimerId(Timer* timer, int64_t seq)
    //     : timer_(timer)
    //     , sequence_(seq) { }
    TimerId(TimePoint expiration, uint64_t seq)
        : expiration_(expiration)
        , sequence_(seq) { }

    ~TimerId() = default;

    TimerId(const TimerId&)            = default;
    TimerId& operator=(const TimerId&) = default;

    bool operator<(const TimerId& other) const {
        if (expiration_ == other.expiration_) {
            return sequence_ < other.sequence_;
        }
        return expiration_ < other.expiration_;
    }

    friend class TimerQueue;

private:
    // Timer*  timer_ { nullptr }; ///< 所绑定的定时器
    TimePoint expiration_;     ///< 超时时间点
    uint64_t  sequence_ { 0 }; ///< 定时器序号
};
} // namespace talko::net