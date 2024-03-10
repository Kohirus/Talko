#pragma once

#include <atomic>
#include <condition_variable>
#include <future>
#include <memory>
#include <mutex>
#include <pool/connection_info.h>
#include <queue>

namespace talko::pool {
class MysqlConnection;
class RedisConnection;

using MysqlConnectionPtr = std::shared_ptr<MysqlConnection>;
using RedisConnectionPtr = std::shared_ptr<RedisConnection>;

/**
 * @brief 数据库连接池，需要开启线程池
 *
 */
class ConnectionPool {
public:
    /** 获取连接池实例对象 */
    static ConnectionPool& instance();

    /**
     * @brief 启动数据库连接池
     *
     * @param mysql_info MySQL数据库的连接信息
     * @param redis_info Redis数据库的连接信息
     */
    void start(const ConnectionInfo& mysql_info, const ConnectionInfo& redis_info);

    /** 关闭数据库连接池 */
    void stop();

    /** 数据库连接池是否正在运行 */
    bool isRunning();

    /** 获取MySQL连接数量 */
    size_t getMysqlConnectionSize();

    /** 获取Redis连接数量 */
    size_t getRedisConnectionSize();

    /** 获取MySQL空闲连接数量 */
    size_t getMysqlIdleConnectionSize();

    /** 获取Redis空闲连接数量 */
    size_t getRedisIdleConnectionSize();

    /** 获取空闲的MySQL连接 */
    MysqlConnectionPtr getMysqlConnection();

    /** 获取空闲的Redis连接 */
    RedisConnectionPtr getRedisConnection();

private:
    ConnectionPool();
    ~ConnectionPool();

    /** 创建新的MySQL连接 */
    void createNewConnection();

    /** 移除空闲连接 */
    void removeIdleConnection();

private:
    std::atomic_bool running_ { false }; /// 是否正在运行连接池

    ConnectionInfo mysql_info_; ///< MySQL数据库的连接信息
    ConnectionInfo redis_info_; ///< Redis数据库的连接信息

    std::queue<MysqlConnection*> mysql_que_; ///< MySQL连接队列
    std::queue<RedisConnection*> redis_que_; ///< Redis连接队列

    std::atomic_size_t mysql_conn_size_; ///< MySQL连接数量
    std::atomic_size_t redis_conn_size_; ///< Redis连接数量

    std::mutex mysql_que_mtx_; ///< 保护MySQL连接队列的线程安全
    std::mutex redis_que_mtx_; ///< 保护Redis连接队列的线程安全

    std::condition_variable mysql_cv_; ///< 用于MySQL连接队列的条件变量
    std::condition_variable redis_cv_; ///< 用于Redis连接队列的条件变量

    const size_t kCheckInterval_; ///< 移除空闲连接的间隔时长
    const size_t kCreateTimeout_; ///< 等待消费者消费连接的超时时间

    std::future<decltype(void())> create_task_; ///< 创建连接任务
    std::future<decltype(void())> remove_task_; ///< 移除连接任务
};

/**
 * @brief 启动数据库连接池
 * @details 必须先启动线程池再启动连接池
 *
 * @param mysql_info MySQL数据库连接信息
 * @param redis_info Redis数据库连接信息
 */
void startConnectionPool(const ConnectionInfo& mysql_info, const ConnectionInfo& redis_info);

/** 停止连接池 */
void stopConnectionPool();

/** 连接池是否正在运行 */
bool isConnectionPoolRunning();

/** 获取MySQL连接数量 */
size_t getMysqlConnectionSize();

/** 获取Redis连接数量 */
size_t getRedisConnectionSize();

/** 获取MySQL空闲连接数量 */
size_t getMysqlIdleConnectionSize();

/** 获取Redis空闲连接数量 */
size_t getRedisIdleConnectionSize();

/** 获取空闲的MySQL连接 */
MysqlConnectionPtr getMysqlConnection();

/** 获取空闲的Redis连接 */
RedisConnectionPtr getRedisConnection();
} // namespace talko::pool