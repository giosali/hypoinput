#pragma once

#include "environment.h"
#include "file.h"
#include "picojson.h"
#include "utils.h"
#include <cstdlib>
#include <sstream>
#include <unordered_map>

namespace expansions {

class TextExpansionManager {
public:
    static void init();
    static std::string parse(std::string&);
    static void refresh();
    static std::string getReplacement(const std::string&);

private:
    TextExpansionManager();

    static std::unordered_map<std::string, std::string> read();

    static std::unordered_map<std::string, std::string> s_textExpansions;
};

} // namespace expansions
