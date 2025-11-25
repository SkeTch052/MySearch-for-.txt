#include <windows.h>
#include <commctrl.h>
#pragma comment(lib, "comctl32.lib")

#include "../include/App.h"
#include "../include/Localization.h"
#include "../include/FileReader.h"
#include "../include/SearchEngine.h"
#include "../include/SearchDialog.h"
#include "../include/ResultsDialog.h"
#include "../include/NotepadHighlighter.h"
#include "../resource.h"
#include <shellapi.h>
#include <vector>

// Функция для получения аргументов командной строки
static std::vector<std::wstring> getArgs() {
    int argc = 0;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc);
    std::vector<std::wstring> out;
    if (argv) {
        out.assign(argv, argv + argc);
        LocalFree(argv);
    }
    return out;
}

// Основная функция приложения
int RunApp(HINSTANCE hInstance, LPWSTR) {
    INITCOMMONCONTROLSEX icc{ sizeof(icc), ICC_LISTVIEW_CLASSES };
    InitCommonControlsEx(&icc);

    Localization loc(hInstance, Lang::Russian);

    auto args = getArgs();
    if (args.size() < 2) {
        MessageBoxW(nullptr, loc.loadString(IDS_NO_FILE_EN).c_str(), L"Error", MB_OK | MB_ICONERROR);
        return 1;
    }

    const std::wstring filePath = args[1];
    std::wstring content;
    try {
        content = FileReader::readUtf8FileToWstring(filePath);
    }
    catch (...) {
        MessageBoxW(nullptr, loc.loadString(IDS_OPEN_FAIL_EN).c_str(), L"Error", MB_OK | MB_ICONERROR);
        return 1;
    }

    SearchDialog inputDlg(hInstance, loc);
    auto [query, searchMode] = inputDlg.show();
    if (query.empty()) {
        return 1;
    }

    // Поиск совпадений с помощью SearchEngine
    SearchEngine engine;
    bool exactMatch = (searchMode == SearchDialog::SearchMode::Exact);
    auto matches = engine.findMatches(content, query, exactMatch, 2);
    if (matches.empty()) {
        MessageBoxW(nullptr, loc.loadString(IDS_NO_MATCHES_EN).c_str(), L"Info", MB_OK);
        return 1;
    }

    NotepadHighlighter np;
    if (!np.openAndWait(filePath, 1000)) {
        MessageBoxW(nullptr, L"Failed to open Notepad", L"Error", MB_OK | MB_ICONERROR);
        return 1;
    }

    ResultsDialog resultsDlg(hInstance, loc, np);
    INT_PTR ret = resultsDlg.show(filePath, content, matches);
    if (ret == -1) {
        MessageBoxW(nullptr, L"Results dialog failed", L"Error", MB_OK | MB_ICONERROR);
        return 1;
    }
    return 0;
}
