#include <cassert>
#include <pool/connection_pool.h>
#include <pool/mysql_connection.h>
#include <pool/redis_connection.h>
#include <pool/thread_pool.h>
#include <thread>

namespace talko::pool {
ConnectionPool::ConnectionPool()
    : kCheckInterval_(2)
    , kCreateTimeout_(500)
    , mysql_conn_size_(0)
    , redis_conn_size_(0) {
}

ConnectionPool::~ConnectionPool() {
    if (running_) {
        stop();
    }
}

ConnectionPool& ConnectionPool::instance() {
    static ConnectionPool pool;
    return pool;
}

void ConnectionPool::start(const ConnectionInfo& mysql_info, const ConnectionInfo& redis_info) {
    assert(pool::isThreadPoolRunning() && "Thread pool is stop");
    mysql_info_ = mysql_info;
    redis_info_ = redis_info;

    // 初始化MySQL数据库
    for (size_t i = 0; i < mysql_info_.init_size; ++i) {
        MysqlConnection* conn = new MysqlConnection();
        conn->connect(mysql_info_.database, mysql_info_.connect_timeout);
        mysql_que_.push(conn);
    }

    // 初始化Redis数据库
    for (size_t i = 0; i < redis_info_.init_size; ++i) {
        RedisConnection* conn = new RedisConnection();
        conn->connect(redis_info_.database, redis_info_.connect_timeout);
        redis_que_.push(conn);
    }

    mysql_conn_size_ = mysql_info_.init_size;
    redis_conn_size_ = redis_info_.init_size;

    running_ = true;

    // 提交线程任务以创建新的连接
    create_task_ = pool::submitTask(std::bind(&ConnectionPool::createNewConnection, this));

    // 提交线程任务以持续检查空闲的连接
    remove_task_ = pool::submitTask(std::bind(&ConnectionPool::removeIdleConnection, this));
}

void ConnectionPool::stop() {
    running_ = false;
    // 等待线程回收
    create_task_.wait();
    remove_task_.wait();
    // 等待连接回收 并释放
    {
        std::unique_lock<std::mutex> lock(mysql_que_mtx_);
        mysql_cv_.wait(lock, [this]() -> bool {
            return mysql_que_.size() == mysql_conn_size_;
        });
        while (!mysql_que_.empty()) {
            auto conn = mysql_que_.front();
            mysql_que_.pop();
            delete conn;
        }
    }
    {
        std::unique_lock<std::mutex> lock(redis_que_mtx_);
        redis_cv_.wait(lock, [this]() -> bool {
            return redis_que_.size() == redis_conn_size_;
        });
        while (!redis_que_.empty()) {
            auto conn = redis_que_.front();
            redis_que_.pop();
            delete conn;
        }
    }
}

bool ConnectionPool::isRunning() {
    return running_;
}

size_t ConnectionPool::getMysqlConnectionSize() {
    return mysql_conn_size_;
}

size_t ConnectionPool::getRedisConnectionSize() {
    return redis_conn_size_;
}

size_t ConnectionPool::getMysqlIdleConnectionSize() {
    std::lock_guard<std::mutex> lock(mysql_que_mtx_);
    return mysql_que_.size();
}

size_t ConnectionPool::getRedisIdleConnectionSize() {
    std::lock_guard<std::mutex> lock(redis_que_mtx_);
    return redis_que_.size();
}

MysqlConnectionPtr ConnectionPool::getMysqlConnection() {
    assert(running_);
    std::unique_lock<std::mutex> lock(mysql_que_mtx_);

    // 等待生产者产生新连接
    mysql_cv_.wait(lock, [this]() -> bool {
        return !mysql_que_.empty();
    });

    // 使用自定义删除器的方式在连接使用完成后自动回收到连接池中
    MysqlConnectionPtr conn(mysql_que_.front(),
        [&](MysqlConnection* pconn) {
            std::lock_guard<std::mutex> lock(mysql_que_mtx_);
            pconn->resetIdleTime(); // 回收到连接池时重置空闲起始时间
            mysql_que_.push(pconn);
            mysql_cv_.notify_all(); // 通知消费者消费连接
        });

    mysql_que_.pop();
    mysql_cv_.notify_all(); // 通知生产者生产新连接

    return conn;
}

RedisConnectionPtr ConnectionPool::getRedisConnection() {
    assert(running_);
    std::unique_lock<std::mutex> lock(redis_que_mtx_);

    // 等待生产者产生新连接
    redis_cv_.wait(lock, [this]() -> bool {
        return !redis_que_.empty();
    });

    // 使用自定义删除器的方式在连接使用完成后自动回收到连接池中
    RedisConnectionPtr conn(redis_que_.front(),
        [&](RedisConnection* pconn) {
            std::lock_guard<std::mutex> lock(redis_que_mtx_);
            pconn->resetIdleTime(); // 回收到连接池时重置空闲起始时间
            redis_que_.push(pconn);
            redis_cv_.notify_all(); // 通知消费者消费连接
        });

    redis_que_.pop();
    redis_cv_.notify_all(); // 通知生产者生产新连接

    return conn;
}

void ConnectionPool::createNewConnection() {
    while (running_) {
        {
            std::unique_lock<std::mutex> lock(mysql_que_mtx_);

            // 等待消费者消费MySQL连接
            if (mysql_cv_.wait_for(lock, std::chrono::milliseconds(kCreateTimeout_), [this]() -> bool {
                    return mysql_que_.empty();
                })) {
                // 连接数量没有到达上限，继续创建新的连接
                if (mysql_conn_size_ < mysql_info_.max_size) {
                    MysqlConnection* conn = new MysqlConnection();
                    conn->connect(mysql_info_.database, mysql_info_.connect_timeout);
                    mysql_que_.push(conn);
                    ++mysql_conn_size_;
                }

                // 通知消费者线程消费MySQL连接
                mysql_cv_.notify_all();
            }
        }

        {
            std::unique_lock<std::mutex> lock(redis_que_mtx_);

            // 等待消费者消费Redis连接
            if (redis_cv_.wait_for(lock, std::chrono::milliseconds(kCreateTimeout_), [this]() -> bool {
                    return redis_que_.empty();
                })) {
                // 连接数量没有到达上限，继续创建新的连接
                if (redis_conn_size_ < redis_info_.max_size) {
                    RedisConnection* conn = new RedisConnection();
                    conn->connect(redis_info_.database, redis_info_.connect_timeout);
                    redis_que_.push(conn);
                    ++redis_conn_size_;
                }

                // 通知消费者线程消费MySQL连接
                redis_cv_.notify_all();
            }
        }
    }
}

void ConnectionPool::removeIdleConnection() {
    while (running_) {
        {
            std::lock_guard<std::mutex> lock(mysql_que_mtx_);

            // 扫描MySQL连接队列 移除其中空闲时间超出最大限度的连接
            while (mysql_conn_size_ > mysql_info_.init_size && !mysql_que_.empty()) {
                MysqlConnection* conn = mysql_que_.front();
                if (conn->getIdleDuration() >= mysql_info_.max_idle_interval) {
                    mysql_que_.pop();
                    --mysql_conn_size_;
                    delete conn; // 释放连接资源
                } else {
                    break; // 若队头的连接未超时 则其它连接也未超时
                }
            }
        }

        {
            std::lock_guard<std::mutex> lock(redis_que_mtx_);

            // 扫描Redis连接队列 移除其中空闲时间超出最大限度的连接
            while (redis_conn_size_ > redis_info_.init_size && !redis_que_.empty()) {
                RedisConnection* conn = redis_que_.front();
                if (conn->getIdleDuration() >= redis_info_.max_idle_interval) {
                    redis_que_.pop();
                    --redis_conn_size_;
                    delete conn; // 释放连接资源
                } else {
                    break; // 若队头的连接未超时 则其它连接也未超时
                }
            }
        }

        // 间隔固定时间进行检查
        std::this_thread::sleep_for(std::chrono::seconds(kCheckInterval_));
    }
}

void startConnectionPool(const ConnectionInfo& mysql_info, const ConnectionInfo& redis_info) {
    ConnectionPool::instance().start(mysql_info, redis_info);
}

void stopConnectionPool() {
    ConnectionPool::instance().stop();
}

bool isConnectionPoolRunning() {
    return ConnectionPool::instance().isRunning();
}

size_t getMysqlConnectionSize() {
    return ConnectionPool::instance().getMysqlConnectionSize();
}

size_t getRedisConnectionSize() {
    return ConnectionPool::instance().getRedisConnectionSize();
}

size_t getMysqlIdleConnectionSize() {
    return ConnectionPool::instance().getMysqlIdleConnectionSize();
}

size_t getRedisIdleConnectionSize() {
    return ConnectionPool::instance().getRedisIdleConnectionSize();
}

MysqlConnectionPtr getMysqlConnection() {
    return ConnectionPool::instance().getMysqlConnection();
}

RedisConnectionPtr getRedisConnection() {
    return ConnectionPool::instance().getRedisConnection();
}
} // namespace talko::pool