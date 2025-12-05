#include "WinSettings.h"
#include <windows.h>
#include <string>

static HWND hWndSettings = NULL;

// Define the main function keys for the program.
static UINT targetKey = VK_F8;
static UINT lockKey   = VK_F9;
static UINT unlockKey = VK_F10;

static bool captureMode = false;
static UINT* captureTarget = nullptr;

static const char* RUN_KEY  = "Software\\Microsoft\\Windows\\CurrentVersion\\Run"; // Path to allow run on startup.
static const char* RUN_NAME = "SplitLoaf";

bool GetRunOnStartup ( ) {
    HKEY hKey;
    if (RegOpenKeyExA(HKEY_CURRENT_USER, RUN_KEY, 0, KEY_READ, & hKey) != ERROR_SUCCESS) {
        // Check if it is already, if not return false.
        return false;
    }

    // If it is then return that it exists.
    char buffer[MAX_PATH];
    DWORD size = sizeof(buffer);
    bool exists = (RegQueryValueExA(hKey, RUN_NAME, NULL, NULL, (LPBYTE)buffer, & size) == ERROR_SUCCESS);

    RegCloseKey(hKey);
    return exists;
}

void SetRunOnStartup (bool enable) { // Set it to be active or deactive.
    HKEY hKey;
    if (RegOpenKeyExA(HKEY_CURRENT_USER, RUN_KEY, 0, KEY_WRITE, & hKey) != ERROR_SUCCESS) {
        // If already active don't do anything.
        return;
    }

    if (enable) { // If you want to enable it.
        char exePath[MAX_PATH];
        GetModuleFileNameA(NULL, exePath, MAX_PATH);
        RegSetValueExA(hKey, RUN_NAME, 0, REG_SZ, (BYTE *)exePath, strlen(exePath) + 1);
    } else { // If you want tpo disable it.
        RegDeleteValueA(hKey, RUN_NAME);
    }

    RegCloseKey(hKey); // Close
}

LRESULT CALLBACK SettingsWndProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {

    switch (msg) {

        case WM_CREATE: { // Create buttons for the settings page.
            HINSTANCE hInst = (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE);

            CreateWindowA("STATIC", "Target Key:", WS_VISIBLE | WS_CHILD,20, 20, 150, 20, hwnd, NULL, hInst, NULL);

            CreateWindowA("BUTTON", "Change", WS_VISIBLE | WS_CHILD,200, 15, 100, 30, hwnd, (HMENU)1, hInst, NULL);

            CreateWindowA("STATIC", "Lock Key:", WS_VISIBLE | WS_CHILD,20, 60, 150, 20, hwnd, NULL, hInst, NULL);

            CreateWindowA("BUTTON", "Change", WS_VISIBLE | WS_CHILD,200, 55, 100, 30, hwnd, (HMENU)2, hInst, NULL);

            CreateWindowA("STATIC", "Unlock Key:", WS_VISIBLE | WS_CHILD,20, 100, 150, 20, hwnd, NULL, hInst, NULL);

            CreateWindowA("BUTTON", "Change", WS_VISIBLE | WS_CHILD,200, 95, 100, 30, hwnd, (HMENU)3, hInst, NULL);

            HWND chk = CreateWindowA("BUTTON", "Run on startup",WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX,20, 140, 200, 20,hwnd, (HMENU)4, hInst, NULL);

            if (GetRunOnStartup()) { // If already active set the button to be checked.
                SendMessage(chk, BM_SETCHECK, BST_CHECKED, 0);
            }

            CreateWindowA("BUTTON", "Close", WS_VISIBLE | WS_CHILD,120, 180, 100, 30, hwnd, (HMENU)10, hInst, NULL);
        }
        break;

        case WM_COMMAND: {
            switch (LOWORD(wParam)) {
                // Execute the functions.
                case 1: // Update the keybind for targeting a window.
                    captureMode = true;
                    captureTarget = & targetKey;
                    SetFocus(hwnd);
                    break;

                case 2: // Update the keybind for locking the keyboard.
                    captureMode = true;
                    captureTarget = & lockKey;
                    SetFocus(hwnd);
                    break;

                case 3: // Update the keybind for unlocking the keyboard.
                    captureMode = true;
                    captureTarget = & unlockKey;
                    SetFocus(hwnd);
                    break;

                case 4: { // Update the run on startup based on the value from the checkbox.
                    bool isChecked = (SendDlgItemMessage(hwnd, 4, BM_GETCHECK, 0, 0) == BST_CHECKED);

                    SetRunOnStartup(isChecked);
                }
                break;

                case 10: // Close the settings window.
                    DestroyWindow(hwnd);
                    hWndSettings = NULL;
                    break;
            }
        }
        break;

        case WM_KEYDOWN:
            if (captureMode && captureTarget) { // Get the keys pressed.
                * captureTarget = (UINT)wParam;
                captureMode = false;
                captureTarget = nullptr;
            }
            break;

        case WM_CLOSE: // On close, destroy window.
            DestroyWindow(hwnd);
            hWndSettings = NULL;
            break;

        case WM_DESTROY: // Close settings.
            hWndSettings = NULL;
            break;
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}

void OpenSettingsWindow (HINSTANCE hInstance, HWND parent) {
    if (hWndSettings) { // If settings is already open, set it to the active window.
        SetForegroundWindow(hWndSettings);
        return;
    }

    WNDCLASSA wc = {};
    wc.lpfnWndProc = SettingsWndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = "SplitLoafSettings";

    RegisterClassA(& wc);

    // Create settings window.
    hWndSettings = CreateWindowA("SplitLoafSettings","Split Loaf Settings",(WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX & ~WS_THICKFRAME),CW_USEDEFAULT, CW_USEDEFAULT,360, 260,parent,NULL,hInstance,NULL);

    Show the settings window.
    ShowWindow(hWndSettings, SW_SHOW);
}
