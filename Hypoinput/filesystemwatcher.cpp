#include "filesystemwatcher.h"

namespace filesystemwatcher {

FileSystemWatcher::FileSystemWatcher(const std::filesystem::path& path)
    : m_path(path)
    , m_hEvent(NULL)
{
    if (!std::filesystem::exists(path)) {
        throw std::invalid_argument("path does not exist");
    }
}

void FileSystemWatcher::watch(unsigned filters)
{
    m_thread = std::thread(&FileSystemWatcher::_watch, this, filters);
    m_thread.detach();
}

void FileSystemWatcher::stop()
{
    if (m_hEvent != NULL) {
        SetEvent(m_hEvent);
    }
}

void FileSystemWatcher::_watch(unsigned filters)
{
    HANDLE dwHandles[2] {};
    dwHandles[0] = CreateEvent(NULL, TRUE, FALSE, L"FileSystemWatcherEvent"), m_hEvent = dwHandles[0];
    dwHandles[1] = FindFirstChangeNotification(utils::stringToWString(m_path.string()).c_str(), FALSE, filters);
    if (dwHandles[0] == INVALID_HANDLE_VALUE || dwHandles[0] == NULL || dwHandles[1] == INVALID_HANDLE_VALUE || dwHandles[1] == NULL) {
        return;
    }

    while (true) {
        DWORD dwWaitStatus = WaitForMultipleObjects(2, dwHandles, FALSE, INFINITE);
        switch (dwWaitStatus) {
        case WAIT_OBJECT_0:
            CloseHandle(m_hEvent);
            return;
        case WAIT_OBJECT_0 + 1:
            if (FindNextChangeNotification(dwHandles[1]) == FALSE) {
                return;
            }

            break;
        default:
            break;
        }
    }
}

} // namespace filesystemwatcher