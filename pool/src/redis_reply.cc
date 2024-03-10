#include <cassert>
#include <pool/redis_reply.h>

namespace talko::pool {
RedisReply::RedisReply(redisReply* reply, std::string* error_msg)
    : reply_(reply)
    , err_msg_(error_msg) {
    assert(reply_);
}

RedisReply::~RedisReply() {
    if (reply_ != nullptr) {
        freeReplyObject(reply_);
    }
}

bool RedisReply::isFailed() const {
    if (reply_->type != REDIS_REPLY_ERROR) {
        err_msg_->clear();
    } else {
        *err_msg_ = reply_->str;
    }
    return reply_->type == REDIS_REPLY_ERROR;
}

std::optional<std::string> RedisReply::getString() const {
    if (reply_->type == REDIS_REPLY_NIL) {
        return std::nullopt;
    }
    assert(reply_->type == REDIS_REPLY_STRING);
    return std::string(reply_->str);
}

std::optional<std::vector<std::string>> RedisReply::getArray() const {
    if (reply_->type == REDIS_REPLY_NIL) {
        return std::nullopt;
    }
    assert(reply_->type == REDIS_REPLY_ARRAY);
    std::vector<std::string> res(reply_->elements);
    for (size_t i = 0; i < reply_->elements; ++i) {
        redisReply* element = reply_->element[i];
        assert(element->type == REDIS_REPLY_STRING);
        res[i] = element->str;
    }
    return res;
}

std::optional<long long> RedisReply::getInteger() const {
    if (reply_->type == REDIS_REPLY_NIL) {
        return std::nullopt;
    }
    assert(reply_->type == REDIS_REPLY_INTEGER);
    return reply_->integer;
}

std::string RedisReply::status() const {
    if (reply_->type != REDIS_REPLY_STATUS) return "";
    return reply_->str;
}

size_t RedisReply::length() const {
    if (reply_->type == REDIS_REPLY_ARRAY) {
        return reply_->elements;
    } else if (reply_->type == REDIS_REPLY_STRING) {
        return reply_->len;
    } else {
        return 0;
    }
}
} // namespace talko::pool