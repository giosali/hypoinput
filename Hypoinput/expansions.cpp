#include "expansions.h"

namespace expansions {

void TextExpansionManager::init()
{
    file::create(environment::getFilePath(environment::SpecialFile::TextExpansions));
    s_textExpansions = read();
}

std::string TextExpansionManager::parse(std::string& input)
{
    // Exits if there are no text expansions.
    if (s_textExpansions.size() == 0) {
        return std::string();
    }

    // Checks if the input is one of the triggers in the text expansions.
    if (s_textExpansions.find(input) != s_textExpansions.end()) {
        // Returns the corresponding replacement.
        return input;
    } else {
        bool startsWithInput = false;
        std::string trimmedInput = input.empty() ? input.substr(0, input.length()) : std::string();
        for (const auto& [key, value] : s_textExpansions) {
            // Checks if any of the keys begins with the trimmed input.
            // This gives the user some room for error or mistypes.
            if (startsWithInput = (utils::startsWith(key, input) || utils::startsWith(key, trimmedInput))) {
                break;
            }
        }

        if (!startsWithInput) {
            input = std::string();
        }

        return std::string();
    }
}

std::string TextExpansionManager::getReplacement(const std::string& trigger)
{
    return s_textExpansions[trigger];
}

std::unordered_map<std::string, std::string> TextExpansionManager::read()
{
    std::filesystem::path textExpansionsFilePath = environment::getFilePath(environment::SpecialFile::TextExpansions);
    std::ifstream ifs;
    ifs.open(textExpansionsFilePath, std::ios::binary);
    if (!ifs.is_open()) {
        return std::unordered_map<std::string, std::string> {};
    }

    std::stringstream ss;
    ss << ifs.rdbuf();
    ifs.close();

    picojson::value v;
    ss >> v;
    std::string err = picojson::get_last_error();
    if (!err.empty()) {
        return std::unordered_map<std::string, std::string> {};
    }

    // Verifies the value is an array.
    if (!v.is<picojson::array>()) {
        return std::unordered_map<std::string, std::string> {};
    }

    std::unordered_map<std::string, std::string> umap;
    picojson::value::array& arr = v.get<picojson::array>();
    for (int i = 0; i < arr.size(); i++) {
        try {
            picojson::object& obj = arr[i].get<picojson::object>();
            const std::string& trigger = obj["trigger"].get<std::string>();
            const std::string& replacement = obj["replacement"].get<std::string>();
            umap[trigger] = replacement;
        } catch (std::runtime_error) {
        }
    }

    return umap;
}

std::unordered_map<std::string, std::string> TextExpansionManager::s_textExpansions {};

} // namespace expansions