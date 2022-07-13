#include "environment.h"

namespace environment {

std::filesystem::path getFolderPath(SpecialFolder specialFolder)
{
    switch (specialFolder) {
    case SpecialFolder::ApplicationData: {
        wchar_t* buffer;
        SHGetKnownFolderPath(FOLDERID_RoamingAppData, KF_FLAG_DEFAULT, NULL, &buffer);
        std::filesystem::path path(utils::wstringToString(buffer));
        CoTaskMemFree(buffer);
        if (!path.empty()) {
            return path;
        }

        break;
    }
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
    case SpecialFile::AddTextExpansion:
        return getFolderPath(SpecialFolder::ApplicationData) / ApplicationName / AddTextExpansionsFileName;
    case SpecialFile::EditTextExpansions:
        return getFolderPath(SpecialFolder::ApplicationData) / ApplicationName / EditTextExpansionsFileName;
    case SpecialFile::Common:
        return getFolderPath(SpecialFolder::ApplicationData) / ApplicationName / CommonFileName;
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