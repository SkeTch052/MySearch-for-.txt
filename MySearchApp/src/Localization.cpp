#include "../include/Localization.h"
#include "../resource.h"

Localization::Localization(HINSTANCE hInst, Lang lang) noexcept
    : m_hInst(hInst), m_lang(lang) {
}

UINT Localization::mapToLanguage(UINT englishId) const noexcept {
    switch (englishId) {
        // Возвращаем русский или английский ID
        case IDS_SEARCH_TITLE_EN:      return (m_lang == Lang::Russian) ? IDS_SEARCH_TITLE_RU : IDS_SEARCH_TITLE_EN;
        case IDS_ENTER_QUERY_EN:       return (m_lang == Lang::Russian) ? IDS_ENTER_QUERY_RU : IDS_ENTER_QUERY_EN;
        case IDS_RESULTS_TITLE_EN:     return (m_lang == Lang::Russian) ? IDS_RESULTS_TITLE_RU : IDS_RESULTS_TITLE_EN;
        case IDS_FOUND_MATCHES_EN:     return (m_lang == Lang::Russian) ? IDS_FOUND_MATCHES_RU : IDS_FOUND_MATCHES_EN;
        case IDS_NEXT_EN:              return (m_lang == Lang::Russian) ? IDS_NEXT_RU : IDS_NEXT_EN;
        case IDS_CLOSE_EN:             return (m_lang == Lang::Russian) ? IDS_CLOSE_RU : IDS_CLOSE_EN;
        case IDS_NO_MATCHES_EN:        return (m_lang == Lang::Russian) ? IDS_NO_MATCHES_RU : IDS_NO_MATCHES_EN;
        case IDS_SEARCH_CANCELLED_EN:  return (m_lang == Lang::Russian) ? IDS_SEARCH_CANCELLED_RU : IDS_SEARCH_CANCELLED_EN;
        case IDS_NO_FILE_EN:           return (m_lang == Lang::Russian) ? IDS_NO_FILE_RU : IDS_NO_FILE_EN;
        case IDS_OPEN_FAIL_EN:         return (m_lang == Lang::Russian) ? IDS_OPEN_FAIL_RU : IDS_OPEN_FAIL_EN;
        case IDS_OK_EN:                return (m_lang == Lang::Russian) ? IDS_OK_RU : IDS_OK_EN;
        case IDS_CANCEL_EN:            return (m_lang == Lang::Russian) ? IDS_CANCEL_RU : IDS_CANCEL_EN;
        case IDS_SEARCH_EN:            return (m_lang == Lang::Russian) ? IDS_SEARCH_RU : IDS_SEARCH_EN;
        case IDS_OUT_OF_EN:            return (m_lang == Lang::Russian) ? IDS_OUT_OF_RU : IDS_OUT_OF_EN;
        case IDS_EXACT_EN:             return (m_lang == Lang::Russian) ? IDS_EXACT_RU : IDS_EXACT_EN;
        case IDS_FUZZY_EN:             return (m_lang == Lang::Russian) ? IDS_FUZZY_RU : IDS_FUZZY_EN;
        // По умолчанию английский
        default:                       return englishId;
    }
}

// Загружает локализованную строку по английскому ID: маппит на ID текущего языка и извлекает из ресурсов
std::wstring Localization::loadString(UINT englishId) const {
    UINT id = mapToLanguage(englishId);
    wchar_t buffer[512] = {};
    int n = LoadStringW(m_hInst, id, buffer, static_cast<int>(std::size(buffer)));
    return (n > 0) ? std::wstring(buffer, n) : L"";
}
