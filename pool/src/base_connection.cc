#include <pool/base_connection.h>

namespace talko::pool {
BaseConnection::BaseConnection()
    : start_idle_time_(std::chrono::high_resolution_clock::now()) {
}

void BaseConnection::resetIdleTime() {
    start_idle_time_ = std::chrono::high_resolution_clock::now();
}

Duration BaseConnection::getIdleDuration() const {
    return std::chrono::duration_cast<Duration>(std::chrono::high_resolution_clock::now() - start_idle_time_);
}
} // namespace talko::pool