#include "../include/NotepadHighlighter.h"

// Находит окно Блокнота по классу
HWND NotepadHighlighter::findNotepad() {
    return FindWindowW(L"Notepad", nullptr);
}

// Находит Edit контроль в окне Блокнота
HWND NotepadHighlighter::findEdit(HWND notepad) {
    return notepad ? FindWindowExW(notepad, nullptr, L"Edit", nullptr) : nullptr;
}

// Открывает Блокнот с файлом и ждет его появления
bool NotepadHighlighter::openAndWait(const std::wstring& filePath, int maxWaitMs) const {
    ShellExecuteW(nullptr, L"open", L"notepad.exe", filePath.c_str(), nullptr, SW_SHOW);
    const int step = 50;
    int waited = 0;
    while (waited < maxWaitMs) {
        HWND np = findNotepad();
        if (np) {
            return true;
        }
        Sleep(step);
        waited += step;
    }
    return findNotepad() != nullptr;
}

// Подсвечивает слово в Блокноте
bool NotepadHighlighter::highlight(const std::wstring& filePath,
                                   const std::wstring& word,
                                   size_t targetLine,
                                   const std::wstring& content,
                                   size_t startPos) const {
    HWND np = findNotepad();
    if (!np) return false;
    HWND edit = findEdit(np);
    if (!edit) return false;

    size_t currentLine = 1;
    size_t pos = 0;
    bool found = false;

    const size_t n = content.size();
    const size_t wlen = word.size();
    for (size_t i = 0; i < n; ++i) {
        if (currentLine == targetLine) {
            if (i + wlen <= n && content.compare(i, wlen, word) == 0 && i >= startPos) {
                pos = i;
                SendMessageW(edit, EM_SETSEL, static_cast<WPARAM>(pos),
                    static_cast<LPARAM>(pos + wlen));
                found = true;
                break;
            }
        }
        if (content[i] == L'\n') ++currentLine;
    }
    if (found) {
        SendMessageW(edit, EM_SCROLLCARET, 0, 0);
    }
    return found;
}
