#pragma once
#include <string>
#include <windows.h>
#include "ResultsDialog.h"
#include "ResultItem.h"

class NotepadHighlighter : public IHighlighter {
public:
    bool openAndWait(const std::wstring& filePath, int maxWaitMs = 1000) const;
    bool highlight(const std::wstring& filePath,
        const std::wstring& word,
        size_t line,
        const std::wstring& content,
        size_t startPos = 0) const override;
private:
    static HWND findNotepad();
    static HWND findEdit(HWND notepad);
};
