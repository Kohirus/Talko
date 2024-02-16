#pragma once

#include <string>
#include <utils/datetime.h>

namespace talko::utils {
/** 文件相关的时间类型 */
enum class FileTime {
    access, ///< 最后访问时间
    modify, ///< 最后修改时间
    status  ///< 最后状态更改时间
};
/**
 * @brief 文件信息类
 *
 */
class FileInfo {
public:
    FileInfo(std::string filename);

    FileInfo(const FileInfo& other)            = default;
    FileInfo& operator=(const FileInfo& other) = default;

    FileInfo(FileInfo&& other)            = default;
    FileInfo& operator=(FileInfo&& other) = default;

    ~FileInfo() = default;

    /** 是否为绝对路径 */
    bool isAbsoulte() const;

    /** 是否为相对路径 */
    bool isRelative() const;

    /** 是否为目录或指向目录的符号链接 */
    bool isDirectory() const;

    /** 是否为文件或指向文件的符号链接 */
    bool isFile() const;

    /** 是否为隐藏文件 */
    bool isHidden() const;

    /** 文件是否存在 */
    bool exists() const;

    /** 文件是否存在 */
    static bool exists(const std::string& filename);

    /** 返回文件的后缀 /path/file.tar.gz ==> gz */
    std::string suffix() const;

    /** 返回文件的完整后缀 /path/file.tar.gz ==> tar.gz */
    std::string completeSuffix() const;

    /** 返回文件的名称(不包含路径) /path/file.tar.gz ==> file.tar.gz */
    std::string fileName() const;

    /** 返回文件的基本名称(不包含路径) /path/file.tar.gz ==> file */
    std::string baseName() const;

    /** 返回文件的完整基本名称(不包含路径) /path/file.tar.gz ==> file.tar */
    std::string completeBaseName() const;

    /** 返回文件名，包含路径(可以是绝对的或相对的) */
    std::string filePath() const;

    /** 返回不包含文件名的绝对路径 /path/file.tar.gz => /path */
    std::string absoultePath() const;

    /** 返回包含文件名的绝对路径 /path/file.tar.gz ==> /path/file.tar.gz */
    std::string absoulteFilePath() const;

    /** 返回文件的路径(不包含文件名) */
    std::string path() const;

    /** 返回文件大小 */
    size_t size() const;

    /** 获取文件相关的时间 */
    std::optional<DateTime> fileTime(FileTime time) const;

private:
    std::string filename_; ///< 文件名
};
} // namespace talko::utils