#pragma once

#include <atomic>
#include <functional>
#include <memory>
#include <mutex>
#include <net/callbacks.h>
#include <net/timer_id.h>
#include <vector>

namespace talko::net {
class EpollPoller;
class Channel;
class TimerQueue;

/**
 * @brief 事件循环
 *
 */
class EventLoop {
public:
    using Functor = std::function<void()>;

    EventLoop();
    ~EventLoop();

    EventLoop(const EventLoop&)            = delete;
    EventLoop& operator=(const EventLoop&) = delete;

    EventLoop(EventLoop&&)            = delete;
    EventLoop& operator=(EventLoop&&) = delete;

    /** 开启事件循环 */
    void loop();

    /** 退出事件循环 */
    void quit();

    /** 唤醒当前事件循环 */
    void wakeup();

    /** 获取poll的返回时间 */
    TimePoint pollReturnTime() const;

    /** 立即在事件循环中执行 */
    void runInLoop(Functor cb);

    /** 在poll完成后执行 */
    void queueInLoop(Functor cb);

    /** 返回待处理事件队列的大小 */
    size_t queueSize();

    /** 在指定时间点执行任务 */
    TimerId runAt(TimePoint time, TimerCallback cb);

    /** 在指定的延迟毫秒数后执行任务 */
    TimerId runAfter(Duration delay, TimerCallback cb);

    /** 每间隔指定的毫秒数执行一次任务 */
    TimerId runEvery(Duration interval, TimerCallback cb);

    /** 取消指定的定时器 */
    void cancel(TimerId timer_id);

    /** 更新通道事件 */
    void updateChannel(Channel* channel);

    /** 将通道从事件循环中移除 */
    void removeChannel(Channel* channel);

    /** 是否拥有指定通道 */
    bool hasChannel(Channel* channel) const;

    /** 是否正在处理事件 */
    bool isHandlingEvent() const;

    /** 是否处于创建者线程中 */
    bool isInCreatorThread() const;

    /** 检查是否处于创建者线程 */
    void checkIsInCreatorThread() const;

private:
    /** 处理可读事件以唤醒当前事件循环 */
    void handleReadToWakeUp();

    /** 处理待处理的事件集合 */
    void handlePendingFunctors();

private:
    using ChannelList = std::vector<Channel*>;

    std::atomic_bool quit_ { false }; ///< 是否退出循环

    bool handling_events_ { false };          ///< 是否正在处理事件
    bool looping_ { false };                  ///< 是否正在循环
    bool calling_pending_functors_ { false }; ///< 正在调用待处理事件

    int  wakeup_fd_; ///< 唤醒描述符
    long thread_id_; ///< 创建线程的编号

    std::unique_ptr<EpollPoller> poller_;         ///< I/O复用模型
    std::unique_ptr<Channel>     wakeup_channel_; ///< 唤醒通道
    std::unique_ptr<TimerQueue>  timer_queue_;    ///< 定时器序列
    std::mutex                   mtx_;            ///< 保护待处理事件队列的线程安全

    ChannelList active_channels_;                ///< 活动事件列表
    Channel*    cur_active_channel_ { nullptr }; ///< 当前活动事件
    TimePoint   poll_return_time_;               ///< poll返回时间点

    std::vector<Functor> pending_functors_; ///< 待处理的事件队列
};
} // namespace talko::net