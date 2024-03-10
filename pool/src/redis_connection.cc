#include <pool/redis_connection.h>
#include <stdarg.h>
#include <pool/redis_reply.h>

namespace talko::pool {
RedisConnection::RedisConnection()
    : handler_(nullptr)
    , err_msg_() {
}

RedisConnection::~RedisConnection() {
    if (handler_ != nullptr) {
        redisFree(handler_);
    }
}

bool RedisConnection::connect(DatabaseInfo info, Duration timeout) {
    resetIdleTime();
    using std::chrono::duration_cast;
    using std::chrono::microseconds;
    using std::chrono::seconds;

    seconds      secs = duration_cast<seconds>(timeout);
    microseconds us   = duration_cast<microseconds>(timeout % std::chrono::seconds(1));

    timeval tv;
    tv.tv_sec  = secs.count();
    tv.tv_usec = us.count();
    handler_   = redisConnectWithTimeout(info.ip.c_str(), info.port, tv);
    if (handler_ == nullptr || handler_->err) {
        return false;
    }

    return true;
}

bool RedisConnection::ping() {
    resetIdleTime();
    if (handler_ == nullptr) {
        return false;
    }
    RedisReply reply(executeCommand("PING"), &err_msg_);
    if (reply.isFailed()) return false;
    if (reply.status() != "PONG") {
        return false;
    }
    return true;
}

std::string RedisConnection::errorMessage() const {
    if (handler_ == nullptr) {
        return "Database not connected or can't allocate redis content";
    } else if (handler_->err) {
        return handler_->errstr;
    } else {
        return err_msg_;
    }
}


size_t RedisConnection::length(const std::string& key) {
    assert(handler_);
    resetIdleTime();
    RedisReply type_reply(executeCommand("TYPE {}", key), &err_msg_);
    assert(!type_reply.isFailed());
    std::string type = type_reply.status();
    if (type == "none") {
        return 0;
    } else if (type == "hash") {
        RedisReply reply(executeCommand("HLEN {}", key), &err_msg_);
        return reply.getInteger().value();
    } else if (type == "list") {
        RedisReply reply(executeCommand("LLEN {}", key), &err_msg_);
        return reply.getInteger().value();
    } else if (type == "set") {
        RedisReply reply(executeCommand("SCARD {}", key), &err_msg_);
        return reply.getInteger().value();
    } else if (type == "string") {
        RedisReply reply(executeCommand("STRLEN {}", key), &err_msg_);
        return reply.getInteger().value();
    } else {
        RedisReply reply(executeCommand("ZCARD {}", key), &err_msg_);
        return reply.getInteger().value();
    }
}

bool RedisConnection::removeKey(const std::string& key) {
    assert(handler_);
    resetIdleTime();
    RedisReply reply(executeCommand("DEL {}", key), &err_msg_);
    return reply.isFailed();
}

bool RedisConnection::isKeyExist(const std::string& key) {
    assert(handler_);
    resetIdleTime();
    RedisReply reply(executeCommand("EXISTS {}", key), &err_msg_);
    assert(reply.isFailed());
    return reply.getInteger().value() > 0;
}

bool RedisConnection::addStringValue(const std::string& key, const std::string& value) {
    assert(handler_);
    resetIdleTime();
    RedisReply reply(executeCommand("SET {} {}", key, value), &err_msg_);
    return reply.isFailed();
}

bool RedisConnection::getStringValue(const std::string& key, std::string* value) {
    assert(handler_);
    resetIdleTime();
    assert(value);
    RedisReply reply(executeCommand("GET {}", key), &err_msg_);
    if (reply.isFailed()) {
        return false;
    }
    auto res = reply.getString();
    if (!res.has_value()) {
        err_msg_ = "Value is nil";
        return false;
    }
    *value = res.value();
    return true;
}

bool RedisConnection::addHashValue(const std::string& key, const Hash& value) {
    assert(handler_);
    resetIdleTime();
    std::string cmd = fmt::format("HMSET {}", key);
    for (auto& [field, val] : value) {
        cmd.append(fmt::format(" {} {}", field, val));
    }

    RedisReply reply(executeCommand(cmd), &err_msg_);
    return reply.isFailed();
}

bool RedisConnection::isHashValueExist(const std::string& key, const std::string& field) {
    assert(handler_);
    resetIdleTime();
    RedisReply reply(executeCommand("HEXISTS {} {}", key, field), &err_msg_);
    if (reply.isFailed()) {
        return false;
    }
    auto res = reply.getInteger();
    assert(res.has_value());
    return res.value() != 0;
}

bool RedisConnection::getHashValue(const std::string& key, const std::string& field, std::string* value) {
    assert(handler_);
    assert(value);
    resetIdleTime();
    RedisReply reply(executeCommand("HGET {} {}", key, field), &err_msg_);
    if (reply.isFailed()) {
        return false;
    }
    auto res = reply.getString();
    if (!res.has_value()) {
        err_msg_ = "Value is nil";
        return false;
    }
    *value = res.value();
    return true;
}

bool RedisConnection::getHashValue(const std::string& key, const Array& fields, Array* values) {
    assert(handler_);
    assert(values);
    resetIdleTime();
    std::string cmd = fmt::format("HGET {}", key);

    for (auto& field : fields) {
        cmd.append(fmt::format(" {}", field));
    }

    RedisReply reply(executeCommand(cmd), &err_msg_);
    if (reply.isFailed()) {
        return false;
    }
    auto res = reply.getArray();
    if (!res.has_value()) {
        err_msg_ = "Value is nil";
        return false;
    }
    *values = res.value();
    return true;
}

bool RedisConnection::getHashValue(const std::string& key, Hash* value) {
    assert(handler_);
    assert(value);
    resetIdleTime();
    value->clear();
    RedisReply reply(executeCommand("HGETALL {}", key), &err_msg_);
    if (reply.isFailed()) {
        return false;
    }
    auto res = reply.getArray();
    if (!res.has_value()) {
        err_msg_ = "Value is nil";
        return false;
    }
    *value = arrayToMap(res.value());
    return true;
}

bool RedisConnection::removeHashValue(const std::string& key, const std::string& field) {
    assert(handler_);
    resetIdleTime();
    RedisReply reply(executeCommand("HDEL {} {}", key, field), &err_msg_);
    return reply.isFailed();
}

bool RedisConnection::removeHashValue(const std::string& key, const Array& fields) {
    assert(handler_);
    resetIdleTime();
    if (fields.empty()) return true;
    std::string cmd = "HDEL";

    for (auto& field : fields) {
        cmd.append(fmt::format(" {}", field));
    }

    RedisReply reply(executeCommand(cmd), &err_msg_);
    return reply.isFailed();
}

bool RedisConnection::addListValue(const std::string& key, const std::string& value, bool back_insert) {
    assert(handler_);
    resetIdleTime();
    RedisReply reply(executeCommand("{}PUSH {} {}", (back_insert ? "R" : "L"), key, value), &err_msg_);
    return reply.isFailed();
}

bool RedisConnection::addListValue(const std::string& key, const Array& values, bool back_insert) {
    assert(handler_);
    resetIdleTime();
    std::string cmd = fmt::format("{}PUSH {}", (back_insert ? "R" : "L"), key);
    for (auto& value : values) {
        cmd.append(fmt::format(" {}", value));
    }

    RedisReply reply(executeCommand(cmd), &err_msg_);
    return reply.isFailed();
}

bool RedisConnection::getListValue(const std::string& key, size_t idx, std::string* value) {
    assert(handler_);
    assert(value);
    resetIdleTime();
    RedisReply reply(executeCommand("LINDEX {} {}", key, idx), &err_msg_);
    if (reply.isFailed()) {
        return false;
    }
    auto res = reply.getString();
    if (!res.has_value()) {
        err_msg_ = "Value is nil";
        return false;
    }
    *value = res.value();
    return true;
}

bool RedisConnection::getListValue(const std::string& key, size_t start, size_t end, std::vector<std::string>* values) {
    assert(handler_);
    assert(values);
    resetIdleTime();
    RedisReply reply(executeCommand("LRANGE {} {} {}", key, start, end), &err_msg_);
    if (reply.isFailed()) {
        return false;
    }
    auto res = reply.getArray();
    if (!res.has_value()) {
        err_msg_ = "Value is nil";
        return false;
    }
    *values = res.value();
    return true;
}

bool RedisConnection::setListValue(const std::string& key, size_t idx, const std::string& value) {
    assert(handler_);
    resetIdleTime();
    RedisReply reply(executeCommand("LSET {} {} {}", key, idx, value), &err_msg_);
    return reply.isFailed();
}

bool RedisConnection::removeListValue(const std::string& key, const std::string& value, int count) {
    assert(handler_);
    resetIdleTime();
    RedisReply reply(executeCommand("LREM {} {} {}", key, count, value), &err_msg_);
    return reply.isFailed();
}

bool RedisConnection::insertListValue(const std::string& key, size_t pivot, const std::string& value, bool before) {
    assert(handler_);
    resetIdleTime();
    std::string pos = before ? "BEFORE" : "AFTER";
    RedisReply  reply(executeCommand("LINSERT {} {} {}", key, pos, pivot, value), &err_msg_);
    return reply.isFailed();
}

bool RedisConnection::popListValue(const std::string& key, std::string* value, bool front) {
    assert(handler_);
    resetIdleTime();
    RedisReply reply(executeCommand("{}POP {}", (front ? "L" : "R"), key), &err_msg_);
    if (reply.isFailed()) {
        return false;
    }
    auto res = reply.getString();
    if (res.has_value() && value != nullptr) {
        *value = res.value();
    }
    return true;
}

bool RedisConnection::trimListValue(const std::string& key, size_t start, size_t end) {
    assert(handler_);
    resetIdleTime();
    RedisReply reply(executeCommand("LTRIM {} {} {}", key, start, end), &err_msg_);
    return reply.isFailed();
}

bool RedisConnection::addSetValue(const std::string& key, const std::string& value) {
    assert(handler_);
    resetIdleTime();
    RedisReply reply(executeCommand("SADD {} {}", key, value), &err_msg_);
    return reply.isFailed();
}

bool RedisConnection::addSetValue(const std::string& key, const Array& values) {
    assert(handler_);
    resetIdleTime();
    std::string cmd = fmt::format("SADD {}", key);
    for (auto& value : values) {
        cmd.append(fmt::format(" {}", value));
    }

    RedisReply reply(executeCommand(cmd), &err_msg_);
    return reply.isFailed();
}

bool RedisConnection::getInterSetValue(const Array& keys, Array* values) {
    assert(handler_);
    resetIdleTime();
    std::string cmd = fmt::format("SINTER");
    for (auto& key : keys) {
        cmd.append(fmt::format(" {}", key));
    }
    RedisReply reply(executeCommand(cmd), &err_msg_);
    auto       res = reply.getArray();
    if (!res.has_value()) {
        err_msg_ = "Value is nil";
        return false;
    }
    return true;
}

bool RedisConnection::getUnionSetValue(const Array& keys, Array* values) {
    assert(handler_);
    resetIdleTime();
    std::string cmd = fmt::format("SUNION");
    for (auto& key : keys) {
        cmd.append(fmt::format(" {}", key));
    }
    RedisReply reply(executeCommand(cmd), &err_msg_);
    auto       res = reply.getArray();
    if (!res.has_value()) {
        err_msg_ = "Value is nil";
        return false;
    }
    return true;
}

bool RedisConnection::isSetValueExist(const std::string& key, const std::string& value) {
    assert(handler_);
    resetIdleTime();
    RedisReply reply(executeCommand("SISMEMBER {} {}", key, value), &err_msg_);
    if (reply.isFailed()) {
        return false;
    }
    auto res = reply.getInteger();
    assert(res.has_value());
    return res.value() != 0;
}

bool RedisConnection::getSetValue(const std::string& key, Array* values) {
    assert(handler_);
    resetIdleTime();
    RedisReply reply(executeCommand("SMEMBERS {}", key), &err_msg_);
    auto       res = reply.getArray();
    if (!res.has_value()) {
        err_msg_ = "Value is nil";
        return false;
    }
    *values = res.value();
    return true;
}

bool RedisConnection::removeSetValue(const std::string& key, size_t count, Array* values) {
    assert(handler_);
    resetIdleTime();
    RedisReply reply(executeCommand("SPOP {} {}", key, count), &err_msg_);
    auto       res = reply.getArray();
    if (!res.has_value()) {
        err_msg_ = "Value is nil";
        return false;
    }
    *values = res.value();
    return true;
}

bool RedisConnection::removeSetValue(const std::string& key, const std::string& value) {
    assert(handler_);
    resetIdleTime();
    RedisReply reply(executeCommand("SREM {} {}", key, value), &err_msg_);
    return reply.isFailed();
}

bool RedisConnection::removeSetValue(const std::string& key, const Array& values) {
    assert(handler_);
    resetIdleTime();
    std::string cmd = fmt::format("SREM {}", key);
    for (auto& value : values) {
        cmd.append(fmt::format(" {}", value));
    }
    RedisReply reply(executeCommand(cmd), &err_msg_);
    return reply.isFailed();
}

std::map<std::string, std::string> RedisConnection::arrayToMap(const std::vector<std::string>& arr) {
    assert(handler_);
    resetIdleTime();
    std::map<std::string, std::string> res;
    assert(arr.size() % 2 == 0);
    for (size_t i = 0; i < arr.size(); i += 2) {
        res.insert({ arr[i], arr[i + 1] });
    }
    return res;
}
} // namespace talko::pool