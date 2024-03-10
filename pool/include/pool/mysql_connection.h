#pragma once

#include <pool/base_connection.h>

class MYSQL;
class MYSQL_RES;

namespace talko::pool {
/**
 * @brief MySQL数据库连接
 *
 */
class MysqlConnection : public BaseConnection {
public:
    MysqlConnection();
    ~MysqlConnection();

    MysqlConnection(const MysqlConnection&)            = delete;
    MysqlConnection& operator=(const MysqlConnection&) = delete;

    /** 检查与数据库服务器的连接 */
    bool ping() override;

    /**
     * @brief 连接数据库
     *
     * @param info 数据库信息(IP、端口号、用户名、密码、数据库名)
     * @param timeout 超时时间
     * @return 连接成功则返回true，否则返回false
     */
    bool connect(DatabaseInfo info, Duration timeout) override;

    /** 更新数据库，包括 INSERT、DELETE、UPDATE 操作 */
    bool update(const std::string& sql);

    /** 查询数据库 */
    MYSQL_RES* query(const std::string& sql);

    /** 获取错误信息 */
    std::string errorMessage() const override;

private:
    MYSQL* handler_; ///< MySQL控制句柄
};
} // namespace talko::pool