#pragma once
#include <windows.h>
#include <string>

#define TRAY_CALLBACK (WM_USER + 1)
#define CMD_SETTINGS 1001
#define CMD_EXIT 1002


void InitTrayIcon (HWND hwnd);
void RemoveTrayIcon ( );
void ShowTrayMenu (HWND hwnd);
void UpdateTrayTooltip (const std::string & status);

void Tray_SetIdle ( );
void Tray_SetTargeted (const std::string & windowName);
void Tray_SetLocked (const std::string & windowName);