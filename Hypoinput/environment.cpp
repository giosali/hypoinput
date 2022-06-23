#include "environment.h"

namespace environment {

std::filesystem::path getFolderPath(SpecialFolder specialFolder)
{
    switch (specialFolder) {
    case SpecialFolder::ApplicationData:
        char* buffer;
        if (_dupenv_s(&buffer, NULL, "APPDATA") == 0 && buffer != nullptr) {
            std::filesystem::path path = buffer;
            free(buffer);
            return path;
        }

        break;
    }

    return std::filesystem::path();
}

std::filesystem::path getFilePath(SpecialFile specialFile)
{
    switch (specialFile) {
    case SpecialFile::TextExpansions:
        return getFolderPath(SpecialFolder::ApplicationData) / ApplicationName / TextExpansionsFileName;
    }

    return std::filesystem::path();
}

} // namespace environment