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
/*#include <stdio.h>
#include <float.h>*/
#include "rsp_config.h"
/*#include "Cpu.h"
#include "RSP registers.h"
#include "RSP Command.h"
#include "memory.h"*/
#include "RSP_OpCode.h"
/*#include "log.h"
#include "Profiling.h"
#include "breakpoint.h"
#include "x86.h"*/
#include "../Types.h"

MIPS_DWORD EleSpec[16], Indx[16];
OPCODE RSPOpC;
DWORD /*NextInstruction,*/ RSP_Running;

void * RSP_Opcode[64];
void * RSP_RegImm[32];
void * RSP_Special[64];
void * RSP_Cop0[32];
void * RSP_Cop2[32];
void * RSP_Vector[64];
void * RSP_Lc2[32];
void * RSP_Sc2[32];

void BuildInterpreterRspCPU(void);
void BuildRecompilerRspCPU(void);

void SetRspCPU(DWORD core) {
	WaitForSingleObjectEx(hRspConfigMutex, INFINITE, FALSE);
	RspCPUCore = core;
	switch (core) {
	case RecompilerCPU:
		BuildRecompilerRspCPU();
		break;
	case InterpreterCPU:
		BuildInterpreterRspCPU();
		break;
	}
	ReleaseMutex(hRspConfigMutex);
}

void Build_RSP ( void ) {
	SetRspCPU(RspCPUCore);
	/*ResetTimerList();*/

	EleSpec[ 0].DW = 0x0001020304050607; /* None */
	EleSpec[ 1].DW = 0x0001020304050607; /* None */
	EleSpec[ 2].DW = 0x0000020204040606; /* 0q */
	EleSpec[ 3].DW = 0x0101030305050707; /* 1q */
	EleSpec[ 4].DW = 0x0000000004040404; /* 0h */
	EleSpec[ 5].DW = 0x0101010105050505; /* 1h */
	EleSpec[ 6].DW = 0x0202020206060606; /* 2h */
	EleSpec[ 7].DW = 0x0303030307070707; /* 3h */
	EleSpec[ 8].DW = 0x0000000000000000; /* 0 */
	EleSpec[ 9].DW = 0x0101010101010101; /* 1 */
	EleSpec[10].DW = 0x0202020202020202; /* 2 */
	EleSpec[11].DW = 0x0303030303030303; /* 3 */
	EleSpec[12].DW = 0x0404040404040404; /* 4 */
	EleSpec[13].DW = 0x0505050505050505; /* 5 */
	EleSpec[14].DW = 0x0606060606060606; /* 6 */
	EleSpec[15].DW = 0x0707070707070707; /* 7 */

	Indx[ 0].DW = 0x0001020304050607; /* None */
	Indx[ 1].DW = 0x0001020304050607; /* None */
	Indx[ 2].DW = 0x0103050700020406; /* 0q */
	Indx[ 3].DW = 0x0002040601030507; /* 1q */
	Indx[ 4].DW = 0x0102030506070004; /* 0h */
	Indx[ 5].DW = 0x0002030406070105; /* 1h */
	Indx[ 6].DW = 0x0001030405070206; /* 2h */
	Indx[ 7].DW = 0x0001020405060307; /* 3h */
	Indx[ 8].DW = 0x0102030405060700; /* 0 */
	Indx[ 9].DW = 0x0002030405060701; /* 1 */
	Indx[10].DW = 0x0001030405060702; /* 2 */
	Indx[11].DW = 0x0001020405060703; /* 3 */
	Indx[12].DW = 0x0001020305060704; /* 4 */
	Indx[13].DW = 0x0001020304060705; /* 5 */
	Indx[14].DW = 0x0001020304050706; /* 6 */
	Indx[15].DW = 0x0001020304050607; /* 7 */

	for (int i = 0; i < 16; i ++) {
		int count;

		for (count = 0; count < 8; count ++) {
			Indx[i].B[count] = 7 - Indx[i].B[count];
			EleSpec[i].B[count] = 7 - EleSpec[i].B[count];
		}
		for (count = 0; count < 4; count ++) {
			BYTE Temp;
			
			Temp = Indx[i].B[count];
			Indx[i].B[count] = Indx[i].B[7 - count]; 
			Indx[i].B[7 - count] = Temp;
		}
	}
}
