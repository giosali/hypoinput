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

    static const std::string ApplicationName = "Hypoinput";
    static const std::string SettingsFileName = "Settings.ini";
    static const std::string TextExpansionsFileName = "TextExpansions.json";
    static const std::string AddTextExpansionsFileName = "Add-TextExpansion.ps1";
    static const std::string EditTextExpansionsFileName = "Edit-TextExpansions.ps1";
    static const std::string CommonFileName = "Common.ps1";

} // namespace

enum class SpecialFolder {
    ApplicationData,
    HypoinputApplicationData
};

enum class SpecialFile {
    Settings,
    TextExpansions,
    AddTextExpansion,
    EditTextExpansions,
    Common
};

std::filesystem::path getFolderPath(SpecialFolder);
std::filesystem::path getFilePath(SpecialFile);
std::string getResource(uint32_t, const std::wstring&);

} // namespace environment