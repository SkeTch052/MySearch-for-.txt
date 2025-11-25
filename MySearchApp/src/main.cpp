#include <windows.h>
#include "../include/App.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int) {
    return RunApp(hInstance, GetCommandLineW());
}
