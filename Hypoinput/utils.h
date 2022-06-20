#pragma once

#include <string>
#include <Windows.h>

namespace utils {

std::string wstringToString(const std::wstring&);
bool startsWith(const std::string&, const std::string&);

} // namespace utils