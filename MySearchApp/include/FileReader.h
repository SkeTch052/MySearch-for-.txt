#pragma once
#include <string>
#include <stdexcept>

class FileReader {
public:
    static std::wstring readUtf8FileToWstring(const std::wstring& path);
};
