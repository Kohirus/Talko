#pragma once

#include <json/json_node.h>
#include <json/token.h>
#include <istream>
#include <queue>

namespace talko::json {
class Json {
public:
    /** 解析输入流 */
    static JsonNode parseFromStream(std::istream& stream);

    /** 解析字符串 */
    static JsonNode parseFromString(const std::string& json);

    /** 解析文件 */
    static JsonNode parseFromFile(const std::string& filename);

private:
    using TokenQueue = std::queue<Token>;

    /**
     * @brief 拆分输入流为令牌序列
     *
     * @param[in] stream 输入流
     * @param[out] tokens 令牌序列
     */
    static void tokenize(std::istream& stream, TokenQueue& tokens);

    /** 解析对象节点 */
    static JsonNode parseObjectNode(TokenQueue& tokens);

    /** 解析数组节点 */
    static JsonNode parseArrayNode(TokenQueue& tokens);

    /** 读取空节点 */
    static Token readNull(std::istream& stream);

    /** 读取布尔节点 */
    static Token readBoolean(std::istream& stream);

    /** 读取数值节点 */
    static Token readNumber(std::istream& stream);

    /** 读取字符串节点 */
    static Token readString(std::istream& stream);

    /** 是否为空白字符 */
    static bool isWhiteSpace(char ch);

    /** 是否为转义字符 */
    static bool isEscape(char ch);

    /** //TODO: debug print */
    static void printTokens(TokenQueue& tokens);

private:
    static TokenType pre_type_; ///< 上一个类型
};
} // namespace talko::json