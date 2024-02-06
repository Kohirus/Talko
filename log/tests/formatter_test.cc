#include <gtest/gtest.h>
#include <log/log_formatter.h>
#include <log/log_level.h>
#include <utils/datetime.h>

using talko::log::LogFormatter;
using talko::log::LogInfo;
// using talko::log::LogLevel;
// using talko::utils::DateTime;
using talko::log::LogLocation;

std::string getTime(std::string format) {
    std::time_t now      = ::time(nullptr);
    std::tm*    timeinfo = ::localtime(&now);
    char        buf[80];
    ::strftime(buf, sizeof(buf), format.c_str(), timeinfo);
    std::string res(buf);
    res.push_back('\n');
    return res;
}

TEST(FormatterTest, ShortWeekday) {
    LogFormatter formatter("%a");
    std::string  buffer = "";
    LogInfo      info;
    formatter.format(info, buffer);
    ASSERT_STREQ(buffer.c_str(), getTime("%a").c_str());
}

TEST(FormatterTest, Weekday) {
    LogFormatter formatter("%A");
    std::string  buffer = "";
    LogInfo      info;
    formatter.format(info, buffer);
    ASSERT_STREQ(buffer.c_str(), getTime("%A").c_str());
}

TEST(FormatterTest, ShortMonthStr) {
    LogFormatter formatter("%b");
    std::string  buffer = "";
    LogInfo      info;
    formatter.format(info, buffer);
    ASSERT_STREQ(buffer.c_str(), getTime("%b").c_str());
}

TEST(FormatterTest, MonthStr) {
    LogFormatter formatter("%B");
    std::string  buffer = "";
    LogInfo      info;
    formatter.format(info, buffer);
    ASSERT_STREQ(buffer.c_str(), getTime("%B").c_str());
}

TEST(FormatterTest, LoggerName) {
    LogFormatter formatter("%C");
    std::string  buffer = "";
    LogInfo      info;
    info.logger_name = "name213";
    formatter.format(info, buffer);
    ASSERT_STREQ(buffer.c_str(), "name213\n");
}

TEST(FormatterTest, ShortDate) {
    LogFormatter formatter("%d");
    std::string  buffer = "";
    LogInfo      info;
    formatter.format(info, buffer);
    ASSERT_STREQ(buffer.c_str(), getTime("%x").c_str());
}

TEST(FormatterTest, DayOfMonth) {
    LogFormatter formatter("%D");
    std::string  buffer = "";
    LogInfo      info;
    formatter.format(info, buffer);
    ASSERT_STREQ(buffer.c_str(), getTime("%d").c_str());
}

TEST(FormatterTest, Date) {
    LogFormatter formatter("%F");
    std::string  buffer = "";
    LogInfo      info;
    formatter.format(info, buffer);
    ASSERT_STREQ(buffer.c_str(), getTime("%F").c_str());
}

TEST(FormatterTest, SourceFileName) {
    LogFormatter formatter("%G");
    std::string  buffer = "";
    LogInfo      info;
    LogLocation  loc("../log/src/log_formatter.cc", 85, "format");
    info.location = loc;
    formatter.format(info, buffer);
    ASSERT_STREQ(buffer.c_str(), "log_formatter.cc\n");
}

TEST(FormatterTest, Hour24) {
    LogFormatter formatter("%h");
    std::string  buffer = "";
    LogInfo      info;
    formatter.format(info, buffer);
    ASSERT_STREQ(buffer.c_str(), getTime("%H").c_str());
}

TEST(FormatterTest, Hour12) {
    LogFormatter formatter("%H");
    std::string  buffer = "";
    LogInfo      info;
    formatter.format(info, buffer);
    ASSERT_STREQ(buffer.c_str(), getTime("%I").c_str());
}

TEST(FormatterTest, SourceFilePath) {
    LogFormatter formatter("%i");
    std::string  buffer = "";
    LogInfo      info;
    LogLocation  loc("../log/src/log_formatter.cc", 85, "format");
    info.location = loc;
    formatter.format(info, buffer);
    ASSERT_STREQ(buffer.c_str(), "/home/leaos/Project/talko/log/src/log_formatter.cc\n");
}

TEST(FormatterTest, SourceFileLine) {
    LogFormatter formatter("%I");
    std::string  buffer = "";
    LogInfo      info;
    LogLocation  loc("../log/src/log_formatter.cc", 85, "format");
    info.location = loc;
    formatter.format(info, buffer);
    ASSERT_STREQ(buffer.c_str(), "85\n");
}

TEST(FormatterTest, DayOfYear) {
    LogFormatter formatter("%j");
    std::string  buffer = "";
    LogInfo      info;
    formatter.format(info, buffer);
    buffer.pop_back();
    std::string tmp = getTime("%j");
    tmp.pop_back();
    ASSERT_EQ(std::stoi(buffer), std::stoi(tmp));
}

TEST(FormatterTest, SourceFileFunc) {
    LogFormatter formatter("%J");
    std::string  buffer = "";
    LogInfo      info;
    LogLocation  loc("../log/src/log_formatter.cc", 85, "format");
    info.location = loc;
    formatter.format(info, buffer);
    ASSERT_STREQ(buffer.c_str(), "format\n");
}

TEST(FormatterTest, SourceFileLocation) {
    LogFormatter formatter("%k");
    std::string  buffer = "";
    LogInfo      info;
    LogLocation  loc("../log/src/log_formatter.cc", 85, "format");
    info.location = loc;
    formatter.format(info, buffer);
    ASSERT_STREQ(buffer.c_str(), "log_formatter.cc:85\n");
}

TEST(FormatterTest, SourceFileFullLocation) {
    LogFormatter formatter("%K");
    std::string  buffer = "";
    LogInfo      info;
    LogLocation  loc("../log/src/log_formatter.cc", 85, "format");
    info.location = loc;
    formatter.format(info, buffer);
    ASSERT_STREQ(buffer.c_str(), "/home/leaos/Project/talko/log/src/log_formatter.cc:85\n");
}

TEST(FormatterTest, ShortLogLevel) {
    LogFormatter formatter("%l");
    std::string  buffer = "";
    LogInfo      info;
    formatter.format(info, buffer);
    ASSERT_STREQ(buffer.c_str(), "I\n");
}

TEST(FormatterTest, LogLevel) {
    LogFormatter formatter("%L");
    std::string  buffer = "";
    LogInfo      info;
    formatter.format(info, buffer);
    ASSERT_STREQ(buffer.c_str(), "info\n");
}

TEST(FormatterTest, Minute) {
    LogFormatter formatter("%m");
    std::string  buffer = "";
    LogInfo      info;
    formatter.format(info, buffer);
    ASSERT_STREQ(buffer.c_str(), getTime("%M").c_str());
}

TEST(FormatterTest, Month) {
    LogFormatter formatter("%M");
    std::string  buffer = "";
    LogInfo      info;
    formatter.format(info, buffer);
    ASSERT_STREQ(buffer.c_str(), getTime("%m").c_str());
}

TEST(FormatterTest, NewLine) {
    LogFormatter formatter("%n");
    std::string  buffer = "";
    LogInfo      info;
    formatter.format(info, buffer);
    ASSERT_STREQ(buffer.c_str(), "\n\n");
}

TEST(FormatterTest, AMPM) {
    LogFormatter formatter("%p");
    std::string  buffer = "";
    LogInfo      info;
    formatter.format(info, buffer);
    ASSERT_STREQ(buffer.c_str(), getTime("%p").c_str());
}

TEST(FormatterTest, HourAndMinute) {
    LogFormatter formatter("%R");
    std::string  buffer = "";
    LogInfo      info;
    formatter.format(info, buffer);
    ASSERT_STREQ(buffer.c_str(), getTime("%R").c_str());
}

TEST(FormatterTest, Seconds) {
    LogFormatter formatter("%s");
    std::string  buffer = "";
    LogInfo      info;
    formatter.format(info, buffer);
    ASSERT_STREQ(buffer.c_str(), getTime("%S").c_str());
}

TEST(FormatterTest, Tab) {
    LogFormatter formatter("%t");
    std::string  buffer = "";
    LogInfo      info;
    formatter.format(info, buffer);
    ASSERT_STREQ(buffer.c_str(), "\t\n");
}

TEST(FormatterTest, Time) {
    LogFormatter formatter("%T");
    std::string  buffer = "";
    LogInfo      info;
    formatter.format(info, buffer);
    ASSERT_STREQ(buffer.c_str(), getTime("%T").c_str());
}

TEST(FormatterTest, Content) {
    LogFormatter formatter("%v");
    std::string  buffer = "";
    LogInfo      info;
    info.content = "some message...";
    formatter.format(info, buffer);
    ASSERT_STREQ(buffer.c_str(), "some message...\n");
}

TEST(FormatterTest, ShortYear) {
    LogFormatter formatter("%y");
    std::string  buffer = "";
    LogInfo      info;
    formatter.format(info, buffer);
    ASSERT_STREQ(buffer.c_str(), getTime("%y").c_str());
}

TEST(FormatterTest, Year) {
    LogFormatter formatter("%Y");
    std::string  buffer = "";
    LogInfo      info;
    formatter.format(info, buffer);
    ASSERT_STREQ(buffer.c_str(), getTime("%Y").c_str());
}

TEST(FormatterTest, Zone) {
    LogFormatter formatter("%z");
    std::string  buffer = "";
    LogInfo      info;
    formatter.format(info, buffer);
    ASSERT_STREQ(buffer.c_str(), getTime("%Z").c_str());
}

int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
};