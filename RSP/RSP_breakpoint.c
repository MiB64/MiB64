/*
 * MiB64 - A Nintendo 64 emulator.
 *
 * Project64 (c) Copyright 2001 Zilmar, Jabo, Smiff, Gent, Witten
 * Projectg64 Legacy (c) Copyright 2010 PJ64LegacyTeam
 * MiB64 (c) Copyright 2024 MiB64Team
 *
 * MiB64 Homepage: www.mib64.net
 *
 * Permission to use, copy, modify and distribute MiB64 in both binary and
 * source form, for non-commercial purposes, is hereby granted without fee,
 * providing that this license information and copyright notice appear with
 * all copies and any derived work.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event shall the authors be held liable for any damages
 * arising from the use of this software.
 *
 * MiB64 is freeware for PERSONAL USE only. Commercial users should
 * seek permission of the copyright holders first. Commercial use includes
 * charging money for MiB64 or software derived from MiB64.
 *
 * The copyright holders request that bug fixes and improvements to the code
 * should be forwarded to them so if they want them.
 *
 */

#include <windows.h>
#include <stdio.h>
#include "../Main.h"
#include "../BreakPoints.h"
#include "../resource.h"
/*#include "rsp.h"*/
#include "RSP_breakpoint.h"
#include "rsp_registers.h"

static HWND BPoint_Win_hDlg, hRSPLocation = NULL;

void __cdecl Add_TextFieldRspBPoint ( void ) {
	char Title[10];

	GetWindowText(hRSPLocation,Title,sizeof(Title));
	if (!AddRSP_BPoint(AsciiToHex(Title),TRUE )) {
		SendMessage(hRSPLocation,EM_SETSEL,(WPARAM)0,(LPARAM)-1);
		SetFocus(hRSPLocation);	
	}
}

int AddRSP_BPoint( DWORD Location, int Confirm ) {
	int count;

	if (NoOfRspBpoints == MaxRspBPoints) { 
		DisplayError("Max amount of Break Points set");
		return FALSE;
	}

	for (count = 0; count < NoOfRspBpoints; count ++) {
		if (RspBPoint[count].Location == Location) {
			DisplayError("You already have this Break Point");
			return FALSE;
		}
	}

	if (Confirm) {
		char Message[150];
		int Response;

		sprintf(Message,"Break when:\n\nRSP's Program Counter = 0x%03X\n\nIs this correct?",
			Location); 
		Response = MessageBox(BPoint_Win_hDlg, Message, "Breakpoint", MB_YESNO | MB_ICONINFORMATION);
		if (Response == IDNO) {
			return FALSE;
		}
	}
	RspBPoint[NoOfRspBpoints].Location = Location;
	NoOfRspBpoints += 1;
	RefreshBreakPoints();
	return TRUE;
}

/*int CheckForRSPBPoint ( DWORD Location ) {
	int count;
	
	for (count = 0; count < NoOfBpoints; count ++){
		if (BPoint[count].Location == Location) {
			return TRUE;
		}
	}
	return FALSE;
}*/

void __cdecl CreateRspBPPanel ( HWND hDlg, RECT rcBox ) {
	UNREFERENCED_PARAMETER(rcBox);

	if (hRSPLocation != NULL) { return; }

	BPoint_Win_hDlg = hDlg;
	
	hRSPLocation = CreateWindowEx(0,"EDIT","", WS_CHILD | WS_BORDER | ES_UPPERCASE | WS_TABSTOP,
		83,90,100,17,hDlg,(HMENU)IDC_LOCATION_EDIT,hInst,NULL);		
	if (hRSPLocation) {
		char Title[20];
		SendMessage(hRSPLocation,WM_SETFONT,(WPARAM)GetStockObject(DEFAULT_GUI_FONT),0);
		SendMessage(hRSPLocation,EM_SETLIMITTEXT,(WPARAM)3,(LPARAM)0);
		sprintf(Title,"%03X",SP_PC_REG);
		SetWindowText(hRSPLocation,Title);
	}
}

/*void HideBPPanel ( void ) {
	ShowWindow(hRSPLocation,FALSE);
}

void PaintBPPanel ( PAINTSTRUCT ps ) {
	TextOut( ps.hdc, 29,60,"Break when the Program Counter equals",37);
	TextOut( ps.hdc, 59,85,"0x",2);
}

void ShowBPPanel ( void ) {
	ShowWindow(hRSPLocation,TRUE);
}

void RefreshBpoints ( HWND hList ) {
	char Message[100];
	int count, location;

	for (count = 0; count < NoOfBpoints; count ++ ) {
		sprintf(Message," at 0x%03X (RSP)", BPoint[count].Location);
		location = SendMessage(hList,LB_ADDSTRING,0,(LPARAM)Message);	
		SendMessage(hList,LB_SETITEMDATA,(WPARAM)location,(LPARAM)BPoint[count].Location);	
	}
}

void RemoveAllBpoint ( void ) {
	NoOfBpoints = 0;
}

void RemoveBpoint ( HWND hList, int index ) {
	DWORD location;
	
	location = SendMessage(hList,LB_GETITEMDATA,(WPARAM)index,0);	
	RemoveRSPBreakPoint(location);
}

void RemoveRSPBreakPoint (DWORD Location) {
	int count, location = -1;
	
	for (count = 0; count < NoOfBpoints; count ++){
		if (BPoint[count].Location == Location) {
			location = count;
			count = NoOfBpoints;
		}
	}
	
	if (location >= 0) {
		for (count = location; count < NoOfBpoints - 1; count ++ ){ 
			BPoint[count].Location = BPoint[count + 1].Location;
		}
		NoOfBpoints -= 1;
		DebugInfo.UpdateBreakPoints();
	}
}*/
