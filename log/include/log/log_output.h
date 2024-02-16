#pragma once

#include <cstdio>
#include <future>
#include <string>

namespace talko::log {
/**
 * @brief 用于同步/异步的日志输出 不保证线程安全
 *
 */
class LogOutput {
public:
    LogOutput(FILE* handler, bool async = false);
    ~LogOutput();

    LogOutput(const LogOutput&)            = delete;
    LogOutput& operator=(const LogOutput&) = delete;

    LogOutput(LogOutput&& other) noexcept;
    LogOutput& operator=(LogOutput&& other) noexcept;

    /** 与另一个日志输出交换 */
    void swap(LogOutput& other) noexcept;

    /** 设置文件句柄 */
    void setHandler(FILE* handler);

    /** 开启异步输出方式 */
    void enableAsync(bool enabled = true);

    /** 输出内容 */
    void output(std::string_view content);

    /** 刷新内核缓冲区 */
    void flush();

private:
    /** 同步输出 */
    void sync(std::string_view content);

    /** 异步输出 */
    void async(std::string_view content);

private:
    using LogBuffer = std::string;

    static size_t max_buffer_size_; ///< 最大缓冲区长度

    FILE* handler_ { nullptr }; ///< 文件句柄
    bool  is_async_ { false };  ///< 是否为异步输出

    LogBuffer input_buf_ {};  ///< 输入缓冲区
    LogBuffer output_buf_ {}; ///< 输出缓冲区

    std::future<void> res_; ///< 异步结果
};
} // namespace talko::log