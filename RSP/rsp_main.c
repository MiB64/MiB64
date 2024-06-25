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

#include "rsp_config.h"
#include "rsp_main.h"
#include "rsp_memory.h"
#include "rsp_registers.h"
#include "RSP_Profiling.h"
#include "RSP Recompiler CPU.h"
#include "../Plugin.h"
#include "../mi_registers.h"
#include "../rdp_registers.h"
#include "../Types.h"
#include "../exception.h"

void __cdecl LogMessage(char* Message, ...);

DWORD RunInterpreterRspCPU(DWORD Cycles);

DWORD __cdecl InternalDoRspCycles(DWORD numberOfCycles) {
	DWORD TaskType = *(DWORD*)(DMEM + 0xFC0);

	if (TaskType == 1 && GraphicsHle) {
		if (ProcessDList != NULL) {
			ProcessDList();
		}
		SP_STATUS_REG |= (SP_STATUS_SIG2 | SP_STATUS_BROKE | SP_STATUS_HALT);
		if ((SP_STATUS_REG & SP_STATUS_INTR_BREAK) != 0) {
			MI_INTR_REG |= MI_INTR_SP;
			CheckInterrupts();
		}

		DPC_STATUS_REG &= ~0x0002;
		return numberOfCycles;
	}
	else if (TaskType == 2 && AudioHle) {
		if (ProcessAList != NULL) {
			ProcessAList();
		}
		SP_STATUS_REG |= (SP_STATUS_SIG2 | SP_STATUS_BROKE | SP_STATUS_HALT);
		if ((SP_STATUS_REG & SP_STATUS_INTR_BREAK) != 0) {
			MI_INTR_REG |= MI_INTR_SP;
			CheckInterrupts();
		}
		return numberOfCycles;
	}
	else if (TaskType == 7) {
		if (ShowCFB != NULL) {
			ShowCFB();
		}
	}

#if !defined(EXTERNAL_RELEASE)
	if (RspProfiling) {
		StopRspTimer();
		if (!IndividualRspBlock) { StartRspTimer("RSP Running"); }
	}
#endif

	DWORD executedCycles;

	WaitForSingleObjectEx(hRspConfigMutex, INFINITE, FALSE);
	switch (RspCPUCore) {
	case RecompilerCPU:
		executedCycles = RunRecompilerRspCPU(numberOfCycles);
		break;
	case InterpreterCPU:
		executedCycles = RunInterpreterRspCPU(numberOfCycles);
		break;
	default:
		// shoud never happen
		return 0;
	}
	ReleaseMutex(hRspConfigMutex);

#if !defined(EXTERNAL_RELEASE)
	if (RspProfiling) {
		StopRspTimer();
		StartRspTimer("r4300i code");
	}
#endif

	return executedCycles;
}

void notifyRSPOfIMEMChange() {
	IMEMIsUpdated = TRUE;
}
