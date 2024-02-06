#include <algorithm>
#include <log/log_level.h>

namespace talko {
namespace log {
namespace level {
static const std::string level_strings[] { "trace", "debug", "info", "warn", "error", "fatal", "off" };
static const std::string level_short_strings[] { "T", "D", "I", "W", "E", "F", "O" };

std::string toString(LogLevel level) {
    return level_strings[static_cast<int>(level)];
}

std::string toShortString(LogLevel level) {
    return level_short_strings[static_cast<int>(level)];
}

LogLevel fromString(std::string_view str) {
    auto it = std::find(std::begin(level_strings), std::end(level_strings), str);
    if (it != std::end(level_strings)) {
        return static_cast<LogLevel>(std::distance(std::begin(level_strings), it));
    }

    return LogLevel::off;
}
} // namespace level
} // namespace log
} // namespace talko