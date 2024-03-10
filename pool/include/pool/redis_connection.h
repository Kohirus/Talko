#pragma once

#include <cassert>
#include <fmt/core.h>
#include <fmt/format.h>
#include <hiredis/hiredis.h>
#include <map>
#include <pool/base_connection.h>
#include <unordered_map>
#include <vector>

namespace talko::pool {
class RedisConnection : public BaseConnection {
public:
    using Array = std::vector<std::string>;
    using Hash  = std::map<std::string, std::string>;

    RedisConnection();
    ~RedisConnection();

    /**
     * @brief 连接数据库
     *
     * @param info 数据库信息(IP、端口号)
     * @param timeout 超时时间
     * @return 连接成功则返回true，否则返回false
     */
    bool connect(DatabaseInfo info, Duration timeout) override;

    /** 获取错误信息 */
    std::string errorMessage() const override;

    /** 测试与Redis服务器的连接 */
    bool ping() override;

    /**
     * @brief 获取长度
     * @details 如果为字符串，则是字符串长度，如果为哈希表，则为哈希表长度，如果为数组，则为数组长度
     *
     * @param key 要查询的键
     * @return size_t 返回长度
     */
    size_t length(const std::string& key);

    /**
     * @brief 删除键
     *
     * @param key 键
     * @return 移除成功则返回true
     */
    bool removeKey(const std::string& key);

    /**
     * @brief 指定键是否存在
     *
     * @param key 键
     * @return 存在则返回true
     */
    bool isKeyExist(const std::string& key);

    /**
     * @brief 添加字符串键
     *
     * @param key 键
     * @param value 值
     * @return 添加成功则返回true
     */
    bool addStringValue(const std::string& key, const std::string& value);

    /**
     * @brief 获取字符串对应的值
     *
     * @param[in] key 键
     * @param[out] value 值
     * @return 获取成功则返回true
     */
    bool getStringValue(const std::string& key, std::string* value);

    /**
     * @brief 添加哈希元素
     *
     * @param key 键
     * @param value 哈希元素
     * @return 添加成功则返回true
     */
    bool addHashValue(const std::string& key, const Hash& value);

    /**
     * @brief 指定哈希表中是否存在对应的元素
     *
     * @param key 键
     * @param field 要查询的哈希元素名称
     * @return 存在则返回true，否则返回false
     */
    bool isHashValueExist(const std::string& key, const std::string& field);

    /**
     * @brief 获取单个哈希元素
     *
     * @param[in] key 键
     * @param[in] field 元素名称
     * @param[out] value 元素的值
     * @return 获取成功则返回true
     */
    bool getHashValue(const std::string& key, const std::string& field, std::string* value);

    /**
     * @brief 获取多个哈希元素
     *
     * @param[in] key 键
     * @param[in] fields 要获取的元素名称
     * @param[out] values 元素名称所对应的值
     * @return 获取成功则返回true
     */
    bool getHashValue(const std::string& key, const Array& fields, Array* values);

    /**
     * @brief 获取哈希表
     *
     * @param[in] key 键
     * @param[out] value 哈希表
     * @return 获取成功返回true
     */
    bool getHashValue(const std::string& key, Hash* value);

    /**
     * @brief 删除单个哈希元素
     *
     * @param key 键
     * @param field 要删除的元素
     * @return 删除成功则返回true
     */
    bool removeHashValue(const std::string& key, const std::string& field);

    /**
     * @brief 删除多个哈希元素
     *
     * @param key 键
     * @param fields 要删除的多个元素
     * @return 删除成功则返回true
     */
    bool removeHashValue(const std::string& key, const Array& fields);

    /**
     * @brief 添加单个列表元素
     *
     * @param key 键
     * @param value 值
     * @param back_insert 是否尾插
     * @return 添加成功则返回true
     */
    bool addListValue(const std::string& key, const std::string& value, bool back_insert);

    /**
     * @brief 添加多个列表元素
     *
     * @param key 键
     * @param values 多个值
     * @param back_insert 是否尾插
     * @return 添加成功则返回true
     * @return false
     */
    bool addListValue(const std::string& key, const Array& values, bool back_insert);

    /**
     * @brief 获取列表中指定索引的值
     *
     * @param[in] key 键
     * @param[in] idx 要获取的索引
     * @param[out] value 值
     * @return 获取成功则返回true
     */
    bool getListValue(const std::string& key, size_t idx, std::string* value);

    /**
     * @brief 获取列表中 [start, end] 之间的元素的值
     *
     * @param[in] key 键
     * @param[in] start 起始索引
     * @param[in] end 结束索引
     * @param[out] values 值
     * @return 获取成功则返回true
     */
    bool getListValue(const std::string& key, size_t start, size_t end, std::vector<std::string>* values);

    /**
     * @brief 修改列表中索引为 idx 的元素的值
     *
     * @param key 键
     * @param idx 要修改的元素的索引
     * @param value 值
     * @return 获取成功则返回true
     */
    bool setListValue(const std::string& key, size_t idx, const std::string& value);

    /**
     * @brief 从列表中删除元素
     *
     * @param key 键
     * @param value 要删除的值
     * @param count 等于0时删除所有等于value的元素，不为0时删除count个等于value的元素，大于0时从头向尾搜索，小于0时从尾向头搜索
     * @return 移除成功则返回true，否则返回false
     */
    bool removeListValue(const std::string& key, const std::string& value, int count);

    /**
     * @brief 向列表中插入元素
     *
     * @param key 键
     * @param pivot 基准索引
     * @param value 要插入的值
     * @param before 是否在基准元素前插入
     * @return 插入成功则返回true
     */
    bool insertListValue(const std::string& key, size_t pivot, const std::string& value, bool before = true);

    /**
     * @brief 弹出列表元素
     *
     * @param[in] key 键
     * @param[out] value 弹出元素的值，可为 nullptr
     * @param front 是否弹出列表头的元素，为false时弹出列表尾的元素
     * @return 弹出成功则返回true
     */
    bool popListValue(const std::string& key, std::string* value, bool front = true);

    /**
     * @brief 仅保留 [start, end] 之间的列表元素
     *
     * @param key 键
     * @param start 起始索引
     * @param end 结束索引
     * @return 删除成功则返回true
     */
    bool trimListValue(const std::string& key, size_t start, size_t end);

    /**
     * @brief 添加无序集合元素
     *
     * @param key 键
     * @param value 无序集合元素
     * @return 添加成功则返回true
     */
    bool addSetValue(const std::string& key, const std::string& value);

    /**
     * @brief 添加多个无序集合元素
     *
     * @param key 键
     * @param values 要添加的多个元素
     * @return 添加成功则返回true
     */
    bool addSetValue(const std::string& key, const Array& values);

    /**
     * @brief 获取多个无序集合的交集
     *
     * @param[in] keys 多个无序集合的键
     * @param[out] values 交集元素
     * @return 获取成功则返回true
     */
    bool getInterSetValue(const std::vector<std::string>& keys, std::vector<std::string>* values);

    /**
     * @brief 获取多个无序集合的并集
     *
     * @param keys 多个无序集合的键
     * @param values 并集元素
     * @return 获取成功则返回true
     * @return false
     */
    bool getUnionSetValue(const std::vector<std::string>& keys, Array* values);

    /**
     * @brief 指定集合中是否已经存在某个特定值
     *
     * @param[in] key 键
     * @param[in] value 要查询的值
     * @return 查询到则返回true
     */
    bool isSetValueExist(const std::string& key, const std::string& value);

    /**
     * @brief 获取集合中所有元素的值
     *
     * @param[in] key 键
     * @param[out] values 集合中所有元素的值
     * @return 获取成功则返回true
     */
    bool getSetValue(const std::string& key, Array* values);

    /**
     * @brief 随机从无序集合中删除元素
     *
     * @param[in] key 键
     * @param[in] count 要删除的数量
     * @param[out] values 被删除元素的值
     * @return 删除成功则返回true
     */
    bool removeSetValue(const std::string& key, size_t count, Array* values);

    /**
     * @brief 从无序集合中删除元素
     *
     * @param key 键
     * @param value 要删除元素的值
     * @return 删除成功则返回true
     */
    bool removeSetValue(const std::string& key, const std::string& value);

    /**
     * @brief 从无序集合中删除多个元素
     *
     * @param key 键
     * @param values 要删除的多个元素的值
     * @return 删除成功则返回true
     */
    bool removeSetValue(const std::string& key, const Array& values);

private:
    /**
     * @brief 执行命令
     *
     * @param format 格式化字符串
     * @param ... 可变参数
     * @return 返回结果
     */
    template <typename... Args>
    redisReply* executeCommand(fmt::format_string<Args...> fmt, Args&&... args) {
        std::string buffer;
        fmt::vformat_to(std::back_inserter(buffer), fmt, fmt::make_format_args(args...));
        redisReply* reply = static_cast<redisReply*>(redisCommand(handler_, buffer.c_str()));
        return reply;
    }

    /** 将数组转化为哈希表 */
    Hash arrayToMap(const Array& arr);

private:
    redisContext* handler_; ///< Redis控制句柄
    std::string   err_msg_; ///< 错误消息
};
} // namespace talko::pool