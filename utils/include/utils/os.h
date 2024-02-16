#pragma once

#include <cstddef>

namespace talko::utils::os {
/** 获取线程号 */
long threadId();

/** 获取进程号 */
int processId();
} // namespace talko::utils::os