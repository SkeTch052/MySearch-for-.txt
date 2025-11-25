#pragma once
#include <string>

// Структура для хранения результата поиска: слово, номер строки и позиция
struct ResultItem {
    std::wstring word;
    size_t line;
    size_t startPos;
};
