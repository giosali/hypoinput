#include "main.h"

#include "environment.h"
#include "expansions.h"
#include "file.h"
#include "filesystemwatcher.h"
#include "ini.h"
#include "keyboard.h"
#include "localization.h"
#include "picojson.h"
#include "utils.h"
#include <Windows.h>
#include <cpr/cpr.h>
#include <cstdint>
#include <string>
#include <tchar.h>
#include <vector>

// Global variables:
const uint32_t g_notifyIconId = 1;
const UINT WMAPP_NOTIFYCALLBACK = WM_APP + 1;
static std::wstring g_windowClass = L"hypoinput";
static std::wstring g_title = L"Hypoinput";
static std::wstring g_version = L"v1.1.0";
static ini::File g_settings;
static keyboard::KeyboardHook g_keyboardHook;
static filesystemwatcher::FileSystemWatcher g_textExpansionsFsw;
static localization::Resource g_resource;
HINSTANCE g_hInst = NULL;

// Forward declarations of functions included in this code module:
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
BOOL addNotificationIcon(HWND&);
BOOL deleteNotificationIcon(HWND&);
void showContextMenu(HWND&, POINT&);
void editContextMenuItem(HMENU&, int, UINT, bool, const wchar_t* = L"", bool byPosition = false);
std::string onKeyDown(unsigned);
void onTextExpansionsChanged();
void registerRunValue();
void deleteRunValue();
bool isUpdateAvailable();
void updateSettingsIni();
void updateLocale(localization::Locale, localization::Locale);

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
    wcex.lpszClassName = g_windowClass.c_str();
    wcex.hIconSm = LoadIcon(wcex.hInstance, IDI_APPLICATION);
    RegisterClassEx(&wcex);

    // Causes the application to look crisp on displays with high DPIs.
    // This must be called before creating any HWNDs.
    SetProcessDPIAware();

    // We cannot use HWND_MESSAGE as it causes the window to become a
    // message-only window. Since a message-only window cannot receive
    // broadcast messages, we won't be able to recreate the application's
    // notification icon on taskbar creation.
    HWND hWnd = CreateWindowEx(WS_EX_OVERLAPPEDWINDOW, g_windowClass.c_str(), g_title.c_str(), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 800, 450, NULL, NULL, hInstance, NULL);
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
    case WM_CREATE: {
        s_uTaskbarRestart = RegisterWindowMessage(TEXT("TaskbarCreated"));

        if (!addNotificationIcon(hWnd)) {
            return -1;
        }

        std::filesystem::path settingsPath = environment::getFilePath(environment::SpecialFile::Settings);
        if (!std::filesystem::exists(settingsPath)) {
            // Writes the sample .INI resource file to application data if it doesn't exist.
            file::write(settingsPath, environment::getResource(IDR_SETTINGSINI, L"INI"));

            registerRunValue();
        }

        // Reads the Settings.ini file in AppData.
        g_settings = ini::open(settingsPath);
        updateSettingsIni();

        expansions::TextExpansionManager::init();

        g_keyboardHook = keyboard::KeyboardHook(onKeyDown);
        g_keyboardHook.add(g_hInst);

        // Sets the language to use for the application.
        g_resource = localization::Resource(static_cast<localization::Locale>(g_settings[std::string(environment::constants::settingsSection)].get<int>(std::string(environment::constants::languageKey))));

        // Sets up a file system watcher for the text expansions file.
        g_textExpansionsFsw = filesystemwatcher::FileSystemWatcher(environment::getFolderPath(environment::SpecialFolder::HypoinputApplicationData), onTextExpansionsChanged);
        g_textExpansionsFsw.watch(FILE_NOTIFY_CHANGE_LAST_WRITE);

        // Creates PowerShell files.
        file::write(environment::getFilePath(environment::SpecialFile::AddTextExpansion), environment::getResource(IDR_ADDTEXTEXPANSIONPS1, L"PS1"));
        file::write(environment::getFilePath(environment::SpecialFile::EditTextExpansions), environment::getResource(IDR_EDITTEXTEXPANSIONSPS1, L"PS1"));
        file::write(environment::getFilePath(environment::SpecialFile::Common), environment::getResource(IDR_COMMONPS1, L"PS1"));
        break;
    }
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
            bool runAtStartup = !g_settings[std::string(environment::constants::settingsSection)].get<bool>(std::string(environment::constants::runAtStartupKey));
            g_settings[std::string(environment::constants::settingsSection)].set<bool>(std::string(environment::constants::runAtStartupKey), runAtStartup);
            g_settings.write(environment::getFilePath(environment::SpecialFile::Settings));

            // Edits the registry value.
            if (runAtStartup) {
                registerRunValue();
            } else {
                deleteRunValue();
            }

            break;
        }
        case IDM_LANGUAGE_ENGLISH:
            updateLocale(localization::Locale::EN, static_cast<localization::Locale>(g_settings[std::string(environment::constants::settingsSection)].get<int>(std::string(environment::constants::languageKey))));
            break;
        case IDM_LANGUAGE_ESPANOL:
            updateLocale(localization::Locale::ES, static_cast<localization::Locale>(g_settings[std::string(environment::constants::settingsSection)].get<int>(std::string(environment::constants::languageKey))));
            break;
        case IDM_LANGUAGE_FRANCAIS:
            updateLocale(localization::Locale::FR, static_cast<localization::Locale>(g_settings[std::string(environment::constants::settingsSection)].get<int>(std::string(environment::constants::languageKey))));
            break;
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
        case IDM_CHECKFORUPDATES: {
            int button = isUpdateAvailable() ? MessageBox(NULL, g_resource[localization::Text::UpdatesAvailableText].c_str(), g_resource[localization::Text::UpdatesAvailableCaption].c_str(), MB_YESNO) : MessageBox(NULL, g_resource[localization::Text::NoUpdatesAvailableText].c_str(), g_resource[localization::Text::NoUpdatesAvailableCaption].c_str(), MB_OK);
            if (button == IDYES) {
                std::filesystem::path updateExecutablePath = environment::getFilePath(environment::SpecialFile::UpdaterExecutable);
                if (std::filesystem::exists(updateExecutablePath)) {
                    // Runs updater.exe.
                    ShellExecute(NULL, L"runas", utils::stringToWString(updateExecutablePath.string()).c_str(), NULL, NULL, SW_SHOW);

                    // Shuts down application.
                    DestroyWindow(hWnd);
                }
            }

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
    wcscpy_s(nid.szTip, (g_title + L" " + g_version).c_str());
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

    editContextMenuItem(hMenu, IDM_ENABLE, MIIM_DATA | MIIM_STRING, false, g_resource[g_keyboardHook.s_isEnabled ? localization::Text::Disable : localization::Text::Enable].c_str());
    editContextMenuItem(hMenu, IDM_RUNATSTARTUP, MIIM_STATE | MIIM_DATA | MIIM_STRING, g_settings[std::string(environment::constants::settingsSection)].get<bool>(std::string(environment::constants::runAtStartupKey)), g_resource[localization::Text::RunAtStartup].c_str());
    editContextMenuItem(hMenu, IDM_OPENFILE, MIIM_DATA | MIIM_STRING, false, g_resource[localization::Text::OpenFile].c_str());
    editContextMenuItem(hMenu, IDM_ADDTEXTEXPANSION, MIIM_DATA | MIIM_STRING, false, g_resource[localization::Text::AddTextExpansion].c_str());
    editContextMenuItem(hMenu, IDM_EDITTEXTEXPANSIONS, MIIM_DATA | MIIM_STRING, false, g_resource[localization::Text::EditTextExpansions].c_str());
    editContextMenuItem(hMenu, IDM_CHECKFORUPDATES, MIIM_DATA | MIIM_STRING, false, g_resource[localization::Text::CheckForUpdates].c_str());
    editContextMenuItem(hMenu, IDM_EXIT, MIIM_DATA | MIIM_STRING, false, g_resource[localization::Text::Exit].c_str());
    editContextMenuItem(hSubMenu, 2, MIIM_DATA | MIIM_STRING, false, g_resource[localization::Text::Language].c_str(), true);

    switch (static_cast<localization::Locale>(g_settings[std::string(environment::constants::settingsSection)].get<int>(std::string(environment::constants::languageKey)))) {
    case localization::Locale::EN:
        editContextMenuItem(hMenu, IDM_LANGUAGE_ENGLISH, MIIM_STATE, true);
        break;
    case localization::Locale::ES:
        editContextMenuItem(hMenu, IDM_LANGUAGE_ESPANOL, MIIM_STATE, true);
        break;
    case localization::Locale::FR:
        editContextMenuItem(hMenu, IDM_LANGUAGE_FRANCAIS, MIIM_STATE, true);
        break;
    }

    // The window must be the foreground window before calling TrackPopupMenu
    // or the menu will not disappear when the user clicks away.
    SetForegroundWindow(hWnd);

    // Respects menu drop alignment and returns menu item id.
    UINT uFlags = TPM_RIGHTBUTTON;
    uFlags |= GetSystemMetrics(SM_MENUDROPALIGNMENT) != 0 ? TPM_RIGHTALIGN : TPM_LEFTALIGN;

    TrackPopupMenuEx(hSubMenu, uFlags, pt.x, pt.y, hWnd, NULL);
    DestroyMenu(hMenu);
}

void editContextMenuItem(HMENU& hMenu, int idm, UINT fMask, bool check, const wchar_t* caption, bool byPosition)
{
    MENUITEMINFO mii = {
        sizeof(MENUITEMINFO),
        fMask = fMask
    };
    GetMenuItemInfo(hMenu, idm, byPosition, &mii);

    if ((fMask & MIIM_STATE) == MIIM_STATE) {
        mii.fState = check ? MFS_CHECKED : MFS_UNCHECKED;
    }

    if ((fMask & MIIM_STRING) == MIIM_STRING) {
        mii.dwTypeData = const_cast<LPWSTR>(caption);
    }

    SetMenuItemInfo(hMenu, idm, byPosition, &mii);
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
        RegSetValueEx(hKey, g_title.c_str(), 0, REG_SZ, (BYTE*)fp.c_str(), (unsigned)((fp.size() + 1) * sizeof(wchar_t)));
    }
}

void deleteRunValue()
{
    HKEY hKey;
    RegOpenKeyEx(HKEY_CURRENT_USER, std::wstring(environment::constants::runSubkey).c_str(), 0, KEY_ALL_ACCESS, &hKey);
    RegDeleteValue(hKey, g_title.c_str());
    RegCloseKey(hKey);
}

bool isUpdateAvailable()
{
    const std::string latestReleaseEp = "https://api.github.com/repos/giosali/hypoinput/releases/latest";
    cpr::Response r = cpr::Get(cpr::Url { latestReleaseEp });
    if (r.status_code != cpr::status::HTTP_OK && r.status_code >= 400) {
        return false;
    }

    try {
        picojson::value v;
        std::string err = picojson::parse(v, r.text);
        if (!err.empty()) {
            return false;
        }

        if (!v.is<picojson::object>()) {
            return false;
        }

        // Removes the "v" prepended to the version strings.
        std::vector<std::string> v1NumStrings = utils::split(v.get("tag_name").get<std::string>().substr(1), '.');
        std::vector<std::string> v2NumStrings = utils::split(utils::wstringToString(g_version).substr(1), '.');

        for (size_t i = 0; i < v1NumStrings.size(); i++) {
            if (std::stoi(v1NumStrings[i]) > std::stoi(v2NumStrings[i])) {
                return true;
            }
        }

        return false;
    } catch (std::runtime_error) {
        return false;
    }
}

void updateSettingsIni()
{
    ini::File currentSettings = ini::load(environment::getResource(IDR_SETTINGSINI, L"INI"));
    ini::Section currentSettingsSection = currentSettings[std::string(environment::constants::settingsSection)];
    ini::Section& settingsSection = g_settings[std::string(environment::constants::settingsSection)];
    if (currentSettingsSection.size() > settingsSection.size()) {
        for (const auto& [key, value] : currentSettingsSection) {
            if (!settingsSection.has_key(key)) {
                settingsSection[key] = value;
            }
        }
    }

    g_settings.write(environment::getFilePath(environment::SpecialFile::Settings));
}

void updateLocale(localization::Locale locale, localization::Locale currentLocale)
{
    if (locale == currentLocale) {
        return;
    }

    g_settings[std::string(environment::constants::settingsSection)].set<int>(std::string(environment::constants::languageKey), static_cast<int>(locale));
    g_settings.write(environment::getFilePath(environment::SpecialFile::Settings));
    g_resource = localization::Resource(locale);
}