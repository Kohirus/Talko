#include <utils/directory.h>

namespace talko {
namespace utils {
Directory::Directory(std::string path)
    : path_(std::move(path)) {
}
} // namespace utils
} // namespace talko