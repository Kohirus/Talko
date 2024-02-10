#include <log/output_strategy.h>

namespace talko::log {
SyncStrategy::SyncStrategy() {
    handler_ = stdout;
}

void SyncStrategy::output(std::string_view content) {
    ::fwrite_unlocked(content.data(), sizeof(char), content.size(), stdout);
    ::fflush_unlocked(stdout); // 立即输出到终端
}
} // namespace talko::log