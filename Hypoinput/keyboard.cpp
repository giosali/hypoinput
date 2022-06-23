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
                // Erases the trigger text that the user typed.
                std::string replacement = expansions::TextExpansionManager::getReplacement(trigger);
                repeat(VK_BACK, replacement.length() - 1);
                inject(replacement);

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

void inject(const std::string& input)
{
    std::vector<INPUT> inputs;
    char previousCh = '\0';
    for (size_t i = 0; i < input.length(); i++) {
        char ch = input[i];

        // Adds nothing if the current character is the same as the previous character.
        if (ch == previousCh) {
            INPUT input {};
            input.type = INPUT_KEYBOARD;
            input.ki.wVk = 0;
            input.ki.wScan = 0;
            input.ki.dwFlags = KEYEVENTF_UNICODE;
            inputs.push_back(input);
        }

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
        default: {
            INPUT input {};
            input.type = INPUT_KEYBOARD;
            input.ki.wVk = 0;
            input.ki.wScan = ch;
            input.ki.dwFlags = KEYEVENTF_UNICODE;
            inputs.push_back(input);
            break;
        }
        }

        previousCh = ch;
    }

    SendInput((UINT)inputs.size(), &inputs[0], sizeof(INPUT));
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

    SendInput((UINT)inputs.size(), &inputs[0], sizeof(INPUT));
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