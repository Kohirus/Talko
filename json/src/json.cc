#include <cassert>
#include <fstream>
#include <iostream> // TODO: delete
#include <json/json.h>
#include <json/json_expection.h>
#include <sstream>

namespace talko::json {
const int kBeginObject = 1;
const int kEndObject   = 2;
const int kBeginArray  = 4;
const int kEndArray    = 8;
const int kNull        = 16;
const int kNumber      = 32;
const int kString      = 64;
const int kBoolean     = 128;
const int kSepColon    = 256;
const int kSepComma    = 512;
const int kEndDocument = 1024;

TokenType Json::pre_type_ = TokenType::EndDocument;

JsonNode Json::parseFromStream(std::istream& stream) {
    TokenQueue tokens;
    tokenize(stream, tokens);
    pre_type_ = TokenType::EndDocument;
    return parseObjectNode(tokens);
}

JsonNode Json::parseFromString(const std::string& json) {
    std::istringstream iss(json);
    return parseFromStream(iss);
}

JsonNode Json::parseFromFile(const std::string& filename) {
    std::fstream file(filename);
    if (file.is_open()) {
        return parseFromStream(file);
    } else {
        throw JsonParseException("Cannot open file");
    }
}

void Json::tokenize(std::istream& stream, TokenQueue& tokens) {
    while (stream.peek() != std::char_traits<char>::eof()) {
        char ch = stream.get();

        if (isWhiteSpace(ch))
            continue;

        if (ch >= '0' && ch <= '9') {
            stream.putback(ch);
            tokens.push(readNumber(stream));
            continue;
        }

        switch (ch) {
        case '{':
            tokens.push(Token(TokenType::BeginObject, "{"));
            break;
        case '}':
            tokens.push(Token(TokenType::EndObject, "}"));
            break;
        case '[':
            tokens.push(Token(TokenType::BeginArray, "["));
            break;
        case ']':
            tokens.push(Token(TokenType::EndArray, "]"));
            break;
        case ',':
            tokens.push(Token(TokenType::SepComma, ","));
            break;
        case ':':
            tokens.push(Token(TokenType::SepColon, ":"));
            break;
        case 'n':
            tokens.push(readNull(stream));
            break;
        case 't':
        case 'f':
            tokens.push(readBoolean(stream));
            break;
        case '"':
            tokens.push(readString(stream));
            break;
        case '-':
            stream.putback('-');
            tokens.push(readNumber(stream));
            break;
        default:
            throw JsonParseException("Illegal character");
        }
    }

    tokens.push(Token(TokenType::EndDocument));
}

JsonNode Json::parseObjectNode(TokenQueue& tokens) {
    JsonNode    node;
    std::string key = "";

    int expection_token = kString | kBeginObject;
    if (pre_type_ == TokenType::BeginObject) {
        expection_token |= kEndObject;
    }

    while (!tokens.empty()) {
        Token token = tokens.front();
        if (!(expection_token & static_cast<int>(token.type()))) {
            throw JsonParseException("Unexpected token");
        }

        switch (token.type()) {
        case TokenType::BeginObject:
            tokens.pop();
            pre_type_ = TokenType::BeginObject;
            if (!key.empty())
                node.append(key, parseObjectNode(tokens));
            else
                node = parseObjectNode(tokens);
            expection_token = kSepComma | kEndObject | kEndDocument;
            break;
        case TokenType::EndObject:
            return node;
            break;
        case TokenType::BeginArray:
            tokens.pop();
            pre_type_ = TokenType::BeginArray;
            if (!key.empty())
                node.append(key, parseArrayNode(tokens));
            else
                node = parseArrayNode(tokens);
            expection_token = kSepComma | kEndObject | kEndDocument;
            break;
        case TokenType::Null:
            node.append(key, JsonNode::invalid());
            expection_token = kSepComma | kEndObject;
            break;
        case TokenType::Number:
            try {
                if (token.isDecimal()) {
                    node.append(key, JsonNode(token.toDouble()));
                } else {
                    node.append(key, JsonNode(token.toInt()));
                }
            } catch (std::exception e) {
                throw JsonParseException(e.what());
            }
            expection_token = kSepComma | kEndObject;
            break;
        case TokenType::String:
            if (pre_type_ == TokenType::SepColon) {
                node.append(key, JsonNode(token.toString()));
                expection_token = kSepComma | kEndObject;
            } else {
                key             = token.toString();
                expection_token = kSepColon;
            }
            break;
        case TokenType::Boolean:
            node.append(key, JsonNode(token.toBoolean()));
            expection_token = kSepComma | kEndObject;
            break;
        case TokenType::SepColon:
            expection_token = kNull | kNumber | kBoolean
                | kString | kBeginObject | kBeginArray;
            break;
        case TokenType::SepComma:
            expection_token = kString | kNumber | kNull
                | kBoolean | kBeginArray | kBeginObject;
            break;
        case TokenType::EndDocument:
            return node;
        default:
            throw JsonParseException("Unexpected token");
        }

        pre_type_ = token.type();
        tokens.pop();
    }

    throw JsonParseException("Invalid token");
}

JsonNode Json::parseArrayNode(TokenQueue& tokens) {
    int expection_token = kString | kEndObject | kBeginArray | kEndArray
        | kBeginObject | kNull | kNumber | kBoolean | kString;
    JsonNode node;

    while (!tokens.empty()) {
        Token token = tokens.front();
        if (!(expection_token & static_cast<int>(token.type()))) {
            throw JsonParseException("Unexpected token");
        }

        switch (token.type()) {
        case TokenType::BeginObject:
            tokens.pop();
            pre_type_ = TokenType::BeginObject;
            node.append(parseObjectNode(tokens));
            expection_token = kSepComma | kEndArray | kEndDocument;
            break;
        case TokenType::BeginArray:
            tokens.pop();
            pre_type_ = TokenType::BeginArray;
            node.append(parseArrayNode(tokens));
            expection_token = kSepComma | kEndArray | kEndDocument;
            break;
        case TokenType::EndArray:
            return node;
            break;
        case TokenType::Null:
            node.append(JsonNode::invalid());
            expection_token = kSepComma | kEndArray;
            break;
        case TokenType::Number:
            try {
                if (token.isDecimal()) {
                    node.append(JsonNode(token.toDouble()));
                } else {
                    node.append(JsonNode(token.toInt()));
                }
            } catch (std::exception e) {
                throw JsonParseException(e.what());
            }
            expection_token = kSepComma | kEndArray;
            break;
        case TokenType::String:
            node.append(JsonNode(token.toString()));
            expection_token = kSepComma | kEndArray;
            break;
        case TokenType::Boolean:
            expection_token = kSepComma | kEndArray;
            break;
        case TokenType::SepComma:
            expection_token = kString | kNumber | kNull
                | kBoolean | kBeginArray | kBeginObject;
            break;
        case TokenType::EndDocument:
            return node;
        default:
            throw JsonParseException("Unexpected token");
        }

        pre_type_ = token.type();
        tokens.pop();
    }

    throw JsonParseException("Invalid token");
}

Token Json::readNull(std::istream& stream) {
    char ch1 = stream.get();
    char ch2 = stream.get();
    char ch3 = stream.get();
    if (!(ch1 == 'u' && ch2 == 'l' && ch3 == 'l')) {
        throw JsonParseException("Invalid json string");
    }

    return Token(TokenType::Null);
}

Token Json::readBoolean(std::istream& stream) {
    char ch1 = stream.get();
    if (ch1 == 'r') {
        char ch2 = stream.get();
        char ch3 = stream.get();
        if (!(ch1 == 'r' && ch2 == 'u' && ch3 == 'e')) {
            throw JsonParseException("Invalid json string");
        }

        return Token(TokenType::Boolean, "true");
    } else {
        char ch2 = stream.get();
        char ch3 = stream.get();
        char ch4 = stream.get();
        if (!(ch1 == 'a' && ch2 == 'l' && ch3 == 's' && ch4 == 'e')) {
            throw JsonParseException("Invalid json string");
        }

        return Token(TokenType::Boolean, "false");
    }
}

Token Json::readNumber(std::istream& stream) {
    std::string val;

    bool is_decimal = false;

    while (stream.peek() != '\n' && stream.peek() != '\r'
        && stream.peek() != ',' && stream.peek() != ' ') {
        char ch = stream.get();

        switch (ch) {
        case 'e':
        case 'E':
        case '.':
            is_decimal = true;
            val.push_back(ch);
            break;
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
        case '+':
        case '-':
            val.push_back(ch);
            break;
        default:
            throw JsonParseException("Invalid number character");
        }
    }

    Token res(TokenType::Number, val);
    res.setDecimal(is_decimal);
    return res;
}

Token Json::readString(std::istream& stream) {
    std::string val;
    while (stream.peek() != '"') {
        char ch = stream.get();

        switch (ch) {
        case '\\': { // 处理转义字符
            if (stream.peek() != std::char_traits<char>::eof() && !isEscape(stream.peek())) {
                throw JsonParseException("Invalid escape character");
            }
            val.push_back(ch);
            val.push_back(stream.get());
        } break;
        case '\r':
        case '\n': // JSON字符串不允许换行
            throw JsonParseException("Invalid character");
        default:
            val.push_back(ch);
            break;
        }
    }
    stream.get();
    return Token(TokenType::String, val);
}

bool Json::isWhiteSpace(char ch) {
    return (ch == '\n' || ch == ' ' || ch == '\t' || ch == '\r' || ch == '\f');
}

bool Json::isEscape(char ch) {
    return (ch == '"' || ch == '\\' || ch == 'u' || ch == 'r'
        || ch == 'n' || ch == 'b' || ch == 't' || ch == 'f');
}

void Json::printTokens(TokenQueue& tokens) {
    while (!tokens.empty()) {
        switch (tokens.front().type()) {
        case TokenType::BeginObject:
            std::cout << "BeginObject: " << tokens.front().value() << std::endl;
            break;
        case TokenType::EndObject:
            std::cout << "EndObject: " << tokens.front().value() << std::endl;
            break;
        case TokenType::BeginArray:
            std::cout << "BeginArray: " << tokens.front().value() << std::endl;
            break;
        case TokenType::EndArray:
            std::cout << "EndArray: " << tokens.front().value() << std::endl;
            break;
        case TokenType::Null:
            std::cout << "Null: " << tokens.front().value() << std::endl;
            break;
        case TokenType::Number:
            std::cout << "Number: " << tokens.front().value() << std::endl;
            break;
        case TokenType::String:
            std::cout << "String: " << tokens.front().value() << std::endl;
            break;
        case TokenType::Boolean:
            std::cout << "Boolean: " << tokens.front().value() << std::endl;
            break;
        case TokenType::SepColon:
            std::cout << "SepColon: " << tokens.front().value() << std::endl;
            break;
        case TokenType::SepComma:
            std::cout << "SepComma: " << tokens.front().value() << std::endl;
            break;
        case TokenType::EndDocument:
            std::cout << "EndDocument: " << tokens.front().value() << std::endl;
            break;
        }
        tokens.pop();
    }
}
} // namespace talko::json