#pragma once

#include <windows.h>

#define InterpreterCPU	0
#define RecompilerCPU	1

extern DWORD RspCPUCore;

void __cdecl rspConfig(HWND hwnd);
