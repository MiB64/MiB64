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

#include "rsp_log.h"

#include <windows.h>
#include <stdio.h>

#ifdef RspLog_x86Code
static HANDLE hCPULogFile = NULL;
#endif 

#ifdef RspLog_x86Code
void RSP_CPU_Message (char * Message, ...) {
	DWORD dwWritten;
	char Msg[400];
	va_list ap;

	va_start( ap, Message );
	vsprintf( Msg, Message, ap );
	va_end( ap );
	
	strcat(Msg,"\r\n");
	WriteFile( hCPULogFile,Msg,strlen(Msg),&dwWritten,NULL );
}
#endif

#ifdef RspLog_x86Code
void RSP_Start_x86_Log (void) {
	char path_buffer[_MAX_PATH], drive[_MAX_DRIVE] ,dir[_MAX_DIR];
	char File[_MAX_PATH];

	GetModuleFileName(NULL,path_buffer,_MAX_PATH);
	_splitpath(path_buffer, drive, dir, NULL, NULL);
	
	sprintf(File, "%s%s\\RSPx86Log.log", drive, dir);

	hCPULogFile = CreateFile(File,GENERIC_WRITE, FILE_SHARE_READ,NULL,CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);
	SetFilePointer(hCPULogFile,0,NULL,FILE_BEGIN);
}
#endif
