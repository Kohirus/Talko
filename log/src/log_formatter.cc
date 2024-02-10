#include <cassert>
#include <fmt/core.h>
#include <log/log_formatter.h>
#include <typeinfo>
#include <utils/datetime.h>
#include <utils/file_info.h>
#include <utils/os.h>

namespace talko::log {
LogFormatter::FlagHash LogFormatter::hash_ = {
    { 'a', []() { return std::make_unique<ShortWeekdayFlag>(); } },
    { 'A', []() { return std::make_unique<WeekdayFlag>(); } },
    { 'b', []() { return std::make_unique<ShortMonthStringFlag>(); } },
    { 'B', []() { return std::make_unique<MonthStringFlag>(); } },
    { 'c', []() { return std::make_unique<StandardDateTimeFlag>(); } },
    { 'C', []() { return std::make_unique<NameFlag>(); } },
    { 'd', []() { return std::make_unique<ShortDateFlag>(); } },
    { 'D', []() { return std::make_unique<DayFlag>(); } },
    { 'e', []() { return std::make_unique<MillisecondsFlag>(); } },
    { 'E', []() { return std::make_unique<ThreadIdFlag>(); } },
    { 'f', []() { return std::make_unique<MillisecondsFlag>(); } },
    { 'F', []() { return std::make_unique<DateFlag>(); } },
    { 'g', []() { return std::make_unique<SecondsSinceEpochFlag>(); } },
    { 'G', []() { return std::make_unique<SourceFileNameFlag>(); } },
    { 'h', []() { return std::make_unique<HourFlag>(); } },
    { 'H', []() { return std::make_unique<Hour12Flag>(); } },
    { 'i', []() { return std::make_unique<SourceFilePathFlag>(); } },
    { 'I', []() { return std::make_unique<SourceFileLineFlag>(); } },
    { 'j', []() { return std::make_unique<DayOfYearFlag>(); } },
    { 'J', []() { return std::make_unique<SourceFileFuncNameFlag>(); } },
    { 'k', []() { return std::make_unique<SourceFileLocationFlag>(); } },
    { 'K', []() { return std::make_unique<FullSourceFileLocationFlag>(); } },
    { 'l', []() { return std::make_unique<ShortLogLevelFlag>(); } },
    { 'L', []() { return std::make_unique<LogLevelFlag>(); } },
    { 'm', []() { return std::make_unique<MinuteFlag>(); } },
    { 'M', []() { return std::make_unique<MonthFlag>(); } },
    { 'n', []() { return std::make_unique<NewLineFlag>(); } },
    { 'N', []() { return std::make_unique<NanosecondsFlag>(); } },
    { 'p', []() { return std::make_unique<AmPmFlag>(); } },
    { 'P', []() { return std::make_unique<ProcessIdFlag>(); } },
    { 'r', []() { return std::make_unique<Time12Flag>(); } },
    { 'R', []() { return std::make_unique<HourAndMinuteFlag>(); } },
    { 's', []() { return std::make_unique<SecondsFlag>(); } },
    { 't', []() { return std::make_unique<TabFlag>(); } },
    { 'T', []() { return std::make_unique<TimeFlag>(); } },
    { 'v', []() { return std::make_unique<LogMessageFlag>(); } },
    { 'y', []() { return std::make_unique<ShortYearFlag>(); } },
    { 'Y', []() { return std::make_unique<YearFlag>(); } },
    { 'z', []() { return std::make_unique<ZoneFlag>(); } },
    { '%', []() { return std::make_unique<PercentFlag>(); } },
    { '#', []() { return std::make_unique<DefaultFlag>(); } }
};

static std::string short_weekday_strings[] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };

void ShortWeekdayFlag::format(const LogInfo& info, std::string& buffer) {
    utils::DateTime datetime(info.time_point);
    if (use_utc_) datetime.toUtcTime();
    buffer.append(short_weekday_strings[datetime.weekday()]);
}

static std::string weekday_strings[] = { "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday" };

void WeekdayFlag::format(const LogInfo& info, std::string& buffer) {
    utils::DateTime datetime(info.time_point);
    if (use_utc_) datetime.toUtcTime();
    buffer.append(weekday_strings[datetime.weekday()]);
}

static std::string short_month_strings[] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };

void ShortMonthStringFlag::format(const LogInfo& info, std::string& buffer) {
    utils::DateTime datetime(info.time_point);
    if (use_utc_) datetime.toUtcTime();
    buffer.append(short_month_strings[datetime.month() - 1]);
}

static std::string month_strings[] = { "January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December" };

void MonthStringFlag::format(const LogInfo& info, std::string& buffer) {
    utils::DateTime datetime(info.time_point);
    if (use_utc_) datetime.toUtcTime();
    buffer.append(month_strings[datetime.month() - 1]);
}

void StandardDateTimeFlag::format(const LogInfo& info, std::string& buffer) {
    utils::DateTime datetime(info.time_point);
    if (use_utc_) datetime.toUtcTime();
    buffer.append(short_weekday_strings[datetime.weekday()]);
    buffer.push_back(' ');
    buffer.append(short_month_strings[datetime.month() - 1]);
    buffer.push_back(' ');
    buffer.append(fmt::format("{:02d}", datetime.day()));
    buffer.push_back(' ');
    buffer.append(fmt::format("{:02d}", datetime.hour()));
    buffer.push_back(':');
    buffer.append(fmt::format("{:02d}", datetime.minute()));
    buffer.push_back(':');
    buffer.append(fmt::format("{:02d}", datetime.seconds()));
    buffer.push_back(' ');
    buffer.append(fmt::format("{:04d}", datetime.year()));
}

void NameFlag::format(const LogInfo& info, std::string& buffer) {
    buffer.append(info.logger_name);
}

void ShortDateFlag::format(const LogInfo& info, std::string& buffer) {
    utils::DateTime datetime(info.time_point);
    if (use_utc_) datetime.toUtcTime();
    buffer.append(fmt::format("{:02d}", datetime.month()));
    buffer.push_back('/');
    buffer.append(fmt::format("{:02d}", datetime.day()));
    buffer.push_back('/');
    buffer.append(fmt::format("{:02d}", datetime.year() % 100));
}

void DayFlag::format(const LogInfo& info, std::string& buffer) {
    utils::DateTime datetime(info.time_point);
    if (use_utc_) datetime.toUtcTime();
    buffer.append(fmt::format("{:02d}", datetime.day()));
}

void MillisecondsFlag::format(const LogInfo& info, std::string& buffer) {
    utils::DateTime datetime(info.time_point);
    if (use_utc_) datetime.toUtcTime();
    buffer.append(fmt::format("{:03d}", datetime.milliseconds()));
}

void ThreadIdFlag::format(const LogInfo& info, std::string& buffer) {
    buffer.append(fmt::format("{}", info.thread_id));
}

void MicrosecondsFlag::format(const LogInfo& info, std::string& buffer) {
    utils::DateTime datetime(info.time_point);
    if (use_utc_) datetime.toUtcTime();
    buffer.append(fmt::format("{:06d}", datetime.microseconds()));
}

void DateFlag::format(const LogInfo& info, std::string& buffer) {
    utils::DateTime datetime(info.time_point);
    if (use_utc_) datetime.toUtcTime();
    buffer.append(fmt::format("{:02d}", datetime.year()));
    buffer.push_back('-');
    buffer.append(fmt::format("{:02d}", datetime.month()));
    buffer.push_back('-');
    buffer.append(fmt::format("{:02d}", datetime.day()));
}

void SecondsSinceEpochFlag::format(const LogInfo& info, std::string& buffer) {
    utils::DateTime datetime(info.time_point);
    if (use_utc_) datetime.toUtcTime();
    buffer.append(fmt::format("{:010d}", datetime.secondsSinceEpoch()));
}

void SourceFileNameFlag::format(const LogInfo& info, std::string& buffer) {
    utils::FileInfo fileinfo(info.location.filename);
    buffer.append(fileinfo.fileName());
}

void HourFlag::format(const LogInfo& info, std::string& buffer) {
    utils::DateTime datetime(info.time_point);
    if (use_utc_) datetime.toUtcTime();
    buffer.append(fmt::format("{:02d}", datetime.hour()));
}

void Hour12Flag::format(const LogInfo& info, std::string& buffer) {
    utils::DateTime datetime(info.time_point);
    if (use_utc_) datetime.toUtcTime();
    buffer.append(fmt::format("{:02d}", datetime.hour12()));
}

void SourceFilePathFlag::format(const LogInfo& info, std::string& buffer) {
    utils::FileInfo fileinfo(info.location.filename);
    buffer.append(fileinfo.absoulteFilePath());
}

void SourceFileLineFlag::format(const LogInfo& info, std::string& buffer) {
    buffer.append(fmt::format("{}", info.location.line));
}

void DayOfYearFlag::format(const LogInfo& info, std::string& buffer) {
    utils::DateTime datetime(info.time_point);
    if (use_utc_) datetime.toUtcTime();
    buffer.append(fmt::format("{:03d}", datetime.dayOfYear()));
}

void SourceFileFuncNameFlag::format(const LogInfo& info, std::string& buffer) {
    buffer.append(info.location.funcname);
}

void SourceFileLocationFlag::format(const LogInfo& info, std::string& buffer) {
    utils::FileInfo fileinfo(info.location.filename);
    buffer.append(fileinfo.fileName());
    buffer.push_back(':');
    buffer.append(fmt::format("{}", info.location.line));
}

void FullSourceFileLocationFlag::format(const LogInfo& info, std::string& buffer) {
    utils::FileInfo fileinfo(info.location.filename);
    buffer.append(fileinfo.absoulteFilePath());
    buffer.push_back(':');
    buffer.append(fmt::format("{}", info.location.line));
}

void ShortLogLevelFlag::format(const LogInfo& info, std::string& buffer) {
    buffer.append(level::toShortString(info.level));
}

void LogLevelFlag::format(const LogInfo& info, std::string& buffer) {
    buffer.append(level::toString(info.level));
}

void MonthFlag::format(const LogInfo& info, std::string& buffer) {
    utils::DateTime datetime(info.time_point);
    buffer.append(fmt::format("{:02d}", datetime.month()));
}

void MinuteFlag::format(const LogInfo& info, std::string& buffer) {
    utils::DateTime datetime(info.time_point);
    buffer.append(fmt::format("{:02d}", datetime.minute()));
}

void NewLineFlag::format(const LogInfo& info, std::string& buffer) {
    buffer.push_back('\n');
}

void NanosecondsFlag::format(const LogInfo& info, std::string& buffer) {
    utils::DateTime datetime(info.time_point);
    if (use_utc_) datetime.toUtcTime();
    buffer.append(fmt::format("{:09d}", datetime.nanoseconds()));
}

void AmPmFlag::format(const LogInfo& info, std::string& buffer) {
    utils::DateTime datetime(info.time_point);
    if (use_utc_) datetime.toUtcTime();
    buffer.append(datetime.isAm() ? "AM" : "PM");
}

void ProcessIdFlag::format(const LogInfo& info, std::string& buffer) {
    buffer.append(fmt::format("{:06d}", utils::os::processId()));
}

void Time12Flag::format(const LogInfo& info, std::string& buffer) {
    utils::DateTime datetime(info.time_point);
    if (use_utc_) datetime.toUtcTime();
    buffer.append(fmt::format("{:02d}", datetime.hour12()));
    buffer.push_back(':');
    buffer.append(fmt::format("{:02d}", datetime.minute()));
    buffer.push_back(':');
    buffer.append(fmt::format("{:02d}", datetime.seconds()));
    buffer.push_back(' ');
    buffer.append(datetime.isAm() ? "AM" : "PM");
}

void HourAndMinuteFlag::format(const LogInfo& info, std::string& buffer) {
    utils::DateTime datetime(info.time_point);
    if (use_utc_) datetime.toUtcTime();
    buffer.append(fmt::format("{:02d}", datetime.hour()));
    buffer.push_back(':');
    buffer.append(fmt::format("{:02d}", datetime.minute()));
}

void SecondsFlag::format(const LogInfo& info, std::string& buffer) {
    utils::DateTime datetime(info.time_point);
    if (use_utc_) datetime.toUtcTime();
    buffer.append(fmt::format("{:02d}", datetime.seconds()));
}

void TabFlag::format(const LogInfo& info, std::string& buffer) {
    buffer.push_back('\t');
}

void TimeFlag::format(const LogInfo& info, std::string& buffer) {
    utils::DateTime datetime(info.time_point);
    if (use_utc_) datetime.toUtcTime();
    buffer.append(fmt::format("{:02d}", datetime.hour()));
    buffer.push_back(':');
    buffer.append(fmt::format("{:02d}", datetime.minute()));
    buffer.push_back(':');
    buffer.append(fmt::format("{:02d}", datetime.seconds()));
}

void LogMessageFlag::format(const LogInfo& info, std::string& buffer) {
    buffer.append(info.content);
}

void ShortYearFlag::format(const LogInfo& info, std::string& buffer) {
    utils::DateTime datetime(info.time_point);
    if (use_utc_) datetime.toUtcTime();
    buffer.append(fmt::format("{:02d}", datetime.year() % 100));
}

void YearFlag::format(const LogInfo& info, std::string& buffer) {
    utils::DateTime datetime(info.time_point);
    if (use_utc_) datetime.toUtcTime();
    buffer.append(fmt::format("{:04d}", datetime.year()));
}

void ZoneFlag::format(const LogInfo& info, std::string& buffer) {
    utils::DateTime datetime(info.time_point);
    if (use_utc_) datetime.toUtcTime();
    buffer.append(datetime.zone());
}

void PercentFlag::format(const LogInfo& info, std::string& buffer) {
    buffer.push_back('%');
}

void DefaultFlag::format(const LogInfo& info, std::string& buffer) {
    utils::DateTime datetime(info.time_point);
    if (use_utc_) datetime.toUtcTime();

    // 日期部分
    buffer.push_back('[');
    buffer.append(fmt::format("{:04d}", datetime.year()));
    buffer.push_back('-');
    buffer.append(fmt::format("{:02d}", datetime.month()));
    buffer.push_back('-');
    buffer.append(fmt::format("{:02d}", datetime.day()));
    buffer.push_back(' ');

    // 时间部分
    buffer.append(fmt::format("{:02d}", datetime.hour()));
    buffer.push_back(':');
    buffer.append(fmt::format("{:02d}", datetime.minute()));
    buffer.push_back(':');
    buffer.append(fmt::format("{:02d}", datetime.seconds()));
    buffer.push_back('.');
    buffer.append(fmt::format("{:02d}", datetime.milliseconds()));
    buffer.push_back(']');
    buffer.push_back(' ');

    // 日志名称
    buffer.push_back('[');
    buffer.append(info.logger_name);
    buffer.push_back(']');
    buffer.push_back(' ');

    // 日志级别
    buffer.push_back('[');
    buffer.append(level::toString(info.level));
    buffer.push_back(']');
    buffer.push_back(' ');

    // 线程号
    buffer.push_back('[');
    buffer.append(fmt::format("{}", info.thread_id));
    buffer.push_back(']');
    buffer.push_back(' ');

    // 日志消息
    buffer.append(info.content);
}

void ContentFlag::format(const LogInfo& info, std::string& buffer) {
    buffer.append(content_);
}

void ContentFlag::append(char ch) {
    content_.push_back(ch);
}

LogFormatter::LogFormatter(std::string pattern, bool use_utc)
    : pattern_(std::move(pattern))
    , use_utc_(use_utc) {
    parse();
}

void LogFormatter::format(const LogInfo& info, std::string& buffer) {
    for (auto& f : flags_) {
        f->enableUtcTime(use_utc_);
        f->format(info, buffer);
    }

    buffer.push_back('\n');
}

void LogFormatter::setPattern(std::string pattern) {
    pattern_ = std::move(pattern);
    parse();
}

bool LogFormatter::addCustomFlag(char flag, std::function<AbstractFlagPtr()> fun) {
    auto it = hash_.find(flag);
    if (it == hash_.end()) {
        return false;
    }

    hash_[flag] = std::move(fun);
    return true;
}

void LogFormatter::removeFlag(char flag) {
    auto it = hash_.find(flag);
    if (it != hash_.end()) {
        hash_.erase(it);
    }
}

std::unique_ptr<LogFormatter> LogFormatter::clone() {
    auto cloned = std::make_unique<LogFormatter>(pattern_);
    return cloned;
}

void LogFormatter::parse() {
    auto end = pattern_.cend();

    std::unique_ptr<ContentFlag> content;
    flags_.clear();

    for (auto it = pattern_.cbegin(); it != end; ++it) {
        if (*it == '%') {
            if (content != nullptr) {
                flags_.push_back(std::move(content));
            }

            handleFlag(*++it);
        } else {
            if (content == nullptr) {
                content = std::make_unique<ContentFlag>();
            }
            content->append(*it);
        }
    }

    if (content != nullptr) {
        flags_.push_back(std::move(content));
    }
}

void LogFormatter::handleFlag(char flag) {
    auto it = hash_.find(flag);
    assert(it != hash_.end() && "Invalid formatting flag.");
    flags_.push_back(std::move(it->second()));
}

} // namespace talko::log