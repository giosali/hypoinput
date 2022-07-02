#pragma once

#include "environment.h"
#include "utils.h"
#include <filesystem>
#include <fstream>
#include <map>
#include <optional>
#include <string>

namespace ini {

struct Value {
    bool boolean = false;
    std::string string;
};

class IniFile {
public:
    IniFile() = default;
    explicit IniFile(const std::filesystem::path&);

    std::string& operator[](const std::string&);
    void save();
    void set(const std::string&, bool);
    void set(const std::string&, const std::string&);
    template <typename T>
    std::optional<Value> get(const std::string&);

    std::filesystem::path m_filePath;
    bool m_exists;

private:
    void read();

    std::map<std::string, std::string> m_map;
};

} // namespace ini