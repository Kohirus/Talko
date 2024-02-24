#pragma once

#include <atomic>
#include <net/callbacks.h>

namespace talko::net {
/**
 * @brief 定时器
 *
 */
class Timer {
public:
    /**
     * @brief Construct a new Timer object
     *
     * @param sequence 定时器序号
     * @param cb 定时器任务
     * @param when 超时时间点
     * @param interval 重复触发的间隔时间，大于0则重复触发，等于0则只触发一次
     */
    Timer(uint64_t sequence, TimerCallback cb, TimePoint when, Duration interval);

    ~Timer();

    Timer(const Timer&)            = delete;
    Timer& operator=(const Timer&) = delete;

    /** 执行定时器任务 */
    void run() const;

    /** 获取到期时间点 */
    TimePoint expiration() const;

    /** 是否重复执行任务 */
    bool repeat() const;

    /** 重新启动定时器 */
    void restart(TimePoint now);

    /** 获取定时器序号 */
    uint64_t sequence() const;

private:
    const TimerCallback callback_; ///< 回调函数
    const Duration      interval_; ///< 间隔时间
    const bool          repeat_;   ///< 是否重复定时任务
    const uint64_t      sequence_; ///< 定时器序号

    TimePoint expiration_; ///< 到期时间点
};
} // namespace talko::net