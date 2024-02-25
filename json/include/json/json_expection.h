#pragma once

#include <exception>
#include <string>

namespace talko::json {

class JsonParseException : public std::exception {
public:
    JsonParseException(const std::string& msg)
        : message(msg) { }

    const char* what() const noexcept override {
        return message.c_str();
    }

private:
    std::string message;
};
} // namespace talko::json