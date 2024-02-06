#include <iostream>
#include <utils/datetime.h>

int main() {
    talko::utils::DateTime datetime = talko::utils::DateTime::now();
    std::cout << "Local time:" << std::endl;
    std::cout << datetime.year() << "-" << datetime.month() << "-"
              << datetime.day() << " " << datetime.hour() << ":"
              << datetime.minute() << ":" << datetime.seconds() << "."
              << datetime.milliseconds() << "." << datetime.microseconds() << "."
              << datetime.nanoseconds() << std::endl;
    std::cout << datetime.dayOfYear() << "st days of the year" << std::endl;
    std::cout << (datetime.isAm() ? "AM" : "PM") << std::endl;
    std::cout << datetime.hour12() << " under 12-hour" << std::endl;
    std::cout << "Weekday: " << datetime.weekday() << std::endl;
    std::cout << "Zone: " << datetime.zone() << std::endl;
    std::cout << "Seconds since epoch: " << datetime.secondsSinceEpoch() << std::endl;

    std::cout << "====================================" << std::endl;

    datetime.toUtcTime();
    std::cout << "UTC time:" << std::endl;
    std::cout << datetime.year() << "-" << datetime.month() << "-"
              << datetime.day() << " " << datetime.hour() << ":"
              << datetime.minute() << ":" << datetime.seconds() << "."
              << datetime.milliseconds() << "." << datetime.microseconds() << "."
              << datetime.nanoseconds() << std::endl;
    std::cout << datetime.dayOfYear() << "st days of the year" << std::endl;
    std::cout << (datetime.isAm() ? "AM" : "PM") << std::endl;
    std::cout << datetime.hour12() << " under 12-hour" << std::endl;
    std::cout << "Weekday: " << datetime.weekday() << std::endl;
    std::cout << "Zone: " << datetime.zone() << std::endl;
    std::cout << "Seconds since epoch: " << datetime.secondsSinceEpoch() << std::endl;
    return 0;
}