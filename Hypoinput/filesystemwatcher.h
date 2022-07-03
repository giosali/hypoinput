#pragma once

#include "utils.h"
#include <Windows.h>
#include <filesystem>
#include <thread>

namespace filesystemwatcher {

class FileSystemWatcher {
public:
    FileSystemWatcher() = default;
    FileSystemWatcher(const std::filesystem::path&);

    void watch(unsigned);
    void stop();

private:
    void _watch(unsigned);

    std::filesystem::path m_path;
    HANDLE m_hEvent;
    std::thread m_thread;
};

} // namespace filesystemwatcher