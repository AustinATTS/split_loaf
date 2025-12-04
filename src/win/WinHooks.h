#pragma once
#include <windows.h>

LRESULT CALLBACK LowLevelKeyboardProc (int code, WPARAM wParam, LPARAM lParam);

HHOOK & WinHooks_GetKeyboardHook ( );
HWND & WinHooks_GetTargetWindow ( );
bool & WinHooks_GetLockedFlag ( );
bool & WinHooks_GetTargetFocusFlag ( );

void SendVirtualKeyToTarget (DWORD vkCode);
