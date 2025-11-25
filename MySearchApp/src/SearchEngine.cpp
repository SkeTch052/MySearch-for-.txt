#include "../include/SearchEngine.h"
#include <cwctype>
#include <algorithm>
#include <clocale>

// Вспомогательная функция для нахождения минимума из трех чисел
static size_t min3(size_t a, size_t b, size_t c) {
    return std::min(a, std::min(b, c));
}

// Вычисляет расстояние Левенштейна между двумя строками
size_t SearchEngine::levenshtein(const std::wstring& s1, const std::wstring& s2) {
    const size_t m = s1.size(), n = s2.size();
    if (m == 0) return n;
    if (n == 0) return m;
    // Оптимизация по памяти: два ряда
    std::vector<size_t> prev(n + 1), curr(n + 1);
    for (size_t j = 0; j <= n; ++j) prev[j] = j;
    for (size_t i = 1; i <= m; ++i) {
        curr[0] = i;
        for (size_t j = 1; j <= n; ++j) {
            size_t cost = (s1[i - 1] == s2[j - 1]) ? 0 : 1;
            curr[j] = min3(prev[j - 1] + cost, prev[j] + 1, curr[j - 1] + 1);
        }
        std::swap(prev, curr);
    }
    return prev[n];
}

// Токенизирует текст на слова с указанием номера строки
std::vector<ResultItem> SearchEngine::tokenizeWordsWithLines(const std::wstring& content) const {
    std::vector<ResultItem> words;
    std::wstring token;
    size_t line = 1;
    const size_t n = content.size();

    for (size_t i = 0; i <= n; ++i) {
        if (i == n || iswspace(content[i])) {
            if (!token.empty()) {
                words.push_back({ token, line, 0 });
                token.clear();
            }
            if (i < n && content[i] == L'\n') ++line;
        }
        else {
            token += content[i];
        }
    }
    return words;
}

// Находит совпадения
std::vector<ResultItem> SearchEngine::fuzzyMatches(const std::vector<ResultItem>& words,
                                                   const std::wstring& query,
                                                   bool exactMatch,
                                                   int maxDistance) const {
    std::vector<ResultItem> out;
    out.reserve(words.size() / 8 + 1);
    bool fullMatchFound = false;
    setlocale(LC_ALL, "");

    for (const auto& w : words) {
        if (exactMatch) {
            if (w.word == query && !fullMatchFound) {
                out.push_back({ query, w.line, 0 }); // Полное совпадение слова
                fullMatchFound = true; // Учитываем только первое полное совпадение
            }
            else if (w.word.find(query) != std::wstring::npos && w.word != query) {
                size_t pos = w.word.find(query);
                out.push_back({ query, w.line, pos }); // Подстрока в слове
            }
        }
        else {
            // Для fuzzy-поиска приводим к нижнему регистру
            std::wstring lowerWord = w.word;
            std::transform(lowerWord.begin(), lowerWord.end(), lowerWord.begin(), ::towlower);
            std::wstring lowerQuery = query;
            std::transform(lowerQuery.begin(), lowerQuery.end(), lowerQuery.begin(), ::towlower);

            // Очистка знаков препинания
            lowerWord.erase(std::remove_if(lowerWord.begin(), lowerWord.end(), ::iswpunct), lowerWord.end());

            size_t dist = levenshtein(lowerWord, lowerQuery);
            double normDist = static_cast<double>(dist) / std::max(lowerWord.size(), lowerQuery.size());

            if (dist <= static_cast<size_t>(maxDistance) && normDist <= 0.3) {
                out.push_back({ w.word, w.line, 0 });
            }
        }
    }
    return out;
}

// Выполняет полный поиск и возвращает результаты в формате ResultItem
std::vector<ResultItem> SearchEngine::findMatches(const std::wstring& content,
                                                  const std::wstring& query,
                                                  bool exactMatch,
                                                  int maxDistance) const {
    std::vector<ResultItem> matches;
    auto words = tokenizeWordsWithLines(content);
    matches.reserve(words.size());

    for (const auto& w : fuzzyMatches(words, query, exactMatch, maxDistance)) {
        matches.push_back({ w.word, w.line, w.startPos });
    }

    return matches;
}
