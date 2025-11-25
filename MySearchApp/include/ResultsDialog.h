#pragma once
#include <string>
#include <vector>
#include <windows.h>
#include "Localization.h"
#include "ResultItem.h"

// Интерфейс IHighlighter для подсветки совпадений
class IHighlighter {
public:
    virtual bool highlight(const std::wstring& filePath,
                           const std::wstring& word,
                           size_t line,
                           const std::wstring& content,
                           size_t startPos = 0) const = 0;
                           virtual ~IHighlighter() = default;
};

class ResultsDialog {
public:
    ResultsDialog(HINSTANCE hInst, const Localization& loc, const IHighlighter& highlighter);
    INT_PTR show(const std::wstring& filePath,
                 const std::wstring& content,
                 const std::vector<ResultItem>& matches,
                 HWND parent = nullptr
    );

private:
    struct Ctx {const Localization* loc{};
                const IHighlighter* highlighter{};
                const std::wstring* filePath{};
                const std::wstring* content{};
                const std::vector<ResultItem>* matches{};
                size_t currentIndex{ 0 };
    };

    static INT_PTR CALLBACK DlgProc(HWND, UINT, WPARAM, LPARAM);
    HINSTANCE m_hInst;
    const Localization& m_loc;
    const IHighlighter& m_highlighter;
};
