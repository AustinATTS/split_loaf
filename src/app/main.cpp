#include <windows.h>
#include "win/WinTray.h"
#include "core/Platform.h"

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_DESTROY:
            RemoveTrayIcon();
            PostQuitMessage(0);
            return 0;

        case WM_USER + 1: // tray icon callback
            if (lParam == WM_RBUTTONUP) {
                ShowTrayMenu(hwnd);
            }
            return 0;

        case WM_COMMAND: // menu selection
            switch(LOWORD(wParam)) {
            case 1: // Settings
                    MessageBox(hwnd, "Settings dialog would appear here.", "Settings", MB_OK);
                    break;
            case 2: // Exit
                    DestroyWindow(hwnd);
                    break;
            }
            return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int) {
    const char CLASS_NAME[] = "SplitLoafTray";

    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;

    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(
        0,
        CLASS_NAME,
        "Split Loaf",
        0, 0, 0, 0, 0,
        NULL, NULL, hInstance, NULL
    );

    InitTrayIcon(hwnd);

    // --- Core Platform initialization ---
    Platform::init();

    MSG msg = {};
    while (true) {
        // Process Windows messages
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);

            if (msg.message == WM_QUIT)
                goto quit;
        }

        // Process your platform-specific events
        Platform::processEvents();
    }

    quit:
        Platform::shutdown();
    return 0;
}
