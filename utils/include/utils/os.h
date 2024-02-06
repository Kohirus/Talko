#pragma once

#include <cstddef>

namespace talko {
namespace utils {
namespace os {
/** 获取线程号 */
long threadId();

/** 获取进程号 */
int processId();
} // namespace os
} // namespace utils
} // namespace talko