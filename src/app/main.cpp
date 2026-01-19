#ifdef _WIN32
#include <windows.h>
extern int RunWindowsApp ( );

int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
    return RunWindowsApp();
}

#else

#endif
