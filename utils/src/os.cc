#include <sys/syscall.h>
#include <sys/types.h>
#include <unistd.h>
#include <utils/os.h>

namespace talko {
namespace utils {
namespace os {
long threadId() {
    static thread_local const long tid = ::syscall(SYS_gettid);
    return tid;
}

int processId() {
    static pid_t pid = ::getpid();
    return pid;
}
} // namespace os
} // namespace utils
} // namespace talko