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

} // namespace utils