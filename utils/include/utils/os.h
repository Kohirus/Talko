#pragma once

#include <cstddef>
#include <string>

namespace talko::utils::os {
/** 获取线程号 */
long threadId();

/** 获取进程号 */
int processId();

/** 当前线程休眠指定秒数 */
void sleepForSeconds(size_t sec);

/** 当前线程休眠指定毫秒数 */
void sleepForMilliseconds(size_t milli);

/** 当前线程休眠指定微秒数 */
void sleepForMicroseconds(size_t micro);

/** 当前线程休眠指定纳秒数 */
void sleepForNanoseconds(size_t nano);
} // namespace talko::utils::os