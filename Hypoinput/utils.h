#pragma once

#include <Windows.h>
#include <algorithm>
#include <sstream>
#include <string>
#include <vector>

namespace utils {

std::string wstringToString(const std::wstring&);
std::wstring stringToWString(const std::string&);
bool startsWith(const std::string&, const std::string&);
std::string ltrim(const std::string&, char = ' ');
std::string rtrim(const std::string&, char = ' ');
std::string boolToString(bool b);
bool stringToBool(std::string);
std::vector<std::string> split(const std::string&, char = ' ');
std::string trim(const std::string&, char = ' ');
std::string replace(std::string, const std::string&);

} // namespace utils