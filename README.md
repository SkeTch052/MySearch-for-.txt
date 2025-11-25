# MySearch

Контекстный поиск по `.txt` файлам и подсветкой совпадений.

### Возможности
- Точный и неточный (с ошибками) поиск  
- Подсветка найденного текста в открытом Блокноте  
- Переход по совпадениям
- Локализация: русский ↔ английский  
- COM Shell Extension – пункт «Поиск с MySearch» в контекстном меню `.txt` файлов

### Установка
1. Перейдите в [Releases](https://github.com/SkeTch052/MySearch-for-.txt/releases/latest)  
2. Скачайте `MySearchSetup.exe`  
3. Запустите от имени администратора  

После установки пункт появится в контекстном меню `.txt` файлов.

# Сборка из исходного кода

### Требования для сборки

Visual Studio 2022\
CMake ≥ 3.21\
Windows 10 x64

### Склонируйте репозиторий

```bash
git clone https://github.com/SkeTch052/MySearch-for-.txt.git
```

### CMake + Visual Studio 2022 (x64)
```bash
cmake -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Release --parallel
```

Готовые файлы `MySearchApp.exe` и `MySearchShell.dll` будут в папке\
build/MySearchApp/Release/

## Ручная регистрация / удаление

Откройте CMD **от имени администратора** в build/MySearchApp/Release/ (в папке с готовыми файлами).

Введите для регистрации:
```bash
regsvr32 MySearchShell.dll
```

Для удаления:
```bash
regsvr32 /u MySearchShell.dll
```

>[!NOTE]\
После регистрации пункт «Поиск с MySearch» появится в контекстном меню .txt.\
При ручном удалении проводник нужно перезапустить вручную


## Лицензия
Проект распространяется под лицензией MIT (см. файл LICENSE).