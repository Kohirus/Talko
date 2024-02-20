#pragma once

#include <net/callbacks.h>

namespace talko::net {
class EventLoop;

/** 通道状态 */
enum class Status {
    New,     ///< 未添加
    Added,   ///< 已添加
    Deleted, ///< 已删除
};

/**
 * @brief 通道类
 *
 */
class Channel {
public:
    using EventCallback     = std::function<void()>;
    using ReadEventCallback = std::function<void(TimePoint)>;

    /**
     * @brief Construct a new Channel object
     *
     * @param loop 事件循环
     * @param fd 文件描述符
     */
    Channel(EventLoop* loop, int fd);
    ~Channel();

    Channel(const Channel&)            = delete;
    Channel& operator=(const Channel&) = delete;

    Channel(Channel&&)            = delete;
    Channel& operator=(Channel&&) = delete;

    /** 处理事件 */
    void handleEvent(TimePoint receive_time);

    /** 设置读回调方法 */
    void setReadCallback(ReadEventCallback cb);

    /** 设置写回调方法 */
    void setWriteCallback(EventCallback cb);

    /** 设置关闭回调方法 */
    void setCloseCallback(EventCallback cb);

    /** 设置错误回调方法 */
    void setErrorCallback(EventCallback cb);

    /** 获取文件描述符 */
    int fd() const;

    /** 获取感兴趣的事件 */
    int events() const;

    /** 设置实际发生的事件 */
    void setRevents(int revt);

    /** 是否对任何事件都不感兴趣 */
    bool isNoneEvent() const;

    /** 是否对可读事件感兴趣 */
    bool isReading() const;

    /** 是否对可写事件感兴趣 */
    bool isWriting() const;

    /** 对可读事件感兴趣 */
    void enableReading(bool enabled = true);

    /** 对可写事件感兴趣 */
    void enableWriting(bool enabled = true);

    /** 关闭所有事件 */
    void disableAll();

    /** 获取通道状态 */
    Status status() const;

    /** 设置状态 */
    void setStatus(Status status);

    /** 防止在 handleEvent() 时对象被析构以延长其生命期 */
    void ensureAlive(const std::shared_ptr<void>& obj);

    /** 从事件循环中移除 */
    void remove();

    /** 获取所属的事件循环 */
    EventLoop* ownerLoop() const;

private:
    /** 更新事件 */
    void update();

    /** 处理描述符上发生的事件 */
    void handleEvent_(TimePoint receive_time);

private:
    static const int kNoneEvent;  ///< 无事件
    static const int kReadEvent;  ///< 读事件
    static const int kWriteEvent; ///< 写事件

    EventLoop* loop_; ///< 事件循环
    const int  fd_;   ///< 文件描述符

    int events_;  ///< 感兴趣的事件
    int revents_; ///< 实际发生的事件

    Status status_ { Status::New }; ///< 通道状态

    bool event_handling_ { false }; ///< 是否正在处理事件
    bool added_to_loop_ { false };  ///< 是否被添加进事件循环
    bool prolonged_ { false };      ///< 用于延长Channel对象的生命期

    std::weak_ptr<void> prolong_; ///< 用于延长Channel对象的生命期

    ReadEventCallback read_callback_ {};  ///< 读回调方法
    EventCallback     write_callback_ {}; ///< 写回调方法
    EventCallback     close_callback_ {}; ///< 关闭回调方法
    EventCallback     error_callback_ {}; ///< 错误回调方法
};
} // namespace talko::net