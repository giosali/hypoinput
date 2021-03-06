#include "environment.h"

namespace environment {

std::filesystem::path getFolderPath(SpecialFolder specialFolder)
{
    switch (specialFolder) {
    case SpecialFolder::ApplicationData: {
        wchar_t* buffer;
        SHGetKnownFolderPath(FOLDERID_RoamingAppData, KF_FLAG_DEFAULT, NULL, &buffer);
        std::filesystem::path path(buffer);
        CoTaskMemFree(buffer);
        if (!path.empty()) {
            return path;
        }

        break;
    }
    case SpecialFolder::HypoinputApplicationData:
        return getFolderPath(SpecialFolder::ApplicationData) / s_applicationName;
    case SpecialFolder::HypoinputTemp:
        return std::filesystem::temp_directory_path() / s_applicationName;
    case SpecialFolder::Executable: {
        wchar_t buffer[MAX_PATH];
        GetModuleFileName(NULL, buffer, MAX_PATH);
        return std::filesystem::path(buffer).parent_path();
    }
    }

    return std::filesystem::path();
}

std::filesystem::path getFilePath(SpecialFile specialFile)
{
    switch (specialFile) {
    case SpecialFile::Settings:
        return getFolderPath(SpecialFolder::ApplicationData) / s_applicationName / s_settingsFileName;
    case SpecialFile::TextExpansions:
        return getFolderPath(SpecialFolder::ApplicationData) / s_applicationName / s_textExpansionsFileName;
    case SpecialFile::AddTextExpansion:
        return getFolderPath(SpecialFolder::ApplicationData) / s_applicationName / s_addTextExpansionsFileName;
    case SpecialFile::EditTextExpansions:
        return getFolderPath(SpecialFolder::ApplicationData) / s_applicationName / s_editTextExpansionsFileName;
    case SpecialFile::Common:
        return getFolderPath(SpecialFolder::ApplicationData) / s_applicationName / s_commonFileName;
    case SpecialFile::ApplicationExecutable:
        return (getFolderPath(SpecialFolder::Executable) / s_applicationName).replace_extension(s_executableExtension);
    case SpecialFile::UpdaterExecutable:
        return getFolderPath(SpecialFolder::Executable) / s_updaterExecutableFileName;
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