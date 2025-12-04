#pragma once
#include <windows.h>

#define TRAY_CALLBACK (WM_USER + 1)
#define CMD_SETTINGS 1001
#define CMD_EXIT 1002


void InitTrayIcon (HWND hwnd);
void RemoveTrayIcon ( );
void ShowTrayMenu (HWND hwnd);
