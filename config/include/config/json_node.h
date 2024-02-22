#pragma once

#include <map>
#include <string>
#include <variant>
#include <vector>

namespace talko::config {
/**
 * @brief JSON节点类型
 *
 */
class JsonNode {
public:
    JsonNode() = default;

    explicit JsonNode(int value)
        : data_(value) { }

    explicit JsonNode(double value)
        : data_(value) { }

    explicit JsonNode(bool value)
        : data_(value) { }

    explicit JsonNode(const std::string& value)
        : data_(value) { }

    JsonNode(const JsonNode&)            = default;
    JsonNode& operator=(const JsonNode&) = default;

    /** 获取空值节点 */
    static JsonNode null();

    /**
     * @brief 获取键值为 key 的节点
     *
     * @throw JsonParseException 当前节点非Object类型时抛出异常
     */
    JsonNode& operator[](const std::string& key);

    /**
     * @brief 获取键值为 key 的节点
     *
     * @throw JsonParseException 当前节点非Object类型时抛出异常
     */
    const JsonNode& at(const std::string& key) const;

    /**
     * @brief 获取索引为 index 的节点
     *
     * @throw JsonParseException 当前节点非Array类型时抛出异常
     */
    JsonNode& operator[](size_t index);

    /**
     * @brief 获取索引为 index 的节点
     *
     * @throw JsonParseException 当前节点非Array类型时抛出异常
     */
    const JsonNode& at(size_t index) const;

    /**
     * @brief 获取节点值
     *
     * @tparam T 值类型
     * @return T 节点值
     * @throw JsonParseException 类型T不正确时抛出异常
     */
    template <typename T>
    T value() const {
        return std::get<T>(data_);
    }

    /** 是否为空节点 */
    bool isNull() const;

    /**
     * @brief 向Object节点中追加元素
     *
     * @param key 键值
     * @param node 节点
     * @throw JsonParseException 当前节点非Object节点类型时抛出异常
     */
    void append(const std::string& key, const JsonNode& node);

    /**
     * @brief 向Array节点中追加元素
     *
     * @param node 节点
     * @throw JsonParseException 当前节点非Array节点类型时抛出异常
     */
    void append(const JsonNode& node);

    /**
     * @brief 获取当前容器节点的子节点数量
     *
     * @return size_t 返回子节点数量
     * @throw JsonParseException 当前节点非容器节点时抛出异常
     */
    size_t count() const;

    /**
     * @brief 是否拥有某个键值为 key 的子节点
     *
     * @param key 要查询的键值
     * @return 存在则返回true，否则返回false
     * @throw JsonParseException 当前容器非Object节点时抛出异常
     */
    bool has(const std::string& key);

    /** // TODO: delete 测试打印节点 */
    void print(int level = 0);

private:
    using ObjectNode = std::map<std::string, JsonNode>;
    using ArrayNode  = std::vector<JsonNode>;
    using Variant    = std::variant<std::monostate, int, double, bool, std::string, ObjectNode, ArrayNode>;

    Variant data_; ///< 数据域
};
} // namespace talko::config