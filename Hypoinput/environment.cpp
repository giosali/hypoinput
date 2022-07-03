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
    case SpecialFolder::HypoinputApplicationData:
        return getFolderPath(SpecialFolder::ApplicationData) / ApplicationName;
    }

    return std::filesystem::path();
}

std::filesystem::path getFilePath(SpecialFile specialFile)
{
    switch (specialFile) {
    case SpecialFile::Settings:
        return getFolderPath(SpecialFolder::ApplicationData) / ApplicationName / SettingsFileName;
    case SpecialFile::TextExpansions:
        return getFolderPath(SpecialFolder::ApplicationData) / ApplicationName / TextExpansionsFileName;
    }

    return std::filesystem::path();
}

std::string getResource(uint32_t resourceId, const std::wstring& resourceType)
{
    HINSTANCE hInstance = GetModuleHandle(NULL);
    HRSRC hResource = FindResource(hInstance, MAKEINTRESOURCE(resourceId), resourceType.c_str());
    if (hResource == NULL) {
        return std::string();
    }

    HGLOBAL hResourceData = LoadResource(hInstance, hResource);
    if (hResourceData == NULL) {
        return std::string();
    }

    return std::string(static_cast<const char*>(LockResource(hResourceData)));
}

} // namespace environment