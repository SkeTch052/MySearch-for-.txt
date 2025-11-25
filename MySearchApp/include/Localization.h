#pragma once
#include <windows.h>
#include <string>

enum class Lang { English, Russian };

class Localization {
public:
    explicit Localization(HINSTANCE hInst, Lang lang = Lang::Russian) noexcept;
    std::wstring loadString(UINT englishId) const;
    Lang lang() const noexcept { return m_lang; }
private:
    HINSTANCE m_hInst;
    Lang m_lang;
    UINT mapToLanguage(UINT englishId) const noexcept; // map EN->RU id
};
