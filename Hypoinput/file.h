#pragma once

#include "utils.h"
#include <filesystem>
#include <fstream>

namespace file {

void create(const std::filesystem::path&);
void write(const std::filesystem::path&, const std::string&);

} // namespace file