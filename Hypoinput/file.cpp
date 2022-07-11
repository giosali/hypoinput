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
    std::ofstream ofs;
    ofs.open(path);
    ofs.close();
}

void write(const std::filesystem::path& path, const std::string& text)
{
    // Creates any missing parent directories.
    std::filesystem::create_directories(path.parent_path());

    std::ofstream ofs(path);
    if (ofs.is_open()) {
        char bom[] = { (char)0xEF, (char)0xBB, (char)0xBF, '\0' };
        std::vector<std::string> lines = utils::split(text, '\n');
        for (int i = 0; i < lines.size(); i++) {
            ofs << utils::trim(utils::replace(lines[i], std::string(bom)), '\r') << std::endl;
        }
    }

    ofs.close();
}

} // namespace file