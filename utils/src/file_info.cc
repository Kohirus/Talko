#include <cstdlib>
#include <limits.h>
#include <sys/stat.h>
#include <unistd.h>
#include <utils/file_info.h>

namespace talko {
namespace utils {
FileInfo::FileInfo(std::string filename)
    : filename_(std::move(filename)) {
}

bool FileInfo::isAbsoulte() const {
    if (filename_.empty()) return false;
    return filename_[0] == '/';
}

bool FileInfo::isRelative() const {
    if (filename_.empty()) return false;
    return filename_[0] != '/';
}

bool FileInfo::isDirectory() const {
    struct stat st;
    if (::stat(filename_.c_str(), &st) != 0) {
        return false;
    }

    return S_ISDIR(st.st_mode) || S_ISLNK(st.st_mode);
}

bool FileInfo::isFile() const {
    struct stat st;
    if (::stat(filename_.c_str(), &st) != 0) {
        return false;
    }

    return S_ISREG(st.st_mode) || S_ISLNK(st.st_mode);
}

bool FileInfo::isHidden() const {
    std::string filename = fileName();
    if (filename.empty()) return false;
    return filename[0] == '.';
}

bool FileInfo::exists() const {
    return ::access(filename_.c_str(), F_OK) == 0;
}

bool FileInfo::exists(const std::string& filename) {
    return ::access(filename.c_str(), F_OK) == 0;
}

std::string FileInfo::suffix() const {
    std::string filename = fileName();
    if (filename.empty()) return "";

    size_t idx = filename.rfind('.');
    if (idx == filename.npos || idx == 0) {
        return "";
    }

    return filename.substr(idx + 1, filename.size() - idx);
}

std::string FileInfo::completeSuffix() const {
    std::string filename = fileName();
    if (filename.empty()) return "";

    size_t idx = filename.find('.', (filename[0] != '.') ? 0 : 1);
    if (idx == std::string::npos) {
        return "";
    }

    return filename.substr(idx + 1, filename.size() - idx);
}

std::string FileInfo::fileName() const {
    std::string path = absoulteFilePath();
    if (path.empty()) return "";
    if (path.back() == '/') path.pop_back();

    size_t      idx = path.rfind('/');
    std::string tmp = path;
    if (idx != std::string::npos) {
        tmp = path.substr(idx + 1);
    }

    return tmp;
}

std::string FileInfo::baseName() const {
    std::string filename = fileName();
    if (filename.empty()) return "";

    size_t idx = filename.find('.', filename[0] == '.' ? 1 : 0);
    if (idx == std::string::npos) {
        idx = filename.size();
    }

    return filename.substr(0, idx);
}

std::string FileInfo::completeBaseName() const {
    std::string filename = fileName();
    if (filename.empty()) return "";

    size_t idx = filename.rfind('.');
    if (idx == std::string::npos || idx == 0) {
        idx = filename.size();
    }

    return filename.substr(0, idx);
}

std::string FileInfo::filePath() const {
    return filename_;
}

std::string FileInfo::absoultePath() const {
    std::string path = absoulteFilePath();
    if (path.empty()) return "";

    size_t idx = path.rfind('/', path.back() == '/' ? path.size() - 1 : std::string::npos);
    if (idx == std::string::npos) {
        return "";
    }
    return path.substr(0, idx);
}

std::string FileInfo::absoulteFilePath() const {
    char resolvedPath[PATH_MAX];
    if (realpath(filename_.c_str(), resolvedPath) == nullptr) {
        return "";
    }
    return resolvedPath;
}

std::string FileInfo::path() const {
    if (filename_.empty()) return "";
    size_t idx = filename_.rfind('/');
    if (idx == std::string::npos) {
        return "";
    }
    return filename_.substr(0, idx);
}

size_t FileInfo::size() const {
    struct stat st;
    if (::stat(filename_.c_str(), &st) != 0) {
        return 0;
    }

    return st.st_size;
}

DateTime FileInfo::fileTime(FileTime time) const {
    struct stat st;

    if (::stat(filename_.c_str(), &st) != 0) {
        return DateTime::invalid();
    }

    switch (time) {
    case FileTime::access:
        return DateTime(st.st_atime);
    case FileTime::modify:
        return DateTime(st.st_mtime);
    case FileTime::status:
        return DateTime(st.st_ctime);
    default:
        return DateTime::invalid();
    }

    return DateTime::invalid();
}
} // namespace utils
} // namespace talko