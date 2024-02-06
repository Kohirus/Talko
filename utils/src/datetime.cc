#include <cassert>
#include <iomanip>
#include <sstream>
#include <utils/datetime.h>

namespace talko {
namespace utils {
DateTime::DateTime(TimePoint time_point, bool valid)
    : time_point_(time_point)
    , local_time_(true)
    , valid_(valid) {
}

DateTime::DateTime(std::time_t ctime, bool valid)
    : DateTime(std::chrono::high_resolution_clock::from_time_t(ctime), valid) {
}

DateTime DateTime::now() {
    return DateTime(std::chrono::high_resolution_clock::now());
}

DateTime DateTime::invalid() {
    return DateTime(TimePoint(), false);
}

long DateTime::secondsSinceEpoch() const {
    if (!valid_) return 0;
    auto seconds = std::chrono::duration_cast<std::chrono::seconds>(time_point_.time_since_epoch());
    return seconds.count();
}

int DateTime::year() const {
    if (!valid_) return 0;
    return getTm()->tm_year + 1900;
}

int DateTime::month() const {
    if (!valid_) return 0;
    return getTm()->tm_mon + 1;
}

int DateTime::day() const {
    if (!valid_) return 0;
    return getTm()->tm_mday;
}

int DateTime::dayOfYear() const {
    if (!valid_) return 0;
    return getTm()->tm_yday + 1;
}

int DateTime::weekday() const {
    if (!valid_) return 0;
    return getTm()->tm_wday;
}

int DateTime::hour() const {
    if (!valid_) return 0;
    return getTm()->tm_hour;
}

int DateTime::hour12() const {
    if (!valid_) return 0;
    int res = getTm()->tm_hour % 12;
    return (res == 0) ? 12 : res;
}

bool DateTime::isAm() const {
    if (!valid_) return 0;
    return getTm()->tm_hour < 12;
}

int DateTime::minute() const {
    if (!valid_) return 0;
    return getTm()->tm_min;
}

int DateTime::seconds() const {
    if (!valid_) return 0;
    return getTm()->tm_sec;
}

int DateTime::milliseconds() const {
    if (!valid_) return 0;

    using std::chrono::duration_cast;
    using std::chrono::milliseconds;
    using std::chrono::seconds;

    auto duration = time_point_.time_since_epoch();

    auto secs = duration_cast<seconds>(duration);
    auto ms   = duration_cast<milliseconds>(duration) - duration_cast<milliseconds>(secs);
    return ms.count();
}

long DateTime::microseconds() const {
    if (!valid_) return 0;

    using std::chrono::duration_cast;
    using std::chrono::microseconds;
    using std::chrono::seconds;

    auto duration = time_point_.time_since_epoch();

    auto secs = duration_cast<seconds>(duration);
    auto us   = duration_cast<microseconds>(duration) - duration_cast<microseconds>(secs);
    return us.count();
}

long DateTime::nanoseconds() const {
    if (!valid_) return 0;

    using std::chrono::duration_cast;
    using std::chrono::nanoseconds;
    using std::chrono::seconds;

    auto duration = time_point_.time_since_epoch();

    auto secs = duration_cast<seconds>(duration);
    auto ns   = duration_cast<nanoseconds>(duration) - duration_cast<nanoseconds>(secs);
    return ns.count();
}

std::string DateTime::zone() const {
    if (!valid_) return "";
    return std::string(getTm()->tm_zone);
}

DateTime& DateTime::toLocalTime() {
    local_time_ = true;
    return *this;
}

DateTime& DateTime::toUtcTime() {
    local_time_ = false;
    return *this;
}

std::string DateTime::toDateString() const {
    if (!valid_) return "";
    std::stringstream ss;

    std::tm* ctime = getTm();
    ss << std::setw(4) << std::setfill('0') << ctime->tm_year + 1900;
    ss << "-";
    ss << std::setw(2) << std::setfill('0') << ctime->tm_mon + 1;
    ss << "-";
    ss << std::setw(2) << std::setfill('0') << ctime->tm_mday;

    return ss.str();
}

std::string DateTime::toTimeString() const {
    if (!valid_) return "";
    std::stringstream ss;

    std::tm* ctime = getTm();
    ss << std::setw(2) << std::setfill('0') << ctime->tm_hour;
    ss << ":";
    ss << std::setw(2) << std::setfill('0') << ctime->tm_min;
    ss << ":";
    ss << std::setw(2) << std::setfill('0') << ctime->tm_sec;

    return ss.str();
}

std::string DateTime::toString() const {
    if (!valid_) return "";
    std::stringstream ss;

    std::tm* ctime = getTm();
    ss << std::setw(4) << std::setfill('0') << ctime->tm_year + 1900;
    ss << "-";
    ss << std::setw(2) << std::setfill('0') << ctime->tm_mon + 1;
    ss << "-";
    ss << std::setw(2) << std::setfill('0') << ctime->tm_mday;
    ss << " ";
    ss << std::setw(2) << std::setfill('0') << ctime->tm_hour;
    ss << ":";
    ss << std::setw(2) << std::setfill('0') << ctime->tm_min;
    ss << ":";
    ss << std::setw(2) << std::setfill('0') << ctime->tm_sec;

    return ss.str();
}

std::tm* DateTime::getTm() const {
    std::time_t now_c = std::chrono::high_resolution_clock::to_time_t(time_point_);
    if (local_time_) {
        return ::localtime(&now_c);
    }

    return ::gmtime(&now_c);
}

} // namespace utils
} // namespace talko