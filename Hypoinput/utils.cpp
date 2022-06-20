#include "utils.h"

namespace utils {

std::string wstringToString(const std::wstring& ws)
{
    int bufferSize = WideCharToMultiByte(CP_UTF8, 0, ws.c_str(), (int)ws.size(), NULL, 0, NULL, NULL);
    std::string buffer(bufferSize, 0);
    WideCharToMultiByte(CP_UTF8, 0, ws.c_str(), -1, &buffer[0], bufferSize, NULL, NULL);
    return buffer;
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