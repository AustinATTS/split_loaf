#include <windows.h>
#include "WinApp.h"
#include "WinTray.h"
#include "core/Platform.h"
#include "WinSettings.h"


static const char* WINDOW_CLASS = "SplitLoafTray";

LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg) {

        case TRAY_CALLBACK:
            if (lParam == WM_RBUTTONUP)
                ShowTrayMenu(hwnd);
            break;

        case WM_COMMAND:
            switch (LOWORD(wParam)) {
            case CMD_SETTINGS:
                    OpenSettingsWindow(hInstance, hwnd);
                    break;

            case CMD_EXIT:
                    PostQuitMessage(0);
                    break;
            }
            break;

        case WM_DESTROY:
            RemoveTrayIcon();
            PostQuitMessage(0);
            return 0;
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}

int RunWindowsApp()
{
    HINSTANCE hInstance = GetModuleHandle(NULL);

    // Register class
    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = WINDOW_CLASS;
    RegisterClass(&wc);

    // Create hidden window
    HWND hwnd = CreateWindowEx(
        0, WINDOW_CLASS, "Split Loaf",
        0, 0, 0, 0, 0,
        NULL, NULL, hInstance, NULL
    );

    InitTrayIcon(hwnd);

    Platform::init();

    MSG msg {};
    while (true) {
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);

            if (msg.message == WM_QUIT) {
                Platform::shutdown();
                return 0;
            }
        }

        Platform::processEvents();
    }
}
