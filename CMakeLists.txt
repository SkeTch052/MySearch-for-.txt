cmake_minimum_required(VERSION 3.21)
project(MySearch
    VERSION 1.0.0
    DESCRIPTION "Contextual fuzzy/exact search for .txt files with real-time Notepad highlighting"
    LANGUAGES CXX
)

# ------------------------------------------------------------------
# C++ and compiler settings
# ------------------------------------------------------------------
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Runtime selection: /MD (Release), /MDd (Debug)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")

# ------------------------------------------------------------------
# Source and include paths
# ------------------------------------------------------------------
set(APP_SRC_DIR   "${CMAKE_SOURCE_DIR}/MySearchApp/src")
set(APP_INC_DIR   "${CMAKE_SOURCE_DIR}/MySearchApp/include")
set(SHELL_SRC_DIR "${CMAKE_SOURCE_DIR}/MySearchShell")
set(RES_DIR       "${CMAKE_SOURCE_DIR}/MySearchApp")

# Pre-compiled headers (Shell only)
set(PCH_SHELL_H   "${SHELL_SRC_DIR}/pch.h")
set(PCH_SHELL_CPP "${SHELL_SRC_DIR}/pch.cpp")

# Resources
set(APP_RC "${RES_DIR}/MySearchApp.rc")

# ------------------------------------------------------------------
# MySearchApp.exe (GUI application)
# ------------------------------------------------------------------
add_executable(MySearchApp WIN32
    "${APP_SRC_DIR}/App.cpp"
    "${APP_SRC_DIR}/FileReader.cpp"
    "${APP_SRC_DIR}/Localization.cpp"
    "${APP_SRC_DIR}/main.cpp"
    "${APP_SRC_DIR}/NotepadHighlighter.cpp"
    "${APP_SRC_DIR}/ResultsDialog.cpp"
    "${APP_SRC_DIR}/SearchDialog.cpp"
    "${APP_SRC_DIR}/SearchEngine.cpp"
    ${APP_RC}
)

target_include_directories(MySearchApp PRIVATE "${APP_INC_DIR}")
target_link_libraries(MySearchApp PRIVATE comctl32 shlwapi shell32)

# ------------------------------------------------------------------
# MySearchShell.dll (COM Shell Extension)
# ------------------------------------------------------------------
add_library(MySearchShell SHARED
    "${SHELL_SRC_DIR}/MySearchShell.cpp"
    "${PCH_SHELL_CPP}"
    "${SHELL_SRC_DIR}/MySearchShell.def"
)

target_include_directories(MySearchShell PRIVATE "${SHELL_SRC_DIR}")
target_precompile_headers(MySearchShell PRIVATE "${PCH_SHELL_H}")
target_link_libraries(MySearchShell PRIVATE shell32 shlwapi ole32)

set_target_properties(MySearchShell PROPERTIES
    PREFIX ""
    OUTPUT_NAME "MySearchShell"
    SUFFIX ".dll"
)

# ------------------------------------------------------------------
# Output directories
# ------------------------------------------------------------------
set_target_properties(MySearchApp MySearchShell PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

foreach(CONFIG Debug Release RelWithDebInfo MinSizeRel)
    set_target_properties(MySearchApp MySearchShell PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY_${CONFIG} "${CMAKE_BINARY_DIR}/bin/${CONFIG}"
        LIBRARY_OUTPUT_DIRECTORY_${CONFIG} "${CMAKE_BINARY_DIR}/bin/${CONFIG}"
    )
endforeach()

# ------------------------------------------------------------------
# Install rules
# ------------------------------------------------------------------
install(TARGETS MySearchApp MySearchShell
    RUNTIME DESTINATION .
    LIBRARY DESTINATION .
)

# ------------------------------------------------------------------
# Build information
# ------------------------------------------------------------------
message(STATUS "")
message(STATUS "=====================================================================")
message(STATUS " MySearch build configured successfully!")
message(STATUS " Project: ${PROJECT_NAME} v${PROJECT_VERSION}")
message(STATUS " Output directory: ${CMAKE_BINARY_DIR}/bin/<Debug|Release>/")
message(STATUS "")
message(STATUS " Build Release (recommended for distribution):")
message(STATUS "   cmake --build . --config Release --parallel")
message(STATUS "")
message(STATUS " Build Debug (for development):")
message(STATUS "   cmake --build . --config Debug")
message(STATUS "=====================================================================")
message(STATUS "")