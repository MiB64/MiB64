/*
 * RSP Compiler plug in for Project 64 (A Nintendo 64 emulator).
 *
 * (c) Copyright 2001 jabo (jabo@emulation64.com) and
 * zilmar (zilmar@emulation64.com)
 *
 * pj64 homepage: www.pj64.net
 *
 * Permission to use, copy, modify and distribute Project64 in both binary and
 * source form, for non-commercial purposes, is hereby granted without fee,
 * providing that this license information and copyright notice appear with
 * all copies and any derived work.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event shall the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Project64 is freeware for PERSONAL USE only. Commercial users should
 * seek permission of the copyright holders first. Commercial use includes
 * charging money for Project64 or software derived from Project64.
 *
 * The copyright holders request that bug fixes and improvements to the code
 * should be forwarded to them so if they want them.
 *
 */

#include "../main.h"
#include "../resource.h"

#include <windows.h>
#include <windowsx.h>

BOOL AudioHle = FALSE;
BOOL GraphicsHle = FALSE;
int CPUCore = 0;

static BOOL SettingsLoaded = FALSE;

BOOL GetBooleanCheck(HWND hDlg, DWORD DialogID) {
	return (IsDlgButtonChecked(hDlg, DialogID) == BST_CHECKED) ? TRUE : FALSE;
}

static BOOL CALLBACK ConfigDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	HWND hWndItem;
	DWORD value;

	switch (uMsg) {
	case WM_INITDIALOG:
		if (AudioHle == TRUE) {
			CheckDlgButton(hDlg, IDC_AUDIOHLE, BST_CHECKED);
		}

		if (GraphicsHle == TRUE) {
			CheckDlgButton(hDlg, IDC_GRAPHICSHLE, BST_CHECKED);
		}

		hWndItem = GetDlgItem(hDlg, IDC_COMPILER_SELECT);
		ComboBox_AddString(hWndItem, "Interpreter");
		ComboBox_AddString(hWndItem, "Recompiler");
		ComboBox_SetCurSel(hWndItem, CPUCore);
		break;

	case WM_COMMAND:
		switch (GET_WM_COMMAND_ID(wParam, lParam)) {
		case IDOK:
			hWndItem = GetDlgItem(hDlg, IDC_COMPILER_SELECT);
			value = ComboBox_GetCurSel(hWndItem);
			SetCPU(value);

			AudioHle = GetBooleanCheck(hDlg, IDC_AUDIOHLE);
			GraphicsHle = GetBooleanCheck(hDlg, IDC_GRAPHICSHLE);

			EndDialog(hDlg, TRUE);
			break;
		case IDCANCEL:
			EndDialog(hDlg, TRUE);
			break;
		}
		break;

	default:
		return FALSE;
	}
	return TRUE;
}

void __cdecl rspConfig(HWND hWnd) {
	if (!SettingsLoaded) {
		LoadSettings();
	}

	DialogBox(hInst, "RSPCONFIG", hWnd, ConfigDlgProc);
}
