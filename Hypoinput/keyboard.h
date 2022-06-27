#pragma once

#include "expansions.h"
#include "utils.h"
#include <Windows.h>
#include <array>
#include <functional>
#include <vector>

namespace keyboard {

class KeyboardHook {
public:
    KeyboardHook();
    explicit KeyboardHook(const std::function<std::string(unsigned)>&);

    void remove() const;
    void add(HINSTANCE&);

private:
    static LRESULT CALLBACK hookCallBack(_In_ int, _In_ WPARAM, _In_ LPARAM);

    static std::function<std::string(unsigned)> s_func;
    static bool s_isEnabled;

    HHOOK m_hookId;
};

std::string mapVirtualKey(int);
void inject(const std::wstring&);
void repeat(int, size_t);
template <size_t N>
std::vector<INPUT> inputFromVirtualKeys(std::array<int, N>);

} // namespace keyboard
