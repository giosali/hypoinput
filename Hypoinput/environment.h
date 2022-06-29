#pragma once

#include <filesystem>
#include <string>

namespace environment {

namespace {

    static const std::string ApplicationName = "Hypotext";
    static const std::string TextExpansionsFileName = "TextExpansions.json";
    static const std::string SettingsFileName = "Settings.ini";

} // namespace

enum class SpecialFolder {
    ApplicationData
};

enum class SpecialFile {
    TextExpansions,
    SettingsFileName
};

std::filesystem::path getFolderPath(SpecialFolder);
std::filesystem::path getFilePath(SpecialFile);

} // namespace environment