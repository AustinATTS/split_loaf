#include "WinSettings.hpp"
#include "WinHooks.hpp"
#include <windows.h>
#include <string>

static HWND hWndSettings = NULL;

static Keybind targetBind;
static Keybind lockBind;
static Keybind unlockBind;

static const Keybind DEFAULT_TARGET  = {
    VK_F8,
    false,
    false,
    false
};
static const Keybind DEFAULT_LOCK    = {
    VK_F6,
    false,
    false,
    false
};
static const Keybind DEFAULT_UNLOCK  = {
    VK_F7,
    false,
    false,
    false
};

static bool captureMode = false;
static Keybind * captureTarget = nullptr;

static HWND lblTarget, lblLock, lblUnlock;

static const char * RUN_KEY = "Software\\Microsoft\\Windows\\CurrentVersion\\Run";
static const char * RUN_NAME = "SplitLoaf";
static const char * KEYBINDS_KEY = "Software\\SplitLoaf\\Keybinds";

void WinSettings_SaveKeybinds ( ) {
    HKEY hKey;
    if (RegCreateKeyExA(HKEY_CURRENT_USER, KEYBINDS_KEY, 0, NULL, 0, KEY_WRITE, NULL, & hKey, NULL) != ERROR_SUCCESS) {
        return;
    }

    #define SAVE_BIND(name, kb) \
        RegSetValueExA(hKey, #name "Key", 0, REG_DWORD, (BYTE * ) & kb.key, sizeof(UINT)); \
        RegSetValueExA(hKey, #name "Ctrl", 0, REG_DWORD, (BYTE *) & kb.ctrl, sizeof(BOOL)); \
        RegSetValueExA(hKey, #name "Shift", 0, REG_DWORD, (BYTE *) & kb.shift, sizeof(BOOL)); \
        RegSetValueExA(hKey, #name "Alt", 0, REG_DWORD, (BYTE *) & kb.alt, sizeof(BOOL));

    SAVE_BIND(Target, targetBind);
    SAVE_BIND(Lock, lockBind);
    SAVE_BIND(Unlock, unlockBind);

    #undef SAVE_BIND
    RegCloseKey(hKey);
}

void WinSettings_LoadKeybinds ( ) {
    HKEY hKey;
    if (RegOpenKeyExA(HKEY_CURRENT_USER, KEYBINDS_KEY, 0, KEY_READ, & hKey) != ERROR_SUCCESS) {
        targetBind = DEFAULT_TARGET;
        lockBind   = DEFAULT_LOCK;
        unlockBind = DEFAULT_UNLOCK;
        return;
    }

    DWORD size = sizeof(UINT);

    auto LoadBind = [&] (const char * name, Keybind & kb, const Keybind & def) {
        if (RegQueryValueExA(hKey, (std::string(name) + "Key").c_str(), NULL, NULL, (BYTE *) & kb.key, & size) != ERROR_SUCCESS) {
            kb.key = def.key;
        }
        if (RegQueryValueExA(hKey, (std::string(name) + "Ctrl").c_str(), NULL, NULL, (BYTE *) & kb.ctrl, & size) != ERROR_SUCCESS) {
            kb.ctrl = def.ctrl;
        }
        if (RegQueryValueExA(hKey, (std::string(name) + "Shift").c_str(), NULL, NULL, (BYTE *) & kb.shift, & size) != ERROR_SUCCESS) {
            kb.shift = def.shift;
        }
        if (RegQueryValueExA(hKey, (std::string(name) + "Alt").c_str(), NULL, NULL, (BYTE *) & kb.alt, & size) != ERROR_SUCCESS) {
            kb.alt = def.alt;
        }
    };

    LoadBind("Target", targetBind, DEFAULT_TARGET);
    LoadBind("Lock", lockBind, DEFAULT_LOCK);
    LoadBind("Unlock", unlockBind, DEFAULT_UNLOCK);

    RegCloseKey(hKey);
}


const Keybind & WinSettings_GetTargetBind ( ) {
    return targetBind;
}
const Keybind & WinSettings_GetLockBind ( ) {
    return lockBind;
}
const Keybind & WinSettings_GetUnlockBind ( ) {
    return unlockBind;
}


std::string FormatKeybind (const Keybind & kb) {
    if (kb.key == 0) {
        return "[None]";
    }

    std::string out;
    if (kb.ctrl) {
        out += "Ctrl+";
    }
    if (kb.shift) {
        out += "Shift+";
    }
    if (kb.alt) {
        out += "Alt+";
    }

    char name[64];
    GetKeyNameTextA(MapVirtualKeyA(kb.key, MAPVK_VK_TO_VSC) << 16, name, sizeof(name));
    out += name;
    return out;
}

void UpdateLabels ( ) {
    SetWindowTextA(lblTarget, FormatKeybind(targetBind).c_str());
    SetWindowTextA(lblLock,   FormatKeybind(lockBind).c_str());
    SetWindowTextA(lblUnlock, FormatKeybind(unlockBind).c_str());
}


bool GetRunOnStartup ( ) {
    HKEY hKey;
    if (RegOpenKeyExA(HKEY_CURRENT_USER, RUN_KEY, 0, KEY_READ, & hKey) != ERROR_SUCCESS) {
        return false;
    }

    char buffer[MAX_PATH];
    DWORD size = sizeof(buffer);
    bool exists = (RegQueryValueExA(hKey, RUN_NAME, NULL, NULL, (LPBYTE) buffer, & size) == ERROR_SUCCESS);
    RegCloseKey(hKey);
    return exists;
}

void SetRunOnStartup (bool enable) {
    HKEY hKey;
    if (RegOpenKeyExA(HKEY_CURRENT_USER, RUN_KEY, 0, KEY_WRITE, & hKey) != ERROR_SUCCESS) {
        return;
    }

    if (enable) {
        char exePath[MAX_PATH];
        GetModuleFileNameA(NULL, exePath, MAX_PATH);
        RegSetValueExA(hKey, RUN_NAME, 0, REG_SZ, (BYTE *) exePath, strlen(exePath) + 1);
    } else {
        RegDeleteValueA(hKey, RUN_NAME);
    }

    RegCloseKey(hKey);
}


LRESULT CALLBACK SettingsWndProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_CREATE: {
            HINSTANCE hInst = (HINSTANCE) GetWindowLongPtr(hwnd, GWLP_HINSTANCE);

            CreateWindowA("STATIC", "Target Key:", WS_VISIBLE | WS_CHILD, 20, 20, 150, 20, hwnd, NULL, hInst, NULL);
            lblTarget = CreateWindowA("STATIC", "[None]", WS_VISIBLE | WS_CHILD, 20, 40, 150, 20, hwnd, NULL, hInst, NULL);
            CreateWindowA("BUTTON", "Change", WS_VISIBLE | WS_CHILD, 200, 30, 100, 30, hwnd, (HMENU) 1, hInst, NULL);

            CreateWindowA("STATIC", "Lock Key:", WS_VISIBLE | WS_CHILD, 20, 80, 150, 20, hwnd, NULL, hInst, NULL);
            lblLock = CreateWindowA("STATIC", "[None]", WS_VISIBLE | WS_CHILD, 20, 100, 150, 20, hwnd, NULL, hInst, NULL);
            CreateWindowA("BUTTON", "Change", WS_VISIBLE | WS_CHILD, 200, 90, 100, 30, hwnd, (HMENU) 2, hInst, NULL);

            CreateWindowA("STATIC", "Unlock Key:", WS_VISIBLE | WS_CHILD, 20, 140, 150, 20, hwnd, NULL, hInst, NULL);
            lblUnlock = CreateWindowA("STATIC", "[None]", WS_VISIBLE | WS_CHILD, 20, 160, 150, 20, hwnd, NULL, hInst, NULL);
            CreateWindowA("BUTTON", "Change", WS_VISIBLE | WS_CHILD, 200, 150, 100, 30, hwnd, (HMENU) 3, hInst, NULL);

            HWND chk = CreateWindowA("BUTTON", "Run on startup", WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX, 20, 200, 200, 20, hwnd, (HMENU) 4, hInst, NULL);
            if (GetRunOnStartup()) {
                SendMessage(chk, BM_SETCHECK, BST_CHECKED, 0);
            }

            CreateWindowA("BUTTON", "Close", WS_VISIBLE | WS_CHILD, 120, 230, 100, 30, hwnd, (HMENU) 10, hInst, NULL);

            UpdateLabels();
        }
        break;

        case WM_COMMAND:
            switch (LOWORD(wParam)) {
                case 1:
                    captureMode = ! captureMode;
                    captureTarget = captureMode ? & targetBind : nullptr;
                    SetFocus(hwnd);
                    break;
                case 2:
                    captureMode = ! captureMode;
                    captureTarget = captureMode ? & lockBind : nullptr;
                    SetFocus(hwnd);
                    break;
                case 3:
                    captureMode = ! captureMode;
                    captureTarget = captureMode ? & unlockBind : nullptr;
                    SetFocus(hwnd);
                    break;
                case 4:
                    SetRunOnStartup(SendDlgItemMessage(hwnd, 4, BM_GETCHECK, 0, 0) == BST_CHECKED);
                    break;
                case 10:
                    DestroyWindow(hwnd);
                    break;
            }
            break;

        case WM_KEYDOWN:
            if (captureMode && captureTarget) {
                UINT vk = (UINT) wParam;
                captureTarget -> ctrl  = (GetKeyState(VK_CONTROL) & 0x8000) != 0;
                captureTarget -> shift = (GetKeyState(VK_SHIFT) & 0x8000) != 0;
                captureTarget -> alt   = (GetKeyState(VK_MENU) & 0x8000) != 0;

                if (vk != VK_SHIFT && vk != VK_CONTROL && vk != VK_MENU) {
                    captureTarget -> key = vk;
                }

                UpdateLabels();
                WinSettings_SaveKeybinds();
            }
            break;

        case WM_CLOSE:
            DestroyWindow(hwnd);
            break;

        case WM_DESTROY:
            WinSettings_SaveKeybinds();
            hWndSettings = NULL;
            break;
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}

void OpenSettingsWindow (HINSTANCE hInstance, HWND parent) {
    if (hWndSettings && IsWindow(hWndSettings)) {
        SetForegroundWindow(hWndSettings);
        return;
    }

    WNDCLASSA wc = {};
    wc.lpfnWndProc = SettingsWndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = "SplitLoafSettings";
    wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(101));

    RegisterClassA(& wc);

    hWndSettings = CreateWindowA("SplitLoafSettings", "Split Loaf Settings", (WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX & ~WS_THICKFRAME), CW_USEDEFAULT, CW_USEDEFAULT, 350, 310, parent, NULL, hInstance, NULL);

    ShowWindow(hWndSettings, SW_SHOW);
}

void WinSettings_Init ( ) {
    WinSettings_LoadKeybinds();
    UpdateLabels();
}