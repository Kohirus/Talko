#pragma once

#include <string>
#include <filesystem>

namespace talko {
namespace utils {
/**
 * @brief 目录类
 * 
 */
class Directory {
public:
    Directory(std::string path);

private:
    std::string path_; ///< 路径
};
} // namespace utils
} // namespace talko