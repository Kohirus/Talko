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
     * @param expiration 超时时间点
     * @param seq 定时器序号
     */
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
    TimePoint expiration_;     ///< 超时时间点
    uint64_t  sequence_ { 0 }; ///< 定时器序号
};
} // namespace talko::net