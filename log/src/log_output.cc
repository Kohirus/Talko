#include <cassert>
#include <log/log_output.h>
#include <pool/thread_pool.h>

/** 输出缓冲区内容 */
void outputBuffer(FILE* handler, std::string_view content) {
    ::fwrite_unlocked(content.data(), sizeof(char), content.size(), handler);
}

namespace talko::log {
size_t LogOutput::max_buffer_size_ { 4096 };

LogOutput::LogOutput(FILE* handler, bool async)
    : handler_(handler)
    , is_async_(async) {
}

LogOutput::~LogOutput() {
    if (is_async_) {
        // 异步输出时确保对象销毁前 输入缓冲区中的余留日志也完成输出任务
        auto res = tp::submitTask(outputBuffer, handler_, input_buf_);
        res.get(); // 阻塞等待输出完成
    }
}

LogOutput::LogOutput(LogOutput&& other) noexcept
    : handler_(std::move(other.handler_))
    , is_async_(other.is_async_)
    , input_buf_(std::move(other.input_buf_))
    , output_buf_(std::move(other.output_buf_))
    , res_(std::move(other.res_)) {
}

LogOutput& LogOutput::operator=(LogOutput&& other) noexcept {
    this->swap(other);
    return *this;
}

void LogOutput::swap(LogOutput& other) noexcept {
    handler_    = std::move(other.handler_);
    is_async_   = other.is_async_;
    input_buf_  = std::move(other.input_buf_);
    output_buf_ = std::move(other.output_buf_);
    res_        = std::move(other.res_);
}

void LogOutput::setHandler(FILE* handler) {
    handler_ = handler;
}

void LogOutput::enableAsync(bool enabled) {
    is_async_ = enabled;
}

void LogOutput::output(std::string_view content) {
    assert(handler_ != nullptr && "File handler is nullptr.");
    if (is_async_) {
        async(content);
    } else {
        sync(content);
    }
}

void LogOutput::flush() {
    assert(handler_ != nullptr && "File handler is nullptr.");
    ::fflush_unlocked(handler_);
}

void LogOutput::sync(std::string_view contnet) {
    ::fwrite_unlocked(contnet.data(), sizeof(char), contnet.size(), handler_);
}

void LogOutput::async(std::string_view content) {
    assert(tp::isRunning() && "Thread pool is not running.");

    if (input_buf_.size() + content.size() > max_buffer_size_) {
        if (res_.valid())
            res_.get(); // 等待输出缓冲区输出完成
        output_buf_ = std::move(input_buf_);

        res_ = tp::submitTask(outputBuffer, handler_, output_buf_);
    }

    input_buf_.append(content);
}
} // namespace talko::log