#include <cerrno>
#include <chrono>
#include <cstring>
#include <sys/syscall.h>
#include <sys/types.h>
#include <thread>
#include <unistd.h>
#include <utils/os.h>

namespace talko::utils::os {
long threadId() {
    static thread_local const long tid = ::syscall(SYS_gettid);
    return tid;
}

int processId() {
    static pid_t pid = ::getpid();
    return pid;
}

void sleepForSeconds(size_t sec) {
    std::this_thread::sleep_for(std::chrono::seconds(sec));
}

void sleepForMilliseconds(size_t milli) {
    std::this_thread::sleep_for(std::chrono::milliseconds(milli));
}

void sleepForMicroseconds(size_t micro) {
    std::this_thread::sleep_for(std::chrono::microseconds(micro));
}

void sleepForNanoseconds(size_t nano) {
    std::this_thread::sleep_for(std::chrono::nanoseconds(nano));
}
} // namespace talko::utils::os