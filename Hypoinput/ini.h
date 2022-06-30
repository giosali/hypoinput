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
    IniFile() = delete;
    explicit IniFile(const std::filesystem::path&);

    std::string& operator[](const std::string&);
    void save();
    template <typename T>
    std::optional<Value> get(const std::string& key);
    template <typename T>
    void set(const std::string&, const T&);

    std::filesystem::path m_filePath;
    bool m_exists;

private:
    void read();

    std::map<std::string, std::string> m_map;
};

} // namespace ini