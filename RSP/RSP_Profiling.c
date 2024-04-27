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
#include <string.h>

typedef struct {
	char Label[100];
	_int64 TimeTotal;
} TIME_STAMP_ENTRY;

static DWORD StartTimeHi, StartTimeLo, StopTimeHi, StopTimeLo, TSE_Count, TSE_Max;
static TIME_STAMP_ENTRY * TS_Entries = NULL;
static char LastLabel[100];

/*void ResetTimerList (void) {
	if (TS_Entries) { free(TS_Entries); }
	TS_Entries = NULL;
	TSE_Count = 0;
	TSE_Max = 0;
}*/

void StartRspTimer (char * Label) {
	strcpy(LastLabel,Label);
	_asm {
		pushad
		rdtsc
		mov StartTimeHi, edx
		mov StartTimeLo, eax
		popad
	}
}

void StopRspTimer (void) {
	_asm {
		pushad
		rdtsc
		mov StopTimeHi, edx
		mov StopTimeLo, eax
		popad
	}
	if (strlen(LastLabel) == 0) { return; }
	{
		DWORD count;

		for (count = 0; count < TSE_Count; count ++) {
			if (strcmp(LastLabel,TS_Entries[count].Label) == 0) {
				_int64 Time = ((unsigned _int64)StopTimeHi << 32) + (unsigned _int64)StopTimeLo;
				Time -= ((unsigned _int64)StartTimeHi << 32) + (unsigned _int64)StartTimeLo;
				TS_Entries[count].TimeTotal += Time;
				return;
			}
		}
	}
	if (TSE_Count == 0) {
		TS_Entries = (TIME_STAMP_ENTRY *)malloc(sizeof(TIME_STAMP_ENTRY) * 100);
		if (TS_Entries == NULL) {
			MessageBox(NULL,"TIME_STAMP_ENTRY == NULL ??","ERROR",MB_OK|MB_ICONERROR|MB_SETFOREGROUND);
		}
		TSE_Max = 100;
	} else if (TSE_Count == TSE_Max) {
		TSE_Max += 100;
		TS_Entries = (TIME_STAMP_ENTRY *)realloc(TS_Entries,sizeof(TIME_STAMP_ENTRY) * TSE_Max);
		if (TS_Entries == NULL) {
			MessageBox(NULL,"TIME_STAMP_ENTRY == NULL ??","ERROR",MB_OK|MB_ICONERROR|MB_SETFOREGROUND);
		}
	}
	strcpy(TS_Entries[TSE_Count].Label,LastLabel);
	TS_Entries[TSE_Count].TimeTotal  = ((unsigned _int64)StopTimeHi << 32) + (unsigned _int64)StopTimeLo;
	TS_Entries[TSE_Count].TimeTotal -= ((unsigned _int64)StartTimeHi << 32) + (unsigned _int64)StartTimeLo;
	TSE_Count +=1;
}

/*void GenerateTimerResults (void) {
	char buffer[_MAX_PATH], drive[_MAX_DRIVE] ,dir[_MAX_DIR];
	char fname[_MAX_FNAME],ext[_MAX_EXT], LogFileName[_MAX_PATH];
	DWORD dwWritten, count, count2;
	HANDLE hLogFile = NULL;
	_int64 TotalTime;

	StopTimer();

	GetModuleFileName(NULL,buffer,sizeof(buffer));
	_splitpath( buffer, drive, dir, fname, ext );
   	_makepath( LogFileName, drive, dir, "RSP Profiling", "log" );

	hLogFile = CreateFile(LogFileName,GENERIC_WRITE, FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,
		CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);
	SetFilePointer(hLogFile,0,NULL,FILE_BEGIN);
	
	for (count = 0; count < TSE_Count; count ++) {
		for (count2 = 0; count2 < (TSE_Count - 1); count2 ++) {
			if (TS_Entries[count2].TimeTotal < TS_Entries[count2 + 1].TimeTotal) {
				TIME_STAMP_ENTRY Temp;
				memcpy(&Temp,&TS_Entries[count2],sizeof(TIME_STAMP_ENTRY));
				memcpy(&TS_Entries[count2],&TS_Entries[count2 + 1],sizeof(TIME_STAMP_ENTRY));
				memcpy(&TS_Entries[count2 + 1],&Temp,sizeof(TIME_STAMP_ENTRY));
			}
		}
	}
	TotalTime = 0;
	for (count = 0; count < TSE_Count; count ++) {
		TotalTime += TS_Entries[count].TimeTotal;
	}
	for (count = 0; count < (TSE_Count < 50?TSE_Count:50); count ++) {
		sprintf(buffer,"%s - %0.2f%c\r\n",
			TS_Entries[count].Label,
			(((double)TS_Entries[count].TimeTotal / (double)TotalTime) * 100),'%'
		);
		WriteFile( hLogFile,buffer,strlen(buffer),&dwWritten,NULL );
	}
	CloseHandle(hLogFile);
	ResetTimerList();
}*/
