#include "keyboard.h"

namespace keyboard {

KeyboardHook::KeyboardHook()
    : m_hookId(NULL)
{
}

KeyboardHook::KeyboardHook(const std::function<bool(unsigned)>& func)
    : m_hookId(NULL)
{
    s_func = func;
}

LRESULT KeyboardHook::hookCallBack(_In_ int nCode, _In_ WPARAM wParam, _In_ LPARAM lParam)
{
    if (nCode >= 0 && s_isEnabled) {
        switch (wParam) {
        case WM_KEYDOWN:
            KBDLLHOOKSTRUCT* kbStruct = (KBDLLHOOKSTRUCT*)lParam;
            int vkCode = kbStruct->vkCode;
            s_func(vkCode);
            break;
        }
    }

    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

std::string mapVirtualKey(int vkCode)
{
    // This is necessary for detecting key combinations (Shift + 4)
    // or whether the CAPS LOCK key is toggled.
    GetKeyState(vkCode);
    BYTE keyState[256] = {};
    if (!GetKeyboardState(keyState)) {
        return std::string();
    }

    UINT scanCode = MapVirtualKey(vkCode, MAPVK_VK_TO_VSC);
    TCHAR buffer[2] = {};
    int result = ToUnicode(vkCode, scanCode, keyState, buffer, 2, 0);
    std::wstring ws(buffer);
    return result == 1 && !ws.empty() ? utils::wstringToString(ws) : std::string();
}

} // namespace keyboard