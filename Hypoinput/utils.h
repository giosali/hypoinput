#pragma once

#include <string>
#include <Windows.h>

namespace utils {

std::string wstringToString(const std::wstring&);
std::wstring stringToWString(const std::string&);
bool startsWith(const std::string&, const std::string&);

} // namespace utils