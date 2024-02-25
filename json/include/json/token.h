#pragma once

#include <string>

namespace talko::json {
/** 令牌类型 */
enum class TokenType {
    BeginObject = 1,   ///< 对象开始 {
    EndObject   = 2,   ///< 对象结束 }
    BeginArray  = 4,   ///< 数组开始 [
    EndArray    = 8,   ///< 数组结束 ]
    Null        = 16,  ///< 空值 null
    Number      = 32,  ///< 数字
    String      = 64,  ///< 字符串
    Boolean     = 128, ///< 布尔值
    SepColon    = 256, ///< 逗号
    SepComma    = 512, ///< 分号
    EndDocument = 1024 ///< 文档结束
};

/**
 * @brief 令牌类
 *
 */
class Token {
public:
    /**
     * @brief Construct a new Token object
     *
     * @param type 令牌类型
     * @param value 字面值
     */
    explicit Token(TokenType type, const std::string& value = "null")
        : type_(type)
        , value_(value) { }

    ~Token() = default;

    Token(const Token&)            = default;
    Token& operator=(const Token&) = default;

    /** 获取令牌类型 */
    inline TokenType type() const { return type_; }

    /** 获取字面值 */
    inline const std::string& value() const { return value_; }

    /** 转换为字符串 */
    inline const std::string& toString() const { return value_; }

    /** 转换为整型变量 */
    inline int toInt() const { return std::stoi(value_); }

    /** 转换为双精度浮点型变量 */
    inline double toDouble() const { return std::stod(value_); }

    /** 转换为布尔值 */
    inline bool toBoolean() const { return value_ == "true"; }

    /** 设置是否为浮点数 */
    inline void setDecimal(bool on) { decimal_ = on; }

    /** 是否为浮点数 */
    inline bool isDecimal() const { return decimal_; }

    /** 是否为空值 */
    inline bool isNull() const { return value_ == "null"; }

private:
    std::string value_ { "null" };         ///< 字面值
    TokenType   type_ { TokenType::Null }; ///< 令牌类型
    bool        decimal_ { false };        ///< 是否为浮点数
};
} // namespace talko::json