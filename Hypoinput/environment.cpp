#include "environment.h"

namespace environment {

std::filesystem::path getFolderPath(SpecialFolder specialFolder)
{
    switch (specialFolder) {
    case SpecialFolder::ApplicationData:
        if (char* envItem = std::getenv("APPDATA")) {
            return std::filesystem::path(envItem);
        }

        break;
    }
}

std::filesystem::path getFilePath(SpecialFile specialFile)
{
    switch (specialFile) {
    case SpecialFile::TextExpansions:
        return getFolderPath(SpecialFolder::ApplicationData) / ApplicationName / TextExpansionsFileName;
    }
}

} // namespace environment