#include "keyboard.h"

namespace keyboard {

KeyboardHook::KeyboardHook()
    : m_hookId(NULL)
{
}

KeyboardHook::KeyboardHook(const std::function<std::string(unsigned)>& func)
    : m_hookId(NULL)
{
    s_func = func;
}

void KeyboardHook::remove() const
{
    UnhookWindowsHookEx(m_hookId);
}

void KeyboardHook::add(HINSTANCE& hmod)
{
    m_hookId = SetWindowsHookEx(WH_KEYBOARD_LL, hookCallBack, hmod, 0);
}

LRESULT KeyboardHook::hookCallBack(_In_ int nCode, _In_ WPARAM wParam, _In_ LPARAM lParam)
{
    if (nCode >= 0 && s_isEnabled) {
        switch (wParam) {
        case WM_KEYDOWN:
            KBDLLHOOKSTRUCT* kbStruct = (KBDLLHOOKSTRUCT*)lParam;
            int vkCode = kbStruct->vkCode;
            std::string trigger = s_func(vkCode);

            // Sends the text expansion if the trigger text isn't empty.
            if (!trigger.empty()) {
                std::string replacement = expansions::TextExpansionManager::getReplacement(trigger);

                // Replaces the first instance of the Cursor constant with an empty
                // string in order to reposition the cursor.
                size_t cursorPos = replacement.find(Cursor);
                if (cursorPos != std::string::npos) {
                    replacement.replace(cursorPos, Cursor.length(), std::string());
                }

                // Erases the trigger text that the user typed.
                repeat(VK_BACK, trigger.length() - 1);

                // Sends the replacement text.
                inject(utils::stringToWString(replacement));

                // If the Cursor constant was found, move the cursor to its position.
                if (cursorPos != std::string::npos) {
                    repeat(VK_LEFT, replacement.length() - cursorPos);
                }

                // Temporarily blocks keyboard input if there's a text expansion to send.
                return 1;
            }

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

void inject(const std::wstring& input)
{
    std::vector<INPUT> inputs;
    for (size_t i = 0; i < input.length(); i++) {
        wchar_t ch = input[i];
        switch (ch) {
        case '\n': {
            // Simulates the Shift + Enter keys if the character is a newline char
            std::vector<INPUT> vkInputs = inputFromVirtualKeys<2>({ VK_SHIFT, VK_RETURN });
            inputs.insert(std::end(inputs), std::begin(vkInputs), std::end(vkInputs));
            break;
        }
        case '\t': {
            std::vector<INPUT> vkInputs = inputFromVirtualKeys<1>({ VK_TAB });
            inputs.insert(std::end(inputs), std::begin(vkInputs), std::end(vkInputs));
            break;
        }
        case '\0':
            // Handles null-terminated strings since the last character is set to \0.
            break;
        default: {
            INPUT input {};
            input.type = INPUT_KEYBOARD;
            input.ki.wVk = 0;
            input.ki.wScan = ch;
            input.ki.dwFlags = KEYEVENTF_UNICODE;
            inputs.push_back(input);

            // Necessary for handling repeat characters when using SendInput.
            input.ki.dwFlags |= KEYEVENTF_KEYUP;
            inputs.push_back(input);
            break;
        }
        }
    }

    if (!inputs.empty()) {
        SendInput((UINT)inputs.size(), &inputs[0], sizeof(INPUT));
    }
}

void repeat(int vkCode, size_t count)
{
    std::vector<INPUT> inputs = {};
    for (size_t i = 0; i < count; i++) {
        INPUT input {};
        input.type = INPUT_KEYBOARD;
        input.ki.wVk = vkCode;
        input.ki.wScan = 0;
        inputs.push_back(input);

        input.ki.dwFlags = KEYEVENTF_KEYUP;
        inputs.push_back(input);
    }

    if (!inputs.empty()) {
        SendInput((UINT)inputs.size(), &inputs[0], sizeof(INPUT));
    }
}

template <size_t N>
std::vector<INPUT> inputFromVirtualKeys(std::array<int, N> vkCodes)
{
    std::vector<INPUT> inputs(N * 2);
    for (size_t i = 0; i < N; i++) {
        // Presses the key.
        INPUT input {};
        input.type = INPUT_KEYBOARD;
        input.ki.wVk = vkCodes[i];
        input.ki.wScan = 0;
        inputs[i] = input;

        // Releases the key.
        input.ki.dwFlags = KEYEVENTF_KEYUP;
        inputs[i + N] = input;
    }

    return inputs;
}

template std::vector<INPUT> inputFromVirtualKeys<1>(std::array<int, 1>);
template std::vector<INPUT> inputFromVirtualKeys<2>(std::array<int, 2>);

std::function<std::string(unsigned)> KeyboardHook::s_func = {};
bool KeyboardHook::s_isEnabled = true;

} // namespace keyboard