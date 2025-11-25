#include "../include/FileReader.h"
#include <windows.h>
#include <vector>
#include <fstream>
#include <stdexcept>
#include <string>

// Чтение всех байтов из файла
static std::string readAllBytes(const std::wstring& path) {
    // Открываем файл в бинарном режиме
    std::ifstream f(path, std::ios::binary);
    if (!f.is_open()) {
        throw std::runtime_error("open failed");
    }

    f.seekg(0, std::ios::end);
    std::streamoff endPos = f.tellg();
    if (endPos < 0) {
        throw std::runtime_error("tellg failed");
    }
    const size_t size = static_cast<size_t>(endPos);
    f.seekg(0, std::ios::beg);

    std::string data;
    data.resize(size);
    if (size > 0) {
        // Чтение в буфер строки
        f.read(&data[0], static_cast<std::streamsize>(size));
        if (!f) {
            throw std::runtime_error("read failed");
        }
    }
    return data;
}

// Чтение UTF-8 файла в wstring
std::wstring FileReader::readUtf8FileToWstring(const std::wstring& path) {
    std::string bytes = readAllBytes(path);

    // Убираем BOM UTF-8 (EF BB BF), если есть
    if (bytes.size() >= 3 &&
        static_cast<unsigned char>(bytes[0]) == 0xEF &&
        static_cast<unsigned char>(bytes[1]) == 0xBB &&
        static_cast<unsigned char>(bytes[2]) == 0xBF) {
        bytes.erase(0, 3);
    }
    // Если байты пустые, возвращаем пустую wstring
    if (bytes.empty()) return std::wstring();

    // Определяем длину wide-строки
    const int wideLen = MultiByteToWideChar(CP_UTF8, 
                                            MB_ERR_INVALID_CHARS,
                                            bytes.data(), 
                                            static_cast<int>(bytes.size()),
                                            nullptr, 
                                            0);
    if (wideLen <= 0) {
        throw std::runtime_error("convert sizing failed");
    }

    // Создаем wstring нужного размера
    std::wstring w;
    w.resize(static_cast<size_t>(wideLen));
    // Конвертируем байты в wide char
    const int rc = MultiByteToWideChar(CP_UTF8, 
                                       MB_ERR_INVALID_CHARS,
                                       bytes.data(), 
                                       static_cast<int>(bytes.size()),
                                       &w[0], 
                                       wideLen);
    if (rc != wideLen) {
        throw std::runtime_error("convert failed");
    }
    return w;
}
