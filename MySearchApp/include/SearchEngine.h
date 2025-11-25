#pragma once
#include <string>
#include <vector>
#include <utility>
#include "ResultItem.h"

class SearchEngine {
public:
    // Разбивает текст на слова с указанием строк
    std::vector<ResultItem> tokenizeWordsWithLines(const std::wstring& text) const;
    // Находит совпадения
    std::vector<ResultItem> fuzzyMatches(const std::vector<ResultItem>& words,
                                         const std::wstring& query,
                                         bool exactMatch = false,
                                         int maxDistance = 2) const;
    // Выполняет полный поиск и возвращает результаты в формате ResultItem
    std::vector<ResultItem> findMatches(const std::wstring& content,
                                        const std::wstring& query,
                                        bool exactMatch = false,
                                        int maxDistance = 2) const;
    // Вычисляет расстояние Левенштейна между строками
    static size_t levenshtein(const std::wstring& a, const std::wstring& b);
};
