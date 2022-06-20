#include "file.h"

namespace file {

void create(const std::filesystem::path& path)
{
    // Creates any missing parent directories.
    std::filesystem::create_directories(path.parent_path());

    // Exits if the file already exists.
    if (std::filesystem::exists(path)) {
        return;
    }

    // Creates the file.
    std::fstream fs;
    fs.open(path);
    fs.close();
}

}