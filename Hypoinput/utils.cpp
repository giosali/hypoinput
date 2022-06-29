#include "utils.h"

namespace utils {

std::string wstringToString(const std::wstring& ws)
{
    int bufferSize = WideCharToMultiByte(CP_UTF8, 0, ws.c_str(), (int)ws.size(), NULL, 0, NULL, NULL);
    std::string buffer(bufferSize, 0);
    WideCharToMultiByte(CP_UTF8, 0, ws.c_str(), -1, &buffer[0], bufferSize, NULL, NULL);
    return buffer;
}

std::wstring stringToWString(const std::string& s)
{
    const int size = MultiByteToWideChar(CP_UTF8, 0, s.c_str(), -1, NULL, 0);
    std::wstring ws(size, 0);
    MultiByteToWideChar(CP_UTF8, 0, s.c_str(), -1, &ws.at(0), size);
    return ws;
}

bool startsWith(const std::string& instance, const std::string& value)
{
    if (!instance.empty() && value.empty()) {
        return false;
    }

    // Returns true if instance starts with value; otherwise, false.
    return instance.rfind(value, 0) == 0;
}

std::string replace(std::string s, char ch)
{
    s.erase(std::remove(s.begin(), s.end(), ch), s.end());
    return s;
}

std::string replace(std::string oldValue, const std::string& newValue)
{
    size_t length = newValue.length();
    while (true) {
        size_t pos = oldValue.find(newValue);
        if (pos == std::string::npos) {
            break;
        }

        oldValue.erase(pos, length);
    }

    return oldValue;
}

std::string trim(const std::string& s, char ch)
{
    size_t start = 0;
    size_t end = s.length() - 1;

    // Trim head
    for (; start < s.length(); start++) {
        if (s[start] != ch) {
            break;
        }
    }

    // Trim tail
    for (; end >= start; end--) {
        if (s[end] != ch) {
            break;
        }
    }

    size_t length = end - start + 1;
    return s.substr(start, length);
}

std::string ltrim(const std::string& s, char ch)
{
    size_t i = 0;
    for (; i < s.length(); i++) {
        if (s[i] != ch) {
            break;
        }
    }

    return s.substr(i);
}

std::string rtrim(const std::string& s, char ch)
{
    size_t i = s.length() - 1;
    for (; i > 0; i--) {
        if (s[i] != ch) {
            break;
        }
    }

    return s.substr(0, i + 1);
}

std::vector<std::string> split(const std::string& text, char ch)
{
    std::vector<std::string> strings;
    for (std::string line; std::getline(std::stringstream(text), line, ch);) {
        strings.push_back(line);
    }

    return strings;
}

} // namespace utils