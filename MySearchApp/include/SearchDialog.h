#pragma once
#include <string>
#include <windows.h>
#include "Localization.h"

class SearchDialog {
public:
    enum class SearchMode { Exact, Fuzzy };
    SearchDialog(HINSTANCE hInst, const Localization& loc);
    std::pair<std::wstring, SearchMode> show(HWND parent = nullptr);

private:
    static INT_PTR CALLBACK DlgProc(HWND, UINT, WPARAM, LPARAM);
    struct Ctx {
        const Localization* loc{};
        std::wstring* out{};
        SearchMode* mode{};
    };
    HINSTANCE m_hInst;
    const Localization& m_loc;
};
