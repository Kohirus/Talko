#pragma once

#include <fmt/core.h>
#include <fmt/format.h>
#include <hiredis/hiredis.h>
#include <map>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace talko::pool {
/**
 * @brief Redis响应对象 在对象析构时自动销毁redisReply内存
 *
 */
class RedisReply {
public:
    /**
     * @brief Construct a new Redis Reply object
     *
     * @param reply Redis响应，对象析构时自动销毁redisReply内存
     * @param error_msg 错误消息
     */
    RedisReply(redisReply* reply, std::string* error_msg);
    ~RedisReply();

    /** 操作是否失败 */
    bool isFailed() const;

    /** 获取字符串类型 */
    std::optional<std::string> getString() const;

    /** 获取数组类型 */
    std::optional<std::vector<std::string>> getArray() const;

    /** 获取整数类型 */
    std::optional<long long> getInteger() const;

    /** 获取状态字符串 */
    std::string status() const;

    /** 获取字符串的长度或数组的长度 */
    size_t length() const;

private:
    redisReply*  reply_ { nullptr };   ///< Redis响应
    std::string* err_msg_ { nullptr }; ///< 错误消息
};
} // namespace talko::pool