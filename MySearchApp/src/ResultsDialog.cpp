#include "../include/ResultsDialog.h"
#include "../resource.h"
#include <string>

ResultsDialog::ResultsDialog(HINSTANCE hInst, const Localization& loc, const IHighlighter& highlighter)
    : m_hInst(hInst), m_loc(loc), m_highlighter(highlighter) {
}

// Процедура обработки сообщений диалога
INT_PTR CALLBACK ResultsDialog::DlgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    static Ctx* ctx = nullptr;
    switch (msg) {
        case WM_INITDIALOG: {
            ctx = reinterpret_cast<Ctx*>(lParam);
            SetWindowTextW(hwnd, ctx->loc->loadString(IDS_RESULTS_TITLE_EN).c_str());

            wchar_t buf[256];
            // Форматирование текста с количеством совпадений
            _snwprintf_s(buf, _TRUNCATE,
                (ctx->loc->loadString(IDS_FOUND_MATCHES_EN) + L" (%d " + ctx->loc->loadString(IDS_OUT_OF_EN) + L" %d)").c_str(),
                static_cast<unsigned>(ctx->matches->size()),
                ctx->matches->empty() ? 0 : static_cast<unsigned>(ctx->currentIndex + 1),
                static_cast<unsigned>(ctx->matches->size()));
            SetDlgItemTextW(hwnd, IDC_FOUND_LABEL, buf);

            // Кнопка "Далее"
            SetDlgItemTextW(hwnd, IDC_NEXT, ctx->loc->loadString(IDS_NEXT_EN).c_str());
            // Кнопка "Закрыть"
            SetDlgItemTextW(hwnd, IDC_CLOSE, ctx->loc->loadString(IDS_CLOSE_EN).c_str());

            // Если есть совпадения, подсвечиваем первое
            if (!ctx->matches->empty()) {
                const auto& m = (*ctx->matches)[0];
                ctx->highlighter->highlight(*ctx->filePath, m.word, m.line, *ctx->content, m.startPos);
            }
            SetWindowPos(hwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
            // Устанавливаем фокус на окно результатов
            SetForegroundWindow(hwnd);
            return TRUE;
        }

        // Обработка кнопок "Далее" и "Закрыть"
        case WM_COMMAND:
            switch (LOWORD(wParam)) {
                case IDC_NEXT:
                    // Если контекст и совпадения есть
                    if (ctx && ctx->matches && !ctx->matches->empty()) {
                        ctx->currentIndex = (ctx->currentIndex + 1) % ctx->matches->size();
                        const auto& m = (*ctx->matches)[ctx->currentIndex];
                        ctx->highlighter->highlight(*ctx->filePath, m.word, m.line, *ctx->content, m.startPos);

                        // Обновляем текст "Найдено совпадений: x (y из x)"
                        wchar_t buf[256];
                        _snwprintf_s(buf, _TRUNCATE,
                            (ctx->loc->loadString(IDS_FOUND_MATCHES_EN) + L" (%d " + ctx->loc->loadString(IDS_OUT_OF_EN) + L" %d)").c_str(),
                            static_cast<unsigned>(ctx->matches->size()),
                            static_cast<unsigned>(ctx->currentIndex + 1),
                            static_cast<unsigned>(ctx->matches->size()));
                        SetDlgItemTextW(hwnd, IDC_FOUND_LABEL, buf);

                        // Сохраняем фокус на окне
                        SetForegroundWindow(hwnd);
                    }
                    return TRUE;

                case IDC_CLOSE:
                    EndDialog(hwnd, IDCANCEL);
                    return TRUE;
                }
                break;
    }
    // FALSE для необработанных сообщений
    return FALSE;
}

// Показываем диалог
INT_PTR ResultsDialog::show(const std::wstring& filePath,
                            const std::wstring& content,
                            const std::vector<ResultItem>& matches,
                            HWND parent) {
    Ctx c{};
    c.loc = &m_loc;
    c.highlighter = &m_highlighter;
    c.filePath = &filePath;
    c.content = &content;
    c.matches = &matches;

    return DialogBoxParamW(m_hInst, MAKEINTRESOURCEW(RESULTS_DLG), parent, DlgProc, reinterpret_cast<LPARAM>(&c));
}
