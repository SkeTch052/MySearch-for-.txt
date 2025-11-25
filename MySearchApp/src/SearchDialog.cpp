#include "../include/SearchDialog.h"
#include "../resource.h"
#include <commctrl.h>

SearchDialog::SearchDialog(HINSTANCE hInst, const Localization& loc)
    : m_hInst(hInst), m_loc(loc) {
}

INT_PTR CALLBACK SearchDialog::DlgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    static Ctx* ctx = nullptr;
    switch (msg) {
    case WM_INITDIALOG: {
        ctx = reinterpret_cast<Ctx*>(lParam);
        SetWindowTextW(hwnd, ctx->loc->loadString(IDS_SEARCH_TITLE_EN).c_str());
        SetDlgItemTextW(hwnd, IDC_QUERY_LABEL, ctx->loc->loadString(IDS_ENTER_QUERY_EN).c_str());
        SetDlgItemTextW(hwnd, IDC_SEARCH, ctx->loc->loadString(IDS_SEARCH_EN).c_str());
        SetDlgItemTextW(hwnd, IDC_EXACT, ctx->loc->loadString(IDS_EXACT_EN).c_str());
        SetDlgItemTextW(hwnd, IDC_FUZZY, ctx->loc->loadString(IDS_FUZZY_EN).c_str());

        CheckRadioButton(hwnd, IDC_EXACT, IDC_FUZZY, IDC_EXACT);
        SetFocus(GetDlgItem(hwnd, IDC_EDIT));
        return FALSE;
    }
    case WM_COMMAND:
        if (LOWORD(wParam) == IDC_SEARCH) {
            wchar_t buffer[512]{};
            GetDlgItemTextW(hwnd, IDC_EDIT, buffer, static_cast<int>(std::size(buffer)));
            if (ctx && ctx->out) *ctx->out = buffer;
            if (ctx && ctx->mode) {
                *ctx->mode = IsDlgButtonChecked(hwnd, IDC_EXACT) ? SearchMode::Exact : SearchMode::Fuzzy;
            }
            EndDialog(hwnd, IDOK);
            return TRUE;
        }
        else if (LOWORD(wParam) == IDCANCEL) {
            if (ctx && ctx->out) ctx->out->clear();
            if (ctx && ctx->mode) *ctx->mode = SearchMode::Fuzzy;
            EndDialog(hwnd, IDCANCEL);
            return TRUE;
        }
        else if (LOWORD(wParam) == IDC_EXACT || LOWORD(wParam) == IDC_FUZZY) {
            // Взаимоисключение радиокнопок
            CheckRadioButton(hwnd, IDC_EXACT, IDC_FUZZY, LOWORD(wParam));
            return TRUE;
        }
        break;
    }
    return FALSE;
}

std::pair<std::wstring, SearchDialog::SearchMode> SearchDialog::show(HWND parent) {
    std::wstring result;
    SearchMode mode = SearchMode::Fuzzy;
    Ctx c{ &m_loc, &result, &mode };
    INT_PTR ret = DialogBoxParamW(m_hInst, MAKEINTRESOURCEW(SEARCH_DLG), parent, DlgProc, reinterpret_cast<LPARAM>(&c));
    if (ret == -1) return { L"", SearchMode::Fuzzy };
    return { result, mode };
}
