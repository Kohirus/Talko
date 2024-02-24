#pragma once

#include <atomic>
#include <map>
#include <memory>
#include <net/callbacks.h>
#include <net/timer_id.h>
#include <set>

namespace talko::net {
class EventLoop;
class Channel;
class Timer;

class TimerQueue {
public:
    explicit TimerQueue(EventLoop* loop);
    ~TimerQueue();

    /**
     * @brief 添加定时器任务
     *
     * @param cb 定时器任务
     * @param when 超时时间点
     * @param interval 重复触发的间隔时间，单位为ms，大于0则重复触发，等于0则只触发一次
     * @return TimerId 返回定时器编号
     */
    TimerId append(TimerCallback cb, TimePoint when, Duration interval);

    /** 取消定时器任务 */
    void cancel(TimerId timer_id);

private:
    using ChannelPtr    = std::unique_ptr<Channel>;
    using TimerPtr      = std::unique_ptr<Timer>;
    using TimerList     = std::map<TimerId, TimerPtr>;
    using TimerSet      = std::set<uint64_t>;
    using TimerHandlers = std::map<uint64_t, TimerList::const_iterator>;

    /** 处理定时器上的可读事件 */
    void handleRead();

    /**
     * @brief 添加定时器
     *
     * @param sequence 定时器序号
     * @param cb 定时器任务
     * @param when 超时时间点
     * @param interval 重复触发的间隔时间
     */
    void append_(uint64_t sequence, TimerCallback cb, TimePoint when, Duration interval);

    /** 取消定时器任务 */
    void cancel_(TimerId timer_id);

    /** 插入定时器，如果最早到期时间发生改变则返回true，否则返回false */
    bool insert(TimerPtr timer);

    /** 获取过期的定时器序列 */
    std::vector<TimerPtr> getExpiredTimers(TimePoint now);

    /** 重置过期的定时器序列 */
    void reset(std::vector<TimerPtr>& expired, TimePoint now);

private:
    EventLoop*    loop_;             ///< 事件循环
    const int     timer_fd_;         ///< 定时器描述符
    ChannelPtr    timer_channel_;    ///< 定时器通道
    TimerList     timers_;           ///< 定时器序列
    TimerHandlers handlers_;         ///< 定时器句柄序列，用于取消定时器
    TimerSet      canceling_timers_; ///< 取消的定时器集合

    std::atomic_uint64_t sequence_ { 0 }; ///< 定时器序号

    bool handling_expired_timers_; ///< 是否正在处理超时定时器
};
} // namespace talko::net