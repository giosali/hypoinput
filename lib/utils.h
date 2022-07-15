#pragma once

#include <Windows.h>
#include <algorithm>
#include <sstream>
#include <string>
#include <vector>

namespace utils {

std::string boolToString(bool b);
std::string ltrim(const std::string&, char = ' ');
std::string replace(std::string, const std::string&);
std::string rtrim(const std::string&, char = ' ');
std::vector<std::string> split(const std::string&, char = ' ');
bool startsWith(const std::string&, const std::string&);
bool stringToBool(std::string);
std::wstring stringToWString(const std::string&);
std::string trim(const std::string&, char = ' ');
std::string wstringToString(const std::wstring&);

} // namespace utils