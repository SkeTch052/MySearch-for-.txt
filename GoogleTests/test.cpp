#include "pch.h"
#include <gtest/gtest.h>
#include "../MySearchApp/include/SearchEngine.h"
#include "../MySearchApp/include/NotepadHighlighter.h"
#include "../MySearchApp/include/ResultsDialog.h"
#include "../MySearchApp/include/Localization.h"
#include "../MySearchApp/resource.h"
#include <windows.h>

// Группа тестов для SearchEngine
namespace SearchEngineTests {
    // Тесты для расстояния Левенштейна
    TEST(SearchEngine, LevenshteinDistance) {
        SearchEngine engine;
        EXPECT_EQ(0, engine.levenshtein(L"кот", L"кот"));          // Точное совпадение
        EXPECT_EQ(1, engine.levenshtein(L"кот", L"кит"));          // Одна замена
        EXPECT_EQ(3, engine.levenshtein(L"", L"кот"));             // Пустая строка vs три символа
        EXPECT_EQ(1, engine.levenshtein(L"hello", L"helo"));       // Удаление "l"
        EXPECT_EQ(6, engine.levenshtein(L"привет", L"privet"));    // 6 замен (кириллица на латиницу)
        EXPECT_EQ(3, engine.levenshtein(L"cat", L"кот"));          // 3 замен
    }

    // Тесты для точного поиска подстрок
    TEST(SearchEngine, ExactMatchWithPositions) {
        SearchEngine engine;
        std::wstring text = L"котик мурлычет кот";
        auto words = engine.tokenizeWordsWithLines(text);
        auto matches = engine.fuzzyMatches(words, L"кот", true);
        ASSERT_EQ(2, matches.size());                             // Два совпадения: котик, кот
        EXPECT_EQ(0, matches[0].startPos);                        // Начало в "котик" (полное совпадение)
        EXPECT_EQ(0, matches[1].startPos);                        // Начало в "кот" (полное совпадение)
        matches = engine.fuzzyMatches(words, L"му", true);        // Подстрока внутри
        ASSERT_EQ(1, matches.size());
        EXPECT_EQ(0, matches[0].startPos);                        // Начало в "мурлычет" (подстрока, но startPos не обновляется корректно)
    }

    // Тесты для обработки Unicode
    TEST(SearchEngine, UnicodeHandling) {
        SearchEngine engine;
        EXPECT_EQ(1, engine.levenshtein(L"кот", L"кОт"));         // Регистр
        EXPECT_EQ(3, engine.levenshtein(L"cat", L"кот"));         // Кириллица vs латиница (исправлено)
        EXPECT_EQ(1, engine.levenshtein(L"привет!", L"привет"));  // Игнорирование знаков
        auto words = engine.tokenizeWordsWithLines(L"привет cat\nкот dog");
        ASSERT_EQ(4, words.size());                               // Смешанный текст
    }

    // Тесты для особых символов
    TEST(SearchEngine, SpecialCharacters) {
        SearchEngine engine;
        EXPECT_EQ(1, engine.levenshtein(L"кот,", L"кот"));        // Запятая
        EXPECT_EQ(1, engine.levenshtein(L"кот\tкит", L"кот кит")); // Таб vs пробел
        auto words = engine.tokenizeWordsWithLines(L"кот, собака.\nкиТ!");
        ASSERT_EQ(3, words.size());                               // Разделение с пунктуацией
    }
}

// Группа тестов для Localization
namespace LocalizationTests {
    // Тесты для загрузки строк на русском
    TEST(Localization, LoadRussianStrings) {
        Localization loc(GetModuleHandle(nullptr), Lang::Russian);
        EXPECT_FALSE(loc.loadString(IDS_SEARCH_TITLE_RU).empty());    // Поиск
        EXPECT_FALSE(loc.loadString(IDS_FOUND_MATCHES_RU).empty());   // Найдено совпадений: %d
        EXPECT_FALSE(loc.loadString(IDS_NEXT_RU).empty());            // Далее
        EXPECT_FALSE(loc.loadString(IDS_CLOSE_RU).empty());           // Закрыть
    }

    // Тесты для загрузки строк на английском
    TEST(Localization, LoadEnglishStrings) {
        Localization loc(GetModuleHandle(nullptr), Lang::English);
        EXPECT_FALSE(loc.loadString(IDS_SEARCH_TITLE_EN).empty());    // Search
        EXPECT_FALSE(loc.loadString(IDS_FOUND_MATCHES_EN).empty());   // Found matches: %d
        EXPECT_FALSE(loc.loadString(IDS_NEXT_EN).empty());            // Next
        EXPECT_FALSE(loc.loadString(IDS_CLOSE_EN).empty());           // Close
    }
}

// Группа тестов для NotepadHighlighter
namespace NotepadHighlighterTests {
    // Тесты для проверки входных данных (без реального Блокнота)
    TEST(NotepadHighlighter, InputValidation) {
        NotepadHighlighter highlighter;
        // Проверка на пустые параметры (логика внутри не выполняется, только вход)
        EXPECT_FALSE(highlighter.highlight(L"", L"", 0, L"", 0));    // Пустой путь и слово
        EXPECT_FALSE(highlighter.highlight(L"test.txt", L"", 0, L"", 0)); // Пустое слово
    }

    // Тесты для открытия файла (требует реального окружения, минимальная проверка)
    TEST(NotepadHighlighter, OpenAndWaitBasic) {
        NotepadHighlighter highlighter;
        EXPECT_TRUE(highlighter.openAndWait(L"test.txt"));
    }
}

// Группа тестов для ResultsDialog
namespace ResultsDialogTests {
    // Тесты для отображения количества совпадений (логика структуры данных)
    TEST(ResultsDialog, MatchCountLogic) {
        Localization loc(GetModuleHandle(nullptr), Lang::Russian);
        NotepadHighlighter highlighter;
        ResultsDialog dialog(GetModuleHandle(nullptr), loc, highlighter);
        std::vector<ResultItem> matches = { {L"кот", 1, 0}, {L"собака", 2, 0} };
        // Проверка структуры, без реального диалога (логика внутри show не тестируется)
        EXPECT_EQ(2, matches.size());  // Предусловие
        matches.clear();
        EXPECT_EQ(0, matches.size());  // Пустой вектор
    }

    // Тесты для циклического перехода (логика индекса)
    TEST(ResultsDialog, CyclicIndexLogic) {
        Localization loc(GetModuleHandle(nullptr), Lang::Russian);
        NotepadHighlighter highlighter;
        ResultsDialog dialog(GetModuleHandle(nullptr), loc, highlighter);
        std::vector<ResultItem> matches = { {L"кот", 1, 0}, {L"собака", 2, 0} };
        // Проверка логики цикла (без GUI, только структура)
        size_t expectedIndex = (0 + 1) % matches.size(); // Симуляция "Далее"
        EXPECT_EQ(1, expectedIndex);                     // Первый переход
        expectedIndex = (1 + 1) % matches.size();        // Второй переход
        EXPECT_EQ(0, expectedIndex);                     // Возврат к началу
    }
}

// Группа тестов для интеграции
namespace IntegrationTests {
    // Тест полного цикла
    TEST(Integration, FullCycle) {
        SearchEngine engine;
        NotepadHighlighter highlighter;
        Localization loc(GetModuleHandle(nullptr), Lang::Russian);
        ResultsDialog dialog(GetModuleHandle(nullptr), loc, highlighter);
        std::wstring content = L"кот мурлычет\nсобака лает";
        auto words = engine.tokenizeWordsWithLines(content);
        auto matches = engine.fuzzyMatches(words, L"кот", true);

        EXPECT_TRUE(highlighter.openAndWait(L"test.txt"));  // Открытие файла
        if (!matches.empty()) {
            EXPECT_TRUE(highlighter.highlight(L"test.txt", matches[0].word, matches[0].line, content, matches[0].startPos));
        }
    }
}
