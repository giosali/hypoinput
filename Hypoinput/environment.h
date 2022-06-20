#pragma once

#include <filesystem>
#include <string>

namespace environment {

namespace {

    static const std::string ApplicationName = "Hypotext";
    static const std::string TextExpansionsFileName = "TextExpansions.json";

} // namespace

enum class SpecialFolder {
    ApplicationData
};

enum class SpecialFile {
    TextExpansions
};

std::filesystem::path getFolderPath(SpecialFolder);
std::filesystem::path getFilePath(SpecialFile);

} // namespace environment