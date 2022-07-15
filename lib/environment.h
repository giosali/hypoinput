#pragma once

#include "utils.h"
#include <ShlObj.h>
#include <Windows.h>
#include <filesystem>
#include <string>
#include <string_view>

namespace environment {

namespace constants {

    inline constexpr std::string_view runAtStartup = "Settings.runAtStartup";
    inline constexpr std::wstring_view runSubkey = L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run";

} // namespace constants

namespace {

    static const std::string_view ApplicationName = "Hypoinput";
    static const std::string_view SettingsFileName = "Settings.ini";
    static const std::string TextExpansionsFileName = "TextExpansions.json";
    static const std::string AddTextExpansionsFileName = "Add-TextExpansion.ps1";
    static const std::string EditTextExpansionsFileName = "Edit-TextExpansions.ps1";
    static const std::string CommonFileName = "Common.ps1";
    static const std::string TempDirectoryName = "tmp";
    static const std::string ApplicationExecutableFileName = std::string(ApplicationName) + ".exe";
    static const std::string OldApplicationExecutableFileName = std::string(ApplicationName) + "_old.exe";
    static const std::string UpdaterExecutableFileName = "updater.exe";

} // namespace

enum class SpecialFolder {
    ApplicationData,
    HypoinputApplicationData,
    TempHypoinputApplicationData,
    ProgramFiles,
    HypoinputProgramFiles
};

enum class SpecialFile {
    Settings,
    TextExpansions,
    AddTextExpansion,
    EditTextExpansions,
    Common,
    ApplicationExecutable,
    OldApplicationExecutable,
    UpdaterExecutable
};

std::filesystem::path getFolderPath(SpecialFolder);
std::filesystem::path getFilePath(SpecialFile);
std::string getResource(uint32_t, const std::wstring&);

} // namespace environment