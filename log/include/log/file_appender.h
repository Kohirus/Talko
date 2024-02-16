#pragma once

#include <cassert>
#include <filesystem>
#include <log/factory.h>
#include <log/log_appender.h>
#include <log/log_formatter.h>

namespace talko::log {
/**
 * @brief 文件输出器
 *
 * @tparam MutexType 互斥器类型
 */
template <typename MutexType>
class FileAppender : public LogAppender {
public:
    /**
     * @brief Construct a new File Appender object
     *
     * @param filename 文件名称
     * @param rewrite 是否重写文件
     * @param async 是否异步输出
     */
    FileAppender(std::string filename, bool rewrite = false, bool async = false)
        : LogAppender(async)
        , formatter_(std::make_unique<LogFormatter>("%#"))
        , filename_(std::move(filename))
        , mode_(rewrite ? "w" : "a") {
        openLogFile();
    }

    ~FileAppender() {
    }

    void log(const LogInfo& info) override {
        std::lock_guard<MutexType> lock(mtx_);

        // 格式化日志消息
        std::string formatted;
        formatter_->format(info, formatted);

        // 输出日志
        output_.output(formatted);
    }

    /** 刷新日志 */
    void flush() {
        output_.flush();
    }

    /** 设置模式串 */
    void setPattern(const std::string& pattern) override {
        std::lock_guard<MutexType> lock(mtx_);
        formatter_ = std::make_unique<LogFormatter>(pattern);
    }

    /** 设置格式化器 */
    void setFormatter(FormatterPtr formatter) override {
        std::lock_guard<MutexType> lock(mtx_);
        formatter_ = std::move(formatter);
    }

    /** 获取文件名称 */
    const std::string& filename() const {
        return filename_;
    }

private:
    /** 打开日志文件 */
    void openLogFile() {
        std::filesystem::path file_path(filename_);

        // 判断是否含有文件名
        std::string err_msg = "'" + filename_ + "' has not filename";
        assert(file_path.has_filename() && err_msg.c_str());

        if (!std::filesystem::exists(file_path)) {
            // 如果文件不存在则创建路径及文件
            auto parent_path = file_path.parent_path();

            // 判断该路径是否存在 不存在则创建路径
            if (!std::filesystem::exists(parent_path)) {
                err_msg = "Failed to create path: " + parent_path.string() + "'.";
                assert(std::filesystem::create_directories(parent_path) && err_msg.c_str());
            }
        } else {
            // 若文件存在则判断指定路径是否为文件
            err_msg = "File path '" + filename_ + "' is not a regular file.";
            assert(std::filesystem::is_regular_file(file_path) && err_msg.c_str());
        }

        // 打开文件 文件不存在则创建文件
        FILE* file = ::fopen(filename_.c_str(), mode_.c_str());
        err_msg    = "Failed to create file: '" + filename_ + "'.";
        assert(file != nullptr && err_msg.c_str());
        output_.setHandler(file);
    }

private:
    std::string  filename_;  ///< 文件名称
    std::string  mode_;      ///< 文件访问模式
    MutexType    mtx_;       ///< 互斥锁
    FormatterPtr formatter_; ///< 格式化器
};

using FileAppenderSt = FileAppender<FakeMutex>;
using FileAppenderMt = FileAppender<RealMutex>;

/**
 * @brief 获取单线程文件日志器
 *
 * @param logger_name 日志名称
 * @param filename 文件名称
 * @param rewrite 是否重写文件
 * @param async 是否为异步日志器
 * @return std::shared_ptr<Logger> 返回日志器
 */
inline std::shared_ptr<Logger> createFileLoggerSt(const std::string& logger_name, const std::string& filename,
    bool rewrite = false, bool async = false) {
    return Factory::create<FileAppenderSt>(logger_name, filename, rewrite, async);
}

/**
 * @brief 获取多线程文件日志器
 *
 * @param logger_name 日志名称
 * @param filename 文件名称
 * @param rewrite 是否重写文件
 * @param async 是否为异步日志器
 * @return std::shared_ptr<Logger> 返回日志器
 */
inline std::shared_ptr<Logger> createFileLoggerMt(const std::string& logger_name, const std::string& filename,
    bool rewrite = false, bool async = false) {
    return Factory::create<FileAppenderMt>(logger_name, filename, rewrite, async);
}
} // namespace talko::log