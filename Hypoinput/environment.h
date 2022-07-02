#pragma once

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

} // namespace

enum class SpecialFolder {
    ApplicationData
};

enum class SpecialFile {
    Settings,
    TextExpansions
};

std::filesystem::path getFolderPath(SpecialFolder);
std::filesystem::path getFilePath(SpecialFile);
std::string getResource(uint32_t, const std::wstring&);

} // namespace environment