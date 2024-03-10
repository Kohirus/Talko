#include <mysql/mysql.h>
#include <pool/mysql_connection.h>

namespace talko::pool {
MysqlConnection::MysqlConnection()
    : handler_(mysql_init(nullptr)) {
}

MysqlConnection::~MysqlConnection() {
    if (handler_ != nullptr) {
        mysql_close(handler_);
    }
}

bool MysqlConnection::ping() {
    resetIdleTime();
    int ret = mysql_ping(handler_);
    return ret != 0;
}

bool MysqlConnection::connect(DatabaseInfo info, Duration timeout) {
    resetIdleTime();

    // 设置超时时间
    unsigned int time = std::chrono::duration_cast<std::chrono::seconds>(timeout).count();
    if (mysql_options(handler_, MYSQL_OPT_CONNECT_TIMEOUT, &time) != 0) {
        return false;
    }

    MYSQL* conn = mysql_real_connect(handler_, info.ip.c_str(), info.username.c_str(), info.password.c_str(), info.database.c_str(), info.port, nullptr, 0);
    return conn != nullptr;
}

bool MysqlConnection::update(const std::string& sql) {
    resetIdleTime();
    int ret = mysql_query(handler_, sql.c_str());
    return ret == 0;
}

MYSQL_RES* MysqlConnection::query(const std::string& sql) {
    resetIdleTime();
    if (mysql_query(handler_, sql.c_str())) {
        return nullptr;
    }
    return mysql_use_result(handler_);
}

std::string MysqlConnection::errorMessage() const {
    return mysql_error(handler_);
}
} // namespace talko::pool