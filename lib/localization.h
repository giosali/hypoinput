#pragma once

#include <string>
#include <unordered_map>

namespace localization {

enum class Locale {
    EN,
    ES,
    FR
};

enum class Text {
    Disable,
    Enable,
    RunAtStartup,
    Language,
    OpenFile,
    AddTextExpansion,
    EditTextExpansions,
    CheckForUpdates,
    Exit,
    UpdatesAvailableText,
    UpdatesAvailableCaption,
    NoUpdatesAvailableText,
    NoUpdatesAvailableCaption
};

class Resource {
public:
    Resource();
    Resource(Locale);

    const std::wstring& operator[](Text);

private:
    Locale m_locale;
    std::unordered_map<Text, std::wstring> m_textResources;
};

} // namespace localization