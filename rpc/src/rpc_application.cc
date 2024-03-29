#include "pool/connection_info.h"
#include "pool/connection_pool.h"
#include <rpc/rpc_application.h>

namespace talko::rpc {
RpcApplication& RpcApplication::instance() {
    static RpcApplication app;
    return app;
}

void RpcApplication::init(int argc, char** argv, bool is_registry) {
    if (argc > 2) {
        std::cerr << "Usage: ./app [config.json]";
        std::exit(EXIT_SUCCESS);
    }

    if (argc == 2) {
        config_ = json::Json::parseFromFile(argv[1]);
    } else {
        ensureNetAndRpcLogger();
    }

    initThreadPool();
    initLog();
    initNetwork();
    initConnectionPool();
    if (!is_registry) {
        initRegistrant();
    }

    LOGGER_INFO("rpc", "Configuration initialization completed");

    // 连接注册中心
    if (!is_registry && !RpcRegistrant::instance().connect(connect_timeout_, heartbeat_interval_, registry_center_addr_)) {
        LOGGER_FATAL("rpc", "Failed to connect to RegistryCenter");
    }
}

net::InetAddress RpcApplication::serverAddress() const {
    return net::InetAddress(port_, loopback_only_);
}

void RpcApplication::initThreadPool() {
    if (!config_.isInvalid() && config_.has("thread")) {
        // 设置线程池参数
        if (config_["thread"].has("max_thread_num")) {
            pool::setMaxThreadSize(config_["thread"]["max_thread_num"].value<int>());
        }
        if (config_["thread"].has("max_task_num")) {
            pool::setMaxTaskSize(config_["thread"]["max_task_num"].value<int>());
        }
        if (config_["thread"].has("dynamic_mode")) {
            if (config_["thread"]["dynamic_mode"].value<bool>()) {
                pool::setThreadPoolMode(pool::ThreadPoolMode::dynamic);
            } else {
                pool::setThreadPoolMode(pool::ThreadPoolMode::fixed);
            }
        }

        // 启动线程池
        if (config_["thread"].has("init_thread_num")) {
            pool::startThreadPool(config_["thread"]["init_thread_num"].value<int>());
        } else {
            pool::startThreadPool();
        }
    } else {
        pool::startThreadPool();
    }
}

void RpcApplication::initLog() {
    if (config_.isInvalid() || !config_.has("log") || config_["log"].isInvalid()) {
        ensureNetAndRpcLogger();
        return;
    }

    // 用于逐级设置日志等级和日志格式化
    std::queue<std::function<void()>> funcs;

    // 获取全局日志参数
    std::string global_level   = config_["log"].valueOf("level", std::string("info"));
    std::string global_pattern = config_["log"].valueOf("formatter", std::string("%#"));
    funcs.push([global_level, global_pattern]() {
        log::setGlobalLevel(log::level::fromString(global_level));
        log::setPattern(global_pattern);
    });

    // 获取默认日志器的名称
    std::string default_name = "";
    if (config_["log"].has("default")) {
        default_name = config_["log"]["default"].value<std::string>();
    }

    // 配置日志器
    if (config_["log"].has("logger") && !config_["log"]["logger"].isInvalid()) {
        size_t logger_cnt = config_["log"]["logger"].count();
        for (size_t i = 0; i < logger_cnt; ++i) {
            configLogger(funcs, config_["log"]["logger"][i], default_name, global_level, global_pattern);
        }
    } else {
        ensureNetAndRpcLogger();
    }

    // 如果不存在默认日志器的名称则创建该日志器
    if (!default_name.empty() && !log::exist(default_name)) {
        auto logger = log::createConsoleLoggerMt(default_name);
        log::setDefaultLogger(logger);
    }

    // 逐级设置日志等级和日志格式化
    while (!funcs.empty()) {
        funcs.front()();
        funcs.pop();
    }

    configConsoleProperty();
}

void RpcApplication::initNetwork() {
    if (config_.isInvalid() || !config_.has("network") || config_["network"].isInvalid()) {
        return;
    }

    name_          = config_["network"].valueOf("name", std::string("TcpServer"));
    ip_            = config_["network"].valueOf("ip", std::string("127.0.0.1"));
    port_          = static_cast<uint16_t>(config_["network"].valueOf("port", 8000));
    reuse_port_    = config_["network"].valueOf("reuse_port", false);
    loopback_only_ = config_["network"].valueOf("loopback_only", false);
    subloop_num_   = static_cast<size_t>(config_["network"].valueOf("subloop_num", 3));
}

void RpcApplication::initConnectionPool() {
    if (config_.isInvalid() || !config_.has("database")) {
        return;
    }

    if (config_["database"].isInvalid()) {
        std::cerr << "[database] in the config file can't be nil";
        exit(EXIT_FAILURE);
    }

    if (!config_["database"].has("mysql") || config_["database"]["mysql"].isInvalid()) {
        std::cerr << "[mysql] in [database] can't be nil";
        exit(EXIT_FAILURE);
    }

    if (!config_["database"]["mysql"].has("password") || config_["database"]["mysql"]["password"].value<std::string>().empty()) {
        std::cerr << "[password] in [mysql] can't be nil";
        exit(EXIT_FAILURE);
    }

    if (!config_["database"]["mysql"].has("database") || config_["database"]["mysql"]["database"].value<std::string>().empty()) {
        std::cerr << "[database] in [mysql] can't be nil";
        exit(EXIT_FAILURE);
    }

    pool::ConnectionInfo mysql_info, redis_info;
    pool::DatabaseInfo   mysql_dt, redis_dt;
    redis_dt.port = 6379;

    // 读取mysql库信息
    mysql_dt.port        = config_["database"]["mysql"].valueOf("port", 3306);
    mysql_dt.database    = config_["database"]["mysql"]["database"].value<std::string>();
    mysql_dt.ip          = config_["database"]["mysql"].valueOf("ip", std::string("127.0.0.1"));
    mysql_dt.password    = config_["database"]["mysql"]["password"].value<std::string>();
    mysql_dt.username    = config_["database"]["mysql"].valueOf("username", std::string("root"));
    mysql_info.init_size = config_["database"]["mysql"].valueOf("init_size", 3);
    mysql_info.max_size  = config_["database"]["mysql"].valueOf("max_size", 20);
    mysql_info.database  = mysql_dt;

    if (mysql_info.database.username.empty()) {
        std::cerr << "[username] of [mysql] can't be empty";
        exit(EXIT_FAILURE);
    }

    // 读取redis库信息
    if (config_["database"].has("redis")) {
        redis_dt.port        = config_["database"]["redis"].valueOf("port", 6379);
        redis_dt.ip          = config_["database"]["redis"].valueOf("ip", std::string("127.0.0.1"));
        redis_dt.username    = config_["database"]["redis"].valueOf("username", std::string("root"));
        redis_info.init_size = config_["database"]["redis"].valueOf("init_size", 3);
        redis_info.max_size  = config_["database"]["redis"].valueOf("max_size", 20);
        redis_info.database  = redis_dt;
    }

    if (redis_info.database.username.empty()) {
        std::cerr << "[username] of [mysql] can't be empty";
        exit(EXIT_FAILURE);
    }

    // 读取最大空闲时间和连接超时时间
    auto max_idle_interval       = std::chrono::milliseconds(config_["database"].valueOf("max_idle_interval", 50000));
    auto connect_timeout         = std::chrono::milliseconds(config_["database"].valueOf("connect_timeout", 1000));
    mysql_info.max_idle_interval = max_idle_interval;
    redis_info.max_idle_interval = max_idle_interval;
    mysql_info.connect_timeout   = connect_timeout;
    redis_info.connect_timeout   = connect_timeout;

    std::cout << "========================" << std::endl;
    std::cout << "MySQL Configuration:" << std::endl;
    std::cout << "- Port: " << mysql_info.database.port << std::endl;
    std::cout << "- IP: " << mysql_info.database.ip << std::endl;
    std::cout << "- Username: " << mysql_info.database.username << std::endl;
    std::cout << "- Password: " << mysql_info.database.password << std::endl;
    std::cout << "- Database: " << mysql_info.database.database << std::endl;
    std::cout << "- InitSize: " << mysql_info.init_size << std::endl;
    std::cout << "- MaxSize: " << mysql_info.max_size << std::endl;
    std::cout << "- ConnectTimeout: " << mysql_info.connect_timeout.count() << std::endl;
    std::cout << "- IdleInterval: " << mysql_info.max_idle_interval.count() << std::endl;
    std::cout << "========================" << std::endl;
    std::cout << "Redis Configuration:" << std::endl;
    std::cout << "- Port: " << redis_info.database.port << std::endl;
    std::cout << "- IP: " << redis_info.database.ip << std::endl;
    std::cout << "- InitSize: " << redis_info.init_size << std::endl;
    std::cout << "- MaxSize: " << redis_info.max_size << std::endl;
    std::cout << "- ConnectTimeout: " << redis_info.connect_timeout.count() << std::endl;
    std::cout << "- IdleInterval: " << redis_info.max_idle_interval.count() << std::endl;

    pool::startConnectionPool(mysql_info, redis_info);
}

void RpcApplication::initRegistrant() {
    if (config_.isInvalid() || !config_.has("registry") || config_["registry"].isInvalid()) {
        return;
    }

    std::string registry_center_ip   = config_["registry"].valueOf("ip", std::string("127.0.0.1"));
    uint16_t    registry_center_port = static_cast<uint16_t>(config_["registry"].valueOf("port", 8888));

    registry_center_addr_ = net::InetAddress(registry_center_ip, registry_center_port);

    connect_timeout_    = std::chrono::milliseconds(config_["registry"].valueOf("connect_timeout", 1000));
    heartbeat_interval_ = std::chrono::milliseconds(config_["registry"].valueOf("heartbeat_interval", 10000));
}

void RpcApplication::configLogger(std::queue<std::function<void()>>& funcs, const json::JsonNode& node,
    std::string_view default_name, const std::string& global_level, const std::string& global_pattern) {
    // 获取日志器相关属性
    std::string log_name    = node["name"].value<std::string>();
    std::string log_level   = node.valueOf("level", global_level);
    std::string log_pattern = node.valueOf("formatter", global_pattern);

    // 检测该日志器是否已经存在
    if (log::exist(log_name)) {
        std::cerr << fmt::format("Logger named [{}] is existed.", log_name) << std::endl;
        std::exit(EXIT_SUCCESS);
    }

    // 创建并注册日志器
    auto logger = std::make_shared<log::Logger>(log_name);
    log::registerLogger(logger);

    // 检查是否设置为默认日志器
    if (log_name == default_name) {
        log::setDefaultLogger(logger);
    }

    funcs.push([logger, log_level, log_pattern]() {
        logger->setLevel(log::level::fromString(log_level));
        logger->setPattern(log_pattern);
    });

    if (node.has("appender") && !node["appender"].isInvalid()) {
        // 配置输出地
        size_t appender_cnt = node["appender"].count();
        for (size_t i = 0; i < appender_cnt; ++i) {
            auto appender = configAppender(funcs, node["appender"][i], log_level, log_pattern);
            logger->appenders().push_back(appender);
        }
    } else {
        // 如果不存在输出地 则配置为多线程控制台输出
        auto appender = std::make_shared<log::ConsoleAppenderMt>();
        logger->appenders().push_back(appender);
    }
}

log::AppenderPtr RpcApplication::configAppender(FuncQueue& funcs, const json::JsonNode& node,
    const std::string& logger_level, const std::string& logger_pattern) {
    std::string type = node["type"].value<std::string>();

    std::shared_ptr<log::LogAppender> appender;

    bool mt = node.valueOf("multi_thread", true);
    if (type == "console") { // 控制台输出
        if (mt) {
            appender = std::make_shared<log::ConsoleAppenderMt>();
        } else {
            appender = std::make_shared<log::ConsoleAppenderSt>();
        }
    } else if (type == "file") { // 文件输出
        if (!node.has("filename")) {
            std::cerr << "FileAppender must have [filename] property" << std::endl;
            exit(EXIT_SUCCESS);
        }

        std::string filename = node["filename"].value<std::string>();
        bool        rewrite  = node.valueOf("rewrite", true);
        bool        async    = node.valueOf("async", false);
        if (mt) {
            appender = std::make_shared<log::FileAppenderMt>(filename, rewrite, async);
        } else {
            appender = std::make_shared<log::FileAppenderSt>(filename, rewrite, async);
        }
    } else if (type == "rotating_file") { // 旋转文件输出
        if (!node.has("filename") || !node.has("max_file_num") || !node.has("max_file_size")) {
            std::cerr << "FileAppender must have [filename], [max_file_num] and [max_file_size] property" << std::endl;
            exit(EXIT_SUCCESS);
        }

        std::string filename      = node["filename"].value<std::string>();
        int         max_file_num  = node["max_file_num"].value<int>();
        int         max_file_size = node["max_file_size"].value<int>();
        bool        async         = node.valueOf("async", false);
        if (mt) {
            appender = std::make_shared<log::RotatingFileAppenderMt>(filename, max_file_num, max_file_size, async);
        } else {
            appender = std::make_shared<log::RotatingFileAppenderSt>(filename, max_file_num, max_file_size, async);
        }
    } else {
        std::cerr << fmt::format("Unknown appender type: {}", type) << std::endl;
        std::exit(EXIT_SUCCESS);
    }

    // 配置日志输出器参数
    std::string apd_level   = node.valueOf("level", logger_level);
    std::string apd_pattern = node.valueOf("formatter", logger_pattern);

    funcs.push([appender, apd_level, apd_pattern]() {
        appender->setPattern(apd_pattern);
        appender->setLevel(log::level::fromString(apd_level));
    });

    return appender;
}

void RpcApplication::ensureNetAndRpcLogger() {
    // 如果不存在 net 日志器则进行配置
    if (!log::exist("net")) {
        auto net_logger = log::createConsoleLoggerMt("net");
        log::registerLogger(net_logger);
    }

    // 如果不存在 rpc 日志器则进行配置
    if (!log::exist("rpc")) {
        auto rpc_logger = log::createConsoleLoggerSt("rpc");
        log::registerLogger(rpc_logger);
    }
}

void RpcApplication::configConsoleProperty() {
    if (!config_["log"].has("terminal") || config_["log"]["terminal"].isInvalid()) {
        return;
    }

    auto&  node        = config_["log"]["terminal"];
    size_t setting_cnt = node.count();
    for (size_t i = 0; i < setting_cnt; ++i) {
        if (!node[i].has("level")) {
            std::cerr << "Terminal must have [level] property." << std::endl;
            exit(EXIT_SUCCESS);
        }

        std::string level      = node[i]["level"].value<std::string>();
        std::string foreground = node[i].valueOf("foreground", std::string("default"));
        std::string background = node[i].valueOf("background", std::string("default"));

        bool bold      = node[i].valueOf("bold", false);
        bool italic    = node[i].valueOf("italic", false);
        bool underline = node[i].valueOf("underline", false);

        log::TerminalFont font(log::fromString(foreground), log::fromString(background), bold, italic, underline);
        log::ConsoleAppender<log::RealMutex>::setLevelFont(log::level::fromString(level), font);
        log::ConsoleAppender<log::FakeMutex>::setLevelFont(log::level::fromString(level), font);
    }
}
} // namespace talko::rpc