#pragma once

#include <cstdio>
#include <string>

namespace talko::log {
/**
 * @brief 基础策略，不保证线程安全
 *
 */
class BaseStrategy {
public:
    BaseStrategy()          = default;
    virtual ~BaseStrategy() = default;

    BaseStrategy(const BaseStrategy&)            = delete;
    BaseStrategy& operator=(const BaseStrategy&) = delete;

    BaseStrategy(BaseStrategy&&)            = delete;
    BaseStrategy& operator=(BaseStrategy&&) = delete;

    /** 输出内容 */
    virtual void output(std::string_view data) = 0;

protected:
    FILE* handler_ { nullptr }; ///< 文件句柄
};

/**
 * @brief 同步策略
 *
 */
class SyncStrategy : public BaseStrategy {
public:
    SyncStrategy();
    ~SyncStrategy() = default;

    void output(std::string_view data) override;
};

/**
 * @brief 异步I/O策略
 *
 */
class AsyncStrategy : public BaseStrategy {
    // TODO: 实现异步策略机制
};

/**
 * @brief 无锁队列策略
 *
 */
class QueueStrategy : public BaseStrategy {
    // TODO: 实现无锁队列机制
};

/**
 * @brief 内存映射机制
 *
 */
class MmapStrategy : public BaseStrategy {
    // TODO: 实现内存映射机制
};
} // namespace talko::log