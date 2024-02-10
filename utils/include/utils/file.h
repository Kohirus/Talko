#pragma once

#include <string>

namespace talko {
namespace utils {
/**
 * @brief 文件操作类
 * 
 */
class File {
public:
    bool open(std::string filename);
private:
    int fd_; ///< 文件描述符
};
} // namespace utils
} // namespace talko