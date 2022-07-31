#pragma once

#include <ShlObj.h>
#include <Windows.h>
#include <filesystem>
#include <string>
#include <string_view>

namespace environment {

namespace constants {

    inline constexpr std::string_view settingsSection = "Settings";
    inline constexpr std::string_view runAtStartupKey = "runAtStartup";
    inline constexpr std::string_view languageKey = "language";
    inline constexpr std::string_view oldSuffix = "_old";
    inline constexpr std::wstring_view runSubkey = L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run";

} // namespace constants

namespace {

    inline static constexpr std::string_view s_applicationName = "Hypoinput";
    inline static constexpr std::string_view s_settingsFileName = "Settings.ini";
    inline static constexpr std::string_view s_textExpansionsFileName = "TextExpansions.json";
    inline static constexpr std::string_view s_addTextExpansionsFileName = "Add-TextExpansion.ps1";
    inline static constexpr std::string_view s_editTextExpansionsFileName = "Edit-TextExpansions.ps1";
    inline static constexpr std::string_view s_commonFileName = "Common.ps1";
    inline static constexpr std::string_view s_executableExtension = ".exe";
    inline static constexpr std::string_view s_updaterExecutableFileName = "updater.exe";

} // namespace

enum class SpecialFolder {
    ApplicationData,
    HypoinputApplicationData,
    HypoinputTemp,
    Executable
};

enum class SpecialFile {
    Settings,
    TextExpansions,
    AddTextExpansion,
    EditTextExpansions,
    Common,
    ApplicationExecutable,
    UpdaterExecutable
};

std::filesystem::path getFolderPath(SpecialFolder);
std::filesystem::path getFilePath(SpecialFile);
std::string getResource(uint32_t, const std::wstring&);

} // namespace environment