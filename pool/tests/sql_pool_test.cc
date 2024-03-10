#include <pool/connection_pool.h>
#include <pool/thread_pool.h>
#include <thread>
#include <iostream>
using namespace talko;

pool::ConnectionInfo getMysqlConfig() {
    pool::DatabaseInfo database;
    database.ip       = "127.0.0.1";
    database.username = "root";
    database.password = "likehan";
    database.port     = 3306;
    database.database = "test";

    pool::ConnectionInfo mysql_info;
    mysql_info.connect_timeout   = std::chrono::milliseconds(500);
    mysql_info.database          = database;
    mysql_info.init_size         = 1;
    mysql_info.max_idle_interval = std::chrono::seconds(5);
    mysql_info.max_size          = 2;
    return mysql_info;
}

pool::ConnectionInfo getRedisConfig() {
    pool::DatabaseInfo database;
    database.ip   = "127.0.0.1";
    database.port = 6379;

    pool::ConnectionInfo redis_info;
    redis_info.connect_timeout   = std::chrono::milliseconds(500);
    redis_info.database          = database;
    redis_info.init_size         = 2;
    redis_info.max_idle_interval = std::chrono::seconds(5);
    redis_info.max_size          = 4;
    return redis_info;
}

/** 测试最大连接数目 */
void testMaxConnectionNum() {
    pool::submitTask([]() {
        std::cout << "Start thread: " << std::this_thread::get_id() << std::endl;
        auto conn = pool::getMysqlConnection();
        std::cout << std::this_thread::get_id() << " get mysql connection "
                  << conn.get() << ", do business" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(5));
        std::cout << std::this_thread::get_id() << " business is finished" << std::endl;
    });

    pool::submitTask([]() {
        std::cout << "Start thread: " << std::this_thread::get_id() << std::endl;
        auto conn = pool::getMysqlConnection();
        std::cout << std::this_thread::get_id() << " get mysql connection "
                  << conn.get() << ", do business" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(10));
        std::cout << std::this_thread::get_id() << " business is finished" << std::endl;
    });

    // 子线程获取连接数目的已经达到连接上限
    std::cout << "Main thread: " << std::this_thread::get_id() << std::endl;
    std::cout << std::this_thread::get_id() << " try to get mysql connection" << std::endl;
    auto conn = pool::getMysqlConnection();
    std::cout << std::this_thread::get_id() << " get mysql connection "
              << conn.get() << ", do business" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(3));
    std::cout << std::this_thread::get_id() << " business is finished" << std::endl;
}

int main() {
    // 启动线程池
    pool::setThreadPoolMode(pool::ThreadPoolMode::dynamic);
    pool::startThreadPool(10);

    // 初始化连接池
    pool::ConnectionInfo mysql_info = getMysqlConfig();
    pool::ConnectionInfo redis_info = getRedisConfig();
    pool::startConnectionPool(mysql_info, redis_info);

    // 启动连接任务
    testMaxConnectionNum();

    // std::this_thread::sleep_for(std::chrono::seconds(30));

    std::cout << "End progress" << std::endl;

    return 0;
}