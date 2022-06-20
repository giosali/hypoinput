#pragma once

#include "utils.h"
#include <Windows.h>
#include <functional>

namespace keyboard {

class KeyboardHook {
public:
    KeyboardHook();
    explicit KeyboardHook(const std::function<bool(unsigned)>&);

private:
    static LRESULT CALLBACK hookCallBack(_In_ int, _In_ WPARAM, _In_ LPARAM);

    static std::function<bool(unsigned)> s_func;
    static bool s_isEnabled;

    HHOOK m_hookId;
};

std::string mapVirtualKey(int);

} // namespace keyboard
