#include "WinHooks.h"
#include "WinBackend.h"
#include <windows.h>
#include "WinTray.h"
#include <stdio.h>
#include <string>
#include <algorithm>

std::string ToLower (const std::string & s) {
    std::string out = s;
    std::transform(out.begin(), out.end(), out.begin(), ::tolower);
    return out;
}

std::string ToUpper (const std::string & s) {
    std::string out = s;
    std::transform(out.begin(), out.end(), out.begin(), ::toupper);
    return out;
}

std::string GetWindowTitle (HWND hwnd) {
    char buffer[256];
    GetWindowTextA(hwnd, buffer, sizeof(buffer));
    return std::string(buffer);
}

void Tray_SetIdle ( ) {
    UpdateTrayTooltip("Split Loaf - Idle");
}

void Tray_SetTargeted (const std::string & windowName) {
    UpdateTrayTooltip("Split Loaf - " + ToLower(windowName));
}

void Tray_SetLocked (const std::string & windowName) {
    UpdateTrayTooltip("Split Loaf - " + ToUpper(windowName));
}

LRESULT CALLBACK LowLevelKeyboardProc (int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode != HC_ACTION) {
        return CallNextHookEx(NULL, nCode, wParam, lParam);
    }

    auto & targetWindow = WinHooks_GetTargetWindow();
    auto & locked = WinHooks_GetLockedFlag();
    auto & targetHasFocus = WinHooks_GetTargetFocusFlag();

    KBDLLHOOKSTRUCT * kbd = (KBDLLHOOKSTRUCT *)lParam;

    if (kbd -> flags & LLKHF_INJECTED) {
        return CallNextHookEx(NULL, nCode, wParam, lParam);
    }

    if (wParam == WM_KEYDOWN) {
        if (kbd -> vkCode == VK_F8) {
            POINT p;
            GetCursorPos(& p);
            targetWindow = WindowFromPoint(p);
            if (targetWindow) {
                std::string title = GetWindowTitle(targetWindow);
                Tray_SetTargeted(title);
            } else {
                Tray_SetIdle();
            }
            return 1;
        }

        if (kbd -> vkCode == VK_F6) {
            locked = (targetWindow != NULL);
            if (locked) {
                std::string title = GetWindowTitle(targetWindow);
                Tray_SetLocked(title);

                SetForegroundWindow(targetWindow);
                Sleep(1);
                targetHasFocus = 1;
            }
            return 1;
        }

        if (kbd -> vkCode == VK_F7) {
            locked = false;
            targetHasFocus = false;
            if (targetWindow) {
                std::string title = GetWindowTitle(targetWindow);
                Tray_SetTargeted(title);
            } else {
                Tray_SetIdle();
            }
            return 1;
        }
    }

    if (locked && (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN)) {
        if (targetWindow) {
            HWND fg = GetForegroundWindow();

            if (fg != targetWindow) {
                SetForegroundWindow(targetWindow);
                Sleep(1);
                targetHasFocus = 1;
            }

            WinBackend::sendVirtualKey(kbd -> vkCode);
            return 1;
        }
    }

    return CallNextHookEx (NULL, nCode, wParam, lParam);
}

