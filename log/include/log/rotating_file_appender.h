#pragma once

#include <cassert>
#include <filesystem>
#include <iostream>
#include <log/factory.h>
#include <log/log_appender.h>
#include <log/log_formatter.h>

namespace talko::log {
constexpr size_t B  = 1;
constexpr size_t KB = 1024 * B;
constexpr size_t MB = 1024 * KB;
constexpr size_t GB = 1024 * MB;
constexpr size_t TB = 1024 * GB;

/**
 * @brief 旋转文件输出器
 *
 * @tparam MutexType 互斥器类型
 */
template <typename MutexType>
class RotatingFileAppender : public LogAppender {
public:
    /**
     * @brief Construct a new Rotating File Appender object
     *
     * @param filename 文件名称
     * @param max_file_size 最大文件大小
     * @param max_file_num 最大文件数目
     * @param async 是否异步输出
     */
    RotatingFileAppender(const std::string& filename, size_t max_file_size, size_t max_file_num, bool async = false)
        : LogAppender(async)
        , formatter_(std::make_unique<LogFormatter>("%#"))
        , basename_(filename)
        , cur_filename_(basename_)
        , max_file_size_(max_file_size)
        , max_file_num_(max_file_num) {
        assert(!basename_.empty() && "Filename is empty.");
        assert(max_file_num > 0 && "Max file num must be greater than 0.");
        assert(max_file_size > 0 && "Max file size must be greater than 0.");
        openLogFile();
    }

    ~RotatingFileAppender() = default;

    /** 输出日志 */
    void log(const LogInfo& info) override {
        std::lock_guard<MutexType> lock(mtx_);

        // 格式化日志消息
        std::string formatted;
        formatter_->format(info, formatted);

        // 判断是否需要旋转文件
        size_t new_file_size = cur_file_size_ + formatted.size();
        if (new_file_size > max_file_size_) {
            caclCurrentFileName();
            openLogFile();
            new_file_size = formatted.size();
        }

        // 输出日志
        output_.output(formatted);
        cur_file_size_ = new_file_size;
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

private:
    /** 计算获取当前文件名称 */
    void caclCurrentFileName() {
        static size_t cur_idx = 1;
        if (cur_idx != 0) {
            std::filesystem::path file_path(basename_);
            cur_filename_ = file_path.parent_path().string() + "/" + file_path.stem().string() + "." + std::to_string(cur_idx);
            if (file_path.has_extension()) {
                cur_filename_.append(file_path.extension().string());
            }
        } else {
            cur_filename_ = basename_;
        }

        ++cur_idx;
        if (cur_idx >= max_file_num_) {
            cur_idx = 0;
        }
    }

    /** 打开日志文件 */
    void openLogFile() {
        std::filesystem::path file_path(cur_filename_);

        // 判断是否含有文件名
        std::string err_msg = "'" + cur_filename_ + "' has not filename";
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
            err_msg = "File path '" + cur_filename_ + "' is not a regular file.";
            assert(std::filesystem::is_regular_file(file_path) && err_msg.c_str());
        }

        // 打开文件 文件不存在则创建文件
        FILE* file = ::fopen(cur_filename_.c_str(), "w");
        err_msg    = "Failed to open file: '" + cur_filename_ + "'.";
        assert(file != nullptr && err_msg.c_str());
        output_.setHandler(file);
        cur_file_size_ = 0;
    }

private:
    std::string  basename_;     ///< 基础文件名称
    std::string  cur_filename_; ///< 当前文件名称
    MutexType    mtx_;          ///< 互斥锁
    FormatterPtr formatter_;    ///< 格式化器

    size_t max_file_size_;       ///< 最大文件大小
    size_t max_file_num_;        ///< 最大文件数目
    size_t cur_file_size_ { 0 }; ///< 当前文件大小
};

using RotatingFileAppenderSt = RotatingFileAppender<FakeMutex>;
using RotatingFileAppenderMt = RotatingFileAppender<RealMutex>;

/**
 * @brief 获取单线程旋转文件日志器
 *
 * @param logger_name 日志名称
 * @param filename 文件名称
 * @param max_file_size 最大文件大小
 * @param max_file_num 最大文件数目
 * @param async 是否为异步日志器
 * @return std::shared_ptr<Logger> 返回日志器
 */
inline std::shared_ptr<Logger> createRotatingFileLoggerSt(const std::string& logger_name, const std::string& filename,
    size_t max_file_size, size_t max_file_num, bool async = false) {
    return Factory::create<RotatingFileAppenderSt>(logger_name, filename, max_file_size, max_file_num, async);
}

/**
 * @brief 获取多线程旋转文件日志器
 *
 * @param logger_name 日志名称
 * @param filename 文件名称
 * @param max_file_size 最大文件大小
 * @param max_file_num 最大文件数目
 * @param async 是否为异步日志器
 * @return std::shared_ptr<Logger> 返回日志器
 */
inline std::shared_ptr<Logger> createRotatingFileLoggerMt(const std::string& logger_name, const std::string& filename,
    size_t max_file_size, size_t max_file_num, bool async = false) {
    return Factory::create<RotatingFileAppenderMt>(logger_name, filename, max_file_size, max_file_num, async);
}
} // namespace talko::log