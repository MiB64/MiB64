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
#include "RSP_OpCode.h"
#include "rsp_Cpu.h"
#include "rsp_registers.h"
#include "RSP Command.h"
#include "rsp_config.h"
#include "rsp_memory.h"
#include "RSP_breakpoint.h"
#include "../Types.h"
#include "../BreakPoints.h"
#include "../r4300i Commands.h"
#include "../r4300i Registers.h"
#include "../r4300i Memory.h"
#include "../Main.h"

#define RSP_MaxCommandLines		30

#define RSP_Status_PC            1
#define RSP_Status_BP            2

#define IDC_LIST					1000
#define IDC_ADDRESS					1001
#define IDC_FUNCTION_COMBO			1002
#define IDC_GO_BUTTON				1003
#define IDC_BREAK_BUTTON			1004
#define IDC_STEP_BUTTON				1005
#define IDC_SKIP_BUTTON				1006
#define IDC_BP_BUTTON				1007
#define IDC_R4300I_REGISTERS_BUTTON	1008
#define IDC_R4300I_DEBUGGER_BUTTON	1009
#define IDC_RSP_REGISTERS_BUTTON	1010
#define IDC_MEMORY_BUTTON			1011
#define IDC_SCRL_BAR				1012

static void Paint_RSP_Commands (HWND hDlg);
static void RSP_Commands_Setup ( HWND hDlg );
static LRESULT CALLBACK RSP_Commands_Proc (HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

typedef struct {
	DWORD Location;
	DWORD opcode;
	char  String[150];
    DWORD status;
} RSPCOMMANDLINE;

RSPCOMMANDLINE RSPCommandLine[30];
static HWND RSPCommandshWnd, hList, hAddress, hFunctionlist, hGoButton, hBreakButton,
	hStepButton, hSkipButton, hBPButton, hR4300iRegisters, hR4300iDebugger, hRSPRegisters,
	hMemory, hScrlBar;
BOOL InRSPCommandsWindow = FALSE;
static char CommandName[100];
DWORD Stepping_RspCommands = FALSE;
DWORD WaitingForRspStep = FALSE;


static void Create_RSP_Commands_Window ( int Child ) {
	DWORD ThreadID;

	if ( Child ) {
		InRSPCommandsWindow = TRUE;
		DialogBox( hInst, "RSPCOMMAND", NULL,(DLGPROC)RSP_Commands_Proc );

		InRSPCommandsWindow = FALSE;
		memset(RSPCommandLine,0,sizeof(RSPCommandLine));
		SetRSPCommandToRunning();
	} else {
		if (!InRSPCommandsWindow) {
			Stepping_RspCommands = TRUE;
			CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)Create_RSP_Commands_Window,
				(LPVOID)TRUE,0, &ThreadID);	
		} else {
			SetForegroundWindow(RSPCommandshWnd);
		}	
	}
}

void Disable_RSP_Commands_Window ( void ) {
	/*SCROLLINFO si;

	if (!InRSPCommandsWindow) { return; }
	EnableWindow(hList,            FALSE);
	EnableWindow(hAddress,         FALSE);
	EnableWindow(hScrlBar,         FALSE);
	EnableWindow(hGoButton,        FALSE);
	EnableWindow(hStepButton,      FALSE);
	EnableWindow(hSkipButton,      FALSE);
	EnableWindow(hR4300iRegisters, FALSE);
	EnableWindow(hRSPRegisters,    FALSE);
	EnableWindow(hR4300iDebugger,     FALSE);
	EnableWindow(hMemory,          FALSE);
	
	si.cbSize = sizeof(si);
	si.fMask  = SIF_RANGE | SIF_POS | SIF_PAGE;
	si.nMin   = 0;
	si.nMax   = 0;
	si.nPos   = 1;
	si.nPage  = 1;
	SetScrollInfo(hScrlBar,SB_CTL,&si,TRUE);*/
	LogMessage("TODO: Disable_RSP_Commands_Window");
}

int DisplayRSPCommand (DWORD location, int InsertPos) {
	DWORD OpCode, LinesUsed = 1, status;
	BOOL Redraw = FALSE;	

	RSP_LW_IMEM(location, &OpCode);
	
	status = 0;
	if (location == SP_PC_REG) {status = RSP_Status_PC; }
	if (CheckForRSPBPoint(location)) { status |= RSP_Status_BP; }
	if (RSPCommandLine[InsertPos].opcode != OpCode) { Redraw = TRUE; }
	if (RSPCommandLine[InsertPos].Location != location) { Redraw = TRUE; }
	if (RSPCommandLine[InsertPos].status != status) { Redraw = TRUE; }
	if (Redraw) {
		RSPCommandLine[InsertPos].Location = location;
		RSPCommandLine[InsertPos].status = status;
		RSPCommandLine[InsertPos].opcode = OpCode;
		sprintf(RSPCommandLine[InsertPos].String," 0x%03X\t%s",location, 
			RSPOpcodeName ( OpCode, location ));
		if ( SendMessage(hList,LB_GETCOUNT,0,0) <= InsertPos) {
			SendMessage(hList,LB_INSERTSTRING,(WPARAM)InsertPos, (LPARAM)location); 
		} else {
			RECT ItemRC;
			SendMessage(hList,LB_GETITEMRECT,(WPARAM)InsertPos, (LPARAM)&ItemRC); 
			RedrawWindow(hList,&ItemRC,NULL, RDW_INVALIDATE );
		}
	}
	return LinesUsed;
}

/*void DumpRSPCode (void) {
	char string[100], LogFileName[255], *p ;
	DWORD location, OpCode, dwWritten;
	HANDLE hLogFile = NULL;

	strcpy(LogFileName,GetCommandLine() + 1);
	
	if (strchr(LogFileName,'\"')) {
		p = strchr(LogFileName,'\"');
		*p = '\0';
	}
	
	if (strchr(LogFileName,'\\')) {
		p = LogFileName;
		while (strchr(p,'\\')) {
			p = strchr(p,'\\');
			p++;
		}
		p -= 1;
		*p = '\0';
	}

	strcat(LogFileName,"\\RSP code.txt");

	hLogFile = CreateFile(LogFileName,GENERIC_WRITE, FILE_SHARE_READ,NULL,CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);
	SetFilePointer(hLogFile,0,NULL,FILE_BEGIN);

	for (location = 0; location < 0x1000; location += 4) {
		RSP_LW_IMEM(location, &OpCode);
		sprintf(string," 0x%03X\t%s\r\n",location, RSPOpcodeName ( OpCode, location ));
		WriteFile( hLogFile,string,strlen(string),&dwWritten,NULL );
	}
	CloseHandle(hLogFile);
}

void DumpRSPData (void) {
	char string[100], LogFileName[255], *p ;
	DWORD location, value, dwWritten;
	HANDLE hLogFile = NULL;

	strcpy(LogFileName,GetCommandLine() + 1);
	
	if (strchr(LogFileName,'\"')) {
		p = strchr(LogFileName,'\"');
		*p = '\0';
	}
	
	if (strchr(LogFileName,'\\')) {
		p = LogFileName;
		while (strchr(p,'\\')) {
			p = strchr(p,'\\');
			p++;
		}
		p -= 1;
		*p = '\0';
	}

	strcat(LogFileName,"\\RSP data.txt");

	hLogFile = CreateFile(LogFileName,GENERIC_WRITE, FILE_SHARE_READ,NULL,CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);
	SetFilePointer(hLogFile,0,NULL,FILE_BEGIN);

	for (location = 0; location < 0x1000; location += 4) {
		RSP_LW_DMEM(location, &value);
		sprintf(string," 0x%03X\t0x%08X\r\n", location, value);
		WriteFile( hLogFile,string,strlen(string),&dwWritten,NULL );
	}
	CloseHandle(hLogFile);
}*/

void DrawRSPCommand ( LPARAM lParam ) {	
	char Command[150], Offset[30], Instruction[30], Arguments[40];
	LPDRAWITEMSTRUCT ditem;
	COLORREF oldColor;
	int ResetColor;
	HBRUSH hBrush;
	RECT TextRect;
	char *p1, *p2;

	ditem  = (LPDRAWITEMSTRUCT)lParam;
	strcpy(Command, RSPCommandLine[ditem->itemID].String);
	
	if (strchr(Command,'\t')) {
		p1 = strchr(Command,'\t');
		sprintf(Offset,"%.*s",p1 - Command, Command);
		p1++;
		if (strchr(p1,'\t')) {
			p2 = strchr(p1,'\t');
			sprintf(Instruction,"%.*s",p2 - p1, p1);
			sprintf(Arguments,"%s",p2 + 1);
		} else {
			sprintf(Instruction,"%s",p1);
			sprintf(Arguments,"\0");
		}
		sprintf(Command,"\0");
	} else {
		sprintf(Offset,"\0");
		sprintf(Instruction,"\0");
		sprintf(Arguments,"\0");
	}
		
	if (SP_PC_REG == RSPCommandLine[ditem->itemID].Location) {
		ResetColor = TRUE;
		hBrush     = (HBRUSH)(COLOR_HIGHLIGHT + 1);
		oldColor   = SetTextColor(ditem->hDC,RGB(255,255,255));
	} else {
		ResetColor = FALSE;
		hBrush     = (HBRUSH)GetStockObject(WHITE_BRUSH);
		oldColor   = GetTextColor(ditem->hDC);
	}

	if (CheckForRSPBPoint( RSPCommandLine[ditem->itemID].Location )) {
		ResetColor = TRUE;
		if (SP_PC_REG == RSPCommandLine[ditem->itemID].Location) {
			SetTextColor(ditem->hDC,RGB(255,0,0));
		} else {
			oldColor = SetTextColor(ditem->hDC,RGB(255,0,0));
		}
	}

	FillRect( ditem->hDC, &ditem->rcItem,hBrush);	
	SetBkMode( ditem->hDC, TRANSPARENT );

	if (strlen (Command) == 0 ) {
		SetRect(&TextRect,ditem->rcItem.left,ditem->rcItem.top, ditem->rcItem.left + 83,
			ditem->rcItem.bottom);	
		DrawText(ditem->hDC,Offset,strlen(Offset), &TextRect,DT_SINGLELINE | DT_VCENTER);
		
		SetRect(&TextRect,ditem->rcItem.left + 83,ditem->rcItem.top, ditem->rcItem.left + 165,
			ditem->rcItem.bottom);	
		DrawText(ditem->hDC,Instruction,strlen(Instruction), &TextRect,DT_SINGLELINE | DT_VCENTER);

		SetRect(&TextRect,ditem->rcItem.left + 165,ditem->rcItem.top, ditem->rcItem.right,
			ditem->rcItem.bottom);	
		DrawText(ditem->hDC,Arguments,strlen(Arguments), &TextRect,DT_SINGLELINE | DT_VCENTER);
	} else {
		DrawText(ditem->hDC,Command,strlen(Command), &ditem->rcItem,DT_SINGLELINE | DT_VCENTER);
	}

	if (ResetColor == TRUE) {
		SetTextColor( ditem->hDC, oldColor );
	}
}


void Enable_RSP_Commands_Window ( void ) {
	SCROLLINFO si;

	if (!InRSPCommandsWindow) { return; }
	EnableWindow(hList,            TRUE);
	EnableWindow(hAddress,         TRUE);
	EnableWindow(hScrlBar,         TRUE);
	EnableWindow(hGoButton,        TRUE);
	EnableWindow(hStepButton,      TRUE);
	EnableWindow(hSkipButton,      FALSE);
	EnableWindow(hR4300iRegisters, TRUE);
	EnableWindow(hRSPRegisters,    TRUE);
	EnableWindow(hR4300iDebugger,  TRUE);
	EnableWindow(hMemory,          TRUE);
	SendMessage(hBPButton, BM_SETSTYLE, BS_PUSHBUTTON,TRUE);
	SendMessage(hStepButton, BM_SETSTYLE, BS_DEFPUSHBUTTON,TRUE);
	SendMessage(RSPCommandshWnd, DM_SETDEFID,IDC_STEP_BUTTON,0);
	
	if (Stepping_RspCommands) {
		si.cbSize = sizeof(si);
		si.fMask  = SIF_RANGE | SIF_POS | SIF_PAGE;
		si.nMin   = 0;
		si.nMax   = (0x1000 >> 2) -1;
		si.nPos   = (SP_PC_REG >> 2);
		si.nPage  = 30;
		SetScrollInfo(hScrlBar,SB_CTL,&si,TRUE);		
	
		SetRSPCommandViewto( SP_PC_REG );
		SetForegroundWindow(RSPCommandshWnd);
	}
}

void __cdecl Enter_RSP_Commands_Window ( void ) {
    Create_RSP_Commands_Window ( FALSE );
}

void Paint_RSP_Commands (HWND hDlg) {
	PAINTSTRUCT ps;
	RECT rcBox;
	HFONT hOldFont;
	int OldBkMode;

	BeginPaint( hDlg, &ps );
		
	rcBox.left   = 5;   rcBox.top    = 5;
	rcBox.right  = 343; rcBox.bottom = 463;
	DrawEdge( ps.hdc, &rcBox, EDGE_RAISED, BF_RECT );
		
	rcBox.left   = 8;   rcBox.top    = 8;
	rcBox.right  = 340; rcBox.bottom = 460;
	DrawEdge( ps.hdc, &rcBox, EDGE_ETCHED, BF_RECT );

	rcBox.left   = 347; rcBox.top    = 7;
	rcBox.right  = 446; rcBox.bottom = 42;
	DrawEdge( ps.hdc, &rcBox, EDGE_ETCHED, BF_RECT );
		
	rcBox.left   = 352; rcBox.top    = 2;
	rcBox.right  = 400; rcBox.bottom = 15;
	FillRect( ps.hdc, &rcBox,(HBRUSH)COLOR_WINDOW);

	rcBox.left   = 14; rcBox.top    = 14;
	rcBox.right  = 88; rcBox.bottom = 32;
	DrawEdge( ps.hdc, &rcBox, EDGE_ETCHED , BF_RECT );

	rcBox.left   = 86; rcBox.top    = 14;
	rcBox.right  = 173; rcBox.bottom = 32;
	DrawEdge( ps.hdc, &rcBox, EDGE_ETCHED , BF_RECT );

	rcBox.left   = 171; rcBox.top    = 14;
	rcBox.right  = 320; rcBox.bottom = 32;
	DrawEdge( ps.hdc, &rcBox, EDGE_ETCHED , BF_RECT );

	hOldFont = (HFONT)SelectObject( ps.hdc,GetStockObject(DEFAULT_GUI_FONT ) );
	OldBkMode = SetBkMode( ps.hdc, TRANSPARENT );
		
	TextOut( ps.hdc, 23,16,"Offset",6);
	TextOut( ps.hdc, 97,16,"Instruction",11);
	TextOut( ps.hdc, 180,16,"Arguments",9);
	TextOut( ps.hdc, 354,2," Address ",9);
	TextOut( ps.hdc, 358,19,"0x",2);
	
	SelectObject( ps.hdc,hOldFont );
	SetBkMode( ps.hdc, OldBkMode );
		
	EndPaint( hDlg, &ps );
}

void RefreshRSPCommands ( void ) {
	DWORD location, LinesUsed;
	char AsciiAddress[20];
	int count;

	if (InRSPCommandsWindow == FALSE) { return; }

	GetWindowText(hAddress,AsciiAddress,sizeof(AsciiAddress));
	location = AsciiToHex(AsciiAddress) & ~3;

	if (location > 0xF88) { location = 0xF88; }
	for (count = 0 ; count < RSP_MaxCommandLines; count += LinesUsed ){
		LinesUsed = DisplayRSPCommand ( location, count );
		location += 4;
	}
}

LRESULT CALLBACK RSP_Commands_Proc (HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
	case WM_INITDIALOG:
		RSPCommandshWnd = hDlg;
		RSP_Commands_Setup( hDlg );
		break;
	case WM_MOVE:
		StoreCurrentWinPos("RSP Commands",hDlg);
		break;
	case WM_DRAWITEM:
		if (wParam == IDC_LIST) {
			DrawRSPCommand (lParam);
		}
		break;
	case WM_PAINT:
		Paint_RSP_Commands( hDlg );
		RedrawWindow(hScrlBar,NULL,NULL, RDW_INVALIDATE |RDW_ERASE);
		return TRUE;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDC_LIST:
			if (HIWORD(wParam) == LBN_DBLCLK ) {
				DWORD Location, Selected;
				Selected = SendMessage(hList,LB_GETCURSEL,(WPARAM)0, (LPARAM)0); 
				Location = RSPCommandLine[Selected].Location; 
				if (Location != (DWORD)-1) {
					if (CheckForRSPBPoint(Location)) {
						RemoveRSPBreakPoint(Location);
					} else {
						AddRSP_BPoint(Location, FALSE);
					}
					RefreshRSPCommands();
				}
			}
			break;
		case IDC_ADDRESS:
			if (HIWORD(wParam) == EN_CHANGE ) {
				RefreshRSPCommands();
			}
			break;
		case IDC_GO_BUTTON:
			SetRSPCommandToRunning();
			break;
		case IDC_BREAK_BUTTON:	
			SetRSPCommandToStepping();
			break;
		case IDC_STEP_BUTTON:			
			WaitingForRspStep = FALSE;
			break;
		/*case IDC_SKIP_BUTTON:
			SkipNextRSPOpCode = TRUE;
			WaitingFor_RSPStep   = FALSE;
			break;*/
		case IDC_BP_BUTTON:	
			Enter_BPoint_Window(); 
			break;
		case IDC_RSP_REGISTERS_BUTTON:
			Enter_RSP_Register_Window();
			break;
		case IDC_R4300I_DEBUGGER_BUTTON: 
			Enter_R4300i_Commands_Window(); 
			break;
		case IDC_R4300I_REGISTERS_BUTTON:
			Enter_R4300i_Register_Window(); 
			break;
		case IDC_MEMORY_BUTTON:
			Enter_Memory_Window(); 
			break;
		case IDCANCEL:			
			EndDialog( hDlg, IDCANCEL );
			break;
		}
		break;
	case WM_VSCROLL:
		if ((HWND)lParam == hScrlBar) {
			DWORD location;
			char Value[20];
			SCROLLINFO si;
		
			GetWindowText(hAddress,Value,sizeof(Value));
			location = AsciiToHex(Value) & ~3;
			
			switch (LOWORD(wParam))  {			
			case SB_THUMBTRACK:
				sprintf(Value,"%03X",((short int)HIWORD(wParam) << 2 ));
				SetWindowText(hAddress,Value);
				si.cbSize = sizeof(si);			
				si.fMask  = SIF_POS;
				si.nPos   = (short int)HIWORD(wParam);
				SetScrollInfo(hScrlBar,SB_CTL,&si,TRUE);
				break;
			case SB_LINEDOWN:
				if (location < 0xF88) {
					sprintf(Value,"%03X",location + 0x4);
					SetWindowText(hAddress,Value);
					si.cbSize = sizeof(si);			
					si.fMask  = SIF_POS;
					si.nPos   = ((location + 0x4) >> 2);
					SetScrollInfo(hScrlBar,SB_CTL,&si,TRUE);
				} else {
					sprintf(Value,"%03X",0xF88);
					SetWindowText(hAddress,Value);
					si.cbSize = sizeof(si);			
					si.fMask  = SIF_POS;
					si.nPos   = (0xFFC >> 2);
					SetScrollInfo(hScrlBar,SB_CTL,&si,TRUE);
				}
				break;
			case SB_LINEUP:
				if (location > 0x4 ) {
					sprintf(Value,"%03X",location - 0x4);
					SetWindowText(hAddress,Value);
					si.cbSize = sizeof(si);			
					si.fMask  = SIF_POS;
					si.nPos   = ((location - 0x4) >> 2);
					SetScrollInfo(hScrlBar,SB_CTL,&si,TRUE);
				} else {
					sprintf(Value,"%03X",0);
					SetWindowText(hAddress,Value);
					si.cbSize = sizeof(si);			
					si.fMask  = SIF_POS;
					si.nPos   = 0;
					SetScrollInfo(hScrlBar,SB_CTL,&si,TRUE);
				}
				break;
			case SB_PAGEDOWN:				
				if ((location + 0x74)< 0xF88) {
					sprintf(Value,"%03X",location + 0x74);
					SetWindowText(hAddress,Value);
					si.cbSize = sizeof(si);			
					si.fMask  = SIF_POS;
					si.nPos   = ((location + 0x74) >> 2);
					SetScrollInfo(hScrlBar,SB_CTL,&si,TRUE);
				} else {
					sprintf(Value,"%03X",0xF88);
					SetWindowText(hAddress,Value);
					si.cbSize = sizeof(si);			
					si.fMask  = SIF_POS;
					si.nPos   = (0xF8F >> 2);
					SetScrollInfo(hScrlBar,SB_CTL,&si,TRUE);
				}
				break;			
			case SB_PAGEUP:
				if ((location - 0x74) > 0x74 ) {
					sprintf(Value,"%03X",location - 0x74);
					SetWindowText(hAddress,Value);
					si.cbSize = sizeof(si);			
					si.fMask  = SIF_POS;
					si.nPos   = ((location - 0x74) >> 2);
					SetScrollInfo(hScrlBar,SB_CTL,&si,TRUE);
				} else {
					sprintf(Value,"%03X",0);
					SetWindowText(hAddress,Value);
					si.cbSize = sizeof(si);			
					si.fMask  = SIF_POS;
					si.nPos   = 0;
					SetScrollInfo(hScrlBar,SB_CTL,&si,TRUE);
				}
				break;
			}
		}
		break;
	default:
		return FALSE;
	}
	return TRUE;
}

void RSP_Commands_Setup ( HWND hDlg ) {
#define WindowWidth  477
#define WindowHeight 514
	char Location[10];
	DWORD X, Y, WndPos;
	
	hList = CreateWindowEx(WS_EX_STATICEDGE, "LISTBOX","", WS_CHILD | WS_VISIBLE | 
		LBS_OWNERDRAWFIXED | LBS_NOTIFY,14,30,303,445, hDlg, 
		(HMENU)IDC_LIST, hInst,NULL );
	if ( hList) {
		SendMessage(hList,WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT),0);
		SendMessage(hList,LB_SETITEMHEIGHT, (WPARAM)0,(LPARAM)MAKELPARAM(14, 0));
	}

	sprintf(Location,"%03X",SP_PC_REG);
	hAddress = CreateWindowEx(0,"EDIT",Location, WS_CHILD | ES_UPPERCASE | WS_VISIBLE | 
		WS_BORDER | WS_TABSTOP,375,17,36,18, hDlg,(HMENU)IDC_ADDRESS,hInst, NULL );
	if (hAddress) {
		SendMessage(hAddress,WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT),0);
		SendMessage(hAddress,EM_SETLIMITTEXT, (WPARAM)3,(LPARAM)0);
	} 

	hFunctionlist = CreateWindowEx(0,"COMBOBOX","", WS_CHILD | WS_VSCROLL |
		CBS_DROPDOWNLIST | CBS_SORT | WS_TABSTOP,352,56,89,150,hDlg,
		(HMENU)IDC_FUNCTION_COMBO,hInst,NULL);		
	if (hFunctionlist) {
		SendMessage(hFunctionlist,WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT),0);
	} 

	hGoButton = CreateWindowEx(WS_EX_STATICEDGE, "BUTTON","&Go", WS_CHILD | 
		BS_DEFPUSHBUTTON | WS_VISIBLE | WS_TABSTOP, 347,56,100,24, hDlg,(HMENU)IDC_GO_BUTTON,
		hInst,NULL );
	if (hGoButton) {
		SendMessage(hGoButton,WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT),0);
	} 
	
	hBreakButton = CreateWindowEx(WS_EX_STATICEDGE, "BUTTON","&Break", WS_DISABLED | 
		WS_CHILD | BS_PUSHBUTTON | WS_VISIBLE | WS_TABSTOP | BS_TEXT, 347,85,100,24,hDlg,
		(HMENU)IDC_BREAK_BUTTON,hInst,NULL );
	if (hBreakButton) {
		SendMessage(hBreakButton,WM_SETFONT,(WPARAM)GetStockObject(DEFAULT_GUI_FONT),0);
	}

	hStepButton = CreateWindowEx(WS_EX_STATICEDGE, "BUTTON","&Step", WS_CHILD | 
		BS_PUSHBUTTON | WS_VISIBLE | WS_TABSTOP | BS_TEXT, 347,114,100,24,hDlg,
		(HMENU)IDC_STEP_BUTTON,hInst,NULL );
	if (hStepButton) {
		SendMessage(hStepButton,WM_SETFONT,(WPARAM)GetStockObject(DEFAULT_GUI_FONT),0);
	}

	hSkipButton = CreateWindowEx(WS_EX_STATICEDGE, "BUTTON","&Skip", WS_CHILD | 
		BS_PUSHBUTTON | WS_VISIBLE | WS_TABSTOP | BS_TEXT, 347,143,100,24,hDlg,
		(HMENU)IDC_SKIP_BUTTON,hInst,NULL );
	if (hSkipButton) {
		SendMessage(hSkipButton,WM_SETFONT,(WPARAM)GetStockObject(DEFAULT_GUI_FONT),0);
	}

	WndPos = 324;

	hBPButton = CreateWindowEx(WS_EX_STATICEDGE, "BUTTON","&Break Points", WS_CHILD | 
		BS_PUSHBUTTON | WS_VISIBLE | WS_TABSTOP | BS_TEXT, 347,WndPos,100,24,hDlg,
		(HMENU)IDC_BP_BUTTON,hInst,NULL );
	if (hBPButton) {
		SendMessage(hBPButton,WM_SETFONT,(WPARAM)GetStockObject(DEFAULT_GUI_FONT),0);
	}

	WndPos += 29;
	hRSPRegisters = CreateWindowEx(WS_EX_STATICEDGE,"BUTTON", "RSP &Registers...",
		WS_CHILD | BS_PUSHBUTTON | WS_VISIBLE | WS_TABSTOP | BS_TEXT, 347,WndPos,100,24,hDlg,
		(HMENU)IDC_RSP_REGISTERS_BUTTON,hInst,NULL );
	if (hRSPRegisters) {
		SendMessage(hRSPRegisters,WM_SETFONT,(WPARAM)GetStockObject(DEFAULT_GUI_FONT),0);
	} 

	WndPos += 29;
	hR4300iDebugger = CreateWindowEx(WS_EX_STATICEDGE,"BUTTON", "R4300i &Debugger...", 
		WS_CHILD | BS_PUSHBUTTON | WS_VISIBLE | WS_TABSTOP | BS_TEXT, 347,WndPos,100,24,hDlg,
		(HMENU)IDC_R4300I_DEBUGGER_BUTTON,hInst,NULL );
	if (hR4300iDebugger) {
		SendMessage(hR4300iDebugger,WM_SETFONT,(WPARAM)GetStockObject(DEFAULT_GUI_FONT),0);
	}

	WndPos += 29;
	hR4300iRegisters = CreateWindowEx(WS_EX_STATICEDGE,"BUTTON","R4300i R&egisters...",
		WS_CHILD | BS_PUSHBUTTON | WS_VISIBLE | WS_TABSTOP | BS_TEXT, 347,WndPos,100,24,hDlg,
		(HMENU)IDC_R4300I_REGISTERS_BUTTON,hInst,NULL );
	if (hR4300iRegisters) {
		SendMessage(hR4300iRegisters,WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT),0);
	}

	WndPos += 29;
	hMemory = CreateWindowEx(WS_EX_STATICEDGE,"BUTTON", "&Memory...", WS_CHILD | 
		BS_PUSHBUTTON | WS_VISIBLE | WS_TABSTOP | BS_TEXT, 347,WndPos,100,24,hDlg,
		(HMENU)IDC_MEMORY_BUTTON,hInst,NULL );
	if (hMemory) {
		SendMessage(hMemory,WM_SETFONT,(WPARAM)GetStockObject(DEFAULT_GUI_FONT),0);
	}
	
	hScrlBar = CreateWindowEx(WS_EX_STATICEDGE, "SCROLLBAR","", WS_CHILD | WS_VISIBLE | 
		WS_TABSTOP | SBS_VERT, 318,14,18,439, hDlg, (HMENU)IDC_SCRL_BAR, hInst, NULL );

	if ( RSP_Running ) {
		Enable_RSP_Commands_Window();
	} else {
		Disable_RSP_Commands_Window();
	}
	
	if ( !GetStoredWinPos("RSP Commands", &X, &Y ) ) {
		X = (GetSystemMetrics( SM_CXSCREEN ) - WindowWidth) / 2;
		Y = (GetSystemMetrics( SM_CYSCREEN ) - WindowHeight) / 2;
	}
	SetWindowText(hDlg,"RSP Commands");

	SetWindowPos(hDlg,NULL,X,Y,WindowWidth,WindowHeight, SWP_NOZORDER | 
		SWP_SHOWWINDOW);
}

char * RSPSpecialName ( DWORD OpCode ) {
	OPCODE command;
	command.OP.Hex = OpCode;
		
	switch (command.OP.R.funct) {
	case RSP_SPECIAL_SLL:
		if (command.OP.R.rd != 0) {
			sprintf(CommandName,"SLL\t%s, %s, 0x%X",RspGPR_Name(command.OP.R.rd),
				RspGPR_Name(command.OP.R.rt), command.OP.R.sa);
		} else {
			sprintf(CommandName,"NOP");
		}
		break;
	case RSP_SPECIAL_SRL:
		sprintf(CommandName,"SRL\t%s, %s, 0x%X",RspGPR_Name(command.OP.R.rd),
			RspGPR_Name(command.OP.R.rt), command.OP.R.sa);
		break;
	case RSP_SPECIAL_SRA:
		sprintf(CommandName,"SRA\t%s, %s, 0x%X",RspGPR_Name(command.OP.R.rd),
			RspGPR_Name(command.OP.R.rt), command.OP.R.sa);
		break;
	case RSP_SPECIAL_SLLV:
		sprintf(CommandName,"SLLV\t%s, %s, %s",RspGPR_Name(command.OP.R.rd),
			RspGPR_Name(command.OP.R.rt), RspGPR_Name(command.OP.R.rs));
		break;
	case RSP_SPECIAL_SRLV:
		sprintf(CommandName,"SRLV\t%s, %s, %s",RspGPR_Name(command.OP.R.rd),
			RspGPR_Name(command.OP.R.rt), RspGPR_Name(command.OP.R.rs));
		break;
	case RSP_SPECIAL_SRAV:
		sprintf(CommandName,"SRAV\t%s, %s, %s",RspGPR_Name(command.OP.R.rd),
			RspGPR_Name(command.OP.R.rt), RspGPR_Name(command.OP.R.rs));
		break;
	case RSP_SPECIAL_JR:
		sprintf(CommandName,"JR\t%s",RspGPR_Name(command.OP.R.rs));
		break;
	case RSP_SPECIAL_JALR:
		sprintf(CommandName,"JALR\t%s, %s",RspGPR_Name(command.OP.R.rd),RspGPR_Name(command.OP.R.rs));
		break;
	case RSP_SPECIAL_BREAK:
		sprintf(CommandName,"BREAK");
		break;
	case RSP_SPECIAL_ADD:
		sprintf(CommandName,"ADD\t%s, %s, %s",RspGPR_Name(command.OP.R.rd),RspGPR_Name(command.OP.R.rs),
			RspGPR_Name(command.OP.R.rt));
		break;
	case RSP_SPECIAL_ADDU:
		sprintf(CommandName,"ADDU\t%s, %s, %s",RspGPR_Name(command.OP.R.rd),RspGPR_Name(command.OP.R.rs),
			RspGPR_Name(command.OP.R.rt));
		break;
	case RSP_SPECIAL_SUB:
		sprintf(CommandName,"SUB\t%s, %s, %s",RspGPR_Name(command.OP.R.rd),RspGPR_Name(command.OP.R.rs),
			RspGPR_Name(command.OP.R.rt));
		break;
	case RSP_SPECIAL_SUBU:
		sprintf(CommandName,"SUBU\t%s, %s, %s",RspGPR_Name(command.OP.R.rd),RspGPR_Name(command.OP.R.rs),
			RspGPR_Name(command.OP.R.rt));
		break;
	case RSP_SPECIAL_AND:
		sprintf(CommandName,"AND\t%s, %s, %s",RspGPR_Name(command.OP.R.rd),RspGPR_Name(command.OP.R.rs),
			RspGPR_Name(command.OP.R.rt));
		break;
	case RSP_SPECIAL_OR:
		sprintf(CommandName,"OR\t%s, %s, %s",RspGPR_Name(command.OP.R.rd),RspGPR_Name(command.OP.R.rs),
			RspGPR_Name(command.OP.R.rt));
		break;
	case RSP_SPECIAL_XOR:
		sprintf(CommandName,"XOR\t%s, %s, %s",RspGPR_Name(command.OP.R.rd),RspGPR_Name(command.OP.R.rs),
			RspGPR_Name(command.OP.R.rt));
		break;
	case RSP_SPECIAL_NOR:
		sprintf(CommandName,"NOR\t%s, %s, %s",RspGPR_Name(command.OP.R.rd),RspGPR_Name(command.OP.R.rs),
			RspGPR_Name(command.OP.R.rt));
		break;
	case RSP_SPECIAL_SLT:
		sprintf(CommandName,"SLT\t%s, %s, %s",RspGPR_Name(command.OP.R.rd),RspGPR_Name(command.OP.R.rs),
			RspGPR_Name(command.OP.R.rt));
		break;
	case RSP_SPECIAL_SLTU:
		sprintf(CommandName,"SLTU\t%s, %s, %s",RspGPR_Name(command.OP.R.rd),RspGPR_Name(command.OP.R.rs),
			RspGPR_Name(command.OP.R.rt));
		break;
	default:
		sprintf(CommandName,"RSP: Unknown\t%02X %02X %02X %02X",
			command.OP.Ascii[3],command.OP.Ascii[2],command.OP.Ascii[1],command.OP.Ascii[0]);
	}
	return CommandName;
}

char * RSPRegimmName ( DWORD OpCode, DWORD PC ) {
	OPCODE command;
	command.OP.Hex = OpCode;
		
	switch (command.OP.B.rt) {
	case RSP_REGIMM_BLTZ:
		sprintf(CommandName,"BLTZ\t%s, 0x%03X",RspGPR_Name(command.OP.B.rs),
			(PC + ((short)command.OP.B.offset << 2) + 4) & 0xFFC);
		break;
	case RSP_REGIMM_BGEZ:
		sprintf(CommandName,"BGEZ\t%s, 0x%03X",RspGPR_Name(command.OP.B.rs),
			(PC + ((short)command.OP.B.offset << 2) + 4) & 0xFFC);
		break;
	case RSP_REGIMM_BLTZAL:
		sprintf(CommandName,"BLTZAL\t%s, 0x%03X",RspGPR_Name(command.OP.B.rs),
			(PC + ((short)command.OP.B.offset << 2) + 4) & 0xFFC);
		break;
	case RSP_REGIMM_BGEZAL:
		if (command.OP.B.rs == 0) {
			sprintf(CommandName,"BAL\t0x%03X",(PC + ((short)command.OP.B.offset << 2) + 4) & 0xFFC);
		} else {
			sprintf(CommandName,"BGEZAL\t%s, 0x%03X",RspGPR_Name(command.OP.B.rs),
				(PC + ((short)command.OP.B.offset << 2) + 4) & 0xFFC);
		}	
		break;
	default:
		sprintf(CommandName,"RSP: Unknown\t%02X %02X %02X %02X",
			command.OP.Ascii[3],command.OP.Ascii[2],command.OP.Ascii[1],command.OP.Ascii[0]);
	}
	return CommandName;
}

char * RSPCop0Name ( DWORD OpCode ) {
	OPCODE command;
	command.OP.Hex = OpCode;
	switch (command.OP.R.rs) {
	case RSP_COP0_MF:
		sprintf(CommandName,"MFC0\t%s, %s",RspGPR_Name(command.OP.R.rt),RspCOP0_Name(command.OP.R.rd));
		break;
	case RSP_COP0_MT:
		sprintf(CommandName,"MTC0\t%s, %s",RspGPR_Name(command.OP.R.rt),RspCOP0_Name(command.OP.R.rd));
		break;
	default:
		sprintf(CommandName,"RSP: Unknown\t%02X %02X %02X %02X",
			command.OP.Ascii[3],command.OP.Ascii[2],command.OP.Ascii[1],command.OP.Ascii[0]);
	}
	return CommandName;
}

char * RSPCop2Name ( DWORD OpCode ) {
	OPCODE command;
	command.OP.Hex = OpCode;
		
	if ( ( command.OP.R.rs & 0x10 ) == 0 ) {
		switch (command.OP.MV.moveop) {
		case RSP_COP2_MF:
			sprintf(CommandName,"MFC2\t%s, $v%d [%d]",RspGPR_Name(command.OP.MV.rt),
				command.OP.MV.vs, command.OP.MV.element >> 1);
			break;
		case RSP_COP2_CF:		
			sprintf(CommandName,"CFC2\t%s, %d",RspGPR_Name(command.OP.MV.rt),
				command.OP.MV.vs % 4);
			break;
		case RSP_COP2_MT:
			sprintf(CommandName,"MTC2\t%s, $v%d [%d]",RspGPR_Name(command.OP.MV.rt),
				command.OP.MV.vs, command.OP.MV.element >> 1);
			break;
		case RSP_COP2_CT:		
			sprintf(CommandName,"CTC2\t%s, %d",RspGPR_Name(command.OP.MV.rt),
				command.OP.MV.vs % 4);
			break;
		default:
			sprintf(CommandName,"RSP: Unknown\t%02X %02X %02X %02X",
				command.OP.Ascii[3],command.OP.Ascii[2],command.OP.Ascii[1],command.OP.Ascii[0]);
		}
	} else {
		switch (command.OP.V.funct) {
		case RSP_VECTOR_VMULF:
			sprintf(CommandName,"VMULF\t$v%d, $v%d, $v%d%s",command.OP.V.vd, command.OP.V.vs, 
				command.OP.V.vt, RspElementSpecifier(command.OP.V.element));
			break;
		case RSP_VECTOR_VMULU:
			sprintf(CommandName,"VMULU\t$v%d, $v%d, $v%d%s",command.OP.V.vd, command.OP.V.vs, 
				command.OP.V.vt, RspElementSpecifier(command.OP.V.element));
			break;
		case RSP_VECTOR_VRNDP:
			sprintf(CommandName,"VRNDP\t$v%d, $v%d, $v%d%s",command.OP.V.vd, command.OP.V.vs, 
				command.OP.V.vt, RspElementSpecifier(command.OP.V.element));
			break;
		case RSP_VECTOR_VMULQ:
			sprintf(CommandName,"VMULQ\t$v%d, $v%d, $v%d%s",command.OP.V.vd, command.OP.V.vs, 
				command.OP.V.vt, RspElementSpecifier(command.OP.V.element));
			break;
		case RSP_VECTOR_VMUDL:
			sprintf(CommandName,"VMUDL\t$v%d, $v%d, $v%d%s",command.OP.V.vd, command.OP.V.vs, 
				command.OP.V.vt, RspElementSpecifier(command.OP.V.element));
			break;
		case RSP_VECTOR_VMUDM:
			sprintf(CommandName,"VMUDM\t$v%d, $v%d, $v%d%s",command.OP.V.vd, command.OP.V.vs, 
				command.OP.V.vt, RspElementSpecifier(command.OP.V.element));
			break;
		case RSP_VECTOR_VMUDN:
			sprintf(CommandName,"VMUDN\t$v%d, $v%d, $v%d%s",command.OP.V.vd, command.OP.V.vs, 
				command.OP.V.vt, RspElementSpecifier(command.OP.V.element));
			break;
		case RSP_VECTOR_VMUDH:
			sprintf(CommandName,"VMUDH\t$v%d, $v%d, $v%d%s",command.OP.V.vd, command.OP.V.vs, 
				command.OP.V.vt, RspElementSpecifier(command.OP.V.element));
			break;
		case RSP_VECTOR_VMACF:
			sprintf(CommandName,"VMACF\t$v%d, $v%d, $v%d%s",command.OP.V.vd, command.OP.V.vs, 
				command.OP.V.vt, RspElementSpecifier(command.OP.V.element));
			break;
		case RSP_VECTOR_VMACU:
			sprintf(CommandName,"VMACU\t$v%d, $v%d, $v%d%s",command.OP.V.vd, command.OP.V.vs, 
				command.OP.V.vt, RspElementSpecifier(command.OP.V.element));
			break;
		case RSP_VECTOR_VRNDN:
			sprintf(CommandName,"VRNDN\t$v%d, $v%d, $v%d%s",command.OP.V.vd, command.OP.V.vs, 
				command.OP.V.vt, RspElementSpecifier(command.OP.V.element));
			break;
		case RSP_VECTOR_VMACQ:
			sprintf(CommandName,"VMACQ\t$v%d, $v%d, $v%d%s",command.OP.V.vd, command.OP.V.vs, 
				command.OP.V.vt, RspElementSpecifier(command.OP.V.element));
			break;
		case RSP_VECTOR_VMADL:
			sprintf(CommandName,"VMADL\t$v%d, $v%d, $v%d%s",command.OP.V.vd, command.OP.V.vs, 
				command.OP.V.vt, RspElementSpecifier(command.OP.V.element));
			break;
		case RSP_VECTOR_VMADM:
			sprintf(CommandName,"VMADM\t$v%d, $v%d, $v%d%s",command.OP.V.vd, command.OP.V.vs, 
				command.OP.V.vt, RspElementSpecifier(command.OP.V.element));
			break;
		case RSP_VECTOR_VMADN:
			sprintf(CommandName,"VMADN\t$v%d, $v%d, $v%d%s",command.OP.V.vd, command.OP.V.vs, 
				command.OP.V.vt, RspElementSpecifier(command.OP.V.element));
			break;
		case RSP_VECTOR_VMADH:
			sprintf(CommandName,"VMADH\t$v%d, $v%d, $v%d%s",command.OP.V.vd, command.OP.V.vs, 
				command.OP.V.vt, RspElementSpecifier(command.OP.V.element));
			break;
		case RSP_VECTOR_VADD:
			sprintf(CommandName,"VADD\t$v%d, $v%d, $v%d%s",command.OP.V.vd, command.OP.V.vs, 
				command.OP.V.vt, RspElementSpecifier(command.OP.V.element));
			break;
		case RSP_VECTOR_VSUB:
			sprintf(CommandName,"VSUB\t$v%d, $v%d, $v%d%s",command.OP.V.vd, command.OP.V.vs, 
				command.OP.V.vt, RspElementSpecifier(command.OP.V.element));
			break;
		case RSP_VECTOR_VABS:
			sprintf(CommandName,"VABS\t$v%d, $v%d, $v%d%s",command.OP.V.vd, command.OP.V.vs, 
				command.OP.V.vt, RspElementSpecifier(command.OP.V.element));
			break;
		case RSP_VECTOR_VADDC:
			sprintf(CommandName,"VADDC\t$v%d, $v%d, $v%d%s",command.OP.V.vd, command.OP.V.vs, 
				command.OP.V.vt, RspElementSpecifier(command.OP.V.element));
			break;
		case RSP_VECTOR_VSUBC:
			sprintf(CommandName,"VSUBC\t$v%d, $v%d, $v%d%s",command.OP.V.vd, command.OP.V.vs, 
				command.OP.V.vt, RspElementSpecifier(command.OP.V.element));
			break;
		case RSP_VECTOR_VSAR:
			sprintf(CommandName,"VSAR\t$v%d [%d], $v%d, $v%d ",command.OP.V.vd, command.OP.V.element,
				command.OP.V.vd, command.OP.V.vt);
			break;
		case RSP_VECTOR_VLT:
			sprintf(CommandName,"VLT\t$v%d, $v%d, $v%d%s",command.OP.V.vd, command.OP.V.vs, 
				command.OP.V.vt, RspElementSpecifier(command.OP.V.element));
			break;
		case RSP_VECTOR_VEQ:
			sprintf(CommandName,"VEQ\t$v%d, $v%d, $v%d%s",command.OP.V.vd, command.OP.V.vs, 
				command.OP.V.vt, RspElementSpecifier(command.OP.V.element));
			break;
		case RSP_VECTOR_VNE:
			sprintf(CommandName,"VNE\t$v%d, $v%d, $v%d%s",command.OP.V.vd, command.OP.V.vs, 
				command.OP.V.vt, RspElementSpecifier(command.OP.V.element));
			break;
		case RSP_VECTOR_VGE:
			sprintf(CommandName,"VGE\t$v%d, $v%d, $v%d%s",command.OP.V.vd, command.OP.V.vs, 
				command.OP.V.vt, RspElementSpecifier(command.OP.V.element));
			break;
		case RSP_VECTOR_VCL:
			sprintf(CommandName,"VCL\t$v%d, $v%d, $v%d%s",command.OP.V.vd, command.OP.V.vs, 
				command.OP.V.vt, RspElementSpecifier(command.OP.V.element));
			break;
		case RSP_VECTOR_VCH:
			sprintf(CommandName,"VCH\t$v%d, $v%d, $v%d%s",command.OP.V.vd, command.OP.V.vs, 
				command.OP.V.vt, RspElementSpecifier(command.OP.V.element));
			break;
		case RSP_VECTOR_VCR:
			sprintf(CommandName,"VCR\t$v%d, $v%d, $v%d%s",command.OP.V.vd, command.OP.V.vs, 
				command.OP.V.vt, RspElementSpecifier(command.OP.V.element));
			break;
		case RSP_VECTOR_VMRG:
			sprintf(CommandName,"VMRG\t$v%d, $v%d, $v%d%s",command.OP.V.vd, command.OP.V.vs, 
				command.OP.V.vt, RspElementSpecifier(command.OP.V.element));
			break;
		case RSP_VECTOR_VAND:
			sprintf(CommandName,"VAND\t$v%d, $v%d, $v%d%s",command.OP.V.vd, command.OP.V.vs, 
				command.OP.V.vt, RspElementSpecifier(command.OP.V.element));
			break;
		case RSP_VECTOR_VNAND:
			sprintf(CommandName,"VNAND\t$v%d, $v%d, $v%d%s",command.OP.V.vd, command.OP.V.vs, 
				command.OP.V.vt, RspElementSpecifier(command.OP.V.element));
			break;
		case RSP_VECTOR_VOR:
			sprintf(CommandName,"VOR\t$v%d, $v%d, $v%d%s",command.OP.V.vd, command.OP.V.vs, 
				command.OP.V.vt, RspElementSpecifier(command.OP.V.element));
			break;
		case RSP_VECTOR_VNOR:
			sprintf(CommandName,"VNOR\t$v%d, $v%d, $v%d%s",command.OP.V.vd, command.OP.V.vs, 
				command.OP.V.vt, RspElementSpecifier(command.OP.V.element));
			break;
		case RSP_VECTOR_VXOR:
			sprintf(CommandName,"VXOR\t$v%d, $v%d, $v%d%s",command.OP.V.vd, command.OP.V.vs, 
				command.OP.V.vt, RspElementSpecifier(command.OP.V.element));
			break;
		case RSP_VECTOR_VNXOR:
			sprintf(CommandName,"VNXOR\t$v%d, $v%d, $v%d%s",command.OP.V.vd, command.OP.V.vs, 
				command.OP.V.vt, RspElementSpecifier(command.OP.V.element));
			break;
		case RSP_VECTOR_VRCP:
			sprintf(CommandName,"VRCP\t$v%d [%d], $v%d%s",command.OP.V.vd, (command.OP.V.vs & 0x7), 
				command.OP.V.vt, RspElementSpecifier(command.OP.V.element));
			break;
		case RSP_VECTOR_VRCPL:
			sprintf(CommandName,"VRCPL\t$v%d [%d], $v%d%s",command.OP.V.vd, (command.OP.V.vs & 0x7), 
				command.OP.V.vt, RspElementSpecifier(command.OP.V.element));
			break;
		case RSP_VECTOR_VRCPH:
			sprintf(CommandName,"VRCPH\t$v%d [%d], $v%d%s",command.OP.V.vd, (command.OP.V.vs & 0x7), 
				command.OP.V.vt, RspElementSpecifier(command.OP.V.element));
			break;
		case RSP_VECTOR_VMOV:
			sprintf(CommandName,"VMOV\t$v%d, $v%d, $v%d%s",command.OP.V.vd, command.OP.V.vs, 
				command.OP.V.vt, RspElementSpecifier(command.OP.V.element));
			break;
		case RSP_VECTOR_VRSQ:
			sprintf(CommandName,"VRSQ\t$v%d [%d], $v%d%s",command.OP.V.vd, (command.OP.V.vs & 0x7), 
				command.OP.V.vt, RspElementSpecifier(command.OP.V.element));
			break;
		case RSP_VECTOR_VRSQL:
			sprintf(CommandName,"VRSQL\t$v%d [%d], $v%d%s",command.OP.V.vd, (command.OP.V.vs & 0x7), 
				command.OP.V.vt, RspElementSpecifier(command.OP.V.element));
			break;
		case RSP_VECTOR_VRSQH:
			sprintf(CommandName,"VRSQH\t$v%d [%d], $v%d%s",command.OP.V.vd, (command.OP.V.vs & 0x7), 
				command.OP.V.vt, RspElementSpecifier(command.OP.V.element));
			break;
		case RSP_VECTOR_VNOOP:
			sprintf(CommandName,"VNOOP");
			break;
		default:
			sprintf(CommandName,"RSP: Unknown\t%02X %02X %02X %02X",
				command.OP.Ascii[3],command.OP.Ascii[2],command.OP.Ascii[1],command.OP.Ascii[0]);
		}
	}
	return CommandName;
}

char * RSPLc2Name ( DWORD OpCode ) {
	OPCODE command;
	command.OP.Hex = OpCode;

	switch (command.OP.LSV.lsop) {
	case RSP_LSC2_BV:
		sprintf(CommandName,"LBV\t$v%d [%d], 0x%04X (%s)",command.OP.LSV.vt, command.OP.LSV.element, 
			command.OP.LSV.offset, RspGPR_Name(command.OP.LSV.base));
		break;
	case RSP_LSC2_SV:
		sprintf(CommandName,"LSV\t$v%d [%d], 0x%04X (%s)",command.OP.LSV.vt, command.OP.LSV.element, 
			(command.OP.LSV.offset << 1), RspGPR_Name(command.OP.LSV.base));
		break;
	case RSP_LSC2_LV:
		sprintf(CommandName,"LLV\t$v%d [%d], 0x%04X (%s)",command.OP.LSV.vt, command.OP.LSV.element, 
			(command.OP.LSV.offset << 2), RspGPR_Name(command.OP.LSV.base));
		break;
	case RSP_LSC2_DV:
		sprintf(CommandName,"LDV\t$v%d [%d], 0x%04X (%s)",command.OP.LSV.vt, command.OP.LSV.element, 
			(command.OP.LSV.offset << 3), RspGPR_Name(command.OP.LSV.base));
		break;
	case RSP_LSC2_QV:
		sprintf(CommandName,"LQV\t$v%d [%d], 0x%04X (%s)",command.OP.LSV.vt, command.OP.LSV.element, 
			(command.OP.LSV.offset << 4), RspGPR_Name(command.OP.LSV.base));
		break;
	case RSP_LSC2_RV:
		sprintf(CommandName,"LRV\t$v%d [%d], 0x%04X (%s)",command.OP.LSV.vt, command.OP.LSV.element, 
			(command.OP.LSV.offset << 4), RspGPR_Name(command.OP.LSV.base));
		break;
	case RSP_LSC2_PV:
		sprintf(CommandName,"LPV\t$v%d [%d], 0x%04X (%s)",command.OP.LSV.vt, command.OP.LSV.element, 
			(command.OP.LSV.offset << 3), RspGPR_Name(command.OP.LSV.base));
		break;
	case RSP_LSC2_UV:
		sprintf(CommandName,"LUV\t$v%d [%d], 0x%04X (%s)",command.OP.LSV.vt, command.OP.LSV.element, 
			(command.OP.LSV.offset << 3), RspGPR_Name(command.OP.LSV.base));
		break;
	case RSP_LSC2_HV:
		sprintf(CommandName,"LHV\t$v%d [%d], 0x%04X (%s)",command.OP.LSV.vt, command.OP.LSV.element, 
			(command.OP.LSV.offset << 4), RspGPR_Name(command.OP.LSV.base));
		break;
	case RSP_LSC2_FV:
		sprintf(CommandName,"LFV\t$v%d [%d], 0x%04X (%s)",command.OP.LSV.vt, command.OP.LSV.element, 
			(command.OP.LSV.offset << 4), RspGPR_Name(command.OP.LSV.base));
		break;
	case RSP_LSC2_WV:
		sprintf(CommandName,"LWV\t$v%d [%d], 0x%04X (%s)",command.OP.LSV.vt, command.OP.LSV.element, 
			(command.OP.LSV.offset << 4), RspGPR_Name(command.OP.LSV.base));
		break;
	case RSP_LSC2_TV:
		sprintf(CommandName,"LTV\t$v%d [%d], 0x%04X (%s)",command.OP.LSV.vt, command.OP.LSV.element, 
			(command.OP.LSV.offset << 4), RspGPR_Name(command.OP.LSV.base));
		break;
	default:
		sprintf(CommandName,"RSP: Unknown\t%02X %02X %02X %02X",
			command.OP.Ascii[3],command.OP.Ascii[2],command.OP.Ascii[1],command.OP.Ascii[0]);
	}
	return CommandName;
}

char * RSPSc2Name ( DWORD OpCode ) {
	OPCODE command;
	command.OP.Hex = OpCode;

	switch (command.OP.LSV.lsop) {
	case RSP_LSC2_BV:
		sprintf(CommandName,"SBV\t$v%d [%d], 0x%04X (%s)",command.OP.LSV.vt, command.OP.LSV.element, 
			command.OP.LSV.offset, RspGPR_Name(command.OP.LSV.base));
		break;
	case RSP_LSC2_SV:
		sprintf(CommandName,"SSV\t$v%d [%d], 0x%04X (%s)",command.OP.LSV.vt, command.OP.LSV.element, 
			(command.OP.LSV.offset << 1), RspGPR_Name(command.OP.LSV.base));
		break;
	case RSP_LSC2_LV:
		sprintf(CommandName,"SLV\t$v%d [%d], 0x%04X (%s)",command.OP.LSV.vt, command.OP.LSV.element, 
			(command.OP.LSV.offset << 2), RspGPR_Name(command.OP.LSV.base));
		break;
	case RSP_LSC2_DV:
		sprintf(CommandName,"SDV\t$v%d [%d], 0x%04X (%s)",command.OP.LSV.vt, command.OP.LSV.element, 
			(command.OP.LSV.offset << 3), RspGPR_Name(command.OP.LSV.base));
		break;
	case RSP_LSC2_QV:
		sprintf(CommandName,"SQV\t$v%d [%d], 0x%04X (%s)",command.OP.LSV.vt, command.OP.LSV.element, 
			(command.OP.LSV.offset << 4), RspGPR_Name(command.OP.LSV.base));
		break;
	case RSP_LSC2_RV:
		sprintf(CommandName,"SRV\t$v%d [%d], 0x%04X (%s)",command.OP.LSV.vt, command.OP.LSV.element, 
			(command.OP.LSV.offset << 4), RspGPR_Name(command.OP.LSV.base));
		break;
	case RSP_LSC2_PV:
		sprintf(CommandName,"SPV\t$v%d [%d], 0x%04X (%s)",command.OP.LSV.vt, command.OP.LSV.element, 
			(command.OP.LSV.offset << 3), RspGPR_Name(command.OP.LSV.base));
		break;
	case RSP_LSC2_UV:
		sprintf(CommandName,"SUV\t$v%d [%d], 0x%04X (%s)",command.OP.LSV.vt, command.OP.LSV.element, 
			(command.OP.LSV.offset << 3), RspGPR_Name(command.OP.LSV.base));
		break;
	case RSP_LSC2_HV:
		sprintf(CommandName,"SHV\t$v%d [%d], 0x%04X (%s)",command.OP.LSV.vt, command.OP.LSV.element, 
			(command.OP.LSV.offset << 4), RspGPR_Name(command.OP.LSV.base));
		break;
	case RSP_LSC2_FV:
		sprintf(CommandName,"SFV\t$v%d [%d], 0x%04X (%s)",command.OP.LSV.vt, command.OP.LSV.element, 
			(command.OP.LSV.offset << 4), RspGPR_Name(command.OP.LSV.base));
		break;
	case RSP_LSC2_WV:
		sprintf(CommandName,"SWV\t$v%d [%d], 0x%04X (%s)",command.OP.LSV.vt, command.OP.LSV.element, 
			(command.OP.LSV.offset << 4), RspGPR_Name(command.OP.LSV.base));
		break;
	case RSP_LSC2_TV:
		sprintf(CommandName,"STV\t$v%d [%d], 0x%04X (%s)",command.OP.LSV.vt, command.OP.LSV.element, 
			(command.OP.LSV.offset << 4), RspGPR_Name(command.OP.LSV.base));
		break;
	default:
		sprintf(CommandName,"RSP: Unknown\t%02X %02X %02X %02X",
			command.OP.Ascii[3],command.OP.Ascii[2],command.OP.Ascii[1],command.OP.Ascii[0]);
	}
	return CommandName;
}

char * RSPOpcodeName ( DWORD OpCode, DWORD PC ) {
	OPCODE command;
	command.OP.Hex = OpCode;
		
	switch (command.OP.I.op) {
	case RSP_SPECIAL:
		return RSPSpecialName(OpCode);
	case RSP_REGIMM:
		return RSPRegimmName(OpCode,PC);
	case RSP_J:
		sprintf(CommandName,"J\t0x%03X",(command.OP.J.target << 2) & 0xFFC);
		break;
	case RSP_JAL:
		sprintf(CommandName,"JAL\t0x%03X",(command.OP.J.target << 2) & 0xFFC);
		break;
	case RSP_BEQ:
		if (command.OP.I.rs == 0 && command.OP.I.rt == 0) {
			sprintf(CommandName,"B\t0x%03X",(PC + ((short)command.OP.B.offset << 2) + 4) & 0xFFC);
		} else if (command.OP.B.rs == 0 || command.OP.B.rt == 0){
			sprintf(CommandName,"BEQZ\t%s, 0x%03X",RspGPR_Name(command.OP.B.rs == 0 ? command.OP.B.rt : command.OP.B.rs),
				(PC + ((short)command.OP.B.offset << 2) + 4) & 0xFFC);
		} else {
			sprintf(CommandName,"BEQ\t%s, %s, 0x%03X",RspGPR_Name(command.OP.B.rs),RspGPR_Name(command.OP.B.rt),
				(PC + ((short)command.OP.B.offset << 2) + 4) & 0xFFC);
		}
		break;
	case RSP_BNE:
		sprintf(CommandName,"BNE\t%s, %s, 0x%03X",RspGPR_Name(command.OP.B.rs),RspGPR_Name(command.OP.B.rt),
			(PC + ((short)command.OP.B.offset << 2) + 4) & 0xFFC);
		break;
	case RSP_BLEZ:
		sprintf(CommandName,"BLEZ\t%s, 0x%03X",RspGPR_Name(command.OP.B.rs),(PC + ((short)command.OP.B.offset << 2) + 4) & 0xFFC);
		break;
	case RSP_BGTZ:
		sprintf(CommandName,"BGTZ\t%s, 0x%03X",RspGPR_Name(command.OP.B.rs),(PC + ((short)command.OP.B.offset << 2) + 4) & 0xFFC);
		break;
	case RSP_ADDI:
		sprintf(CommandName,"ADDI\t%s, %s, 0x%04X",RspGPR_Name(command.OP.I.rt), RspGPR_Name(command.OP.I.rs),
			command.OP.I.immediate);
		break;
	case RSP_ADDIU:
		sprintf(CommandName,"ADDIU\t%s, %s, 0x%04X",RspGPR_Name(command.OP.I.rt), RspGPR_Name(command.OP.I.rs),
			command.OP.I.immediate);
		break;
	case RSP_SLTI:
		sprintf(CommandName,"SLTI\t%s, %s, 0x%04X",RspGPR_Name(command.OP.I.rt), RspGPR_Name(command.OP.I.rs),
			command.OP.I.immediate);
		break;
	case RSP_SLTIU:
		sprintf(CommandName,"SLTIU\t%s, %s, 0x%04X",RspGPR_Name(command.OP.I.rt), RspGPR_Name(command.OP.I.rs),
			command.OP.I.immediate);
		break;
	case RSP_ANDI:
		sprintf(CommandName,"ANDI\t%s, %s, 0x%04X",RspGPR_Name(command.OP.I.rt), RspGPR_Name(command.OP.I.rs),
			command.OP.I.immediate);
		break;
	case RSP_ORI:
		sprintf(CommandName,"ORI\t%s, %s, 0x%04X",RspGPR_Name(command.OP.I.rt), RspGPR_Name(command.OP.I.rs),
			command.OP.I.immediate);
		break;
	case RSP_XORI:
		sprintf(CommandName,"XORI\t%s, %s, 0x%04X",RspGPR_Name(command.OP.I.rt), RspGPR_Name(command.OP.I.rs),
			command.OP.I.immediate);
		break;
	case RSP_LUI:
		sprintf(CommandName,"LUI\t%s, 0x%04X",RspGPR_Name(command.OP.I.rt), command.OP.I.immediate);
		break;
	case RSP_CP0:
		return RSPCop0Name(OpCode);
	case RSP_CP2:
		return RSPCop2Name(OpCode);
	case RSP_LB:
		sprintf(CommandName,"LB\t%s, 0x%04X(%s)",RspGPR_Name(command.OP.LS.rt), command.OP.LS.offset,
			RspGPR_Name(command.OP.LS.base));
		break;
	case RSP_LH:
		sprintf(CommandName,"LH\t%s, 0x%04X(%s)",RspGPR_Name(command.OP.LS.rt), command.OP.LS.offset,
			RspGPR_Name(command.OP.LS.base));
		break;
	case RSP_LW:
		sprintf(CommandName,"LW\t%s, 0x%04X(%s)",RspGPR_Name(command.OP.LS.rt), command.OP.LS.offset,
			RspGPR_Name(command.OP.LS.base));
		break;
	case RSP_LBU:
		sprintf(CommandName,"LBU\t%s, 0x%04X(%s)",RspGPR_Name(command.OP.LS.rt), command.OP.LS.offset,
			RspGPR_Name(command.OP.LS.base));
		break;
	case RSP_LHU:
		sprintf(CommandName,"LHU\t%s, 0x%04X(%s)",RspGPR_Name(command.OP.LS.rt), command.OP.LS.offset,
			RspGPR_Name(command.OP.LS.base));
		break;
	case RSP_SB:
		sprintf(CommandName,"SB\t%s, 0x%04X(%s)",RspGPR_Name(command.OP.LS.rt), command.OP.LS.offset,
			RspGPR_Name(command.OP.LS.base));
		break;
	case RSP_SH:
		sprintf(CommandName,"SH\t%s, 0x%04X(%s)",RspGPR_Name(command.OP.LS.rt), command.OP.LS.offset,
			RspGPR_Name(command.OP.LS.base));
		break;
	case RSP_SW:
		sprintf(CommandName,"SW\t%s, 0x%04X(%s)",RspGPR_Name(command.OP.LS.rt), command.OP.LS.offset,
			RspGPR_Name(command.OP.LS.base));
		break;
	case RSP_LC2:
		return RSPLc2Name(OpCode);
	case RSP_SC2:
		return RSPSc2Name(OpCode);
	default:
		sprintf(CommandName,"RSP: Unknown\t%02X %02X %02X %02X",
			command.OP.Ascii[3],command.OP.Ascii[2],command.OP.Ascii[1],command.OP.Ascii[0]);
	}
	return CommandName;
}

void SetRSPCommandToRunning ( void ) { 	
	Stepping_RspCommands = FALSE;
	if (InRSPCommandsWindow == FALSE) { return; }
	EnableWindow(hGoButton,    FALSE);
	EnableWindow(hBreakButton, TRUE);
	EnableWindow(hStepButton,  FALSE);
	EnableWindow(hSkipButton,  FALSE);
	SendMessage(RSPCommandshWnd, DM_SETDEFID,IDC_BREAK_BUTTON,0);
	SendMessage(hGoButton, BM_SETSTYLE,BS_PUSHBUTTON,TRUE);
	SendMessage(hBreakButton, BM_SETSTYLE,BS_DEFPUSHBUTTON,TRUE);
	SetFocus(hBreakButton);
}

void SetRSPCommandToStepping ( void ) { 	
	/*if (InRSPCommandsWindow == FALSE) { return; }
	EnableWindow(hGoButton,    TRUE);
	EnableWindow(hBreakButton, FALSE);
	EnableWindow(hStepButton,  TRUE);
	EnableWindow(hSkipButton,  TRUE);
	SendMessage(hBreakButton, BM_SETSTYLE, BS_PUSHBUTTON,TRUE);
	SendMessage(hStepButton, BM_SETSTYLE, BS_DEFPUSHBUTTON,TRUE);
	SendMessage(RSPCommandshWnd, DM_SETDEFID,IDC_STEP_BUTTON,0);
	SetFocus(hStepButton);
	Stepping_Commands = TRUE;*/
	LogMessage("TODO: SetRSPCommandToStepping()");
}

void SetRSPCommandViewto ( UINT NewLocation ) {
	unsigned int location;
	char Value[20];

	if (InRSPCommandsWindow == FALSE) { return; }

	GetWindowText(hAddress,Value,sizeof(Value));
	location = AsciiToHex(Value) & ~3;

	if ( NewLocation < location || NewLocation >= location + 120 ) {
		sprintf(Value,"%03X",NewLocation);
		SetWindowText(hAddress,Value);
	} else {
		RefreshRSPCommands();
	}
}
