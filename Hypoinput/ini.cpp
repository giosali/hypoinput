#include "ini.h"

namespace ini {

IniFile::IniFile(const std::filesystem::path& path)
    : m_filePath(path)
    , m_exists(std::filesystem::exists(path))
{
    read();
}

std::string& IniFile::operator[](const std::string& key)
{
    return m_map[key];
}

void IniFile::save()
{
    std::ofstream fs(m_filePath);
    if (fs.is_open()) {
        std::string section;
        for (const std::pair<const std::string, const std::string>& pair : m_map) {
            size_t dotPos = pair.first.find('.');
            std::string tempSection = pair.first.substr(0, dotPos);
            if (section != tempSection) {
                section = tempSection;
                fs << "[" << section << "]" << std::endl;
            }

            std::string first = pair.first.substr(dotPos + 1);
            fs << first << "=" << pair.second << std::endl;
        }
    }

    fs.close();
}

template <typename T>
std::optional<Value> IniFile::get(const std::string& key)
{
    Value value;
    if (std::is_same<T, bool>::value) {
        value.boolean = utils::stringToBool(m_map[key]);
    } else if (std::is_same<T, std::string>::value) {
        value.string = m_map[key];
    }

    return value;
}

template <typename T>
void IniFile::set(const std::string& key, const T& value)
{
    if (std::is_same<T, bool>::value) {
        m_map[key] = utils::boolToString(value);
    } else if (std::is_same<T, std::string>::value) {
        m_map[key] = value;
    }
}

void IniFile::read()
{
    std::map<std::string, std::string> map;
    std::ifstream fs(m_filePath);
    if (fs.is_open()) {
        std::string section;
        std::string line;
        while (std::getline(fs, line)) {
            if (line.find('[') == 0 && line.rfind(']') == line.length() - 1) {
                section = line.substr(1, line.length() - 2);
                continue;
            }

            // Skips the line if it doesn't contain a '='.
            size_t pos = line.find('=');
            if (pos == std::string::npos) {
                continue;
            }

            std::string name = line.substr(0, pos);
            std::string value = line.substr(pos + 1);
            map[section + "." + utils::rtrim(name)] = utils::ltrim(value);
        }
    }

    fs.close();
    m_map = map;
}

template void IniFile::set<bool>(const std::string&, const bool&);
template void IniFile::set<std::string>(const std::string&, const std::string&);

} // namespace ini