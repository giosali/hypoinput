#include "main.h"

#include "environment.h"
#include "expansions.h"
#include "file.h"
#include "filesystemwatcher.h"
#include "ini.h"
#include "keyboard.h"
#include "utils.h"
#include <Windows.h>
#include <cstdint>
#include <string>
#include <tchar.h>

// Global variables:
const uint32_t g_notifyIconId = 1;
const UINT WMAPP_NOTIFYCALLBACK = WM_APP + 1;
static TCHAR szWindowClass[] = _T("hypoinput");
static TCHAR szTitle[] = _T("Hypoinput");
static ini::IniFile g_settings(environment::getFilePath(environment::SpecialFile::Settings));
static keyboard::KeyboardHook g_keyboardHook;
static filesystemwatcher::FileSystemWatcher g_textExpansionsFsw;
HINSTANCE g_hInst = NULL;

// Forward declarations of functions included in this code module:
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
BOOL addNotificationIcon(HWND&);
BOOL deleteNotificationIcon(HWND&);
void showContextMenu(HWND&, POINT&);
void editContextMenuItem(HMENU&, int, UINT, bool, const wchar_t* = L"");
std::string onKeyDown(unsigned);
void onTextExpansionsChanged();
void registerRunValue();
void deleteRunValue();

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
    // Prevents multiple instances of this application from running.
    HANDLE hMutex = CreateMutexEx(NULL, L"Global\\Hypoinput", CREATE_MUTEX_INITIAL_OWNER, MUTEX_ALL_ACCESS);
    if (hMutex == NULL || GetLastError() == ERROR_ALREADY_EXISTS) {
        return 1;
    }

    // Maintain handle to application instance.
    g_hInst = hInstance;

    // Assign window class information.
    WNDCLASSEX wcex = { sizeof(wcex) };
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(wcex.hInstance, IDI_APPLICATION);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, IDI_APPLICATION);
    RegisterClassEx(&wcex);

    // Causes the application to look crisp on displays with high DPIs.
    // This must be called before creating any HWNDs.
    SetProcessDPIAware();

    // We cannot use HWND_MESSAGE as it causes the window to become a
    // message-only window. Since a message-only window cannot receive
    // broadcast messages, we won't be able to recreate the application's
    // notification icon on taskbar creation.
    HWND hWnd = CreateWindowEx(WS_EX_OVERLAPPEDWINDOW, szWindowClass, szTitle, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 800, 450, NULL, NULL, hInstance, NULL);
    if (!hWnd) {
        return 1;
    }

    ShowWindow(hWnd, SW_HIDE);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    ReleaseMutex(hMutex);
    return (int)msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    static UINT s_uTaskbarRestart = 0;

    switch (msg) {
    case WM_CREATE:
        s_uTaskbarRestart = RegisterWindowMessage(TEXT("TaskbarCreated"));

        if (!addNotificationIcon(hWnd)) {
            return -1;
        }

        if (!g_settings.m_exists) {
            // Writes the sample .INI resource file to application data
            // if it doesn't exist.
            std::string resourceText = environment::getResource(IDR_SETTINGSINI, L"INI");
            file::write(g_settings.m_filePath, resourceText);
            g_settings = ini::IniFile(g_settings.m_filePath);

            // Causes application to run at startup.
            registerRunValue();
        }

        expansions::TextExpansionManager::init();

        g_keyboardHook = keyboard::KeyboardHook(onKeyDown);
        g_keyboardHook.add(g_hInst);

        // Sets up a file system watcher for the text expansions file.
        g_textExpansionsFsw = filesystemwatcher::FileSystemWatcher(environment::getFolderPath(environment::SpecialFolder::HypoinputApplicationData), onTextExpansionsChanged);
        g_textExpansionsFsw.watch(FILE_NOTIFY_CHANGE_LAST_WRITE);

        // Creates PowerShell files.
        file::write(environment::getFilePath(environment::SpecialFile::AddTextExpansion), environment::getResource(IDR_ADDTEXTEXPANSIONPS1, L"PS1"));
        file::write(environment::getFilePath(environment::SpecialFile::EditTextExpansions), environment::getResource(IDR_EDITTEXTEXPANSIONSPS1, L"PS1"));
        file::write(environment::getFilePath(environment::SpecialFile::Common), environment::getResource(IDR_COMMONPS1, L"PS1"));
        break;
    case WM_DESTROY:
        deleteNotificationIcon(hWnd);
        g_textExpansionsFsw.stop();
        PostQuitMessage(0);
        g_keyboardHook.remove();
        break;
    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDM_ENABLE:
            g_keyboardHook.s_isEnabled = !g_keyboardHook.s_isEnabled;
            break;
        case IDM_RUNATSTARTUP: {
            // Edits the .INI settings file.
            bool runAtStartup = !g_settings.get<bool>(std::string(environment::constants::runAtStartup)).value().boolean;
            g_settings.set(std::string(environment::constants::runAtStartup), runAtStartup);
            g_settings.save();

            // Edits the registry value.
            if (runAtStartup) {
                registerRunValue();
            } else {
                deleteRunValue();
            }

            break;
        }
        case IDM_OPENFILE: {
            std::string textExpansionsFilePath = environment::getFilePath(environment::SpecialFile::TextExpansions).string();
            ShellExecute(NULL, NULL, utils::stringToWString(textExpansionsFilePath).c_str(), NULL, NULL, SW_SHOW);
            break;
        }
        case IDM_ADDTEXTEXPANSION: {
            std::wstring path = utils::stringToWString(environment::getFilePath(environment::SpecialFile::AddTextExpansion).string());
            ShellExecute(NULL, NULL, L"powershell.exe", (L"-file " + path).c_str(), NULL, SW_SHOW);
            break;
        }
        case IDM_EDITTEXTEXPANSIONS: {
            std::wstring path = utils::stringToWString(environment::getFilePath(environment::SpecialFile::EditTextExpansions).string());
            ShellExecute(NULL, NULL, L"powershell.exe", (L"-file " + path).c_str(), NULL, SW_SHOW);
            break;
        }
        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;
        default:
            return DefWindowProc(hWnd, msg, wParam, lParam);
        }

        break;
    case WMAPP_NOTIFYCALLBACK:
        switch (LOWORD(lParam)) {
        case WM_CONTEXTMENU:
            POINT pt;
            if (GetCursorPos(&pt)) {
                showContextMenu(hWnd, pt);
            }

            break;
        }

        break;
    default:
        if (msg == s_uTaskbarRestart) {
            if (!addNotificationIcon(hWnd)) {
                return -1;
            }
        }

        return DefWindowProc(hWnd, msg, wParam, lParam);
    }

    return 0;
}

BOOL addNotificationIcon(HWND& hWnd)
{
    NOTIFYICONDATA nid = { sizeof(nid) };
    nid.hWnd = hWnd;
    nid.uFlags = NIF_ICON | NIF_TIP | NIF_MESSAGE | NIF_SHOWTIP;
    nid.uID = g_notifyIconId;
    nid.uCallbackMessage = WMAPP_NOTIFYCALLBACK;
    nid.hIcon = (HICON)LoadImage(g_hInst, MAKEINTRESOURCE(IDI_NOTIFICATIONICON), IMAGE_ICON, 0, 0, LR_DEFAULTCOLOR | LR_DEFAULTSIZE);
    wcscpy_s(nid.szTip, L"Hypoinput v0.1.0");
    Shell_NotifyIcon(NIM_ADD, &nid);

    // NOTIFYICON_VERSION_4 is preferred
    nid.uVersion = NOTIFYICON_VERSION_4;
    return Shell_NotifyIcon(NIM_SETVERSION, &nid);
}

BOOL deleteNotificationIcon(HWND& hWnd)
{
    NOTIFYICONDATA nid = { sizeof(nid) };
    nid.hWnd = hWnd;
    nid.uID = g_notifyIconId;
    return Shell_NotifyIcon(NIM_DELETE, &nid);
}

void showContextMenu(HWND& hWnd, POINT& pt)
{
    HMENU hMenu = LoadMenu(g_hInst, MAKEINTRESOURCE(IDR_NOTIFICATIONICONMENU));
    if (!hMenu) {
        return;
    }

    HMENU hSubMenu = GetSubMenu(hMenu, 0);
    if (!hSubMenu) {
        DestroyMenu(hMenu);
        return;
    }

    std::wstring isEnabledStatus = g_keyboardHook.s_isEnabled ? L"Disable" : L"Enable";
    editContextMenuItem(hMenu, IDM_ENABLE, MIIM_STRING | MIIM_DATA, false, isEnabledStatus.c_str());

    /* TODO: Add .ini file handling */
    editContextMenuItem(hMenu, IDM_RUNATSTARTUP, MIIM_STATE, g_settings.get<bool>(std::string(environment::constants::runAtStartup)).value().boolean);

    // The window must be the foreground window before calling TrackPopupMenu
    // or the menu will not disappear when the user clicks away.
    SetForegroundWindow(hWnd);

    // Respects menu drop alignment and returns menu item id.
    UINT uFlags = TPM_RIGHTBUTTON;
    uFlags |= GetSystemMetrics(SM_MENUDROPALIGNMENT) != 0 ? TPM_RIGHTALIGN : TPM_LEFTALIGN;

    TrackPopupMenuEx(hSubMenu, uFlags, pt.x, pt.y, hWnd, NULL);
    DestroyMenu(hMenu);
}

void editContextMenuItem(HMENU& hMenu, int idm, UINT fMask, bool check, const wchar_t* caption)
{
    MENUITEMINFO mii = { sizeof(MENUITEMINFO) };
    mii.fMask = fMask;
    GetMenuItemInfo(hMenu, idm, FALSE, &mii);

    if ((fMask & MIIM_STATE) == MIIM_STATE) {
        mii.fState = check ? MFS_CHECKED : MFS_UNCHECKED;
    }

    if ((fMask & MIIM_STRING) == MIIM_STRING) {
        mii.dwTypeData = const_cast<LPWSTR>(caption);
    }

    SetMenuItemInfo(hMenu, idm, FALSE, &mii);
}

std::string onKeyDown(unsigned vkCode)
{
    static std::string s_input;

    // Handles the Backspace key.
    if (vkCode == VK_BACK) {
        if (!s_input.empty()) {
            s_input.pop_back();
        }

        return std::string();
    }

    // Exits if the pressed key is garbage.
    std::string key = keyboard::mapVirtualKey(vkCode);
    if (key.empty() || iscntrl(key[0])) {
        return std::string();
    }

    s_input.append(key);
    std::string trigger = expansions::TextExpansionManager::parse(s_input);
    if (!trigger.empty()) {
        // Resets the input after matching the trigger.
        s_input = std::string();
        return trigger;
    }

    return std::string();
}

void onTextExpansionsChanged()
{
    expansions::TextExpansionManager::refresh();
}

void registerRunValue()
{
    wchar_t buffer[MAX_PATH];
    GetModuleFileName(NULL, buffer, MAX_PATH);
    std::wstring fp(buffer);
    HKEY hKey;
    LONG retval = RegOpenKeyEx(HKEY_CURRENT_USER, std::wstring(environment::constants::runSubkey).c_str(), 0, KEY_WRITE, &hKey);
    if (retval == ERROR_SUCCESS) {
        RegSetValueEx(hKey, szTitle, 0, REG_SZ, (BYTE*)fp.c_str(), (unsigned)((fp.size() + 1) * sizeof(wchar_t)));
    }
}

void deleteRunValue()
{
    HKEY hKey;
    RegOpenKeyEx(HKEY_CURRENT_USER, std::wstring(environment::constants::runSubkey).c_str(), 0, KEY_ALL_ACCESS, &hKey);
    RegDeleteValue(hKey, szTitle);
    RegCloseKey(hKey);
}