#pragma once

#include <string>
#include <Windows.h>

namespace utils {

std::string wstringToString(const std::wstring&);
std::wstring stringToWString(const std::string&);
bool startsWith(const std::string&, const std::string&);
std::string ltrim(const std::string&, char = ' ');
std::string rtrim(const std::string&, char = ' ');

} // namespace utils