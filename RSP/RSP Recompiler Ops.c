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
/*#include <stdio.h>*/
#include "rsp_Cpu.h"
#include "RSP Recompiler CPU.h"
#include "RSP Recompiler Ops.h"
#include "RSP Interpreter Ops.h"
#include "RSP Command.h"
#include "rsp_registers.h"
#include "rsp_memory.h"
#include "../Dma.h"
#include "rsp_log.h"
#include "../x86.h"
#include "rsp_config.h"
#include "../Main.h"
#include "../mi_registers.h"
#include "../rdp_registers.h"
#include "../Exception.h"

/*U_WORD Recp, RecpResult, SQroot, SQrootResult;
DWORD ESP_RegSave = 0, EBP_RegSave = 0;*/
static DWORD BranchCompare = 0;
DWORD BeginOfCurrentSubBlock = 0;

/* align option affects: sw, lh, sh */
/* align option affects: lrv, ssv, lsv */

#define Compile_Immediates	/* ADDI, ADDIU, ANDI, ORI, XORI, LUI */
#define Compile_GPRLoads	/* LB, LH, LW, LBU, LHU, LWU */
#define Compile_GPRStores	/* SB, SH, SW */
#define Compile_Special		/* SLL, SRL, SRA, SRLV */
							/* XOR, OR, AND, SUB, SUBU, ADDU, ADD, SLT */
#define Compile_SLTI
#define Compile_SLTIU
#define Compile_SLLV
#define Compile_SRAV
#define Compile_BREAK
#define Compile_NOR
#define Compile_SLTU
#define Compile_Cop0
#define Compile_Cop2

#define Compile_CFC2
#define Compile_CTC2

#define RSP_VectorMuls
/*#define RSP_VectorLoads
#define RSP_VectorMisc*/

#ifdef RSP_VectorMuls
#	define CompileVmulf	/* Verified 12/17/2000 - Jabo */
#	define CompileVmulu
#   define CompileVrndp
#	define CompileVmulq
#	define CompileVmacf	/* Rewritten & Verified 12/15/2000 - Jabo */
#	define CompileVmacu
#	define CompileVrndn
#	define CompileVmacq
#	define CompileVmudm	/* Verified 12/17/2000 - Jabo */
#	define CompileVmudh	/* Verified 12/17/2000 - Jabo */
#	define CompileVmudn	/* Verified 12/17/2000 - Jabo */
#	define CompileVmudl	/* Verified 12/17/2000 - Jabo */
/*#	define CompileVmadl	
#	define CompileVmadm*/	/* Verified 12/17/2000 - Jabo */
/*#	define CompileVmadh*/	/* Verified 12/15/2000 - Jabo */
/*#	define CompileVmadn*/	/* Verified 12/17/2000 - Jabo */
#endif
/*#ifdef RSP_VectorMisc
#	define CompileVrsqh
#	define CompileVrcph
#	define CompileVsaw*/		/* Verified 12/17/2000 - Jabo */
/*#	define CompileVabs*/		/* Verified 12/15/2000 - Jabo */
/*#	define CompileVmov*/		/* Verified 12/17/2000 - Jabo */
/*#	define CompileVxor*/		/* Verified 12/17/2000 - Jabo */
/*#	define CompileVor*/		/* Verified 12/17/2000 - Jabo */
/*#	define CompileVand*/		/* Verified 12/17/2000 - Jabo */
/*#	define CompileVsub*/		/* Verified 12/17/2000 - Jabo (watch flags) */
/*#	define CompileVadd*/		/* Verified 12/17/2000 - Jabo (watch flags) */
/*#	define CompileVaddc
#	define CompileVsubc
#	define CompileVmrg
#endif
#ifdef RSP_VectorLoads
#	define CompileSqv*/		/* Verified 12/17/2000 - Jabo */
/*#	define CompileSdv*/		/* Verified 12/17/2000 - Jabo */
/*#	define CompileSsv*/		/* Verified 12/17/2000 - Jabo */
/*#	define CompileLrv*/		/* Rewritten & Verified 12/17/2000 - Jabo */
/*#	define CompileLqv*/		/* Verified 12/17/2000 - Jabo */
/*#	define CompileLdv*/		/* Verified 12/17/2000 - Jabo */
/*#	define CompileLsv*/		/* Verified 12/17/2000 - Jabo */
/*#	define CompileLlv*/		/* Verified 12/17/2000 - Jabo */
/*#	define CompileSlv
#endif*/

BOOL IsVectorOpcodeRecompiledWithMMX(int funct) {
	switch (funct) {
	case RSP_VECTOR_VMULF:
#ifdef CompileVmulf
		if (IsSse2Enabled == TRUE) return FALSE;
		return TRUE;
#else
		return FALSE;
#endif
	case RSP_VECTOR_VMUDL:
#ifdef CompileVmudl
		if (IsSse2Enabled == TRUE) return FALSE;
		return TRUE;
#else
		return FALSE;
#endif
	case RSP_VECTOR_VMUDM:
#ifdef CompileVmudm
		if (IsSse2Enabled == TRUE) return FALSE;
		return TRUE;
#else
		return FALSE;
#endif
	case RSP_VECTOR_VMUDN:
#ifdef CompileVmudn
		if (IsSse2Enabled == TRUE) return FALSE;
		return TRUE;
#else
		return FALSE;
#endif
	case RSP_VECTOR_VMUDH:
#ifdef CompileVmudh
		if (IsSse2Enabled == TRUE) return FALSE;
		return TRUE;
#else
		return FALSE;
#endif
	default:
		return FALSE;
	}
}

static void CompileRsp_ConsecutiveDelaySlots();

static void Branch_AddRef(DWORD Target, DWORD * X86Loc) {
	if (RspCurrentBlock.ResolveCount >= 150) {
		RspCompilerWarning("Out of branch reference space");
	} else {
		BYTE * KnownCode = *(RspJumpTable + (Target >> 2));

		if (KnownCode == NULL) {
			DWORD i = RspCurrentBlock.ResolveCount;
			RspCurrentBlock.BranchesToResolve[i].TargetPC = Target;
			RspCurrentBlock.BranchesToResolve[i].X86JumpLoc = X86Loc;
			RspCurrentBlock.ResolveCount += 1;
		} else {
			RSP_CPU_Message("      (static jump to %X)", KnownCode);
			x86_SetBranch32b((DWORD*)X86Loc, (DWORD*)KnownCode);
		}
	}
}

static void InterpreterFallback ( void * FunctAddress, char * FunctName) {
	RSP_CPU_Message("  %X %s",RspCompilePC,RSPOpcodeName(RSPOpC.OP.Hex,RspCompilePC));
	MoveConstToVariable(&RspRecompPos, RSPOpC.OP.Hex, &RSPOpC.OP.Hex, "RSPOpC.Hex" );
	Call_Direct(&RspRecompPos, FunctAddress, FunctName);
}

static void InterpreterFallbackNoMessage( void * FunctAddress, char * FunctName) {
	MoveConstToVariable(&RspRecompPos, RSPOpC.OP.Hex, &RSPOpC.OP.Hex, "RSPOpC.Hex" );
	Call_Direct(&RspRecompPos, FunctAddress, FunctName);
}

void x86_SetBranch8b(void * JumpByte, void * Destination) {
	/* calculate 32-bit relative offset */
	signed int n = (BYTE*)Destination - ((BYTE*)JumpByte + 1);

	/* check limits, no pun intended */
	if (n > 0x80 || n < -0x7F) {
		RspCompilerWarning("FATAL: Jump out of 8b range %i (PC = %04X)", n, RspCompilePC);
	} else
		*(BYTE*)(JumpByte) = (BYTE)n;
}

void x86_SetBranch32b(void * JumpByte, void * Destination) {
	*(DWORD*)(JumpByte) = (DWORD)((BYTE*)Destination - (BYTE*)((DWORD*)JumpByte + 1));
}

/*void BreakPoint() {
	CPU_Message("      int 3");
	*(RecompPos++) = 0xCC;
}*/

/************************* OpCode functions *************************/
void CompileRsp_SPECIAL ( void ) {
	((void (*)()) RSP_Special[ RSPOpC.OP.R.funct ])();
}

void CompileRsp_REGIMM ( void ) {
	((void (*)()) RSP_RegImm[ RSPOpC.OP.B.rt ])();
}

void CompileRsp_J ( void ) {
	if (IsRspDelaySlotBranch(RspCompilePC)) {
		CompileRsp_ConsecutiveDelaySlots();
		return;
	}

	if ( RSP_NextInstruction == NORMAL ) {
		RSP_CPU_Message("  %X %s",RspCompilePC,RSPOpcodeName(RSPOpC.OP.Hex,RspCompilePC));
		RSP_NextInstruction = DO_DELAY_SLOT;
	} else if ( RSP_NextInstruction == DELAY_SLOT_DONE ) {
		JmpLabel32 ( &RspRecompPos, "BranchToJump", 0 );
		Branch_AddRef((RSPOpC.OP.J.target << 2) & 0xFFC, (DWORD*)(RspRecompPos - 4));
		RSP_NextInstruction = FINISH_BLOCK;
	} else {
		RspCompilerWarning("J error\nWeird Delay Slot.\n\nNextInstruction = %X\nEmulation will now stop", RSP_NextInstruction);
		BreakPoint(&RspRecompPos);
	}
}

void CompileRsp_JAL ( void ) {
	if (IsRspDelaySlotBranch(RspCompilePC)) {
		CompileRsp_ConsecutiveDelaySlots();
		return;
	}

	if ( RSP_NextInstruction == NORMAL ) {
		RSP_CPU_Message("  %X %s",RspCompilePC,RSPOpcodeName(RSPOpC.OP.Hex,RspCompilePC));
		MoveConstToVariable(&RspRecompPos, (RspCompilePC + 8)&0xFFC, &RSP_GPR[31].UW, "RA.W");
		RSP_NextInstruction = DO_DELAY_SLOT;
	} else if ( RSP_NextInstruction == DELAY_SLOT_DONE ) {
		JmpLabel32 ( &RspRecompPos, "BranchToJump", 0 );
		Branch_AddRef((RSPOpC.OP.J.target << 2) & 0xFFC, (DWORD*)(RspRecompPos - 4));
		RSP_NextInstruction = FINISH_SUB_BLOCK;
	} else {
		RspCompilerWarning("J error\nWeird Delay Slot.\n\nNextInstruction = %X\nEmulation will now stop", RSP_NextInstruction);
		BreakPoint(&RspRecompPos);
	}
}

void CompileRsp_BEQ ( void ) {
	static BOOL bDelayAffect;

	if (IsRspDelaySlotBranch(RspCompilePC)) {
		CompileRsp_ConsecutiveDelaySlots();
		return;
	}

	if ( RSP_NextInstruction == NORMAL ) {
		RSP_CPU_Message("  %X %s",RspCompilePC,RSPOpcodeName(RSPOpC.OP.Hex,RspCompilePC));
		if (RSPOpC.OP.B.rs == 0 && RSPOpC.OP.B.rt == 0) {
			RSP_NextInstruction = DO_DELAY_SLOT;			
			return;
		}
		bDelayAffect = RspDelaySlotAffectBranch(RspCompilePC);
		if (FALSE == bDelayAffect) {
			RSP_NextInstruction = DO_DELAY_SLOT;
			return;
		}
		if (RSPOpC.OP.B.rt == 0) {
			CompConstToVariable(&RspRecompPos, 0,&RSP_GPR[RSPOpC.OP.B.rs].W,RspGPR_Name(RSPOpC.OP.B.rs));
		} else if (RSPOpC.OP.B.rs == 0) {			
			CompConstToVariable(&RspRecompPos, 0,&RSP_GPR[RSPOpC.OP.B.rt].W,RspGPR_Name(RSPOpC.OP.B.rt));
		} else if (IsRspRegConst(RSPOpC.OP.B.rt)) {
			CompConstToVariable(&RspRecompPos, MipsRspRegConst(RSPOpC.OP.B.rt), &RSP_GPR[RSPOpC.OP.B.rs].W, RspGPR_Name(RSPOpC.OP.B.rs));
		} else if (IsRspRegConst(RSPOpC.OP.B.rs)) {
			CompConstToVariable(&RspRecompPos, MipsRspRegConst(RSPOpC.OP.B.rs), &RSP_GPR[RSPOpC.OP.B.rt].W, RspGPR_Name(RSPOpC.OP.B.rt));
		} else {
			MoveVariableToX86reg(&RspRecompPos, &RSP_GPR[RSPOpC.OP.B.rt].W,RspGPR_Name(RSPOpC.OP.B.rt),x86_EAX);
			CompX86regToVariable(&RspRecompPos, x86_EAX,&RSP_GPR[RSPOpC.OP.B.rs].W,RspGPR_Name(RSPOpC.OP.B.rs));
		}
		SetzVariable(&RspRecompPos, &BranchCompare, "BranchCompare");
		RSP_NextInstruction = DO_DELAY_SLOT;
	} else if ( RSP_NextInstruction == DELAY_SLOT_DONE ) {
		DWORD Target = (RspCompilePC + ((short)RSPOpC.OP.B.offset << 2) + 4) & 0xFFC;
		
		if (RSPOpC.OP.B.rs == 0 && RSPOpC.OP.B.rt == 0) {
			JmpLabel32 (&RspRecompPos, "BranchToJump", 0 );
			Branch_AddRef(Target, (DWORD*)(RspRecompPos - 4));
			RSP_NextInstruction = FINISH_BLOCK;
			return;
		}
		if (FALSE == bDelayAffect) {
			if (IsRspRegConst(RSPOpC.OP.B.rt) && IsRspRegConst(RSPOpC.OP.B.rs)) {
				if (MipsRspRegConst(RSPOpC.OP.B.rt) == MipsRspRegConst(RSPOpC.OP.B.rs)) {
					JmpLabel32(&RspRecompPos, "BranchEqual", 0);
					Branch_AddRef(Target, (DWORD*)(RspRecompPos - 4));
					RSP_NextInstruction = FINISH_BLOCK;
					return;
				}
				RSP_NextInstruction = FINISH_SUB_BLOCK;
				return;
			} else if (RSPOpC.OP.B.rt == 0) {
				CompConstToVariable(&RspRecompPos, 0,&RSP_GPR[RSPOpC.OP.B.rs].W,RspGPR_Name(RSPOpC.OP.B.rs));
			} else if (RSPOpC.OP.B.rs == 0) {			
				CompConstToVariable(&RspRecompPos, 0,&RSP_GPR[RSPOpC.OP.B.rt].W,RspGPR_Name(RSPOpC.OP.B.rt));
			} else if (IsRspRegConst(RSPOpC.OP.B.rt)) {
				CompConstToVariable(&RspRecompPos, MipsRspRegConst(RSPOpC.OP.B.rt), &RSP_GPR[RSPOpC.OP.B.rs].W, RspGPR_Name(RSPOpC.OP.B.rs));
			} else if (IsRspRegConst(RSPOpC.OP.B.rs)) {
				CompConstToVariable(&RspRecompPos, MipsRspRegConst(RSPOpC.OP.B.rs), &RSP_GPR[RSPOpC.OP.B.rt].W, RspGPR_Name(RSPOpC.OP.B.rt));
			} else {
				MoveVariableToX86reg(&RspRecompPos, &RSP_GPR[RSPOpC.OP.B.rt].W,RspGPR_Name(RSPOpC.OP.B.rt),x86_EAX);
				CompX86regToVariable(&RspRecompPos, x86_EAX,&RSP_GPR[RSPOpC.OP.B.rs].W,RspGPR_Name(RSPOpC.OP.B.rs));
			}
			JeLabel32(&RspRecompPos, "BranchEqual", 0);
		} else {
			/* take a look at the branch compare variable */
			CompConstToVariable(&RspRecompPos, TRUE, &BranchCompare, "BranchCompare");
			JeLabel32(&RspRecompPos, "BranchEqual", 0);
		}
		Branch_AddRef(Target, (DWORD*)(RspRecompPos - 4));
		RSP_NextInstruction = FINISH_SUB_BLOCK;
	}
	else {
		RspCompilerWarning("BEQ error\nWeird Delay Slot.\n\nNextInstruction = %X\nEmulation will now stop", RSP_NextInstruction);
		BreakPoint(&RspRecompPos);
	}
}

void CompileRsp_BNE ( void ) {
	static BOOL bDelayAffect;

	if (IsRspDelaySlotBranch(RspCompilePC)) {
		CompileRsp_ConsecutiveDelaySlots();
		return;
	}

	if ( RSP_NextInstruction == NORMAL ) {
		RSP_CPU_Message("  %X %s",RspCompilePC,RSPOpcodeName(RSPOpC.OP.Hex,RspCompilePC));
		if (RSPOpC.OP.B.rs == 0 && RSPOpC.OP.B.rt == 0) {
			RSP_NextInstruction = DO_DELAY_SLOT;
			return;
		}

		bDelayAffect = RspDelaySlotAffectBranch(RspCompilePC);
		if (FALSE == bDelayAffect) {
			RSP_NextInstruction = DO_DELAY_SLOT;
			return;
		}
		if (RSPOpC.OP.B.rt == 0) {			
			CompConstToVariable(&RspRecompPos, 0,&RSP_GPR[RSPOpC.OP.B.rs].W,RspGPR_Name(RSPOpC.OP.B.rs));
		} else if (RSPOpC.OP.B.rs == 0) {			
			CompConstToVariable(&RspRecompPos, 0,&RSP_GPR[RSPOpC.OP.B.rt].W,RspGPR_Name(RSPOpC.OP.B.rt));
		} else if (IsRspRegConst(RSPOpC.OP.B.rt)) {
			CompConstToVariable(&RspRecompPos, MipsRspRegConst(RSPOpC.OP.B.rt), &RSP_GPR[RSPOpC.OP.B.rs].W, RspGPR_Name(RSPOpC.OP.B.rs));
		} else if (IsRspRegConst(RSPOpC.OP.B.rs)) {
			CompConstToVariable(&RspRecompPos, MipsRspRegConst(RSPOpC.OP.B.rs), &RSP_GPR[RSPOpC.OP.B.rt].W, RspGPR_Name(RSPOpC.OP.B.rt));
		} else {
			MoveVariableToX86reg(&RspRecompPos, &RSP_GPR[RSPOpC.OP.B.rt].W,RspGPR_Name(RSPOpC.OP.B.rt),x86_EAX);
			CompX86regToVariable(&RspRecompPos, x86_EAX,&RSP_GPR[RSPOpC.OP.B.rs].W,RspGPR_Name(RSPOpC.OP.B.rs));
		}
		SetnzVariable(&RspRecompPos, &BranchCompare, "BranchCompare");
		RSP_NextInstruction = DO_DELAY_SLOT;
	} else if ( RSP_NextInstruction == DELAY_SLOT_DONE ) {
		DWORD Target = (RspCompilePC + ((short)RSPOpC.OP.B.offset << 2) + 4) & 0xFFC;
		
		if (RSPOpC.OP.B.rs == 0 && RSPOpC.OP.B.rt == 0) {			
			RSP_NextInstruction = FINISH_SUB_BLOCK;
			return;
		}

		if (FALSE == bDelayAffect) {
			if (IsRspRegConst(RSPOpC.OP.B.rt) && IsRspRegConst(RSPOpC.OP.B.rs)) {
				if (MipsRspRegConst(RSPOpC.OP.B.rt) != MipsRspRegConst(RSPOpC.OP.B.rs)) {
					JmpLabel32(&RspRecompPos, "BranchEqual", 0);
					Branch_AddRef(Target, (DWORD*)(RspRecompPos - 4));
					RSP_NextInstruction = FINISH_BLOCK;
					return;
				}
				RSP_NextInstruction = FINISH_SUB_BLOCK;
				return;
			} else if (RSPOpC.OP.B.rt == 0) {			
				CompConstToVariable(&RspRecompPos, 0,&RSP_GPR[RSPOpC.OP.B.rs].W,RspGPR_Name(RSPOpC.OP.B.rs));
			} else if (RSPOpC.OP.B.rs == 0) {
				CompConstToVariable(&RspRecompPos, 0,&RSP_GPR[RSPOpC.OP.B.rt].W,RspGPR_Name(RSPOpC.OP.B.rt));
			} else if (IsRspRegConst(RSPOpC.OP.B.rt)) {
				CompConstToVariable(&RspRecompPos, MipsRspRegConst(RSPOpC.OP.B.rt), &RSP_GPR[RSPOpC.OP.B.rs].W, RspGPR_Name(RSPOpC.OP.B.rs));
			} else if (IsRspRegConst(RSPOpC.OP.B.rs)) {
				CompConstToVariable(&RspRecompPos, MipsRspRegConst(RSPOpC.OP.B.rs), &RSP_GPR[RSPOpC.OP.B.rt].W, RspGPR_Name(RSPOpC.OP.B.rt));
			} else {
				MoveVariableToX86reg(&RspRecompPos, &RSP_GPR[RSPOpC.OP.B.rt].W,RspGPR_Name(RSPOpC.OP.B.rt),x86_EAX);
				CompX86regToVariable(&RspRecompPos, x86_EAX,&RSP_GPR[RSPOpC.OP.B.rs].W,RspGPR_Name(RSPOpC.OP.B.rs));
			}
			JneLabel32(&RspRecompPos, "BranchNotEqual", 0);
		} else {
			/* take a look at the branch compare variable */
			CompConstToVariable(&RspRecompPos, TRUE, &BranchCompare, "BranchCompare");
			JeLabel32(&RspRecompPos, "BranchNotEqual", 0);
		}
		Branch_AddRef(Target, (DWORD*)(RspRecompPos - 4));
		RSP_NextInstruction = FINISH_SUB_BLOCK;
	}
	else {
		RspCompilerWarning("BNE error\nWeird Delay Slot.\n\nNextInstruction = %X\nEmulation will now stop", RSP_NextInstruction);
		BreakPoint(&RspRecompPos);
	}
}

void CompileRsp_BLEZ ( void ) {
	static BOOL bDelayAffect;

	if (IsRspDelaySlotBranch(RspCompilePC)) {
		CompileRsp_ConsecutiveDelaySlots();
		return;
	}

	if ( RSP_NextInstruction == NORMAL ) {
		RSP_CPU_Message("  %X %s",RspCompilePC,RSPOpcodeName(RSPOpC.OP.Hex,RspCompilePC));
		if (RSPOpC.OP.B.rs == 0) {
			RSP_NextInstruction = DO_DELAY_SLOT;			
			return;
		}
		bDelayAffect = RspDelaySlotAffectBranch(RspCompilePC);
		if (FALSE == bDelayAffect) {
			RSP_NextInstruction = DO_DELAY_SLOT;
			return;
		}
		CompConstToVariable(&RspRecompPos, 0,&RSP_GPR[RSPOpC.OP.B.rs].W,RspGPR_Name(RSPOpC.OP.B.rs));
		SetleVariable(&RspRecompPos, &BranchCompare, "BranchCompare");
		RSP_NextInstruction = DO_DELAY_SLOT;
	} else if ( RSP_NextInstruction == DELAY_SLOT_DONE ) {
		DWORD Target = (RspCompilePC + ((short)RSPOpC.OP.B.offset << 2) + 4) & 0xFFC;
		
		if (RSPOpC.OP.B.rs == 0) {
			JmpLabel32 ( &RspRecompPos, "BranchToJump", 0 );
			Branch_AddRef(Target, (DWORD*)(RspRecompPos - 4));
			RSP_NextInstruction = FINISH_BLOCK;
			return;
		}
		if (IsRspRegConst(RSPOpC.OP.B.rs)) {
			if ((long)MipsRspRegConst(RSPOpC.OP.B.rs) <= 0) {
				JmpLabel32(&RspRecompPos, "BranchToJump", 0);
				Branch_AddRef(Target, (DWORD*)(RspRecompPos - 4));
				RSP_NextInstruction = FINISH_BLOCK;
				return;
			}
			RSP_NextInstruction = FINISH_SUB_BLOCK;
			return;
		}
		if (FALSE == bDelayAffect) {
			CompConstToVariable(&RspRecompPos, 0,&RSP_GPR[RSPOpC.OP.B.rs].W,RspGPR_Name(RSPOpC.OP.B.rs));
			JleLabel32(&RspRecompPos, "BranchLessEqual", 0);
		} else {
			/* take a look at the branch compare variable */
			CompConstToVariable(&RspRecompPos, TRUE, &BranchCompare, "BranchCompare");
			JeLabel32(&RspRecompPos, "BranchLessEqual", 0);
		}

		Branch_AddRef(Target, (DWORD*)(RspRecompPos - 4));
		RSP_NextInstruction = FINISH_SUB_BLOCK;
	} else {
		RspCompilerWarning("BLEZ error\nWeird Delay Slot.\n\nNextInstruction = %X\nEmulation will now stop", RSP_NextInstruction);
		BreakPoint(&RspRecompPos);
	}
}

void CompileRsp_BGTZ ( void ) {
	static BOOL bDelayAffect;

	if (IsRspDelaySlotBranch(RspCompilePC)) {
		CompileRsp_ConsecutiveDelaySlots();
		return;
	}

	if ( RSP_NextInstruction == NORMAL ) {
		RSP_CPU_Message("  %X %s",RspCompilePC,RSPOpcodeName(RSPOpC.OP.Hex,RspCompilePC));
		if (RSPOpC.OP.B.rs == 0) {
			RSP_NextInstruction = DO_DELAY_SLOT;			
			return;
		}
		bDelayAffect = RspDelaySlotAffectBranch(RspCompilePC);
		if (FALSE == bDelayAffect) {
			RSP_NextInstruction = DO_DELAY_SLOT;
			return;
		}
		CompConstToVariable(&RspRecompPos, 0,&RSP_GPR[RSPOpC.OP.B.rs].W,RspGPR_Name(RSPOpC.OP.B.rs));
		SetgVariable(&RspRecompPos, &BranchCompare, "BranchCompare");
		RSP_NextInstruction = DO_DELAY_SLOT;
	} else if ( RSP_NextInstruction == DELAY_SLOT_DONE ) {
		DWORD Target = (RspCompilePC + ((short)RSPOpC.OP.B.offset << 2) + 4) & 0xFFC;
		
		if (RSPOpC.OP.B.rs == 0) {			
			RSP_NextInstruction = FINISH_SUB_BLOCK;
			return;
		}
		if (IsRspRegConst(RSPOpC.OP.B.rs)) {
			if ((long)MipsRspRegConst(RSPOpC.OP.B.rs) > 0) {
				JmpLabel32(&RspRecompPos, "BranchToJump", 0);
				Branch_AddRef(Target, (DWORD*)(RspRecompPos - 4));
				RSP_NextInstruction = FINISH_BLOCK;
				return;
			}
			RSP_NextInstruction = FINISH_SUB_BLOCK;
			return;
		}
		if (FALSE == bDelayAffect) {
			CompConstToVariable(&RspRecompPos,0,&RSP_GPR[RSPOpC.OP.B.rs].W,RspGPR_Name(RSPOpC.OP.B.rs));
			JgLabel32(&RspRecompPos, "BranchGreater", 0);
		} else {
			/* take a look at the branch compare variable */
			CompConstToVariable(&RspRecompPos, TRUE, &BranchCompare, "BranchCompare");
			JeLabel32(&RspRecompPos, "BranchGreater", 0);
		}
		Branch_AddRef(Target, (DWORD*)(RspRecompPos - 4));
		RSP_NextInstruction = FINISH_SUB_BLOCK;
	} else {
		RspCompilerWarning("BGTZ error\nWeird Delay Slot.\n\nNextInstruction = %X\nEmulation will now stop", RSP_NextInstruction);
		BreakPoint(&RspRecompPos);
	}
}

void CompileRsp_ADDI ( void ) {
	int Immediate = (short)RSPOpC.OP.I.immediate;

	#ifndef Compile_Immediates
	InterpreterFallback((void*)RSP_Opcode_ADDI,"RSP_Opcode_ADDI"); return;
	#endif

	RSP_CPU_Message("  %X %s",RspCompilePC,RSPOpcodeName(RSPOpC.OP.Hex,RspCompilePC));

	if (RSPOpC.OP.I.rt == 0) return;

	if (RSPOpC.OP.I.rt == RSPOpC.OP.I.rs) {
		AddConstToVariable(&RspRecompPos, Immediate, &RSP_GPR[RSPOpC.OP.I.rt].UW, RspGPR_Name(RSPOpC.OP.I.rt));
	} else if (RSPOpC.OP.I.rs == 0) {
		MoveConstToVariable(&RspRecompPos, Immediate, &RSP_GPR[RSPOpC.OP.I.rt].UW, RspGPR_Name(RSPOpC.OP.I.rt));
	} else if (IsRspRegConst(RSPOpC.OP.I.rs)) {
		MoveConstToVariable(&RspRecompPos, (long)MipsRspRegConst(RSPOpC.OP.I.rs) + Immediate, &RSP_GPR[RSPOpC.OP.I.rt].UW, RspGPR_Name(RSPOpC.OP.I.rt));
	} else {
		MoveVariableToX86reg(&RspRecompPos, &RSP_GPR[RSPOpC.OP.I.rs].UW, RspGPR_Name(RSPOpC.OP.I.rs), x86_EAX);
		if (Immediate != 0) {
			AddConstToX86Reg(&RspRecompPos, x86_EAX, Immediate);
		}
		MoveX86regToVariable(&RspRecompPos, x86_EAX, &RSP_GPR[RSPOpC.OP.I.rt].UW, RspGPR_Name(RSPOpC.OP.I.rt));
	}
}

void CompileRsp_ADDIU ( void ) {
	int Immediate = (short)RSPOpC.OP.I.immediate;

	#ifndef Compile_Immediates
	InterpreterFallback((void*)RSP_Opcode_ADDIU,"RSP_Opcode_ADDIU"); return;
	#endif

	RSP_CPU_Message("  %X %s",RspCompilePC,RSPOpcodeName(RSPOpC.OP.Hex,RspCompilePC));

	if (RSPOpC.OP.I.rt == 0) return;

	if (RSPOpC.OP.I.rt == RSPOpC.OP.I.rs) {
		AddConstToVariable(&RspRecompPos, Immediate, &RSP_GPR[RSPOpC.OP.I.rt].UW, RspGPR_Name(RSPOpC.OP.I.rt));
	} else if (RSPOpC.OP.I.rs == 0) {
		MoveConstToVariable(&RspRecompPos, Immediate, &RSP_GPR[RSPOpC.OP.I.rt].UW, RspGPR_Name(RSPOpC.OP.I.rt));
	} else if (IsRspRegConst(RSPOpC.OP.I.rs)) {
		MoveConstToVariable(&RspRecompPos, (long)MipsRspRegConst(RSPOpC.OP.I.rs) + Immediate, &RSP_GPR[RSPOpC.OP.I.rt].UW, RspGPR_Name(RSPOpC.OP.I.rt));
	} else {
		MoveVariableToX86reg(&RspRecompPos, &RSP_GPR[RSPOpC.OP.I.rs].UW, RspGPR_Name(RSPOpC.OP.I.rs), x86_EAX);
		if (Immediate != 0) {
			AddConstToX86Reg(&RspRecompPos, x86_EAX, Immediate);
		}
		MoveX86regToVariable(&RspRecompPos, x86_EAX, &RSP_GPR[RSPOpC.OP.I.rt].UW, RspGPR_Name(RSPOpC.OP.I.rt));
	}
}

void CompileRsp_SLTI ( void ) {
	int Immediate = (short)RSPOpC.OP.I.immediate;

#ifndef Compile_SLTI
	InterpreterFallback((void*)RSP_Opcode_SLTI,"RSP_Opcode_SLTI");
#endif
	RSP_CPU_Message("  %X %s", RspCompilePC, RSPOpcodeName(RSPOpC.OP.Hex, RspCompilePC));
	if (RSPOpC.OP.I.rt == 0) { return; }

	if (RSPOpC.OP.I.rs == 0) {
		if (Immediate > 0) {
			MoveConstToVariable(&RspRecompPos, 1, &RSP_GPR[RSPOpC.OP.I.rt], RspGPR_Name(RSPOpC.OP.I.rt));
		}
		else {
			MoveConstToVariable(&RspRecompPos, 0, &RSP_GPR[RSPOpC.OP.I.rt], RspGPR_Name(RSPOpC.OP.I.rt));
		}
	} else if (IsRspRegConst(RSPOpC.OP.I.rs)) {
		if ((long)MipsRspRegConst(RSPOpC.OP.I.rs) < Immediate) {
			MoveConstToVariable(&RspRecompPos, 1, &RSP_GPR[RSPOpC.OP.I.rt], RspGPR_Name(RSPOpC.OP.I.rt));
		}
		else {
			MoveConstToVariable(&RspRecompPos, 0, &RSP_GPR[RSPOpC.OP.I.rt], RspGPR_Name(RSPOpC.OP.I.rt));
		}
	} if (Immediate == 0) {
		MoveVariableToX86reg(&RspRecompPos, &RSP_GPR[RSPOpC.OP.I.rs].UW, RspGPR_Name(RSPOpC.OP.I.rs), x86_EAX);
		ShiftRightUnsignImmed(&RspRecompPos, x86_EAX, 31);
		MoveX86regToVariable(&RspRecompPos, x86_EAX, &RSP_GPR[RSPOpC.OP.I.rt].UW, RspGPR_Name(RSPOpC.OP.I.rt));
	} else {
		XorX86RegToX86Reg(&RspRecompPos, x86_EAX, x86_EAX);
		CompConstToVariable(&RspRecompPos, Immediate, &RSP_GPR[RSPOpC.OP.I.rs].UW, RspGPR_Name(RSPOpC.OP.I.rs));
		Setl(&RspRecompPos, x86_EAX);
		MoveX86regToVariable(&RspRecompPos, x86_EAX, &RSP_GPR[RSPOpC.OP.I.rt].UW, RspGPR_Name(RSPOpC.OP.I.rt));
	}
}

void CompileRsp_SLTIU ( void ) {
	int Immediate = (short)RSPOpC.OP.I.immediate;

#ifndef Compile_SLTIU
	InterpreterFallback((void*)RSP_Opcode_SLTIU,"RSP_Opcode_SLTIU");
#endif
	RSP_CPU_Message("  %X %s", RspCompilePC, RSPOpcodeName(RSPOpC.OP.Hex, RspCompilePC));
	if (RSPOpC.OP.I.rt == 0) { return; }

	if (RSPOpC.OP.I.rs == 0) {
		if (Immediate > 0) {
			MoveConstToVariable(&RspRecompPos, 1, &RSP_GPR[RSPOpC.OP.I.rt], RspGPR_Name(RSPOpC.OP.I.rt));
		}
		else {
			MoveConstToVariable(&RspRecompPos, 0, &RSP_GPR[RSPOpC.OP.I.rt], RspGPR_Name(RSPOpC.OP.I.rt));
		}
	} else if (IsRspRegConst(RSPOpC.OP.I.rs)) {
		if ((unsigned long)MipsRspRegConst(RSPOpC.OP.I.rs) < (unsigned long)Immediate) {
			MoveConstToVariable(&RspRecompPos, 1, &RSP_GPR[RSPOpC.OP.I.rt], RspGPR_Name(RSPOpC.OP.I.rt));
		}
		else {
			MoveConstToVariable(&RspRecompPos, 0, &RSP_GPR[RSPOpC.OP.I.rt], RspGPR_Name(RSPOpC.OP.I.rt));
		}
	} else {
		XorX86RegToX86Reg(&RspRecompPos, x86_EAX, x86_EAX);
		CompConstToVariable(&RspRecompPos, Immediate, &RSP_GPR[RSPOpC.OP.I.rs].UW, RspGPR_Name(RSPOpC.OP.I.rs));
		Setb(&RspRecompPos, x86_EAX);
		MoveX86regToVariable(&RspRecompPos, x86_EAX, &RSP_GPR[RSPOpC.OP.I.rt].UW, RspGPR_Name(RSPOpC.OP.I.rt));
	}
}

void CompileRsp_ANDI ( void ) {
	int Immediate = (unsigned short)RSPOpC.OP.I.immediate;

	#ifndef Compile_Immediates
	InterpreterFallback((void*)RSP_Opcode_ANDI,"RSP_Opcode_ANDI"); return;
	#endif

	RSP_CPU_Message("  %X %s",RspCompilePC,RSPOpcodeName(RSPOpC.OP.Hex,RspCompilePC));

	if (RSPOpC.OP.I.rt == 0) return;

	if (RSPOpC.OP.I.rt == RSPOpC.OP.I.rs) {
		AndConstToVariable(&RspRecompPos, Immediate, &RSP_GPR[RSPOpC.OP.I.rt].UW, RspGPR_Name(RSPOpC.OP.I.rt));
	} else if (RSPOpC.OP.I.rs == 0) {
		MoveConstToVariable(&RspRecompPos, 0, &RSP_GPR[RSPOpC.OP.I.rt].UW, RspGPR_Name(RSPOpC.OP.I.rt));
	} else if (IsRspRegConst(RSPOpC.OP.I.rs)) {
		MoveConstToVariable(&RspRecompPos, MipsRspRegConst(RSPOpC.OP.I.rs) & Immediate, &RSP_GPR[RSPOpC.OP.I.rt].UW, RspGPR_Name(RSPOpC.OP.I.rt));
	} else {
		MoveVariableToX86reg(&RspRecompPos, &RSP_GPR[RSPOpC.OP.I.rs].UW, RspGPR_Name(RSPOpC.OP.I.rs), x86_EAX);
		AndConstToX86Reg(&RspRecompPos, x86_EAX, Immediate);
		MoveX86regToVariable(&RspRecompPos, x86_EAX, &RSP_GPR[RSPOpC.OP.I.rt].UW, RspGPR_Name(RSPOpC.OP.I.rt));
	}
}

void CompileRsp_ORI ( void ) {
	int Immediate = (unsigned short)RSPOpC.OP.I.immediate;

	#ifndef Compile_Immediates
	InterpreterFallback((void*)RSP_Opcode_ORI,"RSP_Opcode_ORI"); return;
	#endif

	RSP_CPU_Message("  %X %s",RspCompilePC,RSPOpcodeName(RSPOpC.OP.Hex,RspCompilePC));

	if (RSPOpC.OP.I.rt == 0) return;

	if (RSPOpC.OP.I.rt == RSPOpC.OP.I.rs) {
		OrConstToVariable(&RspRecompPos, Immediate, &RSP_GPR[RSPOpC.OP.I.rt].UW, RspGPR_Name(RSPOpC.OP.I.rt));
	} else if (RSPOpC.OP.I.rs == 0) {
		MoveConstToVariable(&RspRecompPos, Immediate, &RSP_GPR[RSPOpC.OP.I.rt].UW, RspGPR_Name(RSPOpC.OP.I.rt));
	} else if (IsRspRegConst(RSPOpC.OP.I.rs)) {
		MoveConstToVariable(&RspRecompPos, MipsRspRegConst(RSPOpC.OP.I.rs) | Immediate, &RSP_GPR[RSPOpC.OP.I.rt].UW, RspGPR_Name(RSPOpC.OP.I.rt));
	} else {
		MoveVariableToX86reg(&RspRecompPos, &RSP_GPR[RSPOpC.OP.I.rs].UW, RspGPR_Name(RSPOpC.OP.I.rs), x86_EAX);
		if (Immediate != 0) {
			OrConstToX86Reg(&RspRecompPos, Immediate, x86_EAX);
		}
		MoveX86regToVariable(&RspRecompPos, x86_EAX, &RSP_GPR[RSPOpC.OP.I.rt].UW, RspGPR_Name(RSPOpC.OP.I.rt));
	}
}

void CompileRsp_XORI ( void ) {
	int Immediate = (unsigned short)RSPOpC.OP.I.immediate;

	#ifndef Compile_Immediates
	InterpreterFallback((void*)RSP_Opcode_XORI,"RSP_Opcode_XORI"); return;
	#endif

	RSP_CPU_Message("  %X %s",RspCompilePC,RSPOpcodeName(RSPOpC.OP.Hex,RspCompilePC));

	if (RSPOpC.OP.I.rt == 0) return;

	if (RSPOpC.OP.I.rt == RSPOpC.OP.I.rs) {
		XorConstToVariable(&RspRecompPos, &RSP_GPR[RSPOpC.OP.I.rt].UW, RspGPR_Name(RSPOpC.OP.I.rt), Immediate);
	} else if (RSPOpC.OP.I.rs == 0) {
		MoveConstToVariable(&RspRecompPos, Immediate, &RSP_GPR[RSPOpC.OP.I.rt].UW, RspGPR_Name(RSPOpC.OP.I.rt));
	} else if (IsRspRegConst(RSPOpC.OP.I.rs)) {
		MoveConstToVariable(&RspRecompPos, MipsRspRegConst(RSPOpC.OP.I.rs) ^ Immediate, &RSP_GPR[RSPOpC.OP.I.rt].UW, RspGPR_Name(RSPOpC.OP.I.rt));
	} else {
		MoveVariableToX86reg(&RspRecompPos, &RSP_GPR[RSPOpC.OP.I.rs].UW, RspGPR_Name(RSPOpC.OP.I.rs), x86_EAX);
		if (Immediate != 0) {
			XorConstToX86Reg(&RspRecompPos, x86_EAX, Immediate);
		}
		MoveX86regToVariable(&RspRecompPos, x86_EAX, &RSP_GPR[RSPOpC.OP.I.rt].UW, RspGPR_Name(RSPOpC.OP.I.rt));
	}
}

void CompileRsp_LUI ( void ) {
	int n = (short)RSPOpC.OP.I.immediate << 16;

	#ifndef Compile_Immediates
	InterpreterFallback((void*)RSP_Opcode_LUI,"RSP_Opcode_LUI"); return;
	#endif

	RSP_CPU_Message("  %X %s",RspCompilePC,RSPOpcodeName(RSPOpC.OP.Hex,RspCompilePC));

	if (RSPOpC.OP.I.rt == 0) return;
	MoveConstToVariable(&RspRecompPos, n, &RSP_GPR[RSPOpC.OP.I.rt].W, RspGPR_Name(RSPOpC.OP.I.rt));
}

void CompileRsp_COP0 (void) {
	((void (*)()) RSP_Cop0[ RSPOpC.OP.I.rs ])();
}

void CompileRsp_COP2 (void) {
	((void (*)()) RSP_Cop2[ RSPOpC.OP.I.rs ])();
}

void CompileRsp_LB ( void ) {
	int Offset = (short)RSPOpC.OP.LS.offset;

	#ifndef Compile_GPRLoads
	InterpreterFallback((void*)RSP_Opcode_LB,"RSP_Opcode_LB"); return;
	#endif

	RSP_CPU_Message("  %X %s",RspCompilePC,RSPOpcodeName(RSPOpC.OP.Hex,RspCompilePC));

	if (RSPOpC.OP.LS.rt == 0) return;

	if (IsRspRegConst(RSPOpC.OP.LS.base) == TRUE) {
		DWORD Addr = (MipsRspRegConst(RSPOpC.OP.LS.base) + Offset) ^ 3;
		Addr &= 0xfff;

		char Address[32];
		sprintf(Address, "Dmem + %Xh", Addr);
		MoveSxVariableToX86regByte(&RspRecompPos, DMEM + Addr, Address, x86_EAX);
		MoveX86regToVariable(&RspRecompPos, x86_EAX, &RSP_GPR[RSPOpC.OP.LS.rt].UW, RspGPR_Name(RSPOpC.OP.LS.rt));
		return;
	}

	MoveVariableToX86reg(&RspRecompPos, &RSP_GPR[RSPOpC.OP.LS.base].UW, RspGPR_Name(RSPOpC.OP.LS.base), x86_EBX);
	if (Offset != 0) AddConstToX86Reg(&RspRecompPos, x86_EBX, Offset);
	XorConstToX86Reg(&RspRecompPos, x86_EBX, 3);
	AndConstToX86Reg(&RspRecompPos, x86_EBX, 0x0fff);

	MoveSxDMemToX86regByte(&RspRecompPos, x86_EAX, x86_EBX);
	MoveX86regToVariable(&RspRecompPos, x86_EAX, &RSP_GPR[RSPOpC.OP.LS.rt].UW, RspGPR_Name(RSPOpC.OP.LS.rt));
}

void CompileRsp_LH ( void ) {
	int Offset = (short)RSPOpC.OP.LS.offset;
	BYTE * Jump[2];

	#ifndef Compile_GPRLoads
	InterpreterFallback((void*)RSP_Opcode_LH,"RSP_Opcode_LH"); return;
	#endif

	RSP_CPU_Message("  %X %s",RspCompilePC,RSPOpcodeName(RSPOpC.OP.Hex,RspCompilePC));

	if (RSPOpC.OP.LS.rt == 0) return;

	if (IsRspRegConst(RSPOpC.OP.LS.base) == TRUE) {
		DWORD Addr = (MipsRspRegConst(RSPOpC.OP.LS.base) + Offset);

		if ((Addr & 1) != 0) {
			//RspCompilerWarning("Unaligned LH at constant address PC = %04X", RspCompilePC);

			char Address[32];
			sprintf(Address, "Dmem + %Xh", (Addr & 0xFFF));
			MoveVariableToX86regHighByte(&RspRecompPos, DMEM + ((Addr & 0xFFF) ^ 3), Address, x86_EAX);
			sprintf(Address, "Dmem + %Xh", ((Addr & 0xFFF) + 1));
			MoveVariableToX86regByte(&RspRecompPos, DMEM + (((Addr + 1) & 0xFFF) ^ 3), Address, x86_EAX);
			MoveSxHalfX86regToX86reg(&RspRecompPos, x86_EAX, x86_EAX);
			MoveX86regToVariable(&RspRecompPos, x86_EAX, &RSP_GPR[RSPOpC.OP.LS.rt].UW, RspGPR_Name(RSPOpC.OP.LS.rt));
			return;
		} else {
			Addr ^= 2;
			Addr &= 0xfff;

			char Address[32];			
			sprintf(Address, "Dmem + %Xh", Addr);
			MoveSxVariableToX86regHalf(&RspRecompPos, DMEM + Addr, Address, x86_EAX);
			MoveX86regToVariable(&RspRecompPos, x86_EAX, &RSP_GPR[RSPOpC.OP.LS.rt].UW, RspGPR_Name(RSPOpC.OP.LS.rt));
			return;
		}
	}

	MoveVariableToX86reg(&RspRecompPos, &RSP_GPR[RSPOpC.OP.LS.base].UW, RspGPR_Name(RSPOpC.OP.LS.base), x86_EBX);
	if (Offset != 0) AddConstToX86Reg(&RspRecompPos, x86_EBX, Offset);

	if (RspCompiler.bAlignGPR == FALSE) {
		TestConstToX86Reg(&RspRecompPos, 1, x86_EBX);
		JneLabel32(&RspRecompPos, "Unaligned", 0);
		Jump[0] = RspRecompPos - 4;

		RspCompilerToggleBuffer();

		RSP_CPU_Message("   Unaligned:");
		x86_SetBranch32b(Jump[0], RspRecompPos);

		AndConstToX86Reg(&RspRecompPos, x86_EBX, 0xFFF);
		LeaSourceAndOffset(&RspRecompPos, x86_EAX, x86_EBX, 1);

		AndConstToX86RegHalf(&RspRecompPos, x86_EAX, 0xFFF);
		XorConstToX86Reg(&RspRecompPos, x86_EAX, 3);
		XorConstToX86Reg(&RspRecompPos, x86_EBX, 3);
		MoveDMemToX86regByte(&RspRecompPos, x86_EAX, x86_EAX);
		MoveDMemToX86regHighByte(&RspRecompPos, x86_EAX, x86_EBX);
		MoveSxHalfX86regToX86reg(&RspRecompPos, x86_EAX, x86_EAX);
		MoveX86regToVariable(&RspRecompPos, x86_EAX, &RSP_GPR[RSPOpC.OP.LS.rt].UW, RspGPR_Name(RSPOpC.OP.LS.rt));

		JmpLabel32(&RspRecompPos, "Done", 0);
		Jump[1] = RspRecompPos - 4;

		RspCompilerToggleBuffer();
	}

	XorConstToX86Reg(&RspRecompPos, x86_EBX, 2);
	AndConstToX86Reg(&RspRecompPos, x86_EBX, 0x0fff);

	MoveSxDMemToX86regHalf(&RspRecompPos, x86_EAX, x86_EBX);
	MoveX86regToVariable(&RspRecompPos, x86_EAX, &RSP_GPR[RSPOpC.OP.LS.rt].UW, RspGPR_Name(RSPOpC.OP.LS.rt));

	if (RspCompiler.bAlignGPR == FALSE) {
		RSP_CPU_Message("   Done:");
		x86_SetBranch32b(Jump[1], RspRecompPos);
	}
}

void CompileRsp_LW ( void ) {
	int Offset = (short)RSPOpC.OP.LS.offset;
	BYTE * Jump[2];

	#ifndef Compile_GPRLoads
	InterpreterFallback((void*)RSP_Opcode_LW,"RSP_Opcode_LW"); return;
	#endif

	RSP_CPU_Message("  %X %s",RspCompilePC,RSPOpcodeName(RSPOpC.OP.Hex,RspCompilePC));

	if (RSPOpC.OP.LS.rt == 0) return;

	if (IsRspRegConst(RSPOpC.OP.LS.base) == TRUE) {
		DWORD Addr = (MipsRspRegConst(RSPOpC.OP.LS.base) + Offset) & 0xfff;

		if ((Addr & 3) != 0) {
			//RspCompilerWarning("Unaligned LW at constant address PC = %04X", RspCompilePC);
			
			char Address[32];
			sprintf(Address, "Dmem + %Xh", (Addr & 0xFFF));
			MoveVariableToX86regHighByte(&RspRecompPos, DMEM + ((Addr & 0xFFF) ^ 3), Address, x86_EAX);
			sprintf(Address, "Dmem + %Xh", ((Addr & 0xFFF) + 1));
			MoveVariableToX86regByte(&RspRecompPos, DMEM + (((Addr + 1) & 0xFFF) ^ 3), Address, x86_EAX);
			MoveX86regHalfToVariable(&RspRecompPos, x86_EAX, &RSP_GPR[RSPOpC.OP.LS.rt].UHW[1], RspGPR_Name(RSPOpC.OP.LS.rt));
			sprintf(Address, "Dmem + %Xh", ((Addr & 0xFFF) + 2));
			MoveVariableToX86regHighByte(&RspRecompPos, DMEM + (((Addr + 2) & 0xFFF) ^ 3), Address, x86_EAX);
			sprintf(Address, "Dmem + %Xh", ((Addr & 0xFFF) + 3));
			MoveVariableToX86regByte(&RspRecompPos, DMEM + (((Addr + 3) & 0xFFF) ^ 3), Address, x86_EAX);
			MoveX86regHalfToVariable(&RspRecompPos, x86_EAX, &RSP_GPR[RSPOpC.OP.LS.rt].UHW[0], RspGPR_Name(RSPOpC.OP.LS.rt));
			return;
		} else {
			char Address[32];			
			sprintf(Address, "Dmem + %Xh", Addr);
			MoveVariableToX86reg(&RspRecompPos, DMEM + Addr, Address, x86_EAX);
			MoveX86regToVariable(&RspRecompPos, x86_EAX, &RSP_GPR[RSPOpC.OP.LS.rt].UW, RspGPR_Name(RSPOpC.OP.LS.rt));
			return;
		}
	}

	MoveVariableToX86reg(&RspRecompPos, &RSP_GPR[RSPOpC.OP.LS.base].UW, RspGPR_Name(RSPOpC.OP.LS.base), x86_EBX);
	if (Offset != 0) AddConstToX86Reg(&RspRecompPos, x86_EBX, Offset);
	
	TestConstToX86Reg(&RspRecompPos, 3, x86_EBX);
	JneLabel32(&RspRecompPos, "UnAligned", 0);
	Jump[0] = RspRecompPos - 4;

	RspCompilerToggleBuffer();

	x86_SetBranch32b(Jump[0], RspRecompPos);
	RSP_CPU_Message("   Unaligned:");

	AndConstToX86Reg(&RspRecompPos, x86_EBX, 0xFFF);
	LeaSourceAndOffset(&RspRecompPos, x86_EAX, x86_EBX, 1);
	LeaSourceAndOffset(&RspRecompPos, x86_ECX, x86_EBX, 2);
	LeaSourceAndOffset(&RspRecompPos, x86_EDX, x86_EBX, 3);

	AndConstToX86RegHalf(&RspRecompPos, x86_EAX, 0xFFF);
	AndConstToX86RegHalf(&RspRecompPos, x86_ECX, 0xFFF);
	AndConstToX86RegHalf(&RspRecompPos, x86_EDX, 0xFFF);
	XorConstToX86Reg(&RspRecompPos, x86_EAX, 3);
	XorConstToX86Reg(&RspRecompPos, x86_EBX, 3);
	XorConstToX86Reg(&RspRecompPos, x86_ECX, 3);
	XorConstToX86Reg(&RspRecompPos, x86_EDX, 3);
	MoveDMemToX86regByte(&RspRecompPos, x86_EAX, x86_EAX);
	MoveDMemToX86regHighByte(&RspRecompPos, x86_EAX, x86_EBX);
	MoveX86regHalfToVariable(&RspRecompPos, x86_EAX, &RSP_GPR[RSPOpC.OP.LS.rt].UHW[1], RspGPR_Name(RSPOpC.OP.LS.rt));
	MoveDMemToX86regHighByte(&RspRecompPos, x86_EAX, x86_ECX);
	MoveDMemToX86regByte(&RspRecompPos, x86_EAX, x86_EDX);
	MoveX86regHalfToVariable(&RspRecompPos, x86_EAX, &RSP_GPR[RSPOpC.OP.LS.rt].UHW[0], RspGPR_Name(RSPOpC.OP.LS.rt));

	JmpLabel32(&RspRecompPos, "Done", 0);
	Jump[1] = RspRecompPos - 4;
	RspCompilerToggleBuffer();

	AndConstToX86Reg(&RspRecompPos, x86_EBX, 0x0fff);
	MoveDMemToX86reg(&RspRecompPos, x86_EAX, x86_EBX);
	MoveX86regToVariable(&RspRecompPos, x86_EAX, &RSP_GPR[RSPOpC.OP.LS.rt].UW, RspGPR_Name(RSPOpC.OP.LS.rt));

	RSP_CPU_Message("   Done:");
	x86_SetBranch32b(Jump[1], RspRecompPos);
}

void CompileRsp_LBU ( void ) {
	int Offset = (short)RSPOpC.OP.LS.offset;

	#ifndef Compile_GPRLoads
	InterpreterFallback((void*)RSP_Opcode_LBU,"RSP_Opcode_LBU"); return;
	#endif

	RSP_CPU_Message("  %X %s",RspCompilePC,RSPOpcodeName(RSPOpC.OP.Hex,RspCompilePC));

	if (RSPOpC.OP.LS.rt == 0) return;

	if (IsRspRegConst(RSPOpC.OP.LS.base) == TRUE) {
		DWORD Addr = (MipsRspRegConst(RSPOpC.OP.LS.base) + Offset) ^ 3;
		Addr &= 0xfff;

		char Address[32];
		sprintf(Address, "Dmem + %Xh", Addr);
		MoveZxVariableToX86regByte(&RspRecompPos, DMEM + Addr, Address, x86_EAX);
		MoveX86regToVariable(&RspRecompPos, x86_EAX, &RSP_GPR[RSPOpC.OP.LS.rt].UW, RspGPR_Name(RSPOpC.OP.LS.rt));
		return;
	}

	MoveVariableToX86reg(&RspRecompPos, &RSP_GPR[RSPOpC.OP.LS.base].UW, RspGPR_Name(RSPOpC.OP.LS.base), x86_EBX);

	if (Offset != 0) AddConstToX86Reg(&RspRecompPos, x86_EBX, Offset);
	XorConstToX86Reg(&RspRecompPos, x86_EBX, 3);
	AndConstToX86Reg(&RspRecompPos, x86_EBX, 0x0fff);

	MoveZxDMemToX86regByte(&RspRecompPos, x86_EAX, x86_EBX);
	MoveX86regToVariable(&RspRecompPos, x86_EAX, &RSP_GPR[RSPOpC.OP.LS.rt].UW, RspGPR_Name(RSPOpC.OP.LS.rt));
}

void CompileRsp_LHU ( void ) {
	int Offset = (short)RSPOpC.OP.LS.offset;
	BYTE * Jump[2];

	#ifndef Compile_GPRLoads
	InterpreterFallback((void*)RSP_Opcode_LHU,"RSP_Opcode_LHU"); return;
	#endif

	RSP_CPU_Message("  %X %s",RspCompilePC,RSPOpcodeName(RSPOpC.OP.Hex,RspCompilePC));

	if (RSPOpC.OP.LS.rt == 0) return;

	if (IsRspRegConst(RSPOpC.OP.LS.base) == TRUE) {
		DWORD Addr = (MipsRspRegConst(RSPOpC.OP.LS.base) + Offset);

		if ((Addr & 1) != 0) {
			//RspCompilerWarning("Unaligned LHU at constant address PC = %04X", RspCompilePC);

			char Address[32];
			sprintf(Address, "Dmem + %Xh", ((Addr & 0xFFF) + 1));
			MoveZxVariableToX86regByte(&RspRecompPos, DMEM + (((Addr + 1) & 0xFFF) ^ 3), Address, x86_EAX);
			sprintf(Address, "Dmem + %Xh", (Addr & 0xFFF));
			MoveVariableToX86regHighByte(&RspRecompPos, DMEM + ((Addr & 0xFFF) ^ 3), Address, x86_EAX);
			MoveX86regToVariable(&RspRecompPos, x86_EAX, &RSP_GPR[RSPOpC.OP.LS.rt].UW, RspGPR_Name(RSPOpC.OP.LS.rt));
			return;
		} else {
			Addr ^= 2;
			Addr &= 0xfff;

			char Address[32];			
			sprintf(Address, "Dmem + %Xh", Addr);
			MoveZxVariableToX86regHalf(&RspRecompPos, DMEM + Addr, Address, x86_ECX);
			MoveX86regToVariable(&RspRecompPos, x86_ECX, &RSP_GPR[RSPOpC.OP.LS.rt].UW, RspGPR_Name(RSPOpC.OP.LS.rt));
			return;
		}
	}

	/*
	 * should really just do it by bytes but whatever for now
	 */
	MoveVariableToX86reg(&RspRecompPos, &RSP_GPR[RSPOpC.OP.LS.base].UW, RspGPR_Name(RSPOpC.OP.LS.base), x86_EBX);
	if (Offset != 0) {
		AddConstToX86Reg(&RspRecompPos, x86_EBX, Offset);
	}
	TestConstToX86Reg(&RspRecompPos, 1, x86_EBX);
	JneLabel32(&RspRecompPos, "Unaligned", 0);
	Jump[0] = RspRecompPos - 4;

	RspCompilerToggleBuffer();
	RSP_CPU_Message("   Unaligned:");
	x86_SetBranch32b(Jump[0], RspRecompPos);

	AndConstToX86Reg(&RspRecompPos, x86_EBX, 0xFFF);
	LeaSourceAndOffset(&RspRecompPos, x86_EAX, x86_EBX, 1);

	AndConstToX86RegHalf(&RspRecompPos, x86_EAX, 0xFFF);
	XorConstToX86Reg(&RspRecompPos, x86_EAX, 3);
	XorConstToX86Reg(&RspRecompPos, x86_EBX, 3);
	MoveZxDMemToX86regByte(&RspRecompPos, x86_EAX, x86_EAX);
	MoveDMemToX86regHighByte(&RspRecompPos, x86_EAX, x86_EBX);
	MoveX86regToVariable(&RspRecompPos, x86_EAX, &RSP_GPR[RSPOpC.OP.LS.rt].UW, RspGPR_Name(RSPOpC.OP.LS.rt));

	//InterpreterFallbackNoMessage((void*)RSP_Opcode_LHU,"RSP_Opcode_LHU");
	JmpLabel32(&RspRecompPos, "Done", 0);
	Jump[1] = RspRecompPos - 4;
	RspCompilerToggleBuffer();

	XorConstToX86Reg(&RspRecompPos, x86_EBX, 2);
	AndConstToX86Reg(&RspRecompPos, x86_EBX, 0x0fff);
	MoveZxDMemToX86regHalf(&RspRecompPos, x86_EAX, x86_EBX);
	MoveX86regToVariable(&RspRecompPos, x86_EAX, &RSP_GPR[RSPOpC.OP.LS.rt].UW, RspGPR_Name(RSPOpC.OP.LS.rt));

	RSP_CPU_Message("   Done:");
	x86_SetBranch32b(Jump[1], RspRecompPos);
}

void CompileRsp_LWU(void) {
	CompileRsp_LW();
}

void CompileRsp_SB ( void ) {
	int Offset = (short)RSPOpC.OP.LS.offset;

	#ifndef Compile_GPRStores
	InterpreterFallback((void*)RSP_Opcode_SB,"RSP_Opcode_SB"); return;
	#endif

	RSP_CPU_Message("  %X %s",RspCompilePC,RSPOpcodeName(RSPOpC.OP.Hex,RspCompilePC));

	if (IsRspRegConst(RSPOpC.OP.LS.base) == TRUE) {
		DWORD Addr = (MipsRspRegConst(RSPOpC.OP.LS.base) + Offset);

		if (IsRspRegConst(RSPOpC.OP.LS.rt) == TRUE) {
			char Address[32];
			sprintf(Address, "Dmem + %Xh", Addr);
			MoveConstByteToVariable(&RspRecompPos, MipsRspRegConst(RSPOpC.OP.LS.rt) & 0xFF, DMEM + ((Addr & 0xFFF) ^ 3), Address);
		} else {
			MoveVariableToX86regByte(&RspRecompPos, &RSP_GPR[RSPOpC.OP.LS.rt].UW, RspGPR_Name(RSPOpC.OP.LS.rt), x86_EAX);
			char Address[32];
			sprintf(Address, "Dmem + %Xh", Addr);
			MoveX86regByteToVariable(&RspRecompPos, x86_EAX, DMEM + ((Addr & 0xFFF) ^ 3), Address);
		}
		return;
	}

	MoveVariableToX86reg(&RspRecompPos, &RSP_GPR[RSPOpC.OP.LS.base].UW, RspGPR_Name(RSPOpC.OP.LS.base), x86_EBX);

	if (Offset != 0) AddConstToX86Reg(&RspRecompPos, x86_EBX, Offset);
	XorConstToX86Reg(&RspRecompPos, x86_EBX, 3);
	AndConstToX86Reg(&RspRecompPos, x86_EBX, 0x0fff);

	if (IsRspRegConst(RSPOpC.OP.LS.rt)) {
		MoveConstByteToDMem(&RspRecompPos, MipsRspRegConst(RSPOpC.OP.LS.rt) & 0xFF, x86_EBX);
	}
	else {
		MoveVariableToX86regByte(&RspRecompPos, &RSP_GPR[RSPOpC.OP.LS.rt].UW, RspGPR_Name(RSPOpC.OP.LS.rt), x86_EAX);
		MoveX86regByteToDMem(&RspRecompPos, x86_EAX, x86_EBX);
	}
}

void CompileRsp_SH ( void ) {
	int Offset = (short)RSPOpC.OP.LS.offset;
	BYTE * Jump[2];

	#ifndef Compile_GPRStores
	InterpreterFallback((void*)RSP_Opcode_SH,"RSP_Opcode_SH"); return;
	#endif

	RSP_CPU_Message("  %X %s",RspCompilePC,RSPOpcodeName(RSPOpC.OP.Hex,RspCompilePC));

	if (IsRspRegConst(RSPOpC.OP.LS.base) == TRUE) {
		DWORD Addr = (MipsRspRegConst(RSPOpC.OP.LS.base) + Offset);

		if ((Addr & 1) != 0) {
			//RspCompilerWarning("Unaligned SH at constant address PC = %04X", RspCompilePC);

			if (IsRspRegConst(RSPOpC.OP.LS.rt)) {
				char Address[32];
				sprintf(Address, "Dmem + %Xh", Addr);
				MoveConstByteToVariable(&RspRecompPos, (MipsRspRegConst(RSPOpC.OP.LS.rt) >> 8) & 0xFF, DMEM + ((Addr & 0xFFF) ^ 3), Address);
				sprintf(Address, "Dmem + %Xh", Addr + 1);
				MoveConstByteToVariable(&RspRecompPos, MipsRspRegConst(RSPOpC.OP.LS.rt) & 0xFF, DMEM + (((Addr + 1) & 0xFFF) ^ 3), Address);
			} else {
				MoveVariableToX86regHalf(&RspRecompPos, &RSP_GPR[RSPOpC.OP.LS.rt].UW, RspGPR_Name(RSPOpC.OP.LS.rt), x86_EAX);
				char Address[32];
				sprintf(Address, "Dmem + %Xh", Addr);
				MoveX86regHighByteToVariable(&RspRecompPos, x86_EAX, DMEM + ((Addr & 0xFFF) ^ 3), Address);
				sprintf(Address, "Dmem + %Xh", Addr + 1);
				MoveX86regByteToVariable(&RspRecompPos, x86_EAX, DMEM + (((Addr + 1) & 0xFFF) ^ 3), Address);
			}

			return;
		} else {
			Addr ^= 2;
			Addr &= 0xfff;

			char Address[32];			
			sprintf(Address, "Dmem + %Xh", Addr);
			if (IsRspRegConst(RSPOpC.OP.LS.rt) == TRUE) {
				MoveConstHalfToVariable(&RspRecompPos, MipsRspRegConst(RSPOpC.OP.LS.rt) & 0xFFFF, DMEM + Addr, Address);
			} else {
				MoveVariableToX86regHalf(&RspRecompPos, &RSP_GPR[RSPOpC.OP.LS.rt].UW, RspGPR_Name(RSPOpC.OP.LS.rt), x86_EAX);
				MoveX86regHalfToVariable(&RspRecompPos, x86_EAX, DMEM + Addr, Address);
			}
			return;
		}
	}

	MoveVariableToX86reg(&RspRecompPos, &RSP_GPR[RSPOpC.OP.LS.base].UW, RspGPR_Name(RSPOpC.OP.LS.base), x86_EBX);
	if (Offset != 0) AddConstToX86Reg(&RspRecompPos, x86_EBX, Offset);

	if (RspCompiler.bAlignGPR == FALSE) {
		TestConstToX86Reg(&RspRecompPos, 1, x86_EBX);
		JneLabel32(&RspRecompPos, "Unaligned", 0);
		Jump[0] = RspRecompPos - 4;

		RspCompilerToggleBuffer();

		RSP_CPU_Message("   Unaligned:");
		x86_SetBranch32b(Jump[0], RspRecompPos);

		AndConstToX86Reg(&RspRecompPos, x86_EBX, 0xFFF);
		LeaSourceAndOffset(&RspRecompPos, x86_ECX, x86_EBX, 1);

		AndConstToX86RegHalf(&RspRecompPos, x86_ECX, 0xFFF);
		XorConstToX86Reg(&RspRecompPos, x86_EBX, 3);
		XorConstToX86Reg(&RspRecompPos, x86_ECX, 3);
		
		if (IsRspRegConst(RSPOpC.OP.LS.rt)) {
			MoveConstByteToDMem(&RspRecompPos, (MipsRspRegConst(RSPOpC.OP.LS.rt) >> 8) & 0xFF, x86_EBX);
			MoveConstByteToDMem(&RspRecompPos, MipsRspRegConst(RSPOpC.OP.LS.rt) & 0xFF, x86_ECX);
		} else {
			MoveVariableToX86regHalf(&RspRecompPos, &RSP_GPR[RSPOpC.OP.LS.rt].UW, RspGPR_Name(RSPOpC.OP.LS.rt), x86_EAX);
			MoveX86regHighByteToDMem(&RspRecompPos, x86_EAX, x86_EBX);
			MoveX86regByteToDMem(&RspRecompPos, x86_EAX, x86_ECX);
		}

		JmpLabel32(&RspRecompPos, "Done", 0);
		Jump[1] = RspRecompPos - 4;

		RspCompilerToggleBuffer();
	}

	XorConstToX86Reg(&RspRecompPos, x86_EBX, 2);
	AndConstToX86Reg(&RspRecompPos, x86_EBX, 0x0fff);

	if (IsRspRegConst(RSPOpC.OP.LS.rt)) {
		MoveConstHalfToDMem(&RspRecompPos, MipsRspRegConst(RSPOpC.OP.LS.rt) & 0xFFFF, x86_EBX);
	} else {
		MoveVariableToX86regHalf(&RspRecompPos, &RSP_GPR[RSPOpC.OP.LS.rt].UW, RspGPR_Name(RSPOpC.OP.LS.rt), x86_EAX);
		MoveX86regHalfToDMem(&RspRecompPos, x86_EAX, x86_EBX);
	}

	if (RspCompiler.bAlignGPR == FALSE) {
		RSP_CPU_Message("   Done:");
		x86_SetBranch32b(Jump[1], RspRecompPos);
	}
}

void CompileRsp_SW ( void ) {
	int Offset = (short)RSPOpC.OP.LS.offset;
	BYTE * Jump[2];

	#ifndef Compile_GPRStores
	InterpreterFallback((void*)RSP_Opcode_SW,"RSP_Opcode_SW"); return;
	#endif

	RSP_CPU_Message("  %X %s",RspCompilePC,RSPOpcodeName(RSPOpC.OP.Hex,RspCompilePC));

	if (IsRspRegConst(RSPOpC.OP.LS.base) == TRUE) {
		DWORD Addr = (MipsRspRegConst(RSPOpC.OP.LS.base) + Offset) & 0xfff;

		if ((Addr & 3) != 0) {
			//RspCompilerWarning("Unaligned SW at constant address PC = %04X", RspCompilePC);
			
			if (IsRspRegConst(RSPOpC.OP.LS.rt)) {
				char Address[32];
				sprintf(Address, "Dmem + %Xh", Addr);
				MoveConstByteToVariable(&RspRecompPos, (MipsRspRegConst(RSPOpC.OP.LS.rt) >> 24) & 0xFF, DMEM + ((Addr & 0xFFF) ^ 3), Address);
				sprintf(Address, "Dmem + %Xh", Addr + 1);
				MoveConstByteToVariable(&RspRecompPos, (MipsRspRegConst(RSPOpC.OP.LS.rt) >> 16) & 0xFF, DMEM + (((Addr + 1) & 0xFFF) ^ 3), Address);
				sprintf(Address, "Dmem + %Xh", Addr + 2);
				MoveConstByteToVariable(&RspRecompPos, (MipsRspRegConst(RSPOpC.OP.LS.rt) >> 8) & 0xFF, DMEM + (((Addr + 2) & 0xFFF) ^ 3), Address);
				sprintf(Address, "Dmem + %Xh", Addr + 3);
				MoveConstByteToVariable(&RspRecompPos, MipsRspRegConst(RSPOpC.OP.LS.rt) & 0xFF, DMEM + (((Addr + 3) & 0xFFF) ^ 3), Address);
			}
			else {
				MoveVariableToX86reg(&RspRecompPos, &RSP_GPR[RSPOpC.OP.LS.rt].UW, RspGPR_Name(RSPOpC.OP.LS.rt), x86_EAX);
				char Address[32];
				sprintf(Address, "Dmem + %Xh", Addr + 2);
				MoveX86regHighByteToVariable(&RspRecompPos, x86_EAX, DMEM + (((Addr + 2) & 0xFFF) ^ 3), Address);
				sprintf(Address, "Dmem + %Xh", Addr + 3);
				MoveX86regByteToVariable(&RspRecompPos, x86_EAX, DMEM + (((Addr + 3) & 0xFFF) ^ 3), Address);
				ShiftRightUnsignImmed(&RspRecompPos, x86_EAX, 16);
				sprintf(Address, "Dmem + %Xh", Addr);
				MoveX86regHighByteToVariable(&RspRecompPos, x86_EAX, DMEM + ((Addr & 0xFFF) ^ 3), Address);
				sprintf(Address, "Dmem + %Xh", Addr + 1);
				MoveX86regByteToVariable(&RspRecompPos, x86_EAX, DMEM + (((Addr + 1) & 0xFFF) ^ 3), Address);
			}
			return;
		} else {
			char Address[32];			
			sprintf(Address, "Dmem + %Xh", Addr);
			MoveVariableToX86reg(&RspRecompPos, &RSP_GPR[RSPOpC.OP.LS.rt].UW, RspGPR_Name(RSPOpC.OP.LS.rt), x86_EAX);
			MoveX86regToVariable(&RspRecompPos, x86_EAX, DMEM + Addr, Address);			
			return;
		}
	}
	
	MoveVariableToX86reg(&RspRecompPos, &RSP_GPR[RSPOpC.OP.LS.base].UW, RspGPR_Name(RSPOpC.OP.LS.base), x86_EBX);
	if (Offset != 0) AddConstToX86Reg(&RspRecompPos, x86_EBX, Offset);
	
	if (RspCompiler.bAlignGPR == FALSE) {
		TestConstToX86Reg(&RspRecompPos, 3, x86_EBX);
		JneLabel32(&RspRecompPos, "Unaligned", 0);
		Jump[0] = RspRecompPos - 4;

		RspCompilerToggleBuffer();

		RSP_CPU_Message("   Unaligned:");
		x86_SetBranch32b(Jump[0], RspRecompPos);

		AndConstToX86Reg(&RspRecompPos, x86_EBX, 0xFFF);
		LeaSourceAndOffset(&RspRecompPos, x86_ECX, x86_EBX, 1);
		LeaSourceAndOffset(&RspRecompPos, x86_EDX, x86_EBX, 2);
		LeaSourceAndOffset(&RspRecompPos, x86_ESI, x86_EBX, 3);

		AndConstToX86RegHalf(&RspRecompPos, x86_ECX, 0xFFF);
		AndConstToX86RegHalf(&RspRecompPos, x86_EDX, 0xFFF);
		AndConstToX86RegHalf(&RspRecompPos, x86_ESI, 0xFFF);
		XorConstToX86Reg(&RspRecompPos, x86_EBX, 3);
		XorConstToX86Reg(&RspRecompPos, x86_ECX, 3);
		XorConstToX86Reg(&RspRecompPos, x86_EDX, 3);
		XorConstToX86Reg(&RspRecompPos, x86_ESI, 3);

		if (IsRspRegConst(RSPOpC.OP.LS.rt)) {
			MoveConstByteToDMem(&RspRecompPos, (MipsRspRegConst(RSPOpC.OP.LS.rt) >> 24) & 0xFF, x86_EBX);
			MoveConstByteToDMem(&RspRecompPos, (MipsRspRegConst(RSPOpC.OP.LS.rt) >> 16) & 0xFF, x86_ECX);
			MoveConstByteToDMem(&RspRecompPos, (MipsRspRegConst(RSPOpC.OP.LS.rt) >> 8) & 0xFF, x86_EDX);
			MoveConstByteToDMem(&RspRecompPos, MipsRspRegConst(RSPOpC.OP.LS.rt) & 0xFF, x86_ESI);
		}
		else {
			MoveVariableToX86reg(&RspRecompPos, &RSP_GPR[RSPOpC.OP.LS.rt].UW, RspGPR_Name(RSPOpC.OP.LS.rt), x86_EAX);
			MoveX86regHighByteToDMem(&RspRecompPos, x86_EAX, x86_EDX);
			MoveX86regByteToDMem(&RspRecompPos, x86_EAX, x86_ESI);
			ShiftRightUnsignImmed(&RspRecompPos, x86_EAX, 16);
			MoveX86regHighByteToDMem(&RspRecompPos, x86_EAX, x86_EBX);
			MoveX86regByteToDMem(&RspRecompPos, x86_EAX, x86_ECX);
		}

		JmpLabel32(&RspRecompPos, "Done", 0);
		Jump[1] = RspRecompPos - 4;

		RspCompilerToggleBuffer();
	}

	AndConstToX86Reg(&RspRecompPos, x86_EBX, 0x0fff);
	if (IsRspRegConst(RSPOpC.OP.LS.rt)) {
		MoveConstToDMem(&RspRecompPos, MipsRspRegConst(RSPOpC.OP.LS.rt), x86_EBX);
	} else {
		MoveVariableToX86reg(&RspRecompPos, &RSP_GPR[RSPOpC.OP.LS.rt].UW, RspGPR_Name(RSPOpC.OP.LS.rt), x86_EAX);
		MoveX86regToDMem(&RspRecompPos, x86_EAX, x86_EBX);
	}

	if (RspCompiler.bAlignGPR == FALSE) {
		RSP_CPU_Message("   Done:");
		x86_SetBranch32b(Jump[1], RspRecompPos);
	}
}

void CompileRsp_LC2 (void) {
	((void (*)()) RSP_Lc2 [ RSPOpC.OP.R.rd ])();
}

void CompileRsp_SC2 (void) {
	((void (*)()) RSP_Sc2 [ RSPOpC.OP.R.rd ])();
}
/********************** R4300i OpCodes: Special **********************/

void CompileRsp_Special_SLL ( void ) {
	#ifndef Compile_Special
	InterpreterFallback((void*)RSP_Special_SLL,"RSP_Special_SLL"); return;
	#endif

	RSP_CPU_Message("  %X %s",RspCompilePC,RSPOpcodeName(RSPOpC.OP.Hex,RspCompilePC));
	if (RSPOpC.OP.R.rd == 0) return;

	if (RSPOpC.OP.R.rd == RSPOpC.OP.R.rt) {
		ShiftLeftSignVariableImmed(&RspRecompPos, &RSP_GPR[RSPOpC.OP.R.rd].W, RspGPR_Name(RSPOpC.OP.R.rd), (BYTE)RSPOpC.OP.R.sa);
	} else if (IsRspRegConst(RSPOpC.OP.R.rt)) {
		MoveConstToVariable(&RspRecompPos, (long)MipsRspRegConst(RSPOpC.OP.R.rt) << RSPOpC.OP.R.sa, &RSP_GPR[RSPOpC.OP.R.rd].W, RspGPR_Name(RSPOpC.OP.R.rd));
	} else {
		MoveVariableToX86reg(&RspRecompPos, &RSP_GPR[RSPOpC.OP.R.rt].W, RspGPR_Name(RSPOpC.OP.R.rt), x86_EAX);
		ShiftLeftSignImmed(&RspRecompPos, x86_EAX, (BYTE)RSPOpC.OP.R.sa);
		MoveX86regToVariable(&RspRecompPos, x86_EAX, &RSP_GPR[RSPOpC.OP.R.rd].W, RspGPR_Name(RSPOpC.OP.R.rd));
	}
}

void CompileRsp_Special_SRL ( void ) {
	#ifndef Compile_Special
	InterpreterFallback((void*)RSP_Special_SRL,"RSP_Special_SRL"); return;
	#endif

	RSP_CPU_Message("  %X %s",RspCompilePC,RSPOpcodeName(RSPOpC.OP.Hex,RspCompilePC));
	if (RSPOpC.OP.R.rd == 0) return;

	if (RSPOpC.OP.R.rd == RSPOpC.OP.R.rt) {
		ShiftRightUnsignVariableImmed(&RspRecompPos, &RSP_GPR[RSPOpC.OP.R.rd].W, RspGPR_Name(RSPOpC.OP.R.rd), (BYTE)RSPOpC.OP.R.sa);
	} else if (IsRspRegConst(RSPOpC.OP.R.rt)) {
		MoveConstToVariable(&RspRecompPos, (unsigned long)MipsRspRegConst(RSPOpC.OP.R.rt) >> RSPOpC.OP.R.sa, &RSP_GPR[RSPOpC.OP.R.rd].W, RspGPR_Name(RSPOpC.OP.R.rd));
	} else {
		MoveVariableToX86reg(&RspRecompPos, &RSP_GPR[RSPOpC.OP.R.rt].W, RspGPR_Name(RSPOpC.OP.R.rt), x86_EAX);
		ShiftRightUnsignImmed(&RspRecompPos, x86_EAX, (BYTE)RSPOpC.OP.R.sa);
		MoveX86regToVariable(&RspRecompPos, x86_EAX, &RSP_GPR[RSPOpC.OP.R.rd].W, RspGPR_Name(RSPOpC.OP.R.rd));
	}
}

void CompileRsp_Special_SRA ( void ) {
	#ifndef Compile_Special
	InterpreterFallback((void*)RSP_Special_SRA,"RSP_Special_SRA"); return;
	#endif

	RSP_CPU_Message("  %X %s",RspCompilePC,RSPOpcodeName(RSPOpC.OP.Hex,RspCompilePC));
	if (RSPOpC.OP.R.rd == 0) return;

	if (RSPOpC.OP.R.rd == RSPOpC.OP.R.rt) {
		ShiftRightSignVariableImmed(&RspRecompPos, &RSP_GPR[RSPOpC.OP.R.rd].W, RspGPR_Name(RSPOpC.OP.R.rd), (BYTE)RSPOpC.OP.R.sa);
	} else if (IsRspRegConst(RSPOpC.OP.R.rt)) {
		MoveConstToVariable(&RspRecompPos, (long)MipsRspRegConst(RSPOpC.OP.R.rt) >> RSPOpC.OP.R.sa, &RSP_GPR[RSPOpC.OP.R.rd].W, RspGPR_Name(RSPOpC.OP.R.rd));
	} else {
		MoveVariableToX86reg(&RspRecompPos, &RSP_GPR[RSPOpC.OP.R.rt].W, RspGPR_Name(RSPOpC.OP.R.rt), x86_EAX);
		ShiftRightSignImmed(&RspRecompPos, x86_EAX, (BYTE)RSPOpC.OP.R.sa);
		MoveX86regToVariable(&RspRecompPos, x86_EAX, &RSP_GPR[RSPOpC.OP.R.rd].W, RspGPR_Name(RSPOpC.OP.R.rd));
	}
}

void CompileRsp_Special_SLLV ( void ) {
	#ifndef Compile_SLLV
	InterpreterFallback((void*)RSP_Special_SLLV, "RSP_Special_SLLV"); return;
	#endif

	RSP_CPU_Message("  %X %s", RspCompilePC, RSPOpcodeName(RSPOpC.OP.Hex, RspCompilePC));
	if (RSPOpC.OP.R.rd == 0) return;

	if (RSPOpC.OP.R.rt == 0) {
		MoveConstToVariable(&RspRecompPos, 0, &RSP_GPR[RSPOpC.OP.R.rd].W, RspGPR_Name(RSPOpC.OP.R.rd));
}
	else if (IsRspRegConst(RSPOpC.OP.R.rt)) {
		if (IsRspRegConst(RSPOpC.OP.R.rs)) {
			int shift = MipsRspRegConst(RSPOpC.OP.R.rs) & 0x1F;
			MoveConstToVariable(&RspRecompPos, (long)MipsRspRegConst(RSPOpC.OP.R.rt) << shift, &RSP_GPR[RSPOpC.OP.R.rd].W, RspGPR_Name(RSPOpC.OP.R.rd));
		}
		else {
			MoveConstToX86reg(&RspRecompPos, MipsRspRegConst(RSPOpC.OP.R.rt), x86_EAX);
			MoveVariableToX86reg(&RspRecompPos, &RSP_GPR[RSPOpC.OP.R.rs].W, RspGPR_Name(RSPOpC.OP.R.rs), x86_ECX);
			ShiftLeftSign(&RspRecompPos, x86_EAX);
			MoveX86regToVariable(&RspRecompPos, x86_EAX, &RSP_GPR[RSPOpC.OP.R.rd].W, RspGPR_Name(RSPOpC.OP.R.rd));
		}
	}
	else if (IsRspRegConst(RSPOpC.OP.R.rs)) {
		if (RSPOpC.OP.R.rt == RSPOpC.OP.R.rd) {
			ShiftLeftSignVariableImmed(&RspRecompPos, &RSP_GPR[RSPOpC.OP.R.rd].W, RspGPR_Name(RSPOpC.OP.R.rd), MipsRspRegConst(RSPOpC.OP.R.rs) & 0x1F);
		}
		else {
			MoveVariableToX86reg(&RspRecompPos, &RSP_GPR[RSPOpC.OP.R.rt].W, RspGPR_Name(RSPOpC.OP.R.rt), x86_EAX);
			ShiftLeftSignImmed(&RspRecompPos, x86_EAX, MipsRspRegConst(RSPOpC.OP.R.rs) & 0x1F);
			MoveX86regToVariable(&RspRecompPos, x86_EAX, &RSP_GPR[RSPOpC.OP.R.rd].W, RspGPR_Name(RSPOpC.OP.R.rd));
		}
	}
	else {
		if (RSPOpC.OP.R.rt == RSPOpC.OP.R.rd) {
			MoveVariableToX86reg(&RspRecompPos, &RSP_GPR[RSPOpC.OP.R.rs].W, RspGPR_Name(RSPOpC.OP.R.rs), x86_ECX);
			ShiftLeftSignVariable(&RspRecompPos, &RSP_GPR[RSPOpC.OP.R.rd].W, RspGPR_Name(RSPOpC.OP.R.rd));
		}
		else {
			MoveVariableToX86reg(&RspRecompPos, &RSP_GPR[RSPOpC.OP.R.rt].W, RspGPR_Name(RSPOpC.OP.R.rt), x86_EAX);
			MoveVariableToX86reg(&RspRecompPos, &RSP_GPR[RSPOpC.OP.R.rs].W, RspGPR_Name(RSPOpC.OP.R.rs), x86_ECX);
			ShiftLeftSign(&RspRecompPos, x86_EAX);
			MoveX86regToVariable(&RspRecompPos, x86_EAX, &RSP_GPR[RSPOpC.OP.R.rd].W, RspGPR_Name(RSPOpC.OP.R.rd));
		}
	}
}

void CompileRsp_Special_SRLV ( void ) {
	#ifndef Compile_Special
	InterpreterFallback((void*)RSP_Special_SRLV,"RSP_Special_SRLV"); return;
	#endif

	RSP_CPU_Message("  %X %s",RspCompilePC,RSPOpcodeName(RSPOpC.OP.Hex,RspCompilePC));
	if (RSPOpC.OP.R.rd == 0) return;

	if (RSPOpC.OP.R.rt == 0) {
		MoveConstToVariable(&RspRecompPos, 0, &RSP_GPR[RSPOpC.OP.R.rd].W, RspGPR_Name(RSPOpC.OP.R.rd));
	} else if(IsRspRegConst(RSPOpC.OP.R.rt)) {
		if (IsRspRegConst(RSPOpC.OP.R.rs)) {
			int shift = MipsRspRegConst(RSPOpC.OP.R.rs) & 0x1F;
			MoveConstToVariable(&RspRecompPos, (unsigned long)MipsRspRegConst(RSPOpC.OP.R.rt) >> shift, &RSP_GPR[RSPOpC.OP.R.rd].W, RspGPR_Name(RSPOpC.OP.R.rd));
		} else {
			MoveConstToX86reg(&RspRecompPos, MipsRspRegConst(RSPOpC.OP.R.rt), x86_EAX);
			MoveVariableToX86reg(&RspRecompPos, &RSP_GPR[RSPOpC.OP.R.rs].W, RspGPR_Name(RSPOpC.OP.R.rs), x86_ECX);
			ShiftRightUnsign(&RspRecompPos, x86_EAX);
			MoveX86regToVariable(&RspRecompPos, x86_EAX, &RSP_GPR[RSPOpC.OP.R.rd].W, RspGPR_Name(RSPOpC.OP.R.rd));
		}
	} else if (IsRspRegConst(RSPOpC.OP.R.rs)) {
		if (RSPOpC.OP.R.rt == RSPOpC.OP.R.rd) {
			ShiftRightUnsignVariableImmed(&RspRecompPos, &RSP_GPR[RSPOpC.OP.R.rd].W, RspGPR_Name(RSPOpC.OP.R.rd), MipsRspRegConst(RSPOpC.OP.R.rs) & 0x1F);
		} else {
			MoveVariableToX86reg(&RspRecompPos, &RSP_GPR[RSPOpC.OP.R.rt].W, RspGPR_Name(RSPOpC.OP.R.rt), x86_EAX);
			ShiftRightUnsignImmed(&RspRecompPos, x86_EAX, MipsRspRegConst(RSPOpC.OP.R.rs) & 0x1F);
			MoveX86regToVariable(&RspRecompPos, x86_EAX, &RSP_GPR[RSPOpC.OP.R.rd].W, RspGPR_Name(RSPOpC.OP.R.rd));
		}
	} else {
		if (RSPOpC.OP.R.rt == RSPOpC.OP.R.rd) {
			MoveVariableToX86reg(&RspRecompPos, &RSP_GPR[RSPOpC.OP.R.rs].W, RspGPR_Name(RSPOpC.OP.R.rs), x86_ECX);
			ShiftRightUnsignVariable(&RspRecompPos, &RSP_GPR[RSPOpC.OP.R.rd].W, RspGPR_Name(RSPOpC.OP.R.rd));
		} else {
			MoveVariableToX86reg(&RspRecompPos, &RSP_GPR[RSPOpC.OP.R.rt].W, RspGPR_Name(RSPOpC.OP.R.rt), x86_EAX);
			MoveVariableToX86reg(&RspRecompPos, &RSP_GPR[RSPOpC.OP.R.rs].W, RspGPR_Name(RSPOpC.OP.R.rs), x86_ECX);
			ShiftRightUnsign(&RspRecompPos, x86_EAX);
			MoveX86regToVariable(&RspRecompPos, x86_EAX, &RSP_GPR[RSPOpC.OP.R.rd].W, RspGPR_Name(RSPOpC.OP.R.rd));
		}
	}
}

void CompileRsp_Special_SRAV ( void ) {
	#ifndef Compile_SRAV
	InterpreterFallback((void*)RSP_Special_SRAV, "RSP_Special_SRAV"); return;
	#endif

	RSP_CPU_Message("  %X %s", RspCompilePC, RSPOpcodeName(RSPOpC.OP.Hex, RspCompilePC));
	if (RSPOpC.OP.R.rd == 0) return;

	if (RSPOpC.OP.R.rt == 0) {
		MoveConstToVariable(&RspRecompPos, 0, &RSP_GPR[RSPOpC.OP.R.rd].W, RspGPR_Name(RSPOpC.OP.R.rd));
	}
	else if (IsRspRegConst(RSPOpC.OP.R.rt)) {
		if (IsRspRegConst(RSPOpC.OP.R.rs)) {
			int shift = MipsRspRegConst(RSPOpC.OP.R.rs) & 0x1F;
			MoveConstToVariable(&RspRecompPos, (long)MipsRspRegConst(RSPOpC.OP.R.rt) >> shift, &RSP_GPR[RSPOpC.OP.R.rd].W, RspGPR_Name(RSPOpC.OP.R.rd));
		}
		else {
			MoveConstToX86reg(&RspRecompPos, MipsRspRegConst(RSPOpC.OP.R.rt), x86_EAX);
			MoveVariableToX86reg(&RspRecompPos, &RSP_GPR[RSPOpC.OP.R.rs].W, RspGPR_Name(RSPOpC.OP.R.rs), x86_ECX);
			ShiftRightSign(&RspRecompPos, x86_EAX);
			MoveX86regToVariable(&RspRecompPos, x86_EAX, &RSP_GPR[RSPOpC.OP.R.rd].W, RspGPR_Name(RSPOpC.OP.R.rd));
		}
	}
	else if (IsRspRegConst(RSPOpC.OP.R.rs)) {
		if (RSPOpC.OP.R.rt == RSPOpC.OP.R.rd) {
			ShiftRightSignVariableImmed(&RspRecompPos, &RSP_GPR[RSPOpC.OP.R.rd].W, RspGPR_Name(RSPOpC.OP.R.rd), MipsRspRegConst(RSPOpC.OP.R.rs) & 0x1F);
		}
		else {
			MoveVariableToX86reg(&RspRecompPos, &RSP_GPR[RSPOpC.OP.R.rt].W, RspGPR_Name(RSPOpC.OP.R.rt), x86_EAX);
			ShiftRightSignImmed(&RspRecompPos, x86_EAX, MipsRspRegConst(RSPOpC.OP.R.rs) & 0x1F);
			MoveX86regToVariable(&RspRecompPos, x86_EAX, &RSP_GPR[RSPOpC.OP.R.rd].W, RspGPR_Name(RSPOpC.OP.R.rd));
		}
	}
	else {
		if (RSPOpC.OP.R.rt == RSPOpC.OP.R.rd) {
			MoveVariableToX86reg(&RspRecompPos, &RSP_GPR[RSPOpC.OP.R.rs].W, RspGPR_Name(RSPOpC.OP.R.rs), x86_ECX);
			ShiftRightSignVariable(&RspRecompPos, &RSP_GPR[RSPOpC.OP.R.rd].W, RspGPR_Name(RSPOpC.OP.R.rd));
		}
		else {
			MoveVariableToX86reg(&RspRecompPos, &RSP_GPR[RSPOpC.OP.R.rt].W, RspGPR_Name(RSPOpC.OP.R.rt), x86_EAX);
			MoveVariableToX86reg(&RspRecompPos, &RSP_GPR[RSPOpC.OP.R.rs].W, RspGPR_Name(RSPOpC.OP.R.rs), x86_ECX);
			ShiftRightSign(&RspRecompPos, x86_EAX);
			MoveX86regToVariable(&RspRecompPos, x86_EAX, &RSP_GPR[RSPOpC.OP.R.rd].W, RspGPR_Name(RSPOpC.OP.R.rd));
		}
	}
}

void CompileRsp_Special_JR (void) {
	static DWORD Target = 0;
	static BOOL bDelayAffect = FALSE;
	BYTE * Jump;

	if (IsRspDelaySlotBranch(RspCompilePC)) {
		CompileRsp_ConsecutiveDelaySlots();
		return;
	}

	if ( RSP_NextInstruction == NORMAL ) {
		RSP_CPU_Message("  %X %s",RspCompilePC,RSPOpcodeName(RSPOpC.OP.Hex,RspCompilePC));
		/* transfer destination to location pointed to by PrgCount */
		bDelayAffect = RspDelaySlotAffectBranch(RspCompilePC);
		if (!IsRspRegConst(RSPOpC.OP.R.rs) && bDelayAffect) {
			MoveVariableToX86reg(&RspRecompPos, &RSP_GPR[RSPOpC.OP.R.rs].W, RspGPR_Name(RSPOpC.OP.R.rs), x86_EAX);
			AndConstToX86Reg(&RspRecompPos, x86_EAX, 0xFFC);
			MoveX86regToVariable(&RspRecompPos, x86_EAX, &Target, "Target");
		}
		RSP_NextInstruction = DO_DELAY_SLOT;
	} else if ( RSP_NextInstruction == DELAY_SLOT_DONE ) {
		if (IsRspRegConst(RSPOpC.OP.R.rs)) {
			MoveConstToX86reg(&RspRecompPos, MipsRspRegConst(RSPOpC.OP.R.rs) & 0xFFC, x86_EAX);
		} else if (!bDelayAffect) {
			MoveVariableToX86reg(&RspRecompPos, &RSP_GPR[RSPOpC.OP.R.rs].W, RspGPR_Name(RSPOpC.OP.R.rs), x86_EAX);
			AndConstToX86Reg(&RspRecompPos, x86_EAX, 0xFFC);
		} else {
			MoveVariableToX86reg(&RspRecompPos, &Target, "Target", x86_EAX);
		}
		MoveX86RegToX86Reg(&RspRecompPos, x86_EAX, x86_EBX);
		AddVariableToX86reg(&RspRecompPos, x86_EAX, &RspJumpTable, "RspJumpTable");
		MoveX86PointerToX86reg(&RspRecompPos, x86_EAX, x86_EAX);

		TestX86RegToX86Reg(&RspRecompPos, x86_EAX, x86_EAX);
		JeLabel8(&RspRecompPos, "Null", 0);
		Jump = RspRecompPos - 1;
		JmpDirectReg(&RspRecompPos, x86_EAX);

		x86_SetBranch8b(Jump, RspRecompPos);
		RSP_CPU_Message(" Null:");
		MoveX86regToVariable(&RspRecompPos, x86_EBX, &SP_PC_REG, "RSP PC");
		Ret(&RspRecompPos);
		RSP_NextInstruction = FINISH_BLOCK;
	} else {
		RspCompilerWarning("WTF\n\nJR\nNextInstruction = %X", RSP_NextInstruction);
		BreakPoint(&RspRecompPos);
	}
}

void CompileRsp_Special_JALR ( void ) {
	static DWORD Target = 0;
	static BOOL bDelayAffect = FALSE;
	BYTE * Jump;
	DWORD Const = (RspCompilePC + 8) & 0xFFC;

	if (IsRspDelaySlotBranch(RspCompilePC)) {
		CompileRsp_ConsecutiveDelaySlots();
		return;
	}

	if (RSP_NextInstruction == NORMAL) {
		RSP_CPU_Message("  %X %s",RspCompilePC,RSPOpcodeName(RSPOpC.OP.Hex,RspCompilePC));
		bDelayAffect = RspDelaySlotAffectBranch(RspCompilePC) || RSPOpC.OP.R.rd == RSPOpC.OP.R.rs;
		if (!IsRspRegConst(RSPOpC.OP.R.rs) && bDelayAffect) {
			MoveVariableToX86reg(&RspRecompPos, &RSP_GPR[RSPOpC.OP.R.rs].W, RspGPR_Name(RSPOpC.OP.R.rs), x86_EAX);
			AndConstToX86Reg(&RspRecompPos, x86_EAX, 0xFFC);
			MoveX86regToVariable(&RspRecompPos, x86_EAX, &Target, "Target");
		}
		MoveConstToVariable(&RspRecompPos, Const, &RSP_GPR[RSPOpC.OP.R.rd].W, RspGPR_Name(RSPOpC.OP.R.rd));
		RSP_NextInstruction = DO_DELAY_SLOT;
	} else if (RSP_NextInstruction == DELAY_SLOT_DONE) {
		if (IsRspRegConst(RSPOpC.OP.R.rs)) {
			MoveConstToX86reg(&RspRecompPos, MipsRspRegConst(RSPOpC.OP.R.rs) & 0xFFC, x86_EAX);
		} else if (!bDelayAffect) {
			MoveVariableToX86reg(&RspRecompPos, &RSP_GPR[RSPOpC.OP.R.rs].W, RspGPR_Name(RSPOpC.OP.R.rs), x86_EAX);
			AndConstToX86Reg(&RspRecompPos, x86_EAX, 0xFFC);
		} else {
			MoveVariableToX86reg(&RspRecompPos, &Target, "Target", x86_EAX);
		}
		MoveX86RegToX86Reg(&RspRecompPos, x86_EAX, x86_EBX);
		AddVariableToX86reg(&RspRecompPos, x86_EAX, &RspJumpTable, "JumpTable");
		MoveX86PointerToX86reg(&RspRecompPos, x86_EAX, x86_EAX);

		TestX86RegToX86Reg(&RspRecompPos, x86_EAX, x86_EAX);
		JeLabel8(&RspRecompPos, "Null", 0);
		Jump = RspRecompPos - 1;
		JmpDirectReg(&RspRecompPos, x86_EAX);

		x86_SetBranch8b(Jump, RspRecompPos);
		RSP_CPU_Message(" Null:");
		MoveX86regToVariable(&RspRecompPos, x86_EBX, &SP_PC_REG, "RSP PC");
		Ret(&RspRecompPos);
		RSP_NextInstruction = FINISH_SUB_BLOCK;
	} else {
		RspCompilerWarning("WTF\n\nJALR\nNextInstruction = %X", RSP_NextInstruction);
		BreakPoint(&RspRecompPos);
	}
}

void CompileRsp_Special_BREAK ( void ) {
	#ifndef Compile_BREAK
	InterpreterFallback((void*)RSP_Special_BREAK, "RSP_Special_BREAK"); return;
	#endif

	MoveConstByteToVariable(&RspRecompPos, 0, &RSP_Running, "RSP_Running");
	OrConstToVariable(&RspRecompPos, (SP_STATUS_HALT | SP_STATUS_BROKE), &SP_STATUS_REG, "SP_STATUS_REG");
	TestVariable(&RspRecompPos, SP_STATUS_INTR_BREAK, &SP_STATUS_REG, "SP_STATUS_REG");
	JeLabel8(&RspRecompPos, "NoSpInterrupt", 0);
	BYTE* Jump = RspRecompPos - 1;

	OrConstToVariable(&RspRecompPos, MI_INTR_SP, &MI_INTR_REG, "MI_INTR_REG");
	Call_Direct(&RspRecompPos, (void*)CheckInterrupts, "CheckInterrupts");

	x86_SetBranch8b(Jump, RspRecompPos);
	RSP_CPU_Message(" NoSpInterrupt:");

	if (RSP_NextInstruction != NORMAL && RSP_NextInstruction != DELAY_SLOT) {
		DisplayError("Compile_Special_BREAK: problem");
	}
	if (RSP_NextInstruction == DELAY_SLOT) {
		return;
	}
	MoveConstToVariable(&RspRecompPos, (RspCompilePC + 4) & 0xFFF,&SP_PC_REG,"RSP PC");
	Ret(&RspRecompPos);
	RSP_NextInstruction = FINISH_BLOCK;
}

void CompileRsp_Special_ADD ( void ) {
	#ifndef Compile_Special
	InterpreterFallback((void*)RSP_Special_ADD,"RSP_Special_ADD"); return;
	#endif

	RSP_CPU_Message("  %X %s",RspCompilePC,RSPOpcodeName(RSPOpC.OP.Hex,RspCompilePC));

	if (RSPOpC.OP.R.rd == 0) return;

	if (RSPOpC.OP.R.rd == RSPOpC.OP.R.rs) {
		if (IsRspRegConst(RSPOpC.OP.R.rt)) {
			AddConstToVariable(&RspRecompPos, MipsRspRegConst(RSPOpC.OP.R.rt), &RSP_GPR[RSPOpC.OP.R.rd].W, RspGPR_Name(RSPOpC.OP.R.rd));
		} else {
			MoveVariableToX86reg(&RspRecompPos, &RSP_GPR[RSPOpC.OP.R.rt].W, RspGPR_Name(RSPOpC.OP.R.rt), x86_EAX);
			AddX86regToVariable(&RspRecompPos, x86_EAX, &RSP_GPR[RSPOpC.OP.R.rd].W, RspGPR_Name(RSPOpC.OP.R.rd));
		}
	} else if (RSPOpC.OP.R.rd == RSPOpC.OP.R.rt) {
		if (IsRspRegConst(RSPOpC.OP.R.rs)) {
			AddConstToVariable(&RspRecompPos, MipsRspRegConst(RSPOpC.OP.R.rs), &RSP_GPR[RSPOpC.OP.R.rd].W, RspGPR_Name(RSPOpC.OP.R.rd));
		} else {
			MoveVariableToX86reg(&RspRecompPos, &RSP_GPR[RSPOpC.OP.R.rs].W, RspGPR_Name(RSPOpC.OP.R.rs), x86_EAX);
			AddX86regToVariable(&RspRecompPos, x86_EAX, &RSP_GPR[RSPOpC.OP.R.rd].W, RspGPR_Name(RSPOpC.OP.R.rd));
		}
	} else if (RSPOpC.OP.R.rs == RSPOpC.OP.R.rt) {
		if (IsRspRegConst(RSPOpC.OP.R.rs)) {
			MoveConstToVariable(&RspRecompPos, MipsRspRegConst(RSPOpC.OP.R.rs) + MipsRspRegConst(RSPOpC.OP.R.rs), &RSP_GPR[RSPOpC.OP.R.rd].W, RspGPR_Name(RSPOpC.OP.R.rd));
		} else {
			MoveVariableToX86reg(&RspRecompPos, &RSP_GPR[RSPOpC.OP.R.rs].W, RspGPR_Name(RSPOpC.OP.R.rs), x86_EAX);
			AddX86RegToX86Reg(&RspRecompPos, x86_EAX, x86_EAX);
			MoveX86regToVariable(&RspRecompPos, x86_EAX, &RSP_GPR[RSPOpC.OP.R.rd].W, RspGPR_Name(RSPOpC.OP.R.rd));
		}
	} else if (RSPOpC.OP.R.rs == 0) {
		if (IsRspRegConst(RSPOpC.OP.R.rt)) {
			MoveConstToVariable(&RspRecompPos, MipsRspRegConst(RSPOpC.OP.R.rt), &RSP_GPR[RSPOpC.OP.R.rd].W, RspGPR_Name(RSPOpC.OP.R.rd));
		} else {
			MoveVariableToX86reg(&RspRecompPos, &RSP_GPR[RSPOpC.OP.R.rt].W, RspGPR_Name(RSPOpC.OP.R.rt), x86_EAX);
			MoveX86regToVariable(&RspRecompPos, x86_EAX, &RSP_GPR[RSPOpC.OP.R.rd].W, RspGPR_Name(RSPOpC.OP.R.rd));
		}
	} else if (RSPOpC.OP.R.rt == 0) {
		if (IsRspRegConst(RSPOpC.OP.R.rs)) {
			MoveConstToVariable(&RspRecompPos, MipsRspRegConst(RSPOpC.OP.R.rs), &RSP_GPR[RSPOpC.OP.R.rd].W, RspGPR_Name(RSPOpC.OP.R.rd));
		} else {
			MoveVariableToX86reg(&RspRecompPos, &RSP_GPR[RSPOpC.OP.R.rs].W, RspGPR_Name(RSPOpC.OP.R.rs), x86_EAX);
			MoveX86regToVariable(&RspRecompPos, x86_EAX, &RSP_GPR[RSPOpC.OP.R.rd].W, RspGPR_Name(RSPOpC.OP.R.rd));
		}
	} else if (IsRspRegConst(RSPOpC.OP.R.rs) && IsRspRegConst(RSPOpC.OP.R.rt)) {
		MoveConstToVariable(&RspRecompPos, MipsRspRegConst(RSPOpC.OP.R.rs) + MipsRspRegConst(RSPOpC.OP.R.rt), &RSP_GPR[RSPOpC.OP.R.rd].W, RspGPR_Name(RSPOpC.OP.R.rd));
	} else if (IsRspRegConst(RSPOpC.OP.R.rs)) {
		MoveVariableToX86reg(&RspRecompPos, &RSP_GPR[RSPOpC.OP.R.rt].W, RspGPR_Name(RSPOpC.OP.R.rt), x86_EAX);
		AddConstToX86Reg(&RspRecompPos, x86_EAX, MipsRspRegConst(RSPOpC.OP.R.rs));
		MoveX86regToVariable(&RspRecompPos, x86_EAX, &RSP_GPR[RSPOpC.OP.R.rd].W, RspGPR_Name(RSPOpC.OP.R.rd));
	} else if (IsRspRegConst(RSPOpC.OP.R.rt)) {
		MoveVariableToX86reg(&RspRecompPos, &RSP_GPR[RSPOpC.OP.R.rs].W, RspGPR_Name(RSPOpC.OP.R.rs), x86_EAX);
		AddConstToX86Reg(&RspRecompPos, x86_EAX, MipsRspRegConst(RSPOpC.OP.R.rt));
		MoveX86regToVariable(&RspRecompPos, x86_EAX, &RSP_GPR[RSPOpC.OP.R.rd].W, RspGPR_Name(RSPOpC.OP.R.rd));
	} else {
		MoveVariableToX86reg(&RspRecompPos, &RSP_GPR[RSPOpC.OP.R.rs].W, RspGPR_Name(RSPOpC.OP.R.rs), x86_EAX);
		AddVariableToX86reg(&RspRecompPos, x86_EAX, &RSP_GPR[RSPOpC.OP.R.rt].W, RspGPR_Name(RSPOpC.OP.R.rt));
		MoveX86regToVariable(&RspRecompPos, x86_EAX, &RSP_GPR[RSPOpC.OP.R.rd].W, RspGPR_Name(RSPOpC.OP.R.rd));
	}
}

void CompileRsp_Special_ADDU ( void ) {
	#ifndef Compile_Special
	InterpreterFallback((void*)RSP_Special_ADDU,"RSP_Special_ADDU"); return;
	#endif

	RSP_CPU_Message("  %X %s",RspCompilePC,RSPOpcodeName(RSPOpC.OP.Hex,RspCompilePC));

	if (RSPOpC.OP.R.rd == 0) return;

	if (RSPOpC.OP.R.rd == RSPOpC.OP.R.rs) {
		if (IsRspRegConst(RSPOpC.OP.R.rt)) {
			AddConstToVariable(&RspRecompPos, MipsRspRegConst(RSPOpC.OP.R.rt), &RSP_GPR[RSPOpC.OP.R.rd].W, RspGPR_Name(RSPOpC.OP.R.rd));
		} else {
			MoveVariableToX86reg(&RspRecompPos, &RSP_GPR[RSPOpC.OP.R.rt].W, RspGPR_Name(RSPOpC.OP.R.rt), x86_EAX);
			AddX86regToVariable(&RspRecompPos, x86_EAX, &RSP_GPR[RSPOpC.OP.R.rd].W, RspGPR_Name(RSPOpC.OP.R.rd));
		}
	} else if (RSPOpC.OP.R.rd == RSPOpC.OP.R.rt) {
		if (IsRspRegConst(RSPOpC.OP.R.rs)) {
			AddConstToVariable(&RspRecompPos, MipsRspRegConst(RSPOpC.OP.R.rs), &RSP_GPR[RSPOpC.OP.R.rd].W, RspGPR_Name(RSPOpC.OP.R.rd));
		} else {
			MoveVariableToX86reg(&RspRecompPos, &RSP_GPR[RSPOpC.OP.R.rs].W, RspGPR_Name(RSPOpC.OP.R.rs), x86_EAX);
			AddX86regToVariable(&RspRecompPos, x86_EAX, &RSP_GPR[RSPOpC.OP.R.rd].W, RspGPR_Name(RSPOpC.OP.R.rd));
		}
	} else if (RSPOpC.OP.R.rs == RSPOpC.OP.R.rt) {
		if (IsRspRegConst(RSPOpC.OP.R.rs)) {
			MoveConstToVariable(&RspRecompPos, MipsRspRegConst(RSPOpC.OP.R.rs) + MipsRspRegConst(RSPOpC.OP.R.rs), &RSP_GPR[RSPOpC.OP.R.rd].W, RspGPR_Name(RSPOpC.OP.R.rd));
		} else {
			MoveVariableToX86reg(&RspRecompPos, &RSP_GPR[RSPOpC.OP.R.rs].W, RspGPR_Name(RSPOpC.OP.R.rs), x86_EAX);
			AddX86RegToX86Reg(&RspRecompPos, x86_EAX, x86_EAX);
			MoveX86regToVariable(&RspRecompPos, x86_EAX, &RSP_GPR[RSPOpC.OP.R.rd].W, RspGPR_Name(RSPOpC.OP.R.rd));
		}
	} else if (RSPOpC.OP.R.rs == 0) {
		if (IsRspRegConst(RSPOpC.OP.R.rt)) {
			MoveConstToVariable(&RspRecompPos, MipsRspRegConst(RSPOpC.OP.R.rt), &RSP_GPR[RSPOpC.OP.R.rd].W, RspGPR_Name(RSPOpC.OP.R.rd));
		} else {
			MoveVariableToX86reg(&RspRecompPos, &RSP_GPR[RSPOpC.OP.R.rt].W, RspGPR_Name(RSPOpC.OP.R.rt), x86_EAX);
			MoveX86regToVariable(&RspRecompPos, x86_EAX, &RSP_GPR[RSPOpC.OP.R.rd].W, RspGPR_Name(RSPOpC.OP.R.rd));
		}
	} else if (RSPOpC.OP.R.rt == 0) {
		if (IsRspRegConst(RSPOpC.OP.R.rs)) {
			MoveConstToVariable(&RspRecompPos, MipsRspRegConst(RSPOpC.OP.R.rs), &RSP_GPR[RSPOpC.OP.R.rd].W, RspGPR_Name(RSPOpC.OP.R.rd));
		} else {
			MoveVariableToX86reg(&RspRecompPos, &RSP_GPR[RSPOpC.OP.R.rs].W, RspGPR_Name(RSPOpC.OP.R.rs), x86_EAX);
			MoveX86regToVariable(&RspRecompPos, x86_EAX, &RSP_GPR[RSPOpC.OP.R.rd].W, RspGPR_Name(RSPOpC.OP.R.rd));
		}
	} else if (IsRspRegConst(RSPOpC.OP.R.rs) && IsRspRegConst(RSPOpC.OP.R.rt)) {
		MoveConstToVariable(&RspRecompPos, MipsRspRegConst(RSPOpC.OP.R.rs) + MipsRspRegConst(RSPOpC.OP.R.rt), &RSP_GPR[RSPOpC.OP.R.rd].W, RspGPR_Name(RSPOpC.OP.R.rd));
	} else if (IsRspRegConst(RSPOpC.OP.R.rs)) {
		MoveVariableToX86reg(&RspRecompPos, &RSP_GPR[RSPOpC.OP.R.rt].W, RspGPR_Name(RSPOpC.OP.R.rt), x86_EAX);
		AddConstToX86Reg(&RspRecompPos, x86_EAX, MipsRspRegConst(RSPOpC.OP.R.rs));
		MoveX86regToVariable(&RspRecompPos, x86_EAX, &RSP_GPR[RSPOpC.OP.R.rd].W, RspGPR_Name(RSPOpC.OP.R.rd));
	} else if (IsRspRegConst(RSPOpC.OP.R.rt)) {
		MoveVariableToX86reg(&RspRecompPos, &RSP_GPR[RSPOpC.OP.R.rs].W, RspGPR_Name(RSPOpC.OP.R.rs), x86_EAX);
		AddConstToX86Reg(&RspRecompPos, x86_EAX, MipsRspRegConst(RSPOpC.OP.R.rt));
		MoveX86regToVariable(&RspRecompPos, x86_EAX, &RSP_GPR[RSPOpC.OP.R.rd].W, RspGPR_Name(RSPOpC.OP.R.rd));
	} else {
		MoveVariableToX86reg(&RspRecompPos, &RSP_GPR[RSPOpC.OP.R.rs].W, RspGPR_Name(RSPOpC.OP.R.rs), x86_EAX);
		AddVariableToX86reg(&RspRecompPos, x86_EAX, &RSP_GPR[RSPOpC.OP.R.rt].W, RspGPR_Name(RSPOpC.OP.R.rt));
		MoveX86regToVariable(&RspRecompPos, x86_EAX, &RSP_GPR[RSPOpC.OP.R.rd].W, RspGPR_Name(RSPOpC.OP.R.rd));
	}
}

void CompileRsp_Special_SUB ( void ) {
	#ifndef Compile_Special
	InterpreterFallback((void*)RSP_Special_SUB,"RSP_Special_SUB"); return;
	#endif

	RSP_CPU_Message("  %X %s",RspCompilePC,RSPOpcodeName(RSPOpC.OP.Hex,RspCompilePC));

	if (RSPOpC.OP.R.rd == 0) return;

	if (RSPOpC.OP.R.rd == RSPOpC.OP.R.rs) {
		if (IsRspRegConst(RSPOpC.OP.R.rt)) {
			SubConstFromVariable(&RspRecompPos, MipsRspRegConst(RSPOpC.OP.R.rt), &RSP_GPR[RSPOpC.OP.R.rd].W, RspGPR_Name(RSPOpC.OP.R.rd));
		} else {
			MoveVariableToX86reg(&RspRecompPos, &RSP_GPR[RSPOpC.OP.R.rt].W, RspGPR_Name(RSPOpC.OP.R.rt), x86_EAX);
			SubX86regFromVariable(&RspRecompPos, x86_EAX, &RSP_GPR[RSPOpC.OP.R.rd].W, "RSP_GPR[RSPOpC.rd].W");
		}
	} else if (RSPOpC.OP.R.rs == RSPOpC.OP.R.rt) {
		MoveConstToVariable(&RspRecompPos, 0, &RSP_GPR[RSPOpC.OP.R.rd].W, RspGPR_Name(RSPOpC.OP.R.rd));
	} else if (IsRspRegConst(RSPOpC.OP.R.rs) && IsRspRegConst(RSPOpC.OP.R.rt)) {
		MoveConstToVariable(&RspRecompPos, (long)MipsRspRegConst(RSPOpC.OP.R.rs) - (long)MipsRspRegConst(RSPOpC.OP.R.rt), &RSP_GPR[RSPOpC.OP.R.rd].W, RspGPR_Name(RSPOpC.OP.R.rd));
	} else if (IsRspRegConst(RSPOpC.OP.R.rs)) {
		MoveConstToX86reg(&RspRecompPos, MipsRspRegConst(RSPOpC.OP.R.rs), x86_EAX);
		SubVariableFromX86reg(&RspRecompPos, x86_EAX, &RSP_GPR[RSPOpC.OP.R.rt].W, RspGPR_Name(RSPOpC.OP.R.rt));
		MoveX86regToVariable(&RspRecompPos, x86_EAX, &RSP_GPR[RSPOpC.OP.R.rd].W, RspGPR_Name(RSPOpC.OP.R.rd));
	} else if (IsRspRegConst(RSPOpC.OP.R.rt)) {
		MoveVariableToX86reg(&RspRecompPos, &RSP_GPR[RSPOpC.OP.R.rs].W, RspGPR_Name(RSPOpC.OP.R.rs), x86_EAX);
		SubConstFromX86Reg(&RspRecompPos, x86_EAX, MipsRspRegConst(RSPOpC.OP.R.rt));
		MoveX86regToVariable(&RspRecompPos, x86_EAX, &RSP_GPR[RSPOpC.OP.R.rd].W, RspGPR_Name(RSPOpC.OP.R.rd));
	} else {
		MoveVariableToX86reg(&RspRecompPos, &RSP_GPR[RSPOpC.OP.R.rs].W, RspGPR_Name(RSPOpC.OP.R.rs), x86_EAX);
		SubVariableFromX86reg(&RspRecompPos, x86_EAX, &RSP_GPR[RSPOpC.OP.R.rt].W, RspGPR_Name(RSPOpC.OP.R.rt));
		MoveX86regToVariable(&RspRecompPos, x86_EAX, &RSP_GPR[RSPOpC.OP.R.rd].W, RspGPR_Name(RSPOpC.OP.R.rd));
	}
}

void CompileRsp_Special_SUBU ( void ) {
	#ifndef Compile_Special
	InterpreterFallback((void*)RSP_Special_SUBU,"RSP_Special_SUBU"); return;
	#endif

	RSP_CPU_Message("  %X %s",RspCompilePC,RSPOpcodeName(RSPOpC.OP.Hex,RspCompilePC));

	if (RSPOpC.OP.R.rd == 0) return;

	if (RSPOpC.OP.R.rd == RSPOpC.OP.R.rs) {
		if (IsRspRegConst(RSPOpC.OP.R.rt)) {
			SubConstFromVariable(&RspRecompPos, MipsRspRegConst(RSPOpC.OP.R.rt), &RSP_GPR[RSPOpC.OP.R.rd].W, RspGPR_Name(RSPOpC.OP.R.rd));
		} else {
			MoveVariableToX86reg(&RspRecompPos, &RSP_GPR[RSPOpC.OP.R.rt].W, RspGPR_Name(RSPOpC.OP.R.rt), x86_EAX);
			SubX86regFromVariable(&RspRecompPos, x86_EAX, &RSP_GPR[RSPOpC.OP.R.rd].W, RspGPR_Name(RSPOpC.OP.R.rd));
		}
	} else if (RSPOpC.OP.R.rs == RSPOpC.OP.R.rt) {
		MoveConstToVariable(&RspRecompPos, 0, &RSP_GPR[RSPOpC.OP.R.rd].W, RspGPR_Name(RSPOpC.OP.R.rd));
	} else if (IsRspRegConst(RSPOpC.OP.R.rs) && IsRspRegConst(RSPOpC.OP.R.rt)) {
		MoveConstToVariable(&RspRecompPos, (unsigned long)MipsRspRegConst(RSPOpC.OP.R.rs) - (unsigned long)MipsRspRegConst(RSPOpC.OP.R.rt), &RSP_GPR[RSPOpC.OP.R.rd].W, RspGPR_Name(RSPOpC.OP.R.rd));
	} else if (IsRspRegConst(RSPOpC.OP.R.rs)) {
		MoveConstToX86reg(&RspRecompPos, MipsRspRegConst(RSPOpC.OP.R.rs), x86_EAX);
		SubVariableFromX86reg(&RspRecompPos, x86_EAX, &RSP_GPR[RSPOpC.OP.R.rt].W, RspGPR_Name(RSPOpC.OP.R.rt));
		MoveX86regToVariable(&RspRecompPos, x86_EAX, &RSP_GPR[RSPOpC.OP.R.rd].W, RspGPR_Name(RSPOpC.OP.R.rd));
	} else if (IsRspRegConst(RSPOpC.OP.R.rt)) {
		MoveVariableToX86reg(&RspRecompPos, &RSP_GPR[RSPOpC.OP.R.rs].W, RspGPR_Name(RSPOpC.OP.R.rs), x86_EAX);
		SubConstFromX86Reg(&RspRecompPos, x86_EAX, MipsRspRegConst(RSPOpC.OP.R.rt));
		MoveX86regToVariable(&RspRecompPos, x86_EAX, &RSP_GPR[RSPOpC.OP.R.rd].W, RspGPR_Name(RSPOpC.OP.R.rd));
	} else {
		MoveVariableToX86reg(&RspRecompPos, &RSP_GPR[RSPOpC.OP.R.rs].W, RspGPR_Name(RSPOpC.OP.R.rs), x86_EAX);
		SubVariableFromX86reg(&RspRecompPos, x86_EAX, &RSP_GPR[RSPOpC.OP.R.rt].W, RspGPR_Name(RSPOpC.OP.R.rt));
		MoveX86regToVariable(&RspRecompPos, x86_EAX, &RSP_GPR[RSPOpC.OP.R.rd].W, RspGPR_Name(RSPOpC.OP.R.rd));
	}
}

void CompileRsp_Special_AND ( void ) {
	#ifndef Compile_Special
	InterpreterFallback((void*)RSP_Special_AND,"RSP_Special_AND"); return;
	#endif

	RSP_CPU_Message("  %X %s",RspCompilePC,RSPOpcodeName(RSPOpC.OP.Hex,RspCompilePC));

	if (RSPOpC.OP.R.rd == 0) return;

	if (RSPOpC.OP.R.rd == RSPOpC.OP.R.rs) {
		if (IsRspRegConst(RSPOpC.OP.R.rt)) {
			AndConstToVariable(&RspRecompPos, MipsRspRegConst(RSPOpC.OP.R.rt), &RSP_GPR[RSPOpC.OP.R.rd].W, RspGPR_Name(RSPOpC.OP.R.rd));
		} else {
			MoveVariableToX86reg(&RspRecompPos, &RSP_GPR[RSPOpC.OP.R.rt].W, RspGPR_Name(RSPOpC.OP.R.rt), x86_EAX);
			AndX86RegToVariable(&RspRecompPos, &RSP_GPR[RSPOpC.OP.R.rd].W, RspGPR_Name(RSPOpC.OP.R.rd), x86_EAX);
		}
	} else if (RSPOpC.OP.R.rd == RSPOpC.OP.R.rt) {
		if (IsRspRegConst(RSPOpC.OP.R.rs)) {
			AndConstToVariable(&RspRecompPos, MipsRspRegConst(RSPOpC.OP.R.rs), &RSP_GPR[RSPOpC.OP.R.rd].W, RspGPR_Name(RSPOpC.OP.R.rd));
		} else {
			MoveVariableToX86reg(&RspRecompPos, &RSP_GPR[RSPOpC.OP.R.rs].W, RspGPR_Name(RSPOpC.OP.R.rs), x86_EAX);
			AndX86RegToVariable(&RspRecompPos, &RSP_GPR[RSPOpC.OP.R.rd].W, RspGPR_Name(RSPOpC.OP.R.rd), x86_EAX);
		}
	} else if (RSPOpC.OP.R.rs == RSPOpC.OP.R.rt) {
		if (IsRspRegConst(RSPOpC.OP.R.rs)) {
			MoveConstToVariable(&RspRecompPos, MipsRspRegConst(RSPOpC.OP.R.rs), &RSP_GPR[RSPOpC.OP.R.rd].W, RspGPR_Name(RSPOpC.OP.R.rd));
		} else {
			MoveVariableToX86reg(&RspRecompPos, &RSP_GPR[RSPOpC.OP.R.rs].W, RspGPR_Name(RSPOpC.OP.R.rs), x86_EAX);
			MoveX86regToVariable(&RspRecompPos, x86_EAX, &RSP_GPR[RSPOpC.OP.R.rd].W, RspGPR_Name(RSPOpC.OP.R.rd));
		}
	} else if (RSPOpC.OP.R.rs == 0 || RSPOpC.OP.R.rt == 0) {
		MoveConstToVariable(&RspRecompPos, 0, &RSP_GPR[RSPOpC.OP.R.rd].W, RspGPR_Name(RSPOpC.OP.R.rd));
	} else if (IsRspRegConst(RSPOpC.OP.R.rs) && IsRspRegConst(RSPOpC.OP.R.rt)) {
		MoveConstToVariable(&RspRecompPos, MipsRspRegConst(RSPOpC.OP.R.rs) & MipsRspRegConst(RSPOpC.OP.R.rt), &RSP_GPR[RSPOpC.OP.R.rd].W, RspGPR_Name(RSPOpC.OP.R.rd));
	} else if (IsRspRegConst(RSPOpC.OP.R.rs)) {
		MoveVariableToX86reg(&RspRecompPos, &RSP_GPR[RSPOpC.OP.R.rt].W, RspGPR_Name(RSPOpC.OP.R.rt), x86_EAX);
		AndConstToX86Reg(&RspRecompPos, x86_EAX, MipsRspRegConst(RSPOpC.OP.R.rs));
		MoveX86regToVariable(&RspRecompPos, x86_EAX, &RSP_GPR[RSPOpC.OP.R.rd].W, RspGPR_Name(RSPOpC.OP.R.rd));
	} else if (IsRspRegConst(RSPOpC.OP.R.rt)) {
		MoveVariableToX86reg(&RspRecompPos, &RSP_GPR[RSPOpC.OP.R.rs].W, RspGPR_Name(RSPOpC.OP.R.rs), x86_EAX);
		AndConstToX86Reg(&RspRecompPos, x86_EAX, MipsRspRegConst(RSPOpC.OP.R.rt));
		MoveX86regToVariable(&RspRecompPos, x86_EAX, &RSP_GPR[RSPOpC.OP.R.rd].W, RspGPR_Name(RSPOpC.OP.R.rd));
	} else {
		MoveVariableToX86reg(&RspRecompPos, &RSP_GPR[RSPOpC.OP.R.rs].W, RspGPR_Name(RSPOpC.OP.R.rs), x86_EAX);
		AndVariableToX86Reg(&RspRecompPos, &RSP_GPR[RSPOpC.OP.R.rt].W, RspGPR_Name(RSPOpC.OP.R.rt), x86_EAX);
		MoveX86regToVariable(&RspRecompPos, x86_EAX, &RSP_GPR[RSPOpC.OP.R.rd].W, RspGPR_Name(RSPOpC.OP.R.rd));
	}
}

void CompileRsp_Special_OR ( void ) {
	#ifndef Compile_Special
	InterpreterFallback((void*)RSP_Special_OR,"RSP_Special_OR"); return;
	#endif

	RSP_CPU_Message("  %X %s",RspCompilePC,RSPOpcodeName(RSPOpC.OP.Hex,RspCompilePC));

	if (RSPOpC.OP.R.rd == 0) return;

	if (RSPOpC.OP.R.rd == RSPOpC.OP.R.rs) {
		if (IsRspRegConst(RSPOpC.OP.R.rt)) {
			OrConstToVariable(&RspRecompPos, MipsRspRegConst(RSPOpC.OP.R.rt), &RSP_GPR[RSPOpC.OP.R.rd].W, RspGPR_Name(RSPOpC.OP.R.rd));
		} else {
			MoveVariableToX86reg(&RspRecompPos, &RSP_GPR[RSPOpC.OP.R.rt].W, RspGPR_Name(RSPOpC.OP.R.rt), x86_EAX);
			OrX86RegToVariable(&RspRecompPos, &RSP_GPR[RSPOpC.OP.R.rd].W, RspGPR_Name(RSPOpC.OP.R.rd), x86_EAX);
		}
	} else if (RSPOpC.OP.R.rd == RSPOpC.OP.R.rt) {
		if (IsRspRegConst(RSPOpC.OP.R.rs)) {
			OrConstToVariable(&RspRecompPos, MipsRspRegConst(RSPOpC.OP.R.rs), &RSP_GPR[RSPOpC.OP.R.rd].W, RspGPR_Name(RSPOpC.OP.R.rd));
		} else {
			MoveVariableToX86reg(&RspRecompPos, &RSP_GPR[RSPOpC.OP.R.rs].W, RspGPR_Name(RSPOpC.OP.R.rs), x86_EAX);
			OrX86RegToVariable(&RspRecompPos, &RSP_GPR[RSPOpC.OP.R.rd].W, RspGPR_Name(RSPOpC.OP.R.rd), x86_EAX);
		}
	} else if (RSPOpC.OP.R.rs == RSPOpC.OP.R.rt) {
		if (IsRspRegConst(RSPOpC.OP.R.rs)) {
			MoveConstToVariable(&RspRecompPos, MipsRspRegConst(RSPOpC.OP.R.rs), &RSP_GPR[RSPOpC.OP.R.rd].W, RspGPR_Name(RSPOpC.OP.R.rd));
		} else {
			MoveVariableToX86reg(&RspRecompPos, &RSP_GPR[RSPOpC.OP.R.rs].W, RspGPR_Name(RSPOpC.OP.R.rs), x86_EAX);
			MoveX86regToVariable(&RspRecompPos, x86_EAX, &RSP_GPR[RSPOpC.OP.R.rd].W, RspGPR_Name(RSPOpC.OP.R.rd));
		}
	} else if (RSPOpC.OP.R.rs == 0 && RSPOpC.OP.R.rt == 0) {
		MoveConstToVariable(&RspRecompPos, 0, &RSP_GPR[RSPOpC.OP.R.rd].W, RspGPR_Name(RSPOpC.OP.R.rd));
	} else if (RSPOpC.OP.R.rs == 0) {
		if (IsRspRegConst(RSPOpC.OP.R.rt)) {
			MoveConstToVariable(&RspRecompPos, MipsRspRegConst(RSPOpC.OP.R.rt), &RSP_GPR[RSPOpC.OP.R.rd].W, RspGPR_Name(RSPOpC.OP.R.rd));
		} else {
			MoveVariableToX86reg(&RspRecompPos, &RSP_GPR[RSPOpC.OP.R.rt].W, RspGPR_Name(RSPOpC.OP.R.rt), x86_EAX);
			MoveX86regToVariable(&RspRecompPos, x86_EAX, &RSP_GPR[RSPOpC.OP.R.rd].W, RspGPR_Name(RSPOpC.OP.R.rd));
		}
	} else if (RSPOpC.OP.R.rt == 0) {
		if (IsRspRegConst(RSPOpC.OP.R.rs)) {
			MoveConstToVariable(&RspRecompPos, MipsRspRegConst(RSPOpC.OP.R.rs), &RSP_GPR[RSPOpC.OP.R.rd].W, RspGPR_Name(RSPOpC.OP.R.rd));
		} else {
			MoveVariableToX86reg(&RspRecompPos, &RSP_GPR[RSPOpC.OP.R.rs].W, RspGPR_Name(RSPOpC.OP.R.rs), x86_EAX);
			MoveX86regToVariable(&RspRecompPos, x86_EAX, &RSP_GPR[RSPOpC.OP.R.rd].W, RspGPR_Name(RSPOpC.OP.R.rd));
		}
	} else if (IsRspRegConst(RSPOpC.OP.R.rs) && IsRspRegConst(RSPOpC.OP.R.rt)) {
		MoveConstToVariable(&RspRecompPos, MipsRspRegConst(RSPOpC.OP.R.rs) | MipsRspRegConst(RSPOpC.OP.R.rt), &RSP_GPR[RSPOpC.OP.R.rd].W, RspGPR_Name(RSPOpC.OP.R.rd));
	} else if (IsRspRegConst(RSPOpC.OP.R.rs)) {
		MoveVariableToX86reg(&RspRecompPos, &RSP_GPR[RSPOpC.OP.R.rt].W, RspGPR_Name(RSPOpC.OP.R.rt), x86_EAX);
		OrConstToX86Reg(&RspRecompPos, MipsRspRegConst(RSPOpC.OP.R.rs), x86_EAX);
		MoveX86regToVariable(&RspRecompPos, x86_EAX, &RSP_GPR[RSPOpC.OP.R.rd].W, RspGPR_Name(RSPOpC.OP.R.rd));
	} else if (IsRspRegConst(RSPOpC.OP.R.rt)) {
		MoveVariableToX86reg(&RspRecompPos, &RSP_GPR[RSPOpC.OP.R.rs].W, RspGPR_Name(RSPOpC.OP.R.rs), x86_EAX);
		OrConstToX86Reg(&RspRecompPos, MipsRspRegConst(RSPOpC.OP.R.rt), x86_EAX);
		MoveX86regToVariable(&RspRecompPos, x86_EAX, &RSP_GPR[RSPOpC.OP.R.rd].W, RspGPR_Name(RSPOpC.OP.R.rd));
	} else {
		MoveVariableToX86reg(&RspRecompPos, &RSP_GPR[RSPOpC.OP.R.rs].W, RspGPR_Name(RSPOpC.OP.R.rs), x86_EAX);
		OrVariableToX86Reg(&RspRecompPos, &RSP_GPR[RSPOpC.OP.R.rt].W, RspGPR_Name(RSPOpC.OP.R.rt), x86_EAX);
		MoveX86regToVariable(&RspRecompPos, x86_EAX, &RSP_GPR[RSPOpC.OP.R.rd].W, RspGPR_Name(RSPOpC.OP.R.rd));
	}
}

void CompileRsp_Special_XOR ( void ) {
	#ifndef Compile_Special
	InterpreterFallback((void*)RSP_Special_XOR,"RSP_Special_XOR"); return;
	#endif
	
	RSP_CPU_Message("  %X %s",RspCompilePC,RSPOpcodeName(RSPOpC.OP.Hex,RspCompilePC));

	if (RSPOpC.OP.R.rd == 0) return;

	if (RSPOpC.OP.R.rd == RSPOpC.OP.R.rs) {
		if (IsRspRegConst(RSPOpC.OP.R.rt)) {
			XorConstToVariable(&RspRecompPos, &RSP_GPR[RSPOpC.OP.R.rd].W, RspGPR_Name(RSPOpC.OP.R.rd), MipsRspRegConst(RSPOpC.OP.R.rt));
		} else {
			MoveVariableToX86reg(&RspRecompPos, &RSP_GPR[RSPOpC.OP.R.rt].W, RspGPR_Name(RSPOpC.OP.R.rt), x86_EAX);
			XorX86RegToVariable(&RspRecompPos, &RSP_GPR[RSPOpC.OP.R.rd].W, RspGPR_Name(RSPOpC.OP.R.rd), x86_EAX);
		}
	} else if (RSPOpC.OP.R.rd == RSPOpC.OP.R.rt) {
		if (IsRspRegConst(RSPOpC.OP.R.rs)) {
			XorConstToVariable(&RspRecompPos, &RSP_GPR[RSPOpC.OP.R.rd].W, RspGPR_Name(RSPOpC.OP.R.rd), MipsRspRegConst(RSPOpC.OP.R.rs));
		} else {
			MoveVariableToX86reg(&RspRecompPos, &RSP_GPR[RSPOpC.OP.R.rs].W, RspGPR_Name(RSPOpC.OP.R.rs), x86_EAX);
			XorX86RegToVariable(&RspRecompPos, &RSP_GPR[RSPOpC.OP.R.rd].W, RspGPR_Name(RSPOpC.OP.R.rd), x86_EAX);
		}
	} else if (RSPOpC.OP.R.rs == RSPOpC.OP.R.rt) {
		MoveConstToVariable(&RspRecompPos, 0, &RSP_GPR[RSPOpC.OP.R.rd].W, RspGPR_Name(RSPOpC.OP.R.rd));
	} else if (RSPOpC.OP.R.rs == 0) {
		if (IsRspRegConst(RSPOpC.OP.R.rt)) {
			MoveConstToVariable(&RspRecompPos, MipsRspRegConst(RSPOpC.OP.R.rt), &RSP_GPR[RSPOpC.OP.R.rd].W, RspGPR_Name(RSPOpC.OP.R.rd));
		} else {
			MoveVariableToX86reg(&RspRecompPos, &RSP_GPR[RSPOpC.OP.R.rt].W, RspGPR_Name(RSPOpC.OP.R.rt), x86_EAX);
			MoveX86regToVariable(&RspRecompPos, x86_EAX, &RSP_GPR[RSPOpC.OP.R.rd].W, RspGPR_Name(RSPOpC.OP.R.rd));
		}
	} else if (RSPOpC.OP.R.rt == 0) {
		if (IsRspRegConst(RSPOpC.OP.R.rs)) {
			MoveConstToVariable(&RspRecompPos, MipsRspRegConst(RSPOpC.OP.R.rs), &RSP_GPR[RSPOpC.OP.R.rd].W, RspGPR_Name(RSPOpC.OP.R.rd));
		} else {
			MoveVariableToX86reg(&RspRecompPos, &RSP_GPR[RSPOpC.OP.R.rs].W, RspGPR_Name(RSPOpC.OP.R.rs), x86_EAX);
			MoveX86regToVariable(&RspRecompPos, x86_EAX, &RSP_GPR[RSPOpC.OP.R.rd].W, RspGPR_Name(RSPOpC.OP.R.rd));
		}
	} else if (IsRspRegConst(RSPOpC.OP.R.rs) && IsRspRegConst(RSPOpC.OP.R.rt)) {
		MoveConstToVariable(&RspRecompPos, MipsRspRegConst(RSPOpC.OP.R.rs) ^ MipsRspRegConst(RSPOpC.OP.R.rt), &RSP_GPR[RSPOpC.OP.R.rd].W, RspGPR_Name(RSPOpC.OP.R.rd));
	} else if (IsRspRegConst(RSPOpC.OP.R.rs)) {
		MoveVariableToX86reg(&RspRecompPos, &RSP_GPR[RSPOpC.OP.R.rt].W, RspGPR_Name(RSPOpC.OP.R.rt), x86_EAX);
		XorConstToX86Reg(&RspRecompPos, x86_EAX, MipsRspRegConst(RSPOpC.OP.R.rs));
		MoveX86regToVariable(&RspRecompPos, x86_EAX, &RSP_GPR[RSPOpC.OP.R.rd].W, RspGPR_Name(RSPOpC.OP.R.rd));
	} else if (IsRspRegConst(RSPOpC.OP.R.rt)) {
		MoveVariableToX86reg(&RspRecompPos, &RSP_GPR[RSPOpC.OP.R.rs].W, RspGPR_Name(RSPOpC.OP.R.rs), x86_EAX);
		XorConstToX86Reg(&RspRecompPos, x86_EAX, MipsRspRegConst(RSPOpC.OP.R.rt));
		MoveX86regToVariable(&RspRecompPos, x86_EAX, &RSP_GPR[RSPOpC.OP.R.rd].W, RspGPR_Name(RSPOpC.OP.R.rd));
	}
	else {
		MoveVariableToX86reg(&RspRecompPos, &RSP_GPR[RSPOpC.OP.R.rs].W, RspGPR_Name(RSPOpC.OP.R.rs), x86_EAX);
		XorVariableToX86reg(&RspRecompPos, &RSP_GPR[RSPOpC.OP.R.rt].W, RspGPR_Name(RSPOpC.OP.R.rt), x86_EAX);
		MoveX86regToVariable(&RspRecompPos, x86_EAX, &RSP_GPR[RSPOpC.OP.R.rd].W, RspGPR_Name(RSPOpC.OP.R.rd));
	}
}

void CompileRsp_Special_NOR ( void ) {
	#ifndef Compile_NOR
	InterpreterFallback((void*)RSP_Special_NOR,"RSP_Special_NOR");
	#endif

	RSP_CPU_Message("  %X %s", RspCompilePC, RSPOpcodeName(RSPOpC.OP.Hex, RspCompilePC));

	if (RSPOpC.OP.R.rd == 0) return;

	if (RSPOpC.OP.R.rs == RSPOpC.OP.R.rt) {
		if (IsRspRegConst(RSPOpC.OP.R.rs)) {
			MoveConstToVariable(&RspRecompPos, ~MipsRspRegConst(RSPOpC.OP.R.rs), &RSP_GPR[RSPOpC.OP.R.rd].W, RspGPR_Name(RSPOpC.OP.R.rd));
		} else {
			MoveVariableToX86reg(&RspRecompPos, &RSP_GPR[RSPOpC.OP.R.rs].W, RspGPR_Name(RSPOpC.OP.R.rs), x86_EAX);
			NotX86Reg(&RspRecompPos, x86_EAX);
			MoveX86regToVariable(&RspRecompPos, x86_EAX, &RSP_GPR[RSPOpC.OP.R.rd].W, RspGPR_Name(RSPOpC.OP.R.rd));
		}
	} else if (RSPOpC.OP.R.rs == 0 && RSPOpC.OP.R.rt == 0) {
		MoveConstToVariable(&RspRecompPos, ~0U, &RSP_GPR[RSPOpC.OP.R.rd].W, RspGPR_Name(RSPOpC.OP.R.rd));
	} else if (RSPOpC.OP.R.rs == 0) {
		if (IsRspRegConst(RSPOpC.OP.R.rt)) {
			MoveConstToVariable(&RspRecompPos, ~MipsRspRegConst(RSPOpC.OP.R.rt), &RSP_GPR[RSPOpC.OP.R.rd].W, RspGPR_Name(RSPOpC.OP.R.rd));
		} else {
			MoveVariableToX86reg(&RspRecompPos, &RSP_GPR[RSPOpC.OP.R.rt].W, RspGPR_Name(RSPOpC.OP.R.rt), x86_EAX);
			NotX86Reg(&RspRecompPos, x86_EAX);
			MoveX86regToVariable(&RspRecompPos, x86_EAX, &RSP_GPR[RSPOpC.OP.R.rd].W, RspGPR_Name(RSPOpC.OP.R.rd));
		}
	} else if (RSPOpC.OP.R.rt == 0) {
		if (IsRspRegConst(RSPOpC.OP.R.rs)) {
			MoveConstToVariable(&RspRecompPos, ~MipsRspRegConst(RSPOpC.OP.R.rs), &RSP_GPR[RSPOpC.OP.R.rd].W, RspGPR_Name(RSPOpC.OP.R.rd));
		} else {
			MoveVariableToX86reg(&RspRecompPos, &RSP_GPR[RSPOpC.OP.R.rs].W, RspGPR_Name(RSPOpC.OP.R.rs), x86_EAX);
			NotX86Reg(&RspRecompPos, x86_EAX);
			MoveX86regToVariable(&RspRecompPos, x86_EAX, &RSP_GPR[RSPOpC.OP.R.rd].W, RspGPR_Name(RSPOpC.OP.R.rd));
		}
	} else if (IsRspRegConst(RSPOpC.OP.R.rs) && IsRspRegConst(RSPOpC.OP.R.rt)) {
		MoveConstToVariable(&RspRecompPos, ~(MipsRspRegConst(RSPOpC.OP.R.rs) | MipsRspRegConst(RSPOpC.OP.R.rt)), &RSP_GPR[RSPOpC.OP.R.rd].W, RspGPR_Name(RSPOpC.OP.R.rd));
	} else if (IsRspRegConst(RSPOpC.OP.R.rs)) {
		MoveVariableToX86reg(&RspRecompPos, &RSP_GPR[RSPOpC.OP.R.rt].W, RspGPR_Name(RSPOpC.OP.R.rt), x86_EAX);
		OrConstToX86Reg(&RspRecompPos, MipsRspRegConst(RSPOpC.OP.R.rs), x86_EAX);
		NotX86Reg(&RspRecompPos, x86_EAX);
		MoveX86regToVariable(&RspRecompPos, x86_EAX, &RSP_GPR[RSPOpC.OP.R.rd].W, RspGPR_Name(RSPOpC.OP.R.rd));
	} else if (IsRspRegConst(RSPOpC.OP.R.rt)) {
		MoveVariableToX86reg(&RspRecompPos, &RSP_GPR[RSPOpC.OP.R.rs].W, RspGPR_Name(RSPOpC.OP.R.rs), x86_EAX);
		OrConstToX86Reg(&RspRecompPos, MipsRspRegConst(RSPOpC.OP.R.rt), x86_EAX);
		NotX86Reg(&RspRecompPos, x86_EAX);
		MoveX86regToVariable(&RspRecompPos, x86_EAX, &RSP_GPR[RSPOpC.OP.R.rd].W, RspGPR_Name(RSPOpC.OP.R.rd));
	} else {
		MoveVariableToX86reg(&RspRecompPos, &RSP_GPR[RSPOpC.OP.R.rs].W, RspGPR_Name(RSPOpC.OP.R.rs), x86_EAX);
		OrVariableToX86Reg(&RspRecompPos, &RSP_GPR[RSPOpC.OP.R.rt].W, RspGPR_Name(RSPOpC.OP.R.rt), x86_EAX);
		NotX86Reg(&RspRecompPos, x86_EAX);
		MoveX86regToVariable(&RspRecompPos, x86_EAX, &RSP_GPR[RSPOpC.OP.R.rd].W, RspGPR_Name(RSPOpC.OP.R.rd));
	}
}

void CompileRsp_Special_SLT ( void ) {
	#ifndef Compile_Special
	InterpreterFallback((void*)RSP_Special_SLT,"RSP_Special_SLT"); return;
	#endif

	RSP_CPU_Message("  %X %s",RspCompilePC,RSPOpcodeName(RSPOpC.OP.Hex,RspCompilePC));
	if (RSPOpC.OP.R.rd == 0) { return; }

	if (RSPOpC.OP.R.rt == RSPOpC.OP.R.rs) {
		MoveConstToVariable(&RspRecompPos, 0, &RSP_GPR[RSPOpC.OP.R.rd].UW, RspGPR_Name(RSPOpC.OP.R.rd));
	} else if (IsRspRegConst(RSPOpC.OP.R.rs) && IsRspRegConst(RSPOpC.OP.R.rt)) {
		if ((long)MipsRspRegConst(RSPOpC.OP.R.rs) < (long)MipsRspRegConst(RSPOpC.OP.R.rt)) {
			MoveConstToVariable(&RspRecompPos, 1, &RSP_GPR[RSPOpC.OP.R.rd].UW, RspGPR_Name(RSPOpC.OP.R.rd));
		}
		else {
			MoveConstToVariable(&RspRecompPos, 0, &RSP_GPR[RSPOpC.OP.R.rd].UW, RspGPR_Name(RSPOpC.OP.R.rd));
		}
	} else {
		XorX86RegToX86Reg(&RspRecompPos, x86_EBX, x86_EBX);
		if (RSPOpC.OP.R.rs == 0) {
			MoveVariableToX86reg(&RspRecompPos, &RSP_GPR[RSPOpC.OP.R.rt].UW, RspGPR_Name(RSPOpC.OP.R.rt), x86_EAX);
			CompConstToX86reg(&RspRecompPos, x86_EAX, 0);
			Setg(&RspRecompPos, x86_EBX);
		} else if (RSPOpC.OP.R.rt == 0) {
			MoveVariableToX86reg(&RspRecompPos, &RSP_GPR[RSPOpC.OP.R.rs].UW, RspGPR_Name(RSPOpC.OP.R.rs), x86_EAX);
			CompConstToX86reg(&RspRecompPos, x86_EAX, 0);
			Setl(&RspRecompPos, x86_EBX);
		} else if (IsRspRegConst(RSPOpC.OP.R.rs)) {
			MoveConstToX86reg(&RspRecompPos, MipsRspRegConst(RSPOpC.OP.R.rs), x86_EAX);
			CompX86regToVariable(&RspRecompPos, x86_EAX, &RSP_GPR[RSPOpC.OP.R.rt].UW, RspGPR_Name(RSPOpC.OP.R.rt));
			Setl(&RspRecompPos, x86_EBX);
		} else if (IsRspRegConst(RSPOpC.OP.R.rt)) {
			CompConstToVariable(&RspRecompPos, MipsRspRegConst(RSPOpC.OP.R.rt), &RSP_GPR[RSPOpC.OP.R.rs].UW, RspGPR_Name(RSPOpC.OP.R.rs));
			Setl(&RspRecompPos, x86_EBX);
		}
		else {
			MoveVariableToX86reg(&RspRecompPos, &RSP_GPR[RSPOpC.OP.R.rs].UW, RspGPR_Name(RSPOpC.OP.R.rs), x86_EAX);
			CompX86regToVariable(&RspRecompPos, x86_EAX, &RSP_GPR[RSPOpC.OP.R.rt].UW, RspGPR_Name(RSPOpC.OP.R.rt));
			Setl(&RspRecompPos, x86_EBX);
		}
		MoveX86regToVariable(&RspRecompPos, x86_EBX, &RSP_GPR[RSPOpC.OP.R.rd].UW, RspGPR_Name(RSPOpC.OP.R.rd));
	}
}

void CompileRsp_Special_SLTU ( void ) {
	#ifndef Compile_SLTU
	InterpreterFallback((void*)RSP_Special_SLTU,"RSP_Special_SLTU");
	#endif

	RSP_CPU_Message("  %X %s", RspCompilePC, RSPOpcodeName(RSPOpC.OP.Hex, RspCompilePC));
	if (RSPOpC.OP.R.rd == 0) { return; }

	if (RSPOpC.OP.R.rt == RSPOpC.OP.R.rs) {
		MoveConstToVariable(&RspRecompPos, 0, &RSP_GPR[RSPOpC.OP.R.rd].UW, RspGPR_Name(RSPOpC.OP.R.rd));
	} else if (IsRspRegConst(RSPOpC.OP.R.rs) && IsRspRegConst(RSPOpC.OP.R.rt)) {
		if ((unsigned long)MipsRspRegConst(RSPOpC.OP.R.rs) < (unsigned long)MipsRspRegConst(RSPOpC.OP.R.rt)) {
			MoveConstToVariable(&RspRecompPos, 1, &RSP_GPR[RSPOpC.OP.R.rd].UW, RspGPR_Name(RSPOpC.OP.R.rd));
		} else {
			MoveConstToVariable(&RspRecompPos, 0, &RSP_GPR[RSPOpC.OP.R.rd].UW, RspGPR_Name(RSPOpC.OP.R.rd));
		}
	} else {
		XorX86RegToX86Reg(&RspRecompPos, x86_EBX, x86_EBX);
		if (RSPOpC.OP.R.rs == 0) {
			MoveVariableToX86reg(&RspRecompPos, &RSP_GPR[RSPOpC.OP.R.rt].UW, RspGPR_Name(RSPOpC.OP.R.rt), x86_EAX);
			CompConstToX86reg(&RspRecompPos, x86_EAX, 0);
			Seta(&RspRecompPos, x86_EBX);
		}
		else if (RSPOpC.OP.R.rt == 0) {
			MoveVariableToX86reg(&RspRecompPos, &RSP_GPR[RSPOpC.OP.R.rs].UW, RspGPR_Name(RSPOpC.OP.R.rs), x86_EAX);
			CompConstToX86reg(&RspRecompPos, x86_EAX, 0);
			Setb(&RspRecompPos, x86_EBX);
		}
		else if (IsRspRegConst(RSPOpC.OP.R.rs)) {
			MoveConstToX86reg(&RspRecompPos, MipsRspRegConst(RSPOpC.OP.R.rs), x86_EAX);
			CompX86regToVariable(&RspRecompPos, x86_EAX, &RSP_GPR[RSPOpC.OP.R.rt].UW, RspGPR_Name(RSPOpC.OP.R.rt));
			Setb(&RspRecompPos, x86_EBX);
		}
		else if (IsRspRegConst(RSPOpC.OP.R.rt)) {
			CompConstToVariable(&RspRecompPos, MipsRspRegConst(RSPOpC.OP.R.rt), &RSP_GPR[RSPOpC.OP.R.rs].UW, RspGPR_Name(RSPOpC.OP.R.rs));
			Setb(&RspRecompPos, x86_EBX);
		}
		else {
			MoveVariableToX86reg(&RspRecompPos, &RSP_GPR[RSPOpC.OP.R.rs].UW, RspGPR_Name(RSPOpC.OP.R.rs), x86_EAX);
			CompX86regToVariable(&RspRecompPos, x86_EAX, &RSP_GPR[RSPOpC.OP.R.rt].UW, RspGPR_Name(RSPOpC.OP.R.rt));
			Setb(&RspRecompPos, x86_EBX);
		}
		MoveX86regToVariable(&RspRecompPos, x86_EBX, &RSP_GPR[RSPOpC.OP.R.rd].UW, RspGPR_Name(RSPOpC.OP.R.rd));
	}
}

/********************** R4300i OpCodes: RegImm **********************/
void CompileRsp_RegImm_BLTZ ( void ) {
	static BOOL bDelayAffect;

	if (IsRspDelaySlotBranch(RspCompilePC)) {
		CompileRsp_ConsecutiveDelaySlots();
		return;
	}

	if ( RSP_NextInstruction == NORMAL ) {
		RSP_CPU_Message("  %X %s",RspCompilePC,RSPOpcodeName(RSPOpC.OP.Hex,RspCompilePC));
		if (RSPOpC.OP.B.rs == 0) {
			RSP_NextInstruction = DO_DELAY_SLOT;			
			return;
		}
		bDelayAffect = RspDelaySlotAffectBranch(RspCompilePC);
		if (FALSE == bDelayAffect) {
			RSP_NextInstruction = DO_DELAY_SLOT;
			return;
		}
		CompConstToVariable(&RspRecompPos, 0,&RSP_GPR[RSPOpC.OP.B.rs].W,RspGPR_Name(RSPOpC.OP.B.rs));
		SetlVariable(&RspRecompPos, &BranchCompare, "BranchCompare");
		RSP_NextInstruction = DO_DELAY_SLOT;
	} else if ( RSP_NextInstruction == DELAY_SLOT_DONE ) {
		DWORD Target = (RspCompilePC + ((short)RSPOpC.OP.B.offset << 2) + 4) & 0xFFC;
		
		if (RSPOpC.OP.B.rs == 0) {
			RSP_NextInstruction = FINISH_SUB_BLOCK;
			return;
		}
		if (IsRspRegConst(RSPOpC.OP.B.rs)) {
			if ((long)MipsRspRegConst(RSPOpC.OP.B.rs) < 0) {
				JmpLabel32(&RspRecompPos, "BranchLess", 0);
				Branch_AddRef(Target, (DWORD*)(RspRecompPos - 4));
				RSP_NextInstruction = FINISH_BLOCK;
				return;
			}
			RSP_NextInstruction = FINISH_SUB_BLOCK;
			return;
		}
		if (FALSE == bDelayAffect) {
			CompConstToVariable(&RspRecompPos, 0,&RSP_GPR[RSPOpC.OP.B.rs].W,RspGPR_Name(RSPOpC.OP.B.rs));
			JlLabel32(&RspRecompPos, "BranchLess", 0);
		} else {
			/* take a look at the branch compare variable */
			CompConstToVariable(&RspRecompPos, TRUE, &BranchCompare, "BranchCompare");
			JeLabel32(&RspRecompPos, "BranchLess", 0);
		}
		Branch_AddRef(Target, (DWORD*)(RspRecompPos - 4));
		RSP_NextInstruction = FINISH_SUB_BLOCK;
	} else {
		RspCompilerWarning("BLTZ error\nWeird Delay Slot.\n\nNextInstruction = %X\nPC = %X\nEmulation will now stop", RSP_NextInstruction, RspCompilePC);
		BreakPoint(&RspRecompPos);
	}
}

void CompileRsp_RegImm_BGEZ ( void ) {
	static BOOL bDelayAffect;

	if (IsRspDelaySlotBranch(RspCompilePC)) {
		CompileRsp_ConsecutiveDelaySlots();
		return;
	}

	if ( RSP_NextInstruction == NORMAL ) {
		RSP_CPU_Message("  %X %s",RspCompilePC,RSPOpcodeName(RSPOpC.OP.Hex,RspCompilePC));
		if (RSPOpC.OP.B.rs == 0) {
			RSP_NextInstruction = DO_DELAY_SLOT;			
			return;
		}
		bDelayAffect = RspDelaySlotAffectBranch(RspCompilePC);
		if (FALSE == bDelayAffect) {
			RSP_NextInstruction = DO_DELAY_SLOT;
			return;
		}
		CompConstToVariable(&RspRecompPos, 0,&RSP_GPR[RSPOpC.OP.B.rs].W,RspGPR_Name(RSPOpC.OP.B.rs));
		SetgeVariable(&RspRecompPos, &BranchCompare, "BranchCompare");
		RSP_NextInstruction = DO_DELAY_SLOT;
	} else if ( RSP_NextInstruction == DELAY_SLOT_DONE ) {
		DWORD Target = (RspCompilePC + ((short)RSPOpC.OP.B.offset << 2) + 4) & 0xFFC;
		
		if (RSPOpC.OP.B.rs == 0) {			
			JmpLabel32 (&RspRecompPos, "BranchToJump", 0 );
			Branch_AddRef(Target, (DWORD*)(RspRecompPos - 4));
			RSP_NextInstruction = FINISH_BLOCK;
			return;
		}
		if (IsRspRegConst(RSPOpC.OP.B.rs)) {
			if ((long)MipsRspRegConst(RSPOpC.OP.B.rs) >= 0) {
				JmpLabel32(&RspRecompPos, "BranchGreaterEqual", 0);
				Branch_AddRef(Target, (DWORD*)(RspRecompPos - 4));
				RSP_NextInstruction = FINISH_BLOCK;
				return;
			}
			RSP_NextInstruction = FINISH_SUB_BLOCK;
			return;
		}
		if (FALSE == bDelayAffect) {
			CompConstToVariable(&RspRecompPos, 0,&RSP_GPR[RSPOpC.OP.B.rs].W,RspGPR_Name(RSPOpC.OP.B.rs));
			JgeLabel32(&RspRecompPos, "BranchGreaterEqual", 0);
		} else {
			/* take a look at the branch compare variable */
			CompConstToVariable(&RspRecompPos, TRUE, &BranchCompare, "BranchCompare");
			JeLabel32(&RspRecompPos, "BranchGreaterEqual", 0);
		}
		Branch_AddRef(Target, (DWORD*)(RspRecompPos - 4));
		RSP_NextInstruction = FINISH_SUB_BLOCK;
	} else {
		RspCompilerWarning("BGEZ error\nWeird Delay Slot.\n\nNextInstruction = %X\nEmulation will now stop", RSP_NextInstruction);
		BreakPoint(&RspRecompPos);
	}
}

void CompileRsp_RegImm_BLTZAL ( void ) {
	static BOOL bDelayAffect;

	if (IsRspDelaySlotBranch(RspCompilePC)) {
		CompileRsp_ConsecutiveDelaySlots();
		return;
	}

	if ( RSP_NextInstruction == NORMAL ) {
		RSP_CPU_Message("  %X %s",RspCompilePC,RSPOpcodeName(RSPOpC.OP.Hex,RspCompilePC));
		if (RSPOpC.OP.B.rs == 0) {
			MoveConstToVariable(&RspRecompPos, (RspCompilePC + 8) & 0xFFC, &RSP_GPR[31].UW, "RA.W");
			RSP_NextInstruction = DO_DELAY_SLOT;			
			return;
		}
		bDelayAffect = RspDelaySlotAffectBranch(RspCompilePC) || RSPOpC.OP.B.rs == 31;
		if (FALSE == bDelayAffect) {
			MoveConstToVariable(&RspRecompPos, (RspCompilePC + 8) & 0xFFC, &RSP_GPR[31].UW, "RA.W");
			RSP_NextInstruction = DO_DELAY_SLOT;
			return;
		}
		CompConstToVariable(&RspRecompPos, 0,&RSP_GPR[RSPOpC.OP.B.rs].W,RspGPR_Name(RSPOpC.OP.B.rs));
		SetlVariable(&RspRecompPos, &BranchCompare, "BranchCompare");
		MoveConstToVariable(&RspRecompPos, (RspCompilePC + 8) & 0xFFC, &RSP_GPR[31].UW, "RA.W");
		RSP_NextInstruction = DO_DELAY_SLOT;
	} else if ( RSP_NextInstruction == DELAY_SLOT_DONE ) {
		DWORD Target = (RspCompilePC + ((short)RSPOpC.OP.B.offset << 2) + 4) & 0xFFC;

		if (RSPOpC.OP.B.rs == 0) {
			RSP_NextInstruction = FINISH_SUB_BLOCK;
			return;
		}
		if (MipsRspRegConst(RSPOpC.OP.B.rs)) {
			if ((long)MipsRspRegConst(RSPOpC.OP.B.rs) < 0) {
				JmpLabel32(&RspRecompPos, "BranchLess", 0);
				Branch_AddRef(Target, (DWORD*)(RspRecompPos - 4));
				RSP_NextInstruction = FINISH_BLOCK;
				return;
			}
			RSP_NextInstruction = FINISH_SUB_BLOCK;
			return;
		}

		if (FALSE == bDelayAffect) {
			CompConstToVariable(&RspRecompPos, 0, &RSP_GPR[RSPOpC.OP.B.rs].W, RspGPR_Name(RSPOpC.OP.B.rs));
			JlLabel32(&RspRecompPos, "BranchLess", 0);
		} else {
			/* take a look at the branch compare variable */
			CompConstToVariable(&RspRecompPos, TRUE, &BranchCompare, "BranchCompare");
			JeLabel32(&RspRecompPos, "BranchLess", 0);
		}
		Branch_AddRef(Target, (DWORD*)(RspRecompPos - 4));
		RSP_NextInstruction = FINISH_SUB_BLOCK;
	} else {
		RspCompilerWarning("BLTZAL error\nWeird Delay Slot.\n\nNextInstruction = %X\nEmulation will now stop", RSP_NextInstruction);
		BreakPoint(&RspRecompPos);
	}
}

void CompileRsp_RegImm_BGEZAL ( void ) {
	static BOOL bDelayAffect;

	if (IsRspDelaySlotBranch(RspCompilePC)) {
		CompileRsp_ConsecutiveDelaySlots();
		return;
	}

	if ( RSP_NextInstruction == NORMAL ) {
		RSP_CPU_Message("  %X %s",RspCompilePC,RSPOpcodeName(RSPOpC.OP.Hex,RspCompilePC));
		if (RSPOpC.OP.B.rs == 0) {
			MoveConstToVariable(&RspRecompPos, (RspCompilePC + 8) & 0xFFC, &RSP_GPR[31].UW, "RA.W");
			RSP_NextInstruction = DO_DELAY_SLOT;
			return;
		}
		bDelayAffect = RspDelaySlotAffectBranch(RspCompilePC) || RSPOpC.OP.B.rs == 31;
		if (FALSE == bDelayAffect) {
			MoveConstToVariable(&RspRecompPos, (RspCompilePC + 8) & 0xFFC, &RSP_GPR[31].UW, "RA.W");
			RSP_NextInstruction = DO_DELAY_SLOT;
			return;
		}
		CompConstToVariable(&RspRecompPos, 0,&RSP_GPR[RSPOpC.OP.B.rs].W,RspGPR_Name(RSPOpC.OP.B.rs));
		SetgeVariable(&RspRecompPos, &BranchCompare, "BranchCompare");
		MoveConstToVariable(&RspRecompPos, (RspCompilePC + 8) & 0xFFC, &RSP_GPR[31].UW, "RA.W");
		RSP_NextInstruction = DO_DELAY_SLOT;
	} else if ( RSP_NextInstruction == DELAY_SLOT_DONE ) {
		DWORD Target = (RspCompilePC + ((short)RSPOpC.OP.B.offset << 2) + 4) & 0xFFC;
		
		if (RSPOpC.OP.B.rs == 0) {			
			JmpLabel32 (&RspRecompPos, "BranchToJump", 0 );
			Branch_AddRef(Target, (DWORD*)(RspRecompPos - 4));
			RSP_NextInstruction = FINISH_SUB_BLOCK;
			return;
		}
		if (MipsRspRegConst(RSPOpC.OP.B.rs)) {
			if ((long)MipsRspRegConst(RSPOpC.OP.B.rs) >= 0) {
				JmpLabel32(&RspRecompPos, "BranchGreaterEqual", 0);
				Branch_AddRef(Target, (DWORD*)(RspRecompPos - 4));
				RSP_NextInstruction = FINISH_BLOCK;
				return;
			}
			RSP_NextInstruction = FINISH_SUB_BLOCK;
			return;
		}
		if (FALSE == bDelayAffect) {
			CompConstToVariable(&RspRecompPos, 0,&RSP_GPR[RSPOpC.OP.B.rs].W,RspGPR_Name(RSPOpC.OP.B.rs));
			JgeLabel32(&RspRecompPos, "BranchGreaterEqual", 0);
		} else {
			/* take a look at the branch compare variable */
			CompConstToVariable(&RspRecompPos, TRUE, &BranchCompare, "BranchCompare");
			JeLabel32(&RspRecompPos, "BranchGreaterEqual", 0);
		}
		Branch_AddRef(Target, (DWORD*)(RspRecompPos - 4));
		RSP_NextInstruction = FINISH_SUB_BLOCK;
	} else {
		RspCompilerWarning("BGEZAL error\nWeird Delay Slot.\n\nNextInstruction = %X\nEmulation will now stop", RSP_NextInstruction);
		BreakPoint(&RspRecompPos);
	}
}

/************************** Cop0 functions *************************/

void CompileRsp_Cop0_MF ( void ) {
	#ifndef Compile_Cop0
	InterpreterFallback((void*)RSP_Cop0_MF,"RSP_Cop0_MF"); return;
	#endif

	RSP_CPU_Message("  %X %s",RspCompilePC,RSPOpcodeName(RSPOpC.OP.Hex,RspCompilePC));

	if (RSPOpC.OP.R.rt == 0) {
		if (RSPOpC.OP.R.rd == 7) {
			MoveConstToVariable(&RspRecompPos, 1, &SP_SEMAPHORE_REG, "SP_SEMAPHORE_REG");
		}
		return;
	}

	switch (RSPOpC.OP.R.rd) {
	case 0:
		MoveVariableToX86reg(&RspRecompPos, &SP_MEM_ADDR_REG, "SP_MEM_ADDR_REG", x86_EAX);
		MoveX86regToVariable(&RspRecompPos, x86_EAX, &RSP_GPR[RSPOpC.OP.R.rt].UW, RspGPR_Name(RSPOpC.OP.R.rt));
		break;
	case 1:
		MoveVariableToX86reg(&RspRecompPos, &SP_DRAM_ADDR_REG, "SP_DRAM_ADDR_REG", x86_EAX);
		MoveX86regToVariable(&RspRecompPos, x86_EAX, &RSP_GPR[RSPOpC.OP.R.rt].UW, RspGPR_Name(RSPOpC.OP.R.rt));
		break;
	case 2:
		MoveVariableToX86reg(&RspRecompPos, &SP_RD_LEN_REG, "SP_RD_LEN_REG", x86_EAX);
		MoveX86regToVariable(&RspRecompPos, x86_EAX, &RSP_GPR[RSPOpC.OP.R.rt].UW, RspGPR_Name(RSPOpC.OP.R.rt));
		break;
	case 3:
		MoveVariableToX86reg(&RspRecompPos, &SP_WR_LEN_REG, "SP_WR_LEN_REG", x86_EAX);
		MoveX86regToVariable(&RspRecompPos, x86_EAX, &RSP_GPR[RSPOpC.OP.R.rt].UW, RspGPR_Name(RSPOpC.OP.R.rt));
		break;
	case 4: 
		MoveVariableToX86reg(&RspRecompPos, &SP_STATUS_REG, "SP_STATUS_REG", x86_EAX);
		MoveX86regToVariable(&RspRecompPos, x86_EAX, &RSP_GPR[RSPOpC.OP.R.rt].UW, RspGPR_Name(RSPOpC.OP.R.rt));
		break;
	case 5: 
		MoveVariableToX86reg(&RspRecompPos, &SP_DMA_FULL_REG, "SP_DMA_FULL_REG", x86_EAX);
		MoveX86regToVariable(&RspRecompPos, x86_EAX, &RSP_GPR[RSPOpC.OP.R.rt].UW, RspGPR_Name(RSPOpC.OP.R.rt));
		break;
	case 6: 
		MoveVariableToX86reg(&RspRecompPos, &SP_DMA_BUSY_REG, "SP_DMA_BUSY_REG", x86_EAX);
		MoveX86regToVariable(&RspRecompPos, x86_EAX, &RSP_GPR[RSPOpC.OP.R.rt].UW, RspGPR_Name(RSPOpC.OP.R.rt));
		break;
	case 7:
		MoveVariableToX86reg(&RspRecompPos, &SP_SEMAPHORE_REG, "SP_SEMAPHORE_REG", x86_EAX);
		MoveX86regToVariable(&RspRecompPos, x86_EAX, &RSP_GPR[RSPOpC.OP.R.rt].UW, RspGPR_Name(RSPOpC.OP.R.rt));
		MoveConstToVariable(&RspRecompPos, 1, &SP_SEMAPHORE_REG, "SP_SEMAPHORE_REG");
		break;
	case 8:
		MoveVariableToX86reg(&RspRecompPos, &DPC_START_REG, "DPC_START_REG", x86_EAX);
		MoveX86regToVariable(&RspRecompPos, x86_EAX, &RSP_GPR[RSPOpC.OP.R.rt].UW, RspGPR_Name(RSPOpC.OP.R.rt));
		break;
	case 9:
		MoveVariableToX86reg(&RspRecompPos, &DPC_END_REG, "DPC_END_REG", x86_EAX);
		MoveX86regToVariable(&RspRecompPos, x86_EAX, &RSP_GPR[RSPOpC.OP.R.rt].UW, RspGPR_Name(RSPOpC.OP.R.rt));
		break;
	case 10:
		MoveVariableToX86reg(&RspRecompPos, &DPC_CURRENT_REG, "DPC_CURRENT_REG", x86_EAX);
		MoveX86regToVariable(&RspRecompPos, x86_EAX, &RSP_GPR[RSPOpC.OP.R.rt].UW, RspGPR_Name(RSPOpC.OP.R.rt));
		break;
	case 11: 
		MoveVariableToX86reg(&RspRecompPos, &DPC_STATUS_REG, "DPC_STATUS_REG", x86_EAX);
		MoveX86regToVariable(&RspRecompPos, x86_EAX, &RSP_GPR[RSPOpC.OP.R.rt].UW, RspGPR_Name(RSPOpC.OP.R.rt));
		break;
	case 12: 
		MoveVariableToX86reg(&RspRecompPos, &DPC_CLOCK_REG, "DPC_CLOCK_REG", x86_EAX);
		MoveX86regToVariable(&RspRecompPos, x86_EAX, &RSP_GPR[RSPOpC.OP.R.rt].UW, RspGPR_Name(RSPOpC.OP.R.rt));
		break;

	default:
		RspCompilerWarning("have not implemented RSP MF CP0 reg %s (%d)",RspCOP0_Name(RSPOpC.OP.R.rd),RSPOpC.OP.R.rd);
	}
}

void CompileRsp_Cop0_MT ( void ) {
#ifndef Compile_Cop0
	InterpreterFallback((void*)RSP_Cop0_MT,"RSP_Cop0_MT");
	if (RSPOpC.OP.R.rd == 4 && RSP_NextInstruction != DELAY_SLOT) {
		CompileRsp_UpdateCycleCounts();
		RspCompilePC += 4;
		RspCompilePC &= 0xFFC;
		CompileRsp_CheckRspIsRunning();
		CompileRsp_SaveBeginOfSubBlock();
		RspCompilePC -= 4;
		RspCompilePC &= 0xFFC;
		return;
	}
	if (RSPOpC.OP.R.rd == 2) {
		BYTE* Jump;

		TestVariable(&RspRecompPos, 0x1000, &SP_MEM_ADDR_REG, "SP_MEM_ADDR_REG");
		JeLabel8(&RspRecompPos, "DontExit", 0);
		Jump = RspRecompPos - 1;

		CompileRsp_UpdateCycleCounts();

		if (RSP_NextInstruction == DELAY_SLOT) {
			MoveConstToVariable(&RspRecompPos, 0, &RSP_Running, "RSP_Running");
			Call_Direct(&RspRecompPos, (void*)SetRspJumpTable, "SetRspJumpTable");
		}
		else {
			MoveConstToVariable(&RspRecompPos, (RspCompilePC + 4) & 0xFFC, &SP_PC_REG, "RSP PC");
			Call_Direct(&RspRecompPos, (void*)SetRspJumpTable, "SetRspJumpTable");
			Ret(&RspRecompPos);
		}

		RSP_CPU_Message("DontExit:");
		x86_SetBranch8b(Jump, RspRecompPos);
	}
#else
	BYTE* Jump;
	BYTE* Jump2;
	BYTE* Jump3;

	RSP_CPU_Message("  %X %s",RspCompilePC,RSPOpcodeName(RSPOpC.OP.Hex,RspCompilePC));

	switch (RSPOpC.OP.R.rd) {
	case 0:
		if (IsRspRegConst(RSPOpC.OP.R.rt)) {
			MoveConstToVariable(&RspRecompPos, MipsRspRegConst(RSPOpC.OP.R.rt) & 0x1FF8, &SP_MEM_ADDR_REGW, "SP_MEM_ADDR_REGW");
		} else {
			MoveVariableToX86reg(&RspRecompPos, &RSP_GPR[RSPOpC.OP.R.rt].UW, RspGPR_Name(RSPOpC.OP.R.rt), x86_EAX);
			AndConstToX86Reg(&RspRecompPos, x86_EAX, 0x1FF8);
			MoveX86regToVariable(&RspRecompPos, x86_EAX, &SP_MEM_ADDR_REGW, "SP_MEM_ADDR_REGW");
		}
		break;
	case 1:
		if (IsRspRegConst(RSPOpC.OP.R.rt)) {
			MoveConstToVariable(&RspRecompPos, MipsRspRegConst(RSPOpC.OP.R.rt) & 0xFFFFF8, &SP_DRAM_ADDR_REGW, "SP_DRAM_ADDR_REGW");
		} else {
			MoveVariableToX86reg(&RspRecompPos, &RSP_GPR[RSPOpC.OP.R.rt].UW, RspGPR_Name(RSPOpC.OP.R.rt), x86_EAX);
			AndConstToX86Reg(&RspRecompPos, x86_EAX, 0xFFFFF8);
			MoveX86regToVariable(&RspRecompPos, x86_EAX, &SP_DRAM_ADDR_REGW, "SP_DRAM_ADDR_REGW");
		}
		break;
	case 2:
		if (IsRspRegConst(RSPOpC.OP.R.rt)) {
			MoveConstToVariable(&RspRecompPos, MipsRspRegConst(RSPOpC.OP.R.rt) & 0xFF8FFFF8, &SP_RD_LEN_REG, "SP_RD_LEN_REG");
		} else {
			MoveVariableToX86reg(&RspRecompPos, &RSP_GPR[RSPOpC.OP.R.rt].UW, RspGPR_Name(RSPOpC.OP.R.rt), x86_EAX);
			AndConstToX86Reg(&RspRecompPos, x86_EAX, 0xFF8FFFF8);
			MoveX86regToVariable(&RspRecompPos, x86_EAX, &SP_RD_LEN_REG, "SP_RD_LEN_REG");
		}
		Call_Direct(&RspRecompPos, (void*)SP_DMA_READ, "SP_DMA_READ");

		TestVariable(&RspRecompPos, 0x1000, &SP_MEM_ADDR_REG, "SP_MEM_ADDR_REG");
		JeLabel8(&RspRecompPos, "DontExit", 0);
		Jump = RspRecompPos - 1;

		CompileRsp_UpdateCycleCounts();

		if (RSP_NextInstruction == DELAY_SLOT) {
			MoveConstToVariable(&RspRecompPos, 0, &RSP_Running, "RSP_Running");
			Call_Direct(&RspRecompPos, (void*)SetRspJumpTable, "SetRspJumpTable");
		}
		else {
			MoveConstToVariable(&RspRecompPos, (RspCompilePC + 4) & 0xFFC, &SP_PC_REG, "RSP PC");
			Call_Direct(&RspRecompPos, (void*)SetRspJumpTable, "SetRspJumpTable");
			Ret(&RspRecompPos);
		}

		RSP_CPU_Message("DontExit:");
		x86_SetBranch8b(Jump, RspRecompPos);
		break;
	case 3:
		if (IsRspRegConst(RSPOpC.OP.R.rt)) {
			MoveConstToVariable(&RspRecompPos, MipsRspRegConst(RSPOpC.OP.R.rt) & 0xFF8FFFF8, &SP_WR_LEN_REG, "SP_WR_LEN_REG");
		} else {
			MoveVariableToX86reg(&RspRecompPos, &RSP_GPR[RSPOpC.OP.R.rt].UW, RspGPR_Name(RSPOpC.OP.R.rt), x86_EAX);
			AndConstToX86Reg(&RspRecompPos, x86_EAX, 0xFF8FFFF8);
			MoveX86regToVariable(&RspRecompPos, x86_EAX, &SP_WR_LEN_REG, "SP_WR_LEN_REG");
		}
		Call_Direct(&RspRecompPos, (void*)SP_DMA_WRITE, "SP_DMA_WRITE");
		break;
	case 4:
		if (IsRspRegConst(RSPOpC.OP.R.rt)) {
			MoveConstToX86reg(&RspRecompPos, MipsRspRegConst(RSPOpC.OP.R.rt), x86_ECX);
		} else {
			MoveVariableToX86reg(&RspRecompPos, &RSP_GPR[RSPOpC.OP.R.rt].UW, RspGPR_Name(RSPOpC.OP.R.rt), x86_ECX);
		}
		Call_Direct(&RspRecompPos, (void*)WriteRspStatusRegister, "WriteRspStatusRegister");
		BOOL neverHalt = IsRspRegConst(RSPOpC.OP.R.rt) && (MipsRspRegConst(RSPOpC.OP.R.rt) & SP_SET_HALT) == 0;
		if (!neverHalt) {
			TestVariable(&RspRecompPos, SP_STATUS_HALT, &SP_STATUS_REG, "SP_STATUS_REG");
			JeLabel8(&RspRecompPos, "NoHalt", 0);
			Jump = RspRecompPos - 1;
			MoveConstByteToVariable(&RspRecompPos, 0, &RSP_Running, "RSP_Running");
			x86_SetBranch8b(Jump, RspRecompPos);
		}
		if(RSP_NextInstruction != DELAY_SLOT && !neverHalt) {
			CompileRsp_UpdateCycleCounts();
			RspCompilePC += 4;
			RspCompilePC &= 0xFFC;
			CompileRsp_CheckRspIsRunning();
			CompileRsp_SaveBeginOfSubBlock();
			RspCompilePC -= 4;
			RspCompilePC &= 0xFFC;
			return;
		}
		break;
	case 7: 
		MoveConstToVariable(&RspRecompPos,0, &SP_SEMAPHORE_REG, "SP_SEMAPHORE_REG");
		break;
	case 8:
		TestVariable(&RspRecompPos, DPC_STATUS_START_VALID, &DPC_STATUS_REG, "DPC_STATUS_REG");
		JneLabel8(&RspRecompPos, "ValidDpcStart", 0);
		Jump = RspRecompPos - 1;
		if (IsRspRegConst(RSPOpC.OP.R.rt)) {
			MoveConstToVariable(&RspRecompPos, MipsRspRegConst(RSPOpC.OP.R.rt) & 0xFFFFF8, &DPC_START_REG, "DPC_START_REG");
		}
		else {
			MoveVariableToX86reg(&RspRecompPos, &RSP_GPR[RSPOpC.OP.R.rt].UW, RspGPR_Name(RSPOpC.OP.R.rt), x86_EAX);
			AndConstToX86Reg(&RspRecompPos, x86_EAX, 0xFFFFF8);
			MoveX86regToVariable(&RspRecompPos, x86_EAX, &DPC_START_REG, "DPC_START_REG");
		}
		OrConstToVariable(&RspRecompPos, DPC_STATUS_START_VALID, &DPC_STATUS_REG, "DPC_STATUS_REG");
		RSP_CPU_Message("ValidDpcStart:");
		x86_SetBranch8b(Jump, RspRecompPos);
		break;
	case 9:
		if (IsRspRegConst(RSPOpC.OP.R.rt)) {
			MoveConstToVariable(&RspRecompPos, MipsRspRegConst(RSPOpC.OP.R.rt) & 0xFFFFF8, &DPC_END_REG, "DPC_END_REG");
		} else {
			MoveVariableToX86reg(&RspRecompPos, &RSP_GPR[RSPOpC.OP.R.rt].UW, RspGPR_Name(RSPOpC.OP.R.rt), x86_EAX);
			AndConstToX86Reg(&RspRecompPos, x86_EAX, 0xFFFFF8);
			MoveX86regToVariable(&RspRecompPos, x86_EAX, &DPC_END_REG, "DPC_END_REG");
		}
		TestVariable(&RspRecompPos, DPC_STATUS_START_VALID, &DPC_STATUS_REG, "DPC_STATUS_REG");
		JeLabel8(&RspRecompPos, "InvalidDpcStart", 0);
		Jump = RspRecompPos - 1;

		MoveVariableToX86reg(&RspRecompPos, &DPC_START_REG, "DPC_START_REG", x86_EAX);
		MoveX86regToVariable(&RspRecompPos, x86_EAX, &DPC_CURRENT_REG, "DPC_CURRENT_REG");
		AndConstToVariable(&RspRecompPos, (DWORD)~DPC_STATUS_START_VALID, &DPC_STATUS_REG, "DPC_STATUS_REG");
		OrConstToVariable(&RspRecompPos, DPC_STATUS_PIPE_BUSY | DPC_STATUS_START_GCLK, &DPC_STATUS_REG, "DPC_STATUS_REG");
		RSP_CPU_Message("InvalidDpcStart:");
		x86_SetBranch8b(Jump, RspRecompPos);

		if (ProcessRDPList) {
			TestVariable(&RspRecompPos, DPC_STATUS_FREEZE, &DPC_STATUS_REG, "DPC_STATUS_REG");
			JneLabel8(&RspRecompPos, "NoProcessRdpList", 0);
			Jump = RspRecompPos - 1;

			TestVariable(&RspRecompPos, DPC_STATUS_START_VALID, &DPC_STATUS_REG, "DPC_STATUS_REG");
			JneLabel8(&RspRecompPos, "NoProcessRdpList", 0);
			Jump2 = RspRecompPos - 1;

			MoveVariableToX86reg(&RspRecompPos, &DPC_CURRENT_REG, "DPC_CURRENT_REG", x86_EAX);
			CompX86regToVariable(&RspRecompPos, x86_EAX, &DPC_END_REG, "DPC_END_REG");
			JaeLabel8(&RspRecompPos, "NoProcessRdpList", 0);
			Jump3 = RspRecompPos - 1;

			Call_Direct(&RspRecompPos, (void*)ProcessRDPList, "ProcessRdpList");

			RSP_CPU_Message("NoProcessRdpList:");
			x86_SetBranch8b(Jump, RspRecompPos);
			x86_SetBranch8b(Jump2, RspRecompPos);
			x86_SetBranch8b(Jump3, RspRecompPos);
		}
		break;
	/*case 10:
		MoveVariableToX86reg(&RSP_GPR[RSPOpC.rt].UW, GPR_Name(RSPOpC.rt), x86_EAX);
		MoveX86regToVariable(x86_EAX, RSPInfo.DPC_CURRENT_REG,"DPC_CURRENT_REG");
		break;*/

	case 11:
		if (IsRspRegConst(RSPOpC.OP.R.rt))
			MoveConstToX86reg(&RspRecompPos, MipsRspRegConst(RSPOpC.OP.R.rt), x86_ECX);
		else
			MoveVariableToX86reg(&RspRecompPos, &RSP_GPR[RSPOpC.OP.R.rt].UW, RspGPR_Name(RSPOpC.OP.R.rt), x86_ECX);
		Call_Direct(&RspRecompPos, (void*)WriteDPCStatusRegister, "WriteDPCStatusRegister");
		break;

	default:
		RspCompilerWarning("have not implemented RSP MT CP0 reg %s (%d)", RspCOP0_Name(RSPOpC.OP.R.rd), RSPOpC.OP.R.rd);
		break;
	}
#endif
}
/************************** Cop2 functions *************************/

void CompileRsp_Cop2_MF ( void ) {
	char Reg[256];
	int element = RSPOpC.OP.LSV.element;

	int element1 = 15 - element;
	int element2 = 15 - ((element + 1) % 16);
	
	#ifndef Compile_Cop2
	InterpreterFallback((void*)RSP_Cop2_MF,"RSP_Cop2_MF"); return;
	#endif

	RSP_CPU_Message("  %X %s",RspCompilePC,RSPOpcodeName(RSPOpC.OP.Hex,RspCompilePC));

	if (RSPOpC.OP.R.rt == 0) return;

	if (element2 != (element1 - 1)) {
		sprintf(Reg, "RSP_Vect[%i].B[%i]", RSPOpC.OP.R.rd, element1);
		MoveVariableToX86regHighByte(&RspRecompPos, &RSP_Vect[RSPOpC.OP.R.rd].B[element1], Reg, x86_EAX);

		sprintf(Reg, "RSP_Vect[%i].B[%i]", RSPOpC.OP.R.rd, element2);
		MoveVariableToX86regByte(&RspRecompPos, &RSP_Vect[RSPOpC.OP.R.rd].B[element2], Reg, x86_EAX);

		Cwde(&RspRecompPos);

		MoveX86regToVariable(&RspRecompPos, x86_EAX, &RSP_GPR[RSPOpC.OP.R.rt].W, RspGPR_Name(RSPOpC.OP.R.rt));
	} else {
		sprintf(Reg, "RSP_Vect[%i].B[%i]", RSPOpC.OP.R.rd, element2);
		MoveSxVariableToX86regHalf(&RspRecompPos, &RSP_Vect[RSPOpC.OP.R.rd].B[element2], Reg, x86_EAX);

		MoveX86regToVariable(&RspRecompPos, x86_EAX, &RSP_GPR[RSPOpC.OP.R.rt].W, RspGPR_Name(RSPOpC.OP.R.rt));
	}
}

void CompileRsp_Cop2_CF ( void ) {
	#ifndef Compile_CFC2
	InterpreterFallback((void*)RSP_Cop2_CF, "RSP_Cop2_CF"); return;
	#endif

	switch ((RSPOpC.OP.R.rd & 0x03)) {
	case 0:
		MoveSxVariableToX86regHalf(&RspRecompPos, &RspVCO, "VCO", x86_EAX);
		MoveX86regToVariable(&RspRecompPos, x86_EAX, &RSP_GPR[RSPOpC.OP.R.rt].W, RspGPR_Name(RSPOpC.OP.R.rt));
		break;
	case 1:
		MoveSxVariableToX86regHalf(&RspRecompPos, &RspVCC, "VCC", x86_EAX);
		MoveX86regToVariable(&RspRecompPos, x86_EAX, &RSP_GPR[RSPOpC.OP.R.rt].W, RspGPR_Name(RSPOpC.OP.R.rt));
		break;
	case 2:
	case 3:
		MoveZxVariableToX86regByte(&RspRecompPos, &RspVCE, "VCE", x86_EAX);
		MoveX86regToVariable(&RspRecompPos, x86_EAX, &RSP_GPR[RSPOpC.OP.R.rt].W, RspGPR_Name(RSPOpC.OP.R.rt));
		break;

	}
}

void CompileRsp_Cop2_MT ( void ) {
	char Reg[256];
	int element = 15 - RSPOpC.OP.MV.element;

	#ifndef Compile_Cop2
	InterpreterFallback((void*)RSP_Cop2_MT,"RSP_Cop2_MT"); return;
	#endif

	RSP_CPU_Message("  %X %s",RspCompilePC,RSPOpcodeName(RSPOpC.OP.Hex,RspCompilePC));
	if (IsRspRegConst(RSPOpC.OP.MV.rt)) {
		if (element == 0) {
			sprintf(Reg, "RSP_Vect[%i].B[%i]", RSPOpC.OP.MV.vs, element);
			MoveConstByteToVariable(&RspRecompPos, (MipsRspRegConst(RSPOpC.OP.MV.rt) >> 8) & 0xFF, &RSP_Vect[RSPOpC.OP.MV.vs].B[element], Reg);
		} else {
			sprintf(Reg, "RSP_Vect[%i].B[%i]", RSPOpC.OP.MV.vs, element - 1);
			MoveConstHalfToVariable(&RspRecompPos, MipsRspRegConst(RSPOpC.OP.MV.rt) & 0xFFFF, &RSP_Vect[RSPOpC.OP.MV.vs].B[element - 1], Reg);
		}
	}
	else {
		if (element == 0) {
			sprintf(Reg, "RSP_GPR[%i].B[1]", RSPOpC.OP.MV.rt);
			MoveVariableToX86regByte(&RspRecompPos, &RSP_GPR[RSPOpC.OP.MV.rt].B[1], Reg, x86_EAX);

			sprintf(Reg, "RSP_Vect[%i].B[%i]", RSPOpC.OP.MV.vs, element);
			MoveX86regByteToVariable(&RspRecompPos, x86_EAX, &RSP_Vect[RSPOpC.OP.MV.vs].B[element], Reg);
		} else {
			sprintf(Reg, "RSP_GPR[%i].B[0]", RSPOpC.OP.MV.rt);
			MoveVariableToX86regHalf(&RspRecompPos, &RSP_GPR[RSPOpC.OP.MV.rt].B[0], Reg, x86_EAX);

			sprintf(Reg, "RSP_Vect[%i].B[%i]", RSPOpC.OP.MV.vs, element - 1);
			MoveX86regHalfToVariable(&RspRecompPos, x86_EAX, &RSP_Vect[RSPOpC.OP.MV.vs].B[element - 1], Reg);
		}
	}
}

void CompileRsp_Cop2_CT ( void ) {
	#ifndef Compile_CTC2
	InterpreterFallback((void*)RSP_Cop2_CT, "RSP_Cop2_CT"); return;
	#endif

	switch ((RSPOpC.OP.R.rd & 0x03)) {
	case 0:
		if (IsRspRegConst(RSPOpC.OP.R.rt)) {
			MoveConstHalfToVariable(&RspRecompPos, MipsRspRegConst(RSPOpC.OP.R.rt) & 0xFFFF, &RspVCO, "VCO");
		} else {
			MoveVariableToX86regHalf(&RspRecompPos, &RSP_GPR[RSPOpC.OP.R.rt].W, RspGPR_Name(RSPOpC.OP.R.rt), x86_EAX);
			MoveX86regHalfToVariable(&RspRecompPos, x86_EAX, &RspVCO, "VCO");
		}
		break;
	case 1:
		if (IsRspRegConst(RSPOpC.OP.R.rt)) {
			MoveConstHalfToVariable(&RspRecompPos, MipsRspRegConst(RSPOpC.OP.R.rt) & 0xFFFF, &RspVCC, "VCC");
		}
		else {
			MoveVariableToX86regHalf(&RspRecompPos, &RSP_GPR[RSPOpC.OP.R.rt].W, RspGPR_Name(RSPOpC.OP.R.rt), x86_EAX);
			MoveX86regHalfToVariable(&RspRecompPos, x86_EAX, &RspVCC, "VCC");
		}
		break;
	case 2:
	case 3:
		if (IsRspRegConst(RSPOpC.OP.R.rt)) {
			MoveConstByteToVariable(&RspRecompPos, MipsRspRegConst(RSPOpC.OP.R.rt) & 0xFF, &RspVCE, "VCE");
		}
		else {
			MoveVariableToX86regByte(&RspRecompPos, &RSP_GPR[RSPOpC.OP.R.rt].W, RspGPR_Name(RSPOpC.OP.R.rt), x86_EAX);
			MoveX86regByteToVariable(&RspRecompPos, x86_EAX, &RspVCE, "VCE");
		}
		break;
	}
}

void CompileRsp_COP2_VECTOR (void) {
	((void (*)()) RSP_Vector[ RSPOpC.OP.R.funct ])();
}

/************************** Vect functions **************************/

MIPS_DWORD MMX_Scratch;

static void RSP_Element2Mmx(int MmxReg) {
	char Reg[256];

	DWORD Element = RSPOpC.OP.V.element & 0x0f;
	DWORD el;

	switch (Element) {
	case 0: case 1:
	case 2: case 3:
	case 4:	case 5:
	case 6:	case 7:
		RspCompilerWarning("Unimplemented RSP_Element2Mmx");
		break;

	default:
		/*
		 * Noticed the exclusive-or of seven to take into account
		 * the pseudo-swapping we have in the vector registers
		 */

		el = (RSPOpC.OP.V.element & 0x07) ^ 7;

		if (IsMmx2Enabled == FALSE) {
			sprintf(Reg, "RSP_Vect[%i].HW[%i]", RSPOpC.OP.V.vt, el);
			MoveVariableToX86regHalf(&RspRecompPos, &RSP_Vect[RSPOpC.OP.V.vt].HW[el], Reg, x86_ECX);
			MoveX86regHalfToVariable(&RspRecompPos, x86_ECX, &MMX_Scratch.HW[0], "MMX_Scratch.HW[0]");
			MoveX86regHalfToVariable(&RspRecompPos, x86_ECX, &MMX_Scratch.HW[1], "MMX_Scratch.HW[1]");
			MoveX86regHalfToVariable(&RspRecompPos, x86_ECX, &MMX_Scratch.HW[2], "MMX_Scratch.HW[2]");
			MoveX86regHalfToVariable(&RspRecompPos, x86_ECX, &MMX_Scratch.HW[3], "MMX_Scratch.HW[3]");
			MmxMoveQwordVariableToReg(&RspRecompPos, MmxReg, &MMX_Scratch.HW[0], "MMX_Scratch.HW[0]");
		} else {
			unsigned long Qword;
			
			Qword = (el >> 2) & 0x1;
			el &= 0x3;

			sprintf(Reg, "RSP_Vect[%i].DW[%i]", RSPOpC.OP.V.vt, Qword);
			MmxShuffleMemoryToReg(&RspRecompPos, MmxReg, 
				&RSP_Vect[RSPOpC.OP.V.vt].DW[Qword], Reg, _MMX_SHUFFLE(el, el, el, el));
		}
		break;
	}
}

static void RSP_Element2Sse(int SseReg) {
	char Reg[256];

	DWORD Element = RSPOpC.OP.V.element & 0x0f;
	DWORD el;

	switch (Element) {
	case 0: case 1:
	case 2: case 3:
	case 4:	case 5:
	case 6:	case 7:
		RspCompilerWarning("Unimplemented RSP_Element2Sse");
		break;

	default:
		/*
		 * Noticed the exclusive-or of seven to take into account
		 * the pseudo-swapping we have in the vector registers
		 */

		el = (RSPOpC.OP.V.element & 0x07) ^ 7;

		if (IsAvxEnabled == TRUE && IsAvx2Enabled == TRUE) {
			sprintf(Reg, "RSP_Vect[%i].UHW[%i]", RSPOpC.OP.V.vt, el);
			AvxVPBroadcastwVariableToReg128(&RspRecompPos, SseReg, &RSP_Vect[RSPOpC.OP.V.vt].UHW[el], Reg);
		} else {
			unsigned long Qword;

			Qword = (el >> 2) & 0x1;
			el &= 0x3;

			sprintf(Reg, "RSP_Vect[%i].DW[%i]", RSPOpC.OP.V.vt, Qword);
			if (Qword == 0) {
				Sse2ShuffleLowWordsMemoryToReg(&RspRecompPos, SseReg,
					&RSP_Vect[RSPOpC.OP.V.vt].UDW, Reg, _MMX_SHUFFLE(el, el, el, el));
				Sse2ShuffleDWordsRegToReg(&RspRecompPos, SseReg, SseReg, _MMX_SHUFFLE(1, 0, 1, 0));
			}
			else {
				Sse2ShuffleHighWordsMemoryToReg(&RspRecompPos, SseReg,
					&RSP_Vect[RSPOpC.OP.V.vt].UDW, Reg, _MMX_SHUFFLE(el, el, el, el));
				Sse2ShuffleDWordsRegToReg(&RspRecompPos, SseReg, SseReg, _MMX_SHUFFLE(3, 2, 3, 2));
			}
		}
		break;
	}
}

static void RSP_MultiElement2Mmx(int MmxReg1, int MmxReg2) {
	char Reg[256];
	DWORD Element = RSPOpC.OP.V.element & 0x0f;

	/*
	 * Ok, this is tricky, hopefully this clears it up:
	 *
	 * $vd[0] = $vd[0] + $vt[2] 
	 * because of swapped registers becomes:
	 * $vd[7] = $vd[7] + $vt[5]
	 *
	 * we must perform this swap correctly, this involves the 3-bit
	 * xclusive or, 2-bits of which are done within a dword boundary, 
	 * the last bit, is ignored because we are loading the source linearly,
	 * so the xclusive or has transparently happened on that side
	 *
	 */

	switch (Element) {
/*	case 0:
		LogMessage("TODO: RSP_MultiElement2Mmx 0");
	case 1:*/
		/*sprintf(Reg, "RSP_Vect[%i].UHW[0]", RSPOpC.rt);
		MmxMoveQwordVariableToReg(MmxReg1, &RSP_Vect[RSPOpC.rt].UHW[0], Reg);
		sprintf(Reg, "RSP_Vect[%i].UHW[4]", RSPOpC.rt);
		MmxMoveQwordVariableToReg(MmxReg2, &RSP_Vect[RSPOpC.rt].UHW[4], Reg);
		break;*/
/*		LogMessage("TODO: RSP_MultiElement2Mmx 1");
	case 2:*/
		/* [0q]    | 0 | 0 | 2 | 2 | 4 | 4 | 6 | 6 | */
/*		sprintf(Reg, "RSP_Vect[%i].DW[0]", RSPOpC.rt);
		MmxShuffleMemoryToReg(MmxReg1, &RSP_Vect[RSPOpC.rt].DW[0], Reg, 0xF5);
		sprintf(Reg, "RSP_Vect[%i].DW[1]", RSPOpC.rt);
		MmxShuffleMemoryToReg(MmxReg2, &RSP_Vect[RSPOpC.rt].DW[1], Reg, 0xF5);
		break;*/
/*		LogMessage("TODO: RSP_MultiElement2Mmx 2");*/
	case 3:
		/* [1q]    | 1 | 1 | 3 | 3 | 5 | 5 | 7 | 7 | */
		sprintf(Reg, "RSP_Vect[%i].DW[0]", RSPOpC.OP.V.vt);
		MmxShuffleMemoryToReg(&RspRecompPos, MmxReg1, &RSP_Vect[RSPOpC.OP.V.vt].DW[0], Reg, 0xA0);
		sprintf(Reg, "RSP_Vect[%i].DW[1]", RSPOpC.OP.V.vt);
		MmxShuffleMemoryToReg(&RspRecompPos, MmxReg2, &RSP_Vect[RSPOpC.OP.V.vt].DW[1], Reg, 0xA0);
		break;
	case 4:
		/* [0h]    | 0 | 0 | 0 | 0 | 4 | 4 | 4 | 4 | */
		sprintf(Reg, "RSP_Vect[%i].DW[0]", RSPOpC.OP.V.vt);
		MmxShuffleMemoryToReg(&RspRecompPos, MmxReg1, &RSP_Vect[RSPOpC.OP.V.vt].DW[0], Reg, 0xFF);
		sprintf(Reg, "RSP_Vect[%i].DW[1]", RSPOpC.OP.V.vt);
		MmxShuffleMemoryToReg(&RspRecompPos, MmxReg2, &RSP_Vect[RSPOpC.OP.V.vt].DW[1], Reg, 0xFF);
		break;
	case 5:
		/* [1h]    | 1 | 1 | 1 | 1 | 5 | 5 | 5 | 5 | */
		sprintf(Reg, "RSP_Vect[%i].DW[0]", RSPOpC.OP.V.vt);
		MmxShuffleMemoryToReg(&RspRecompPos, MmxReg1, &RSP_Vect[RSPOpC.OP.V.vt].DW[0], Reg, 0xAA);
		sprintf(Reg, "RSP_Vect[%i].DW[1]", RSPOpC.OP.V.vt);
		MmxShuffleMemoryToReg(&RspRecompPos, MmxReg2, &RSP_Vect[RSPOpC.OP.V.vt].DW[1], Reg, 0xAA);
		break;
	case 6:
		/* [2h]    | 2 | 2 | 2 | 2 | 6 | 6 | 6 | 6 | */
		sprintf(Reg, "RSP_Vect[%i].DW[0]", RSPOpC.OP.V.vt);
		MmxShuffleMemoryToReg(&RspRecompPos, MmxReg1, &RSP_Vect[RSPOpC.OP.V.vt].DW[0], Reg, 0x55);
		sprintf(Reg, "RSP_Vect[%i].DW[1]", RSPOpC.OP.V.vt);
		MmxShuffleMemoryToReg(&RspRecompPos, MmxReg2, &RSP_Vect[RSPOpC.OP.V.vt].DW[1], Reg, 0x55);
		break;
/*	case 7:*/
		/* [3h]    | 3 | 3 | 3 | 3 | 7 | 7 | 7 | 7 | */
/*		sprintf(Reg, "RSP_Vect[%i].DW[0]", RSPOpC.rt);
		MmxShuffleMemoryToReg(MmxReg1, &RSP_Vect[RSPOpC.rt].DW[0], Reg, 0x00);
		sprintf(Reg, "RSP_Vect[%i].DW[1]", RSPOpC.rt);
		MmxShuffleMemoryToReg(MmxReg2, &RSP_Vect[RSPOpC.rt].DW[1], Reg, 0x00);
		break;*/
//		LogMessage("TODO: RSP_MultiElement2Mmx 7");

	default:
		RspCompilerWarning("Unimplemented RSP_MultiElement2Mmx [%d]", Element);
		LogMessage("TODO: multi");
		break;
	}
}

static void RSP_MultiElement2Sse(int SseReg) {
	char Reg[256];
	DWORD Element = RSPOpC.OP.V.element & 0x0f;

	/*
	 * Ok, this is tricky, hopefully this clears it up:
	 *
	 * $vd[0] = $vd[0] + $vt[2]
	 * because of swapped registers becomes:
	 * $vd[7] = $vd[7] + $vt[5]
	 *
	 * we must perform this swap correctly, this involves the 3-bit
	 * xclusive or, 2-bits of which are done within a dword boundary,
	 * the last bit, is ignored because we are loading the source linearly,
	 * so the xclusive or has transparently happened on that side
	 *
	 */

	switch (Element) {
		/*	case 0:
				LogMessage("TODO: RSP_MultiElement2Mmx 0");
			case 1:*/
			/*sprintf(Reg, "RSP_Vect[%i].UHW[0]", RSPOpC.rt);
			MmxMoveQwordVariableToReg(MmxReg1, &RSP_Vect[RSPOpC.rt].UHW[0], Reg);
			sprintf(Reg, "RSP_Vect[%i].UHW[4]", RSPOpC.rt);
			MmxMoveQwordVariableToReg(MmxReg2, &RSP_Vect[RSPOpC.rt].UHW[4], Reg);
			break;*/
			/*		LogMessage("TODO: RSP_MultiElement2Mmx 1");*/
	case 2:
		/* [0q]    | 0 | 0 | 2 | 2 | 4 | 4 | 6 | 6 | */
		sprintf(Reg, "RSP_Vect[%i]", RSPOpC.OP.V.vt);
		Sse2ShuffleLowWordsMemoryToReg(&RspRecompPos, SseReg, &RSP_Vect[RSPOpC.OP.V.vt].DW[0], Reg, 0xF5);
		Sse2ShuffleHighWordsRegToReg(&RspRecompPos, SseReg, SseReg, 0xF5);
		break;
	case 3:
		/* [1q]    | 1 | 1 | 3 | 3 | 5 | 5 | 7 | 7 | */
		sprintf(Reg, "RSP_Vect[%i]", RSPOpC.OP.V.vt);
		Sse2ShuffleLowWordsMemoryToReg(&RspRecompPos, SseReg, &RSP_Vect[RSPOpC.OP.V.vt].DW[0], Reg, 0xA0);
		Sse2ShuffleHighWordsRegToReg(&RspRecompPos, SseReg, SseReg, 0xA0);
		break;
	case 4:
		/* [0h]    | 0 | 0 | 0 | 0 | 4 | 4 | 4 | 4 | */
		sprintf(Reg, "RSP_Vect[%i]", RSPOpC.OP.V.vt);
		Sse2ShuffleLowWordsMemoryToReg(&RspRecompPos, SseReg, &RSP_Vect[RSPOpC.OP.V.vt].DW[0], Reg, 0xFF);
		Sse2ShuffleHighWordsRegToReg(&RspRecompPos, SseReg, SseReg, 0xFF);
		break;
	case 5:
		/* [1h]    | 1 | 1 | 1 | 1 | 5 | 5 | 5 | 5 | */
		sprintf(Reg, "RSP_Vect[%i]", RSPOpC.OP.V.vt);
		Sse2ShuffleLowWordsMemoryToReg(&RspRecompPos, SseReg, &RSP_Vect[RSPOpC.OP.V.vt].DW[0], Reg, 0xAA);
		Sse2ShuffleHighWordsRegToReg(&RspRecompPos, SseReg, SseReg, 0xAA);
		break;
	case 6:
		/* [2h]    | 2 | 2 | 2 | 2 | 6 | 6 | 6 | 6 | */
		sprintf(Reg, "RSP_Vect[%i]", RSPOpC.OP.V.vt);
		Sse2ShuffleLowWordsMemoryToReg(&RspRecompPos, SseReg, &RSP_Vect[RSPOpC.OP.V.vt].DW[0], Reg, 0x55);
		Sse2ShuffleHighWordsRegToReg(&RspRecompPos, SseReg, SseReg, 0x55);
		break;
	case 7:
		/* [3h]    | 3 | 3 | 3 | 3 | 7 | 7 | 7 | 7 | */
		sprintf(Reg, "RSP_Vect[%i]", RSPOpC.OP.V.vt);
		Sse2ShuffleLowWordsMemoryToReg(&RspRecompPos, SseReg, &RSP_Vect[RSPOpC.OP.V.vt].DW[0], Reg, 0x00);
		Sse2ShuffleHighWordsRegToReg(&RspRecompPos, SseReg, SseReg, 0x00);
		break;

	default:
		RspCompilerWarning("Unimplemented RSP_MultiElement2Sse [%d]", Element);
		LogMessage("TODO: sse multi");
		break;
	}
}

static BOOL Compile_Vector_VMULF_MMX ( void ) {
	char Reg[256];
	static __int64 MIN_VALUE = 0x8000800080008000LL;
	static __int64 ROUNDED_RESULT = 0x7FFF7FFF7FFF7FFFLL;

	/* Do our MMX checks here */
	if (IsMmxEnabled == FALSE)
		return FALSE;
	if ((RSPOpC.OP.V.element & 0x0f) >= 2 && (RSPOpC.OP.V.element & 0x0f) <= 7 && IsMmx2Enabled == FALSE)
		return FALSE;

	sprintf(Reg, "RSP_Vect[%i].UHW[0]", RSPOpC.OP.V.vs);
	MmxMoveQwordVariableToReg(&RspRecompPos, x86_MM0, &RSP_Vect[RSPOpC.OP.V.vs].UHW[0], Reg);
	sprintf(Reg, "RSP_Vect[%i].UHW[4]", RSPOpC.OP.V.vs);
	MmxMoveQwordVariableToReg(&RspRecompPos, x86_MM1, &RSP_Vect[RSPOpC.OP.V.vs].UHW[4], Reg);

	// check if vs is 0x8000
	MmxMoveQwordVariableToReg(&RspRecompPos, x86_MM5, &MIN_VALUE, "MIN_VALUE(0x8000)");
	MmxMoveRegToReg(&RspRecompPos, x86_MM6, x86_MM5);
	MmxCompareEqualWordRegToReg(&RspRecompPos, x86_MM6, x86_MM0);
	MmxMoveRegToReg(&RspRecompPos, x86_MM7, x86_MM5);
	MmxCompareEqualWordRegToReg(&RspRecompPos, x86_MM7, x86_MM1);

	int mmxRegForSecondVT = x86_MM3;

	if ((RSPOpC.OP.V.element & 0xF) < 2) {
		sprintf(Reg, "RSP_Vect[%i].UHW[0]", RSPOpC.OP.V.vt);
		MmxMoveQwordVariableToReg(&RspRecompPos, x86_MM2, &RSP_Vect[RSPOpC.OP.V.vt].UHW[0], Reg);
		sprintf(Reg, "RSP_Vect[%i].UHW[4]", RSPOpC.OP.V.vt);
		MmxMoveQwordVariableToReg(&RspRecompPos, x86_MM3, &RSP_Vect[RSPOpC.OP.V.vt].UHW[4], Reg);
	} else if ((RSPOpC.OP.V.element & 0xF) >= 8) {
		RSP_Element2Mmx(x86_MM2);
		mmxRegForSecondVT = x86_MM2;
	} else {
		RSP_MultiElement2Mmx(x86_MM2, x86_MM3);
	}

	// 1st half of ((vs * vt) << 1) + 0x8000
	MmxMoveRegToReg(&RspRecompPos, x86_MM4, x86_MM0);
	MmxPmulhwRegToReg(&RspRecompPos, x86_MM0, x86_MM2);
	MmxPsllwImmed(&RspRecompPos, x86_MM0, 1);
	MmxPmullwRegToReg(&RspRecompPos, x86_MM4, x86_MM2);
	MmxMoveRegToReg(&RspRecompPos, x86_MM5, x86_MM4);
	MmxPsrlwImmed(&RspRecompPos, x86_MM4, 15);
	MmxPorRegToReg(&RspRecompPos, x86_MM0, x86_MM4);
	MmxPsllwImmed(&RspRecompPos, x86_MM5, 14);
	MmxPsrlwImmed(&RspRecompPos, x86_MM5, 15);
	MmxPaddwRegToReg(&RspRecompPos, x86_MM0, x86_MM5);

	// 2nd half of ((vs * vt) << 1) + 0x8000
	MmxMoveRegToReg(&RspRecompPos, x86_MM4, x86_MM1);
	MmxPmulhwRegToReg(&RspRecompPos, x86_MM1, mmxRegForSecondVT);
	MmxPsllwImmed(&RspRecompPos, x86_MM1, 1);
	MmxPmullwRegToReg(&RspRecompPos, x86_MM4, mmxRegForSecondVT);
	MmxMoveRegToReg(&RspRecompPos, x86_MM5, x86_MM4);
	MmxPsrlwImmed(&RspRecompPos, x86_MM4, 15);
	MmxPorRegToReg(&RspRecompPos, x86_MM1, x86_MM4);
	MmxPsllwImmed(&RspRecompPos, x86_MM5, 14);
	MmxPsrlwImmed(&RspRecompPos, x86_MM5, 15);
	MmxPaddwRegToReg(&RspRecompPos, x86_MM0, x86_MM5);

	// check if vt is 0x8000 and round result when required
	MmxMoveQwordVariableToReg(&RspRecompPos, x86_MM5, &MIN_VALUE, "MIN_VALUE(0x8000)");
	MmxMoveRegToReg(&RspRecompPos, x86_MM4, x86_MM5);
	MmxCompareEqualWordRegToReg(&RspRecompPos, x86_MM4, x86_MM2);
	MmxCompareEqualWordRegToReg(&RspRecompPos, x86_MM5, mmxRegForSecondVT);

	MmxPandRegToReg(&RspRecompPos, x86_MM4, x86_MM6);
	MmxPandRegToReg(&RspRecompPos, x86_MM5, x86_MM7);

	MmxMoveQwordVariableToReg(&RspRecompPos, x86_MM6, &ROUNDED_RESULT, "ROUNDED_RESULT(0x7FFF)");
	MmxMoveRegToReg(&RspRecompPos, x86_MM7, x86_MM6);

	MmxPandRegToReg(&RspRecompPos, x86_MM6, x86_MM4);
	MmxPandRegToReg(&RspRecompPos, x86_MM7, x86_MM5);

	MmxPandnRegToReg(&RspRecompPos, x86_MM4, x86_MM0);
	MmxPandnRegToReg(&RspRecompPos, x86_MM5, x86_MM1);

	MmxPorRegToReg(&RspRecompPos, x86_MM4, x86_MM6);
	MmxPorRegToReg(&RspRecompPos, x86_MM5, x86_MM7);

	sprintf(Reg, "RSP_Vect[%i].UHW[0]", RSPOpC.OP.V.vd);
	MmxMoveQwordRegToVariable(&RspRecompPos, x86_MM4, &RSP_Vect[RSPOpC.OP.V.vd].UHW[0], Reg);
	sprintf(Reg, "RSP_Vect[%i].UHW[4]", RSPOpC.OP.V.vd);
	MmxMoveQwordRegToVariable(&RspRecompPos, x86_MM5, &RSP_Vect[RSPOpC.OP.V.vd].UHW[4], Reg);

	if (IsNextRspInstructionMmx(RspCompilePC) == FALSE)
		MmxEmptyMultimediaState(&RspRecompPos);

	return TRUE;
}

static BOOL Compile_Vector_VMULF_NoAccum_SSE2(void) {
	char Reg[256];
	static VECTOR MIN_VALUE;
	static VECTOR ROUNDED_RESULT;
	static BOOL constInitialized = FALSE;

	if (!constInitialized) {
		MIN_VALUE.UDW[0] = MIN_VALUE.UDW[1] = 0x8000800080008000ULL;
		ROUNDED_RESULT.UDW[0] = ROUNDED_RESULT.UDW[1] = 0x7FFF7FFF7FFF7FFFULL;
		constInitialized = TRUE;
	}

	/* Do our SSE checks here */
	if (IsSseEnabled == FALSE || IsSse2Enabled == FALSE)
		return FALSE;

	sprintf(Reg, "RSP_Vect[%i]", RSPOpC.OP.V.vs);
	SseMoveAlignedVariableToReg(&RspRecompPos, &RSP_Vect[RSPOpC.OP.V.vs].UHW[0], Reg, x86_XMM0, SseType_QuadWord, TRUE);

	// check if vs is 0x8000
	SseMoveAlignedVariableToReg(&RspRecompPos, &MIN_VALUE.sseValue, "MIN_VALUE(0x8000)", x86_XMM5, SseType_QuadWord, TRUE);
	SseMoveRegToReg(&RspRecompPos, x86_XMM6, x86_XMM5, SseType_QuadWord, TRUE);
	Sse2CompareEqualWordRegToReg(&RspRecompPos, x86_XMM6, x86_XMM0);

	if ((RSPOpC.OP.V.element & 0xF) < 2) {
		sprintf(Reg, "RSP_Vect[%i]", RSPOpC.OP.V.vt);
		SseMoveAlignedVariableToReg(&RspRecompPos, &RSP_Vect[RSPOpC.OP.V.vt].UHW[0], Reg, x86_XMM2, SseType_QuadWord, TRUE);
	} else if ((RSPOpC.OP.V.element & 0xF) >= 8) {
		RSP_Element2Sse(x86_XMM2);
	} else {
		RSP_MultiElement2Sse(x86_XMM2);
	}

	// ((vs * vt) << 1) + 0x8000
	SseMoveRegToReg(&RspRecompPos, x86_XMM4, x86_XMM0, SseType_QuadWord, TRUE);
	Sse2PmulhwRegToReg(&RspRecompPos, x86_XMM0, x86_XMM2);
	Sse2PsllwImmed(&RspRecompPos, x86_XMM0, 1);
	Sse2PmullwRegToReg(&RspRecompPos, x86_XMM4, x86_XMM2);
	SseMoveRegToReg(&RspRecompPos, x86_XMM1, x86_XMM4, SseType_QuadWord, TRUE);
	Sse2PsrlwImmed(&RspRecompPos, x86_XMM4, 15);
	Sse2PorRegToReg(&RspRecompPos, x86_XMM0, x86_XMM4);
	Sse2PsllwImmed(&RspRecompPos, x86_XMM1, 14);
	Sse2PsrlwImmed(&RspRecompPos, x86_XMM1, 15);
	Sse2PaddwRegToReg(&RspRecompPos, x86_XMM0, x86_XMM1);

	// check if vt is 0x8000 and round result when required
	Sse2CompareEqualWordRegToReg(&RspRecompPos, x86_XMM5, x86_XMM2);
	Sse2PandRegToReg(&RspRecompPos, x86_XMM5, x86_XMM6);
	SseMoveAlignedVariableToReg(&RspRecompPos, &ROUNDED_RESULT, "ROUNDED_RESULT(0x7FFF)", x86_XMM6, SseType_QuadWord, TRUE);
	Sse2PandRegToReg(&RspRecompPos, x86_XMM6, x86_XMM5);
	Sse2PandnRegToReg(&RspRecompPos, x86_XMM5, x86_XMM0);
	Sse2PorRegToReg(&RspRecompPos, x86_XMM5, x86_XMM6);

	sprintf(Reg, "RSP_Vect[%i]", RSPOpC.OP.V.vd);
	SseMoveAlignedRegToVariable(&RspRecompPos, x86_XMM5, &RSP_Vect[RSPOpC.OP.V.vd].UHW[0], Reg, SseType_QuadWord, TRUE);

	return TRUE;
}

static BOOL Compile_Vector_VMULF_SSE2(BOOL writeToVectorDest) {
	char Reg[256];
	static VECTOR MIN_VALUE;
	static VECTOR MAX_VALUE;
	static BOOL constInitialized = FALSE;

	if (!constInitialized) {
		MIN_VALUE.UDW[0] = MIN_VALUE.UDW[1] = 0x8000800080008000ULL;
		MAX_VALUE.UDW[0] = MAX_VALUE.UDW[1] = 0x7FFF7FFF7FFF7FFFULL;
		constInitialized = TRUE;
	}

	/* Do our SSE checks here */
	if (IsSseEnabled == FALSE || IsSse2Enabled == FALSE)
		return FALSE;

	// load vs and vt
	sprintf(Reg, "RSP_Vect[%i]", RSPOpC.OP.V.vs);
	SseMoveAlignedVariableToReg(&RspRecompPos, &RSP_Vect[RSPOpC.OP.V.vs].UHW[0], Reg, x86_XMM1, SseType_QuadWord, TRUE);

	if ((RSPOpC.OP.V.element & 0xF) < 2) {
		sprintf(Reg, "RSP_Vect[%i]", RSPOpC.OP.V.vt);
		SseMoveAlignedVariableToReg(&RspRecompPos, &RSP_Vect[RSPOpC.OP.V.vt].UHW[0], Reg, x86_XMM2, SseType_QuadWord, TRUE);
	} else if ((RSPOpC.OP.V.element & 0xF) >= 8) {
		RSP_Element2Sse(x86_XMM2);
	} else {
		RSP_MultiElement2Sse(x86_XMM2);
	}

	// check overflow
	SseMoveAlignedVariableToReg(&RspRecompPos, &MIN_VALUE, "MIN_VALUE(0x8000)", x86_XMM7, SseType_QuadWord, TRUE);
	SseMoveRegToReg(&RspRecompPos, x86_XMM0, x86_XMM1, SseType_QuadWord, TRUE);
	Sse2CompareEqualWordRegToReg(&RspRecompPos, x86_XMM0, x86_XMM7);
	SseMoveRegToReg(&RspRecompPos, x86_XMM5, x86_XMM2, SseType_QuadWord, TRUE);
	Sse2CompareEqualWordRegToReg(&RspRecompPos, x86_XMM5, x86_XMM7);
	Sse2PandRegToReg(&RspRecompPos, x86_XMM0, x86_XMM5);

	// ((vs * vt) << 1) + 0x8000
	SseMoveRegToReg(&RspRecompPos, x86_XMM3, x86_XMM2, SseType_QuadWord, TRUE);
	Sse2PmullwRegToReg(&RspRecompPos, x86_XMM3, x86_XMM1);
	Sse2PmulhwRegToReg(&RspRecompPos, x86_XMM1, x86_XMM2);

	// accum low
	SseMoveRegToReg(&RspRecompPos, x86_XMM2, x86_XMM3, SseType_QuadWord, TRUE);
	Sse2PsllwImmed(&RspRecompPos, x86_XMM2, 1);
	Sse2PaddwRegToReg(&RspRecompPos, x86_XMM2, x86_XMM7);
	SseMoveAlignedRegToVariable(&RspRecompPos, x86_XMM2, &RSP_ACCUM_LOW.UHW[0], "RSP_ACCUM_LOW", SseType_QuadWord, TRUE);

	// accum mid
	Sse2PsllwImmed(&RspRecompPos, x86_XMM1, 1);
	SseMoveRegToReg(&RspRecompPos, x86_XMM2, x86_XMM3, SseType_QuadWord, TRUE);
	Sse2PsrlwImmed(&RspRecompPos, x86_XMM2, 15);
	Sse2PorRegToReg(&RspRecompPos, x86_XMM1, x86_XMM2);
	Sse2PsllwImmed(&RspRecompPos, x86_XMM3, 1);
	Sse2PsrlwImmed(&RspRecompPos, x86_XMM3, 15);
	Sse2PaddwRegToReg(&RspRecompPos, x86_XMM1, x86_XMM3);
	SseMoveAlignedRegToVariable(&RspRecompPos, x86_XMM1, &RSP_ACCUM_MID.UHW[0], "RSP_ACCUM_MID", SseType_QuadWord, TRUE);

	// mask
	int blendedReg = x86_XMM1;
	if (IsSse41Enabled == TRUE) {
		Sse41PBlendVariableToRegWithXMM0Mask(&RspRecompPos, x86_XMM1, &MAX_VALUE, "MAX_VALUE(0x7FFF)");
	} else {
		SseMoveRegToReg(&RspRecompPos, x86_XMM4, x86_XMM0, SseType_QuadWord, TRUE);
		Sse2PandnRegToReg(&RspRecompPos, x86_XMM4, x86_XMM1);
		blendedReg = x86_XMM4;
	}

	// vect
	if (writeToVectorDest) {
		if (IsSse41Enabled == FALSE) {
			Sse2PandVariableToReg(&RspRecompPos, x86_XMM0, &MAX_VALUE, "MAX_VALUE(0x7FFF)");
			Sse2PorRegToReg(&RspRecompPos, x86_XMM4, x86_XMM0);
		}
		sprintf(Reg, "RSP_Vect[%i]", RSPOpC.OP.V.vd);
		SseMoveAlignedRegToVariable(&RspRecompPos, blendedReg, &RSP_Vect[RSPOpC.OP.V.vd].UHW[0], Reg, SseType_QuadWord, TRUE);
	}

	// accum high
	Sse2PsrawImmed(&RspRecompPos, blendedReg, 15);
	SseMoveAlignedRegToVariable(&RspRecompPos, blendedReg, &RSP_ACCUM_HIGH.UHW[0], "RSP_ACCUM_HIGH", SseType_QuadWord, TRUE);

	return TRUE;
}

static BOOL Compile_Vector_VMULF_NoAccum_AVX(void) {
	char Reg[256];
	static const DWORD C8000 = 0x8000;
	static const DWORD C7FFF0000 = 0x7FFF0000;

	/* Do our AVX checks here */
	if (IsAvxEnabled == FALSE || IsAvx2Enabled == FALSE || IsSse2Enabled == FALSE || IsSse41Enabled == FALSE)
		return FALSE;

	sprintf(Reg, "RSP_Vect[%i]", RSPOpC.OP.V.vs);
	AvxVPMovesxWordVariableToDWordReg256(&RspRecompPos, x86_YMM0, &RSP_Vect[RSPOpC.OP.V.vs].UHW[0], Reg);

	if ((RSPOpC.OP.V.element & 0xF) < 2) {
		sprintf(Reg, "RSP_Vect[%i]", RSPOpC.OP.V.vt);
		AvxVPMovesxWordVariableToDWordReg256(&RspRecompPos, x86_YMM2, &RSP_Vect[RSPOpC.OP.V.vt].UHW[0], Reg);
	}
	else if ((RSPOpC.OP.V.element & 0xF) >= 8) {
		RSP_Element2Sse(x86_XMM2);
		AvxVPMovesxWordReg128ToDwordReg256(&RspRecompPos, x86_YMM2, x86_XMM2);
	}
	else {
		RSP_MultiElement2Sse(x86_XMM2);
		AvxVPMovesxWordReg128ToDwordReg256(&RspRecompPos, x86_YMM2, x86_XMM2);
	}

	// ((vs * vt) << 1) + 0x8000
	AvxVPMulldRegToReg256(&RspRecompPos, x86_YMM2, x86_YMM2, x86_YMM0);
	AvxVPSlldRegToReg256Immed(&RspRecompPos, x86_YMM2, x86_YMM2, 1);
	AvxVPBroadcastdVariableToReg256(&RspRecompPos, x86_YMM7, (void*)&C8000, "Constant(0x8000)");
	AvxVPAdddRegToReg256(&RspRecompPos, x86_YMM2, x86_YMM2, x86_YMM7);

	// check which result has value 0x80008000
	AvxVPSlldRegToReg256Immed(&RspRecompPos, x86_YMM6, x86_YMM7, 16);
	AvxVPorRegToReg256(&RspRecompPos, x86_YMM7, x86_YMM7, x86_YMM6);

	AvxVPBroadcastdVariableToReg256(&RspRecompPos, x86_YMM6, (void*)&C7FFF0000, "Constant(0x7FFF0000)");
	AvxCompareEqualDWordRegToReg256(&RspRecompPos, x86_YMM1, x86_YMM2, x86_YMM7);
	AvxVPBlendvbRegToReg256(&RspRecompPos, x86_YMM2, x86_YMM2, x86_YMM6, x86_YMM1);

	// build the final result
	AvxVPSrldRegToReg256Immed(&RspRecompPos, x86_YMM2, x86_YMM2, 16);
	AvxVExtracti128RegToReg(&RspRecompPos, x86_XMM1, x86_YMM2, TRUE);
	Sse41PackUnsignedDWordRegToWordReg(&RspRecompPos, x86_XMM2, x86_XMM1);
	SseMoveAlignedRegToVariable(&RspRecompPos, x86_XMM2, &RSP_Vect[RSPOpC.OP.V.vd].UHW[0], Reg, SseType_QuadWord, TRUE);

	return TRUE;
}

static BOOL Compile_Vector_VMULF_AVX(BOOL writeToVectorDest) {
	char Reg[256];
	static const DWORD C8000 = 0x8000;
	static const DWORD C7FFF0000 = 0x7FFF0000;

	/* Do our AVX checks here */
	if (IsAvxEnabled == FALSE || IsAvx2Enabled == FALSE || IsSse2Enabled == FALSE || IsSse41Enabled == FALSE)
		return FALSE;

	sprintf(Reg, "RSP_Vect[%i]", RSPOpC.OP.V.vs);
	AvxVPMovesxWordVariableToDWordReg256(&RspRecompPos, x86_YMM0, &RSP_Vect[RSPOpC.OP.V.vs].UHW[0], Reg);

	if ((RSPOpC.OP.V.element & 0xF) < 2) {
		sprintf(Reg, "RSP_Vect[%i]", RSPOpC.OP.V.vt);
		AvxVPMovesxWordVariableToDWordReg256(&RspRecompPos, x86_YMM2, &RSP_Vect[RSPOpC.OP.V.vt].UHW[0], Reg);
	}
	else if ((RSPOpC.OP.V.element & 0xF) >= 8) {
		RSP_Element2Sse(x86_XMM2);
		AvxVPMovesxWordReg128ToDwordReg256(&RspRecompPos, x86_YMM2, x86_XMM2);
	}
	else {
		RSP_MultiElement2Sse(x86_XMM2);
		AvxVPMovesxWordReg128ToDwordReg256(&RspRecompPos, x86_YMM2, x86_XMM2);
	}

	// ((vs * vt) << 1) + 0x8000
	AvxVPMulldRegToReg256(&RspRecompPos, x86_YMM2, x86_YMM2, x86_YMM0);
	AvxVPSlldRegToReg256Immed(&RspRecompPos, x86_YMM2, x86_YMM2, 1);
	AvxVPBroadcastdVariableToReg256(&RspRecompPos, x86_YMM7, (void*)&C8000, "Constant(0x8000)");
	AvxVPAdddRegToReg256(&RspRecompPos, x86_YMM2, x86_YMM2, x86_YMM7);

	// accum low
	AvxVPSlldRegToReg256Immed(&RspRecompPos, x86_YMM3, x86_YMM2, 16);
	AvxVPSrldRegToReg256Immed(&RspRecompPos, x86_YMM3, x86_YMM3, 16);
	AvxVExtracti128RegToReg(&RspRecompPos, x86_XMM4, x86_YMM3, TRUE);
	Sse41PackUnsignedDWordRegToWordReg(&RspRecompPos, x86_XMM3, x86_XMM4);
	SseMoveAlignedRegToVariable(&RspRecompPos, x86_XMM3, &RSP_ACCUM_LOW.UHW[0], "RSP_ACCUM_LOW", SseType_QuadWord, TRUE);

	// accum mid
	AvxVPSrldRegToReg256Immed(&RspRecompPos, x86_YMM3, x86_YMM2, 16);
	AvxVExtracti128RegToReg(&RspRecompPos, x86_XMM4, x86_YMM3, TRUE);
	Sse41PackUnsignedDWordRegToWordReg(&RspRecompPos, x86_XMM3, x86_XMM4);
	SseMoveAlignedRegToVariable(&RspRecompPos, x86_XMM3, &RSP_ACCUM_MID.UHW[0], "RSP_ACCUM_MID", SseType_QuadWord, TRUE);

	// check which result has value 0x80008000
	AvxVPSlldRegToReg256Immed(&RspRecompPos, x86_YMM6, x86_YMM7, 16);
	AvxVPorRegToReg256(&RspRecompPos, x86_YMM7, x86_YMM7, x86_YMM6);
	AvxCompareEqualDWordRegToReg256(&RspRecompPos, x86_YMM1, x86_YMM2, x86_YMM7);

	if (writeToVectorDest == TRUE) {
		AvxVPBroadcastdVariableToReg256(&RspRecompPos, x86_YMM6, (void*)&C7FFF0000, "Constant(0x7FFF0000)");
		AvxVPBlendvbRegToReg256(&RspRecompPos, x86_YMM2, x86_YMM2, x86_YMM6, x86_YMM1);

		// build the final vec result
		AvxVPSrldRegToReg256Immed(&RspRecompPos, x86_YMM2, x86_YMM2, 16);
		AvxVExtracti128RegToReg(&RspRecompPos, x86_XMM1, x86_YMM2, TRUE);
		Sse41PackUnsignedDWordRegToWordReg(&RspRecompPos, x86_XMM2, x86_XMM1);
		SseMoveAlignedRegToVariable(&RspRecompPos, x86_XMM2, &RSP_Vect[RSPOpC.OP.V.vd].UHW[0], Reg, SseType_QuadWord, TRUE);
	}
	else {
		AvxVPandnRegToReg256(&RspRecompPos, x86_YMM2, x86_YMM1, x86_YMM2);
		AvxVPSrldRegToReg256Immed(&RspRecompPos, x86_YMM2, x86_YMM2, 16);
		AvxVExtracti128RegToReg(&RspRecompPos, x86_XMM1, x86_YMM2, TRUE);
		Sse41PackUnsignedDWordRegToWordReg(&RspRecompPos, x86_XMM2, x86_XMM1);
	}

	// accum high
	Sse2PsrawImmed(&RspRecompPos, x86_XMM2, 15);
	SseMoveAlignedRegToVariable(&RspRecompPos, x86_XMM2, &RSP_ACCUM_HIGH.UHW[0], "RSP_ACCUM_HIGH", SseType_QuadWord, TRUE);

	return TRUE;
}

void CompileRsp_Vector_VMULF ( void ) {
	char Reg[256];
	int count, el, del;

	BOOL bOptimize = ((RSPOpC.OP.V.element & 0x0f) >= 8) ? TRUE : FALSE;
	BOOL bWriteToAccum = WriteToAccum(EntireAccum, RspCompilePC);
	BOOL bWriteToDest = WriteToVectorDest(RSPOpC.OP.V.vd, RspCompilePC);

	#ifndef CompileVmulf
	InterpreterFallback((void*)RSP_Vector_VMULF,"RSP_Vector_VMULF"); return;
	#endif

	RSP_CPU_Message("  %X %s",RspCompilePC,RSPOpcodeName(RSPOpC.OP.Hex,RspCompilePC));

	if (bWriteToDest == FALSE && bWriteToAccum == FALSE) {
		return;
	}

	if (bWriteToAccum == FALSE) {
		if (TRUE == Compile_Vector_VMULF_NoAccum_AVX())
			return;

		if (TRUE == Compile_Vector_VMULF_NoAccum_SSE2())
			return;

		if (TRUE == Compile_Vector_VMULF_MMX())
			return;
	}

	if (TRUE == Compile_Vector_VMULF_AVX(bWriteToDest)) {
		return;
	}

	if (TRUE == Compile_Vector_VMULF_SSE2(bWriteToDest)) {
		return;
	}

	if (bOptimize == TRUE) {
		del = (RSPOpC.OP.V.element & 0x07) ^ 7;
		sprintf(Reg, "RSP_Vect[%i].HW[%i]", RSPOpC.OP.V.vt, del);
		MoveSxVariableToX86regHalf(&RspRecompPos, &RSP_Vect[RSPOpC.OP.V.vt].HW[del], Reg, x86_EBX);
	}

	if (bWriteToDest == TRUE) {
		if (bWriteToAccum == TRUE) {
			MoveConstToX86reg(&RspRecompPos, 0x7fff, x86_ESI);
		} else {
			MoveConstToX86reg(&RspRecompPos, 0x7fff0000, x86_ESI);
		}
	}
	if (bWriteToAccum == TRUE) {
		XorX86RegToX86Reg(&RspRecompPos, x86_EDI, x86_EDI);
	}

	for (count = 0; count < 8; count++) {
		RSP_CPU_Message("     Iteration: %i", count);

		el = Indx[RSPOpC.OP.V.element].B[count];
		del = EleSpec[RSPOpC.OP.V.element].B[el];

		sprintf(Reg, "RSP_Vect[%i].HW[%i]", RSPOpC.OP.V.vs, el);
		MoveSxVariableToX86regHalf(&RspRecompPos, &RSP_Vect[RSPOpC.OP.V.vs].HW[el], Reg, x86_EAX);

		if (RSPOpC.OP.V.vt == RSPOpC.OP.V.vs && el == del) {
			imulX86reg(&RspRecompPos, x86_EAX);
		} else {
			if (bOptimize == FALSE) {
				sprintf(Reg, "RSP_Vect[%i].HW[%i]", RSPOpC.OP.V.vt, del);
				MoveSxVariableToX86regHalf(&RspRecompPos, &RSP_Vect[RSPOpC.OP.V.vt].HW[del], Reg, x86_EBX);
			}
			imulX86reg(&RspRecompPos, x86_EBX);
		}

		ShiftLeftSignImmed(&RspRecompPos, x86_EAX, 1);
		AddConstToX86Reg(&RspRecompPos, x86_EAX, 0x8000);

		if (bWriteToAccum == TRUE) {
			MoveX86regHalfToVariable(&RspRecompPos, x86_EAX, &RSP_ACCUM_LOW.HW[el], "RSP_ACCUM_LOW.HW[el]");
			/* calculate sign extension into edx */
			Cdq(&RspRecompPos);
			MoveX86RegToX86Reg(&RspRecompPos, x86_EAX, x86_ECX);
			ShiftRightUnsignImmed(&RspRecompPos, x86_ECX, 16);
		}

		CompConstToX86reg(&RspRecompPos, x86_EAX, 0x80008000);

		if (bWriteToAccum == TRUE) {
			CondMoveEqual(&RspRecompPos, x86_EDX, x86_EDI);
			MoveX86regHalfToVariable(&RspRecompPos, x86_ECX, &RSP_ACCUM_MID.HW[el], "RSP_ACCUM_MID.HW[el]");
			MoveX86regHalfToVariable(&RspRecompPos, x86_EDX, &RSP_ACCUM_HIGH.HW[el], "RSP_ACCUM_HIGH.HW[el]");
		}

		if (bWriteToDest == TRUE) {
			if (bWriteToAccum == TRUE) {
				CondMoveEqual(&RspRecompPos, x86_ECX, x86_ESI);
				MoveX86regHalfToVariable(&RspRecompPos, x86_ECX, &RSP_Vect[RSPOpC.OP.V.vd].HW[el], "RSP_Vect[RSPOpC.OP.V.vd].HW[el]");
			} else {
				CondMoveEqual(&RspRecompPos, x86_EAX, x86_ESI);
				ShiftRightUnsignImmed(&RspRecompPos, x86_EAX, 16);
				MoveX86regHalfToVariable(&RspRecompPos, x86_EAX, &RSP_Vect[RSPOpC.OP.V.vd].HW[el], "RSP_Vect[RSPOpC.OP.V.vd].HW[el]");
			}
		}
	}
}

static BOOL Compile_Vector_VMULU_NoAccum_SSE2(void) {
	char Reg[256];
	static VECTOR MIN_VALUE;
	static BOOL constInitialized = FALSE;

	if (!constInitialized) {
		MIN_VALUE.UDW[0] = MIN_VALUE.UDW[1] = 0x8000800080008000ULL;
		constInitialized = TRUE;
	}

	/* Do our SSE checks here */
	if (IsSseEnabled == FALSE || IsSse2Enabled == FALSE)
		return FALSE;

	sprintf(Reg, "RSP_Vect[%i]", RSPOpC.OP.V.vs);
	SseMoveAlignedVariableToReg(&RspRecompPos, &RSP_Vect[RSPOpC.OP.V.vs].UHW[0], Reg, x86_XMM0, SseType_QuadWord, TRUE);

	// check if vs is 0x8000
	SseMoveAlignedVariableToReg(&RspRecompPos, &MIN_VALUE.sseValue, "MIN_VALUE(0x8000)", x86_XMM5, SseType_QuadWord, TRUE);
	SseMoveRegToReg(&RspRecompPos, x86_XMM6, x86_XMM5, SseType_QuadWord, TRUE);
	Sse2CompareEqualWordRegToReg(&RspRecompPos, x86_XMM6, x86_XMM0);

	if ((RSPOpC.OP.V.element & 0xF) < 2) {
		sprintf(Reg, "RSP_Vect[%i]", RSPOpC.OP.V.vt);
		SseMoveAlignedVariableToReg(&RspRecompPos, &RSP_Vect[RSPOpC.OP.V.vt].UHW[0], Reg, x86_XMM2, SseType_QuadWord, TRUE);
	}
	else if ((RSPOpC.OP.V.element & 0xF) >= 8) {
		RSP_Element2Sse(x86_XMM2);
	}
	else {
		RSP_MultiElement2Sse(x86_XMM2);
	}

	// ((vs * vt) << 1) + 0x8000
	SseMoveRegToReg(&RspRecompPos, x86_XMM4, x86_XMM0, SseType_QuadWord, TRUE);
	Sse2PmulhwRegToReg(&RspRecompPos, x86_XMM0, x86_XMM2);
	Sse2PsllwImmed(&RspRecompPos, x86_XMM0, 1);
	Sse2PmullwRegToReg(&RspRecompPos, x86_XMM4, x86_XMM2);
	SseMoveRegToReg(&RspRecompPos, x86_XMM1, x86_XMM4, SseType_QuadWord, TRUE);
	Sse2PsrlwImmed(&RspRecompPos, x86_XMM4, 15);
	Sse2PorRegToReg(&RspRecompPos, x86_XMM0, x86_XMM4);
	Sse2PsllwImmed(&RspRecompPos, x86_XMM1, 14);
	Sse2PsrlwImmed(&RspRecompPos, x86_XMM1, 15);
	Sse2PaddwRegToReg(&RspRecompPos, x86_XMM0, x86_XMM1);

	// check if vt is 0x8000 and round result when required
	Sse2CompareEqualWordRegToReg(&RspRecompPos, x86_XMM5, x86_XMM2);
	Sse2PandRegToReg(&RspRecompPos, x86_XMM5, x86_XMM6);
	Sse2PxorRegToReg(&RspRecompPos, x86_XMM6, x86_XMM6);
	Sse2PandnRegToReg(&RspRecompPos, x86_XMM6, x86_XMM5);
	Sse2PxorRegToReg(&RspRecompPos, x86_XMM4, x86_XMM4);
	Sse2CompareGreaterWordRegToReg(&RspRecompPos, x86_XMM4, x86_XMM0);
	Sse2PorRegToReg(&RspRecompPos, x86_XMM5, x86_XMM4);
	Sse2PandnRegToReg(&RspRecompPos, x86_XMM5, x86_XMM0);
	Sse2PorRegToReg(&RspRecompPos, x86_XMM5, x86_XMM6);

	sprintf(Reg, "RSP_Vect[%i]", RSPOpC.OP.V.vd);
	SseMoveAlignedRegToVariable(&RspRecompPos, x86_XMM5, &RSP_Vect[RSPOpC.OP.V.vd].UHW[0], Reg, SseType_QuadWord, TRUE);

	return TRUE;
}

static BOOL Compile_Vector_VMULU_SSE2(BOOL writeToVectorDest) {
	char Reg[256];
	static VECTOR MIN_VALUE;
	static BOOL constInitialized = FALSE;

	if (!constInitialized) {
		MIN_VALUE.UDW[0] = MIN_VALUE.UDW[1] = 0x8000800080008000ULL;
		constInitialized = TRUE;
	}

	/* Do our SSE checks here */
	if (IsSseEnabled == FALSE || IsSse2Enabled == FALSE)
		return FALSE;

	// load vs and vt
	sprintf(Reg, "RSP_Vect[%i]", RSPOpC.OP.V.vs);
	SseMoveAlignedVariableToReg(&RspRecompPos, &RSP_Vect[RSPOpC.OP.V.vs].UHW[0], Reg, x86_XMM1, SseType_QuadWord, TRUE);

	if ((RSPOpC.OP.V.element & 0xF) < 2) {
		sprintf(Reg, "RSP_Vect[%i]", RSPOpC.OP.V.vt);
		SseMoveAlignedVariableToReg(&RspRecompPos, &RSP_Vect[RSPOpC.OP.V.vt].UHW[0], Reg, x86_XMM2, SseType_QuadWord, TRUE);
	}
	else if ((RSPOpC.OP.V.element & 0xF) >= 8) {
		RSP_Element2Sse(x86_XMM2);
	}
	else {
		RSP_MultiElement2Sse(x86_XMM2);
	}

	// check overflow
	SseMoveAlignedVariableToReg(&RspRecompPos, &MIN_VALUE, "MIN_VALUE(0x8000)", x86_XMM7, SseType_QuadWord, TRUE);
	SseMoveRegToReg(&RspRecompPos, x86_XMM0, x86_XMM1, SseType_QuadWord, TRUE);
	Sse2CompareEqualWordRegToReg(&RspRecompPos, x86_XMM0, x86_XMM7);
	SseMoveRegToReg(&RspRecompPos, x86_XMM5, x86_XMM2, SseType_QuadWord, TRUE);
	Sse2CompareEqualWordRegToReg(&RspRecompPos, x86_XMM5, x86_XMM7);
	Sse2PandRegToReg(&RspRecompPos, x86_XMM0, x86_XMM5);

	// ((vs * vt) << 1) + 0x8000
	SseMoveRegToReg(&RspRecompPos, x86_XMM3, x86_XMM2, SseType_QuadWord, TRUE);
	Sse2PmullwRegToReg(&RspRecompPos, x86_XMM3, x86_XMM1);
	Sse2PmulhwRegToReg(&RspRecompPos, x86_XMM1, x86_XMM2);

	// accum low
	SseMoveRegToReg(&RspRecompPos, x86_XMM2, x86_XMM3, SseType_QuadWord, TRUE);
	Sse2PsllwImmed(&RspRecompPos, x86_XMM2, 1);
	Sse2PaddwRegToReg(&RspRecompPos, x86_XMM2, x86_XMM7);
	SseMoveAlignedRegToVariable(&RspRecompPos, x86_XMM2, &RSP_ACCUM_LOW.UHW[0], "RSP_ACCUM_LOW", SseType_QuadWord, TRUE);

	// accum mid
	Sse2PsllwImmed(&RspRecompPos, x86_XMM1, 1);
	SseMoveRegToReg(&RspRecompPos, x86_XMM2, x86_XMM3, SseType_QuadWord, TRUE);
	Sse2PsrlwImmed(&RspRecompPos, x86_XMM2, 15);
	Sse2PorRegToReg(&RspRecompPos, x86_XMM1, x86_XMM2);
	Sse2PsllwImmed(&RspRecompPos, x86_XMM3, 1);
	Sse2PsrlwImmed(&RspRecompPos, x86_XMM3, 15);
	Sse2PaddwRegToReg(&RspRecompPos, x86_XMM1, x86_XMM3);
	SseMoveAlignedRegToVariable(&RspRecompPos, x86_XMM1, &RSP_ACCUM_MID.UHW[0], "RSP_ACCUM_MID", SseType_QuadWord, TRUE);

	// mask
	if (writeToVectorDest) {
		Sse2PxorRegToReg(&RspRecompPos, x86_XMM4, x86_XMM4);
		Sse2PandnRegToReg(&RspRecompPos, x86_XMM4, x86_XMM0);
	}
	Sse2PandnRegToReg(&RspRecompPos, x86_XMM0, x86_XMM1);

	// vect
	if (writeToVectorDest) {
		Sse2PorRegToReg(&RspRecompPos, x86_XMM4, x86_XMM0);
		Sse2PxorRegToReg(&RspRecompPos, x86_XMM5, x86_XMM5);
		Sse2CompareGreaterWordRegToReg(&RspRecompPos, x86_XMM5, x86_XMM0);
		Sse2PandnRegToReg(&RspRecompPos, x86_XMM5, x86_XMM4);
		
		sprintf(Reg, "RSP_Vect[%i]", RSPOpC.OP.V.vd);
		SseMoveAlignedRegToVariable(&RspRecompPos, x86_XMM5, &RSP_Vect[RSPOpC.OP.V.vd].UHW[0], Reg, SseType_QuadWord, TRUE);
	}

	// accum high
	Sse2PsrawImmed(&RspRecompPos, x86_XMM0, 15);
	SseMoveAlignedRegToVariable(&RspRecompPos, x86_XMM0, &RSP_ACCUM_HIGH.UHW[0], "RSP_ACCUM_HIGH", SseType_QuadWord, TRUE);

	return TRUE;
}

static BOOL Compile_Vector_VMULU_NoAccum_AVX(void) {
	char Reg[256];
	static const DWORD C8000 = 0x8000;

	/* Do our AVX checks here */
	if (IsAvxEnabled == FALSE || IsAvx2Enabled == FALSE || IsSse2Enabled == FALSE || IsSse41Enabled == FALSE)
		return FALSE;

	sprintf(Reg, "RSP_Vect[%i]", RSPOpC.OP.V.vs);
	AvxVPMovesxWordVariableToDWordReg256(&RspRecompPos, x86_YMM0, &RSP_Vect[RSPOpC.OP.V.vs].UHW[0], Reg);

	if ((RSPOpC.OP.V.element & 0xF) < 2) {
		sprintf(Reg, "RSP_Vect[%i]", RSPOpC.OP.V.vt);
		AvxVPMovesxWordVariableToDWordReg256(&RspRecompPos, x86_YMM2, &RSP_Vect[RSPOpC.OP.V.vt].UHW[0], Reg);
	}
	else if ((RSPOpC.OP.V.element & 0xF) >= 8) {
		RSP_Element2Sse(x86_XMM2);
		AvxVPMovesxWordReg128ToDwordReg256(&RspRecompPos, x86_YMM2, x86_XMM2);
	}
	else {
		RSP_MultiElement2Sse(x86_XMM2);
		AvxVPMovesxWordReg128ToDwordReg256(&RspRecompPos, x86_YMM2, x86_XMM2);
	}

	// ((vs * vt) << 1) + 0x8000
	AvxVPMulldRegToReg256(&RspRecompPos, x86_YMM2, x86_YMM2, x86_YMM0);
	AvxVPSlldRegToReg256Immed(&RspRecompPos, x86_YMM2, x86_YMM2, 1);
	AvxVPBroadcastdVariableToReg256(&RspRecompPos, x86_YMM7, (void*)&C8000, "Constant(0x8000)");
	AvxVPAdddRegToReg256(&RspRecompPos, x86_YMM2, x86_YMM2, x86_YMM7);

	// check which result has value 0x80008000
	AvxVPSlldRegToReg256Immed(&RspRecompPos, x86_YMM6, x86_YMM7, 16);
	AvxVPorRegToReg256(&RspRecompPos, x86_YMM7, x86_YMM7, x86_YMM6);

	AvxCompareEqualDWordRegToReg256(&RspRecompPos, x86_YMM6, x86_YMM6, x86_YMM6);
	AvxCompareEqualDWordRegToReg256(&RspRecompPos, x86_YMM1, x86_YMM2, x86_YMM7);
	AvxVPxorRegToReg256(&RspRecompPos, x86_YMM4, x86_YMM4, x86_YMM4);
	AvxCompareGreaterDWordRegToReg256(&RspRecompPos, x86_YMM4, x86_YMM4, x86_YMM2);
	AvxVPandnRegToReg256(&RspRecompPos, x86_YMM2, x86_YMM4, x86_YMM2);
	AvxVPBlendvbRegToReg256(&RspRecompPos, x86_YMM2, x86_YMM2, x86_YMM6, x86_YMM1);

	// build the final result
	AvxVPSrldRegToReg256Immed(&RspRecompPos, x86_YMM2, x86_YMM2, 16);
	AvxVExtracti128RegToReg(&RspRecompPos, x86_XMM1, x86_YMM2, TRUE);
	Sse41PackUnsignedDWordRegToWordReg(&RspRecompPos, x86_XMM2, x86_XMM1);
	SseMoveAlignedRegToVariable(&RspRecompPos, x86_XMM2, &RSP_Vect[RSPOpC.OP.V.vd].UHW[0], Reg, SseType_QuadWord, TRUE);

	return TRUE;
}

static BOOL Compile_Vector_VMULU_AVX(BOOL writeToVectorDest) {
	char Reg[256];
	static const DWORD C8000 = 0x8000;

	/* Do our AVX checks here */
	if (IsAvxEnabled == FALSE || IsAvx2Enabled == FALSE || IsSse2Enabled == FALSE || IsSse41Enabled == FALSE)
		return FALSE;

	sprintf(Reg, "RSP_Vect[%i]", RSPOpC.OP.V.vs);
	AvxVPMovesxWordVariableToDWordReg256(&RspRecompPos, x86_YMM0, &RSP_Vect[RSPOpC.OP.V.vs].UHW[0], Reg);

	if ((RSPOpC.OP.V.element & 0xF) < 2) {
		sprintf(Reg, "RSP_Vect[%i]", RSPOpC.OP.V.vt);
		AvxVPMovesxWordVariableToDWordReg256(&RspRecompPos, x86_YMM2, &RSP_Vect[RSPOpC.OP.V.vt].UHW[0], Reg);
	}
	else if ((RSPOpC.OP.V.element & 0xF) >= 8) {
		RSP_Element2Sse(x86_XMM2);
		AvxVPMovesxWordReg128ToDwordReg256(&RspRecompPos, x86_YMM2, x86_XMM2);
	}
	else {
		RSP_MultiElement2Sse(x86_XMM2);
		AvxVPMovesxWordReg128ToDwordReg256(&RspRecompPos, x86_YMM2, x86_XMM2);
	}

	// ((vs * vt) << 1) + 0x8000
	AvxVPMulldRegToReg256(&RspRecompPos, x86_YMM2, x86_YMM2, x86_YMM0);
	AvxVPSlldRegToReg256Immed(&RspRecompPos, x86_YMM2, x86_YMM2, 1);
	AvxVPBroadcastdVariableToReg256(&RspRecompPos, x86_YMM7, (void*)&C8000, "Constant(0x8000)");
	AvxVPAdddRegToReg256(&RspRecompPos, x86_YMM2, x86_YMM2, x86_YMM7);

	// accum low
	AvxVPSlldRegToReg256Immed(&RspRecompPos, x86_YMM3, x86_YMM2, 16);
	AvxVPSrldRegToReg256Immed(&RspRecompPos, x86_YMM3, x86_YMM3, 16);
	AvxVExtracti128RegToReg(&RspRecompPos, x86_XMM4, x86_YMM3, TRUE);
	Sse41PackUnsignedDWordRegToWordReg(&RspRecompPos, x86_XMM3, x86_XMM4);
	SseMoveAlignedRegToVariable(&RspRecompPos, x86_XMM3, &RSP_ACCUM_LOW.UHW[0], "RSP_ACCUM_LOW", SseType_QuadWord, TRUE);

	// accum mid
	AvxVPSrldRegToReg256Immed(&RspRecompPos, x86_YMM3, x86_YMM2, 16);
	AvxVExtracti128RegToReg(&RspRecompPos, x86_XMM4, x86_YMM3, TRUE);
	Sse41PackUnsignedDWordRegToWordReg(&RspRecompPos, x86_XMM3, x86_XMM4);
	SseMoveAlignedRegToVariable(&RspRecompPos, x86_XMM3, &RSP_ACCUM_MID.UHW[0], "RSP_ACCUM_MID", SseType_QuadWord, TRUE);

	// check which result has value 0x80008000
	AvxVPSlldRegToReg256Immed(&RspRecompPos, x86_YMM6, x86_YMM7, 16);
	AvxVPorRegToReg256(&RspRecompPos, x86_YMM7, x86_YMM7, x86_YMM6);
	AvxCompareEqualDWordRegToReg256(&RspRecompPos, x86_YMM1, x86_YMM2, x86_YMM7);

	// accum high
	AvxVPandnRegToReg256(&RspRecompPos, x86_YMM6, x86_YMM1, x86_YMM2);
	AvxVPSrldRegToReg256Immed(&RspRecompPos, x86_YMM6, x86_YMM6, 16);
	AvxVExtracti128RegToReg(&RspRecompPos, x86_XMM5, x86_YMM6, TRUE);
	Sse41PackUnsignedDWordRegToWordReg(&RspRecompPos, x86_XMM6, x86_XMM5);
	Sse2PsrawImmed(&RspRecompPos, x86_XMM6, 15);
	SseMoveAlignedRegToVariable(&RspRecompPos, x86_XMM6, &RSP_ACCUM_HIGH.UHW[0], "RSP_ACCUM_HIGH", SseType_QuadWord, TRUE);

	if (writeToVectorDest == TRUE) {
		AvxCompareEqualDWordRegToReg256(&RspRecompPos, x86_YMM6, x86_YMM6, x86_YMM6); // 0xFFFF
		AvxVPxorRegToReg256(&RspRecompPos, x86_YMM4, x86_YMM4, x86_YMM4);
		AvxCompareGreaterDWordRegToReg256(&RspRecompPos, x86_YMM4, x86_YMM4, x86_YMM2);
		AvxVPandnRegToReg256(&RspRecompPos, x86_YMM2, x86_YMM4, x86_YMM2);
		AvxVPBlendvbRegToReg256(&RspRecompPos, x86_YMM2, x86_YMM2, x86_YMM6, x86_YMM1);

		// build the final vec result
		AvxVPSrldRegToReg256Immed(&RspRecompPos, x86_YMM2, x86_YMM2, 16);
		AvxVExtracti128RegToReg(&RspRecompPos, x86_XMM1, x86_YMM2, TRUE);
		Sse41PackUnsignedDWordRegToWordReg(&RspRecompPos, x86_XMM2, x86_XMM1);
		SseMoveAlignedRegToVariable(&RspRecompPos, x86_XMM2, &RSP_Vect[RSPOpC.OP.V.vd].UHW[0], Reg, SseType_QuadWord, TRUE);
	}

	return TRUE;
}

void CompileRsp_Vector_VMULU ( void ) {
	char Reg[256];
	int count, el, del;

	BOOL bOptimize = ((RSPOpC.OP.V.element & 0x0f) >= 8) ? TRUE : FALSE;
	BOOL bWriteToAccum = WriteToAccum(EntireAccum, RspCompilePC);
	BOOL bWriteToDest = WriteToVectorDest(RSPOpC.OP.V.vd, RspCompilePC);

	#ifndef CompileVmulu
	InterpreterFallback((void*)RSP_Vector_VMULU, "RSP_Vector_VMULU"); return;
	#endif

	RSP_CPU_Message("  %X %s", RspCompilePC, RSPOpcodeName(RSPOpC.OP.Hex, RspCompilePC));

	if (bWriteToDest == FALSE && bWriteToAccum == FALSE) {
		return;
	}

	if (bWriteToAccum == FALSE) {
		if (TRUE == Compile_Vector_VMULU_NoAccum_AVX())
			return;

		if (TRUE == Compile_Vector_VMULU_NoAccum_SSE2())
			return;
	}

	if (TRUE == Compile_Vector_VMULU_AVX(bWriteToDest)) {
		return;
	}

	if (TRUE == Compile_Vector_VMULU_SSE2(bWriteToDest)) {
		return;
	}

	if (bOptimize == TRUE) {
		del = (RSPOpC.OP.V.element & 0x07) ^ 7;
		sprintf(Reg, "RSP_Vect[%i].HW[%i]", RSPOpC.OP.V.vt, del);
		MoveSxVariableToX86regHalf(&RspRecompPos, &RSP_Vect[RSPOpC.OP.V.vt].HW[del], Reg, x86_EBX);
	}

	if (bWriteToDest == TRUE) {
		MoveConstToX86reg(&RspRecompPos, 0xffff, x86_ESI);
	}
	XorX86RegToX86Reg(&RspRecompPos, x86_EDI, x86_EDI);

	for (count = 0; count < 8; count++) {
		RSP_CPU_Message("     Iteration: %i", count);

		el = Indx[RSPOpC.OP.V.element].B[count];
		del = EleSpec[RSPOpC.OP.V.element].B[el];

		sprintf(Reg, "RSP_Vect[%i].HW[%i]", RSPOpC.OP.V.vs, el);
		MoveSxVariableToX86regHalf(&RspRecompPos, &RSP_Vect[RSPOpC.OP.V.vs].HW[el], Reg, x86_EAX);

		if (RSPOpC.OP.V.vt == RSPOpC.OP.V.vs && el == del) {
			imulX86reg(&RspRecompPos, x86_EAX);
		}
		else {
			if (bOptimize == FALSE) {
				sprintf(Reg, "RSP_Vect[%i].HW[%i]", RSPOpC.OP.V.vt, del);
				MoveSxVariableToX86regHalf(&RspRecompPos, &RSP_Vect[RSPOpC.OP.V.vt].HW[del], Reg, x86_EBX);
			}
			imulX86reg(&RspRecompPos, x86_EBX);
		}

		ShiftLeftSignImmed(&RspRecompPos, x86_EAX, 1);
		AddConstToX86Reg(&RspRecompPos, x86_EAX, 0x8000);

		if (bWriteToAccum == TRUE) {
			MoveX86regHalfToVariable(&RspRecompPos, x86_EAX, &RSP_ACCUM_LOW.HW[el], "RSP_ACCUM_LOW.HW[el]");
			/* calculate sign extension into edx */
			Cdq(&RspRecompPos);
		}

		MoveX86RegToX86Reg(&RspRecompPos, x86_EAX, x86_ECX);
		ShiftRightSignImmed(&RspRecompPos, x86_ECX, 16);
		CompConstToX86reg(&RspRecompPos, x86_EAX, 0x80008000);

		if (bWriteToAccum == TRUE) {
			CondMoveEqual(&RspRecompPos, x86_EDX, x86_EDI);
			MoveX86regHalfToVariable(&RspRecompPos, x86_ECX, &RSP_ACCUM_MID.HW[el], "RSP_ACCUM_MID.HW[el]");
			MoveX86regHalfToVariable(&RspRecompPos, x86_EDX, &RSP_ACCUM_HIGH.HW[el], "RSP_ACCUM_HIGH.HW[el]");
		}

		if (bWriteToDest == TRUE) {
			CondMoveEqual(&RspRecompPos, x86_ECX, x86_ESI);
			CompConstToX86reg(&RspRecompPos, x86_ECX, 0);
			CondMoveLess(&RspRecompPos, x86_ECX, x86_EDI);
			MoveX86regHalfToVariable(&RspRecompPos, x86_ECX, &RSP_Vect[RSPOpC.OP.V.vd].HW[el], "RSP_Vect[RSPOpC.OP.V.vd].HW[el]");
		}
	}
}

static BOOL Compile_Vector_VRNDP_SSE2(BOOL writeToVectorDest, BOOL writeToAccum) {
	char Reg[256];

	/* Do our SSE checks here */
	if (IsSseEnabled == FALSE || IsSse2Enabled == FALSE)
		return FALSE;

	// load vs and vt
	if ((RSPOpC.OP.V.element & 0xF) < 2) {
		sprintf(Reg, "RSP_Vect[%i]", RSPOpC.OP.V.vt);
		SseMoveAlignedVariableToReg(&RspRecompPos, &RSP_Vect[RSPOpC.OP.V.vt].UHW[0], Reg, x86_XMM1, SseType_QuadWord, TRUE);
	}
	else if ((RSPOpC.OP.V.element & 0xF) >= 8) {
		RSP_Element2Sse(x86_XMM1);
	}
	else {
		RSP_MultiElement2Sse(x86_XMM1);
	}

	SseMoveAlignedVariableToReg(&RspRecompPos, &RSP_ACCUM_HIGH.UHW[0], "RSP_ACCUM_HIGH", x86_XMM4, SseType_QuadWord, TRUE);
	SseMoveAlignedVariableToReg(&RspRecompPos, &RSP_ACCUM_MID.UHW[0], "RSP_ACCUM_MID", x86_XMM2, SseType_QuadWord, TRUE);
	SseMoveRegToReg(&RspRecompPos, x86_XMM3, x86_XMM2, SseType_QuadWord, TRUE);
	Sse2PunpckHighWordsRegToReg(&RspRecompPos, x86_XMM2, x86_XMM4);
	Sse2PunpckLowWordsRegToReg(&RspRecompPos, x86_XMM3, x86_XMM4);

	Sse2PxorRegToReg(&RspRecompPos, x86_XMM5, x86_XMM5);
	Sse2CompareGreaterWordRegToReg(&RspRecompPos, x86_XMM5, x86_XMM4);
	Sse2PandnRegToReg(&RspRecompPos, x86_XMM5, x86_XMM1);

	if ((RSPOpC.OP.V.vs & 1) != 0) {
		Sse2MoveSxWordRegToDWordReg(&RspRecompPos, x86_XMM0, x86_XMM5, IsSse41Enabled);
		Sse2ShuffleDWordsRegToReg(&RspRecompPos, x86_XMM5, x86_XMM5, _MMX_SHUFFLE(3, 2, 3, 2));
		Sse2MoveSxWordRegToDWordReg(&RspRecompPos, x86_XMM5, x86_XMM5, IsSse41Enabled);
		Sse2PadddRegToReg(&RspRecompPos, x86_XMM2, x86_XMM5);
		Sse2PadddRegToReg(&RspRecompPos, x86_XMM3, x86_XMM0);
	} else {
		SseMoveAlignedVariableToReg(&RspRecompPos, &RSP_ACCUM_LOW.UHW[0], "RSP_ACCUM_LOW", x86_XMM4, SseType_QuadWord, TRUE);

		SseMoveRegToReg(&RspRecompPos, x86_XMM1, x86_XMM4, SseType_QuadWord, TRUE);
		Sse2PaddwRegToReg(&RspRecompPos, x86_XMM4, x86_XMM5);
		if (writeToAccum == TRUE) {
			SseMoveAlignedRegToVariable(&RspRecompPos, x86_XMM4, &RSP_ACCUM_LOW.UHW[0], "RSP_ACCUM_LOW", SseType_QuadWord, TRUE);
		}

		// compute carry
		Sse2PadduswRegToReg(&RspRecompPos, x86_XMM1, x86_XMM5);
		Sse2CompareEqualWordRegToReg(&RspRecompPos, x86_XMM4, x86_XMM1);
		Sse2CompareEqualDWordRegToReg(&RspRecompPos, x86_XMM1, x86_XMM1);
		Sse2PandnRegToReg(&RspRecompPos, x86_XMM4, x86_XMM1);
		Sse2PsrlwImmed(&RspRecompPos, x86_XMM4, 15);
		Sse2PsrawImmed(&RspRecompPos, x86_XMM5, 15);
		Sse2PaddwRegToReg(&RspRecompPos, x86_XMM5, x86_XMM4);

		// add to accum mid and high
		Sse2MoveSxWordRegToDWordReg(&RspRecompPos, x86_XMM0, x86_XMM5, IsSse41Enabled);
		Sse2ShuffleDWordsRegToReg(&RspRecompPos, x86_XMM5, x86_XMM5, _MMX_SHUFFLE(3, 2, 3, 2));
		Sse2MoveSxWordRegToDWordReg(&RspRecompPos, x86_XMM5, x86_XMM5, IsSse41Enabled);
		Sse2PadddRegToReg(&RspRecompPos, x86_XMM2, x86_XMM5);
		Sse2PadddRegToReg(&RspRecompPos, x86_XMM3, x86_XMM0);
	}

	if (writeToVectorDest) {
		if (writeToAccum) {
			SseMoveRegToReg(&RspRecompPos, x86_XMM4, x86_XMM3, SseType_QuadWord, TRUE);
		}
		Sse2PackSignedDWordRegToWordReg(&RspRecompPos, x86_XMM3, x86_XMM2);

		sprintf(Reg, "RSP_Vect[%i]", RSPOpC.OP.V.vd);
		SseMoveAlignedRegToVariable(&RspRecompPos, x86_XMM3, &RSP_Vect[RSPOpC.OP.V.vd].UHW[0], Reg, SseType_QuadWord, TRUE);
	}

	if (writeToAccum) {
		int lowReg = writeToVectorDest ? x86_XMM4 : x86_XMM3;

		SseMoveRegToReg(&RspRecompPos, x86_XMM7, lowReg, SseType_QuadWord, TRUE);
		SseMoveRegToReg(&RspRecompPos, x86_XMM6, x86_XMM2, SseType_QuadWord, TRUE);
		Sse2PsradImmed(&RspRecompPos, x86_XMM7, 16);
		Sse2PsradImmed(&RspRecompPos, x86_XMM6, 16);
		Sse2PackSignedDWordRegToWordReg(&RspRecompPos, x86_XMM7, x86_XMM6);

		SseMoveAlignedRegToVariable(&RspRecompPos, x86_XMM7, &RSP_ACCUM_HIGH.UHW[0], "RSP_ACCUM_HIGH", SseType_QuadWord, TRUE);
		

		Sse2PslldImmed(&RspRecompPos, lowReg, 16);
		Sse2PslldImmed(&RspRecompPos, x86_XMM2, 16);
		Sse2PsradImmed(&RspRecompPos, lowReg, 16);
		Sse2PsradImmed(&RspRecompPos, x86_XMM2, 16);
		Sse2PackSignedDWordRegToWordReg(&RspRecompPos, lowReg, x86_XMM2);

		SseMoveAlignedRegToVariable(&RspRecompPos, lowReg, &RSP_ACCUM_MID.UHW[0], "RSP_ACCUM_MID", SseType_QuadWord, TRUE);
	}

	return TRUE;
}

static BOOL Compile_Vector_VRNDP_AVX(BOOL writeToVectorDest, BOOL writeToAccum) {
	char Reg[256];

	/* Do our AVX checks here */
	if (IsAvxEnabled == FALSE || IsAvx2Enabled == FALSE || IsSse2Enabled == FALSE)
		return FALSE;

	// load vs and vt
	if ((RSPOpC.OP.V.element & 0xF) < 2) {
		sprintf(Reg, "RSP_Vect[%i]", RSPOpC.OP.V.vt);
		SseMoveAlignedVariableToReg(&RspRecompPos, &RSP_Vect[RSPOpC.OP.V.vt].UHW[0], Reg, x86_XMM1, SseType_QuadWord, TRUE);
	} else if ((RSPOpC.OP.V.element & 0xF) >= 8) {
		RSP_Element2Sse(x86_XMM1);
	} else {
		RSP_MultiElement2Sse(x86_XMM1);
	}

	SseMoveAlignedVariableToReg(&RspRecompPos, &RSP_ACCUM_HIGH.UHW[0], "RSP_ACCUM_HIGH", x86_XMM4, SseType_QuadWord, TRUE);
	SseMoveAlignedVariableToReg(&RspRecompPos, &RSP_ACCUM_MID.UHW[0], "RSP_ACCUM_MID", x86_XMM2, SseType_QuadWord, TRUE);
	AvxVPunpckHighWordsRegToReg256(&RspRecompPos, x86_YMM7, x86_YMM2, x86_YMM4);
	AvxVPunpckLowWordsRegToReg256(&RspRecompPos, x86_YMM2, x86_YMM2, x86_YMM4);
	AvxVInserti128RegToReg(&RspRecompPos, x86_YMM2, x86_YMM2, x86_XMM7, TRUE);

	Sse2PxorRegToReg(&RspRecompPos, x86_XMM5, x86_XMM5);
	Sse2CompareGreaterWordRegToReg(&RspRecompPos, x86_XMM5, x86_XMM4);
	AvxVPandnRegToReg128(&RspRecompPos, x86_XMM1, x86_XMM5, x86_XMM1);

	if ((RSPOpC.OP.V.vs & 1) != 0) {
		AvxVPMovesxWordReg128ToDwordReg256(&RspRecompPos, x86_YMM1, x86_XMM1);
		AvxVPAdddRegToReg256(&RspRecompPos, x86_YMM2, x86_YMM2, x86_YMM1);
	} else {
		SseMoveAlignedVariableToReg(&RspRecompPos, &RSP_ACCUM_LOW.UHW[0], "RSP_ACCUM_LOW", x86_XMM4, SseType_QuadWord, TRUE);

		AvxVPAddwRegToReg128(&RspRecompPos, x86_XMM3, x86_XMM4, x86_XMM1);

		if (writeToAccum == TRUE) {
			SseMoveAlignedRegToVariable(&RspRecompPos, x86_XMM3, &RSP_ACCUM_LOW.UHW[0], "RSP_ACCUM_LOW", SseType_QuadWord, TRUE);
		}

		// compute carry
		Sse2PadduswRegToReg(&RspRecompPos, x86_XMM4, x86_XMM1);
		Sse2CompareEqualWordRegToReg(&RspRecompPos, x86_XMM4, x86_XMM3);
		Sse2CompareEqualDWordRegToReg(&RspRecompPos, x86_XMM5, x86_XMM5);
		Sse2PandnRegToReg(&RspRecompPos, x86_XMM4, x86_XMM5);
		Sse2PsrlwImmed(&RspRecompPos, x86_XMM4, 15);
		Sse2PsrawImmed(&RspRecompPos, x86_XMM1, 15);
		Sse2PaddwRegToReg(&RspRecompPos, x86_XMM1, x86_XMM4);

		// add to accum mid and high
		AvxVPMovesxWordReg128ToDwordReg256(&RspRecompPos, x86_YMM1, x86_XMM1);
		AvxVPAdddRegToReg256(&RspRecompPos, x86_YMM2, x86_YMM2, x86_YMM1);
	}

	if (writeToVectorDest) {
		AvxVExtracti128RegToReg(&RspRecompPos, x86_XMM0, x86_YMM2, TRUE);
		AvxVPackSignedDWordRegToWordReg128(&RspRecompPos, x86_XMM0, x86_XMM2, x86_XMM0);

		sprintf(Reg, "RSP_Vect[%i]", RSPOpC.OP.V.vd);
		SseMoveAlignedRegToVariable(&RspRecompPos, x86_XMM0, &RSP_Vect[RSPOpC.OP.V.vd].UHW[0], Reg, SseType_QuadWord, TRUE);
	}

	if (writeToAccum) {
		AvxVPSlldRegToReg256Immed(&RspRecompPos, x86_YMM0, x86_YMM2, 16);
		AvxVPSradRegToReg256Immed(&RspRecompPos, x86_YMM0, x86_YMM0, 16);
		AvxVExtracti128RegToReg(&RspRecompPos, x86_XMM7, x86_YMM0, TRUE);
		AvxVPackSignedDWordRegToWordReg128(&RspRecompPos, x86_XMM0, x86_XMM0, x86_XMM7);
		SseMoveAlignedRegToVariable(&RspRecompPos, x86_XMM0, &RSP_ACCUM_MID.UHW[0], "RSP_ACCUM_MID", SseType_QuadWord, TRUE);

		AvxVPSradRegToReg256Immed(&RspRecompPos, x86_YMM0, x86_YMM2, 16);
		AvxVExtracti128RegToReg(&RspRecompPos, x86_XMM7, x86_YMM0, TRUE);
		AvxVPackSignedDWordRegToWordReg128(&RspRecompPos, x86_XMM0, x86_XMM0, x86_XMM7);
		SseMoveAlignedRegToVariable(&RspRecompPos, x86_XMM0, &RSP_ACCUM_HIGH.UHW[0], "RSP_ACCUM_HIGH", SseType_QuadWord, TRUE);
	}

	return TRUE;
}

void CompileRsp_Vector_VRNDP(void) {
	char Reg[256];
	int count, el, del;

	BOOL bWriteToAccum = FALSE;
	if ((RSPOpC.OP.V.vs & 1) != 0) bWriteToAccum = WriteToAccum(High16BitAccum | Middle16BitAccum, RspCompilePC);
	else bWriteToAccum = WriteToAccum(EntireAccum, RspCompilePC);
	BOOL bWriteToDest = WriteToVectorDest(RSPOpC.OP.V.vd, RspCompilePC);

#ifndef CompileVrndp
	InterpreterFallback((void*)RSP_Vector_VRNDP, "RSP_Vector_VRNDP"); return;
#endif

	RSP_CPU_Message("  %X %s", RspCompilePC, RSPOpcodeName(RSPOpC.OP.Hex, RspCompilePC));

	if (bWriteToDest == FALSE && bWriteToAccum == FALSE) {
		return;
	}

	if (TRUE == Compile_Vector_VRNDP_AVX(bWriteToDest, bWriteToAccum)) {
		return;
	}

	if (TRUE == Compile_Vector_VRNDP_SSE2(bWriteToDest, bWriteToAccum)) {
		return;
	}

	MoveConstToX86reg(&RspRecompPos, 0x7fff, x86_ESI);
	MoveConstToX86reg(&RspRecompPos, 0xffff8000, x86_EDI);

	for (count = 0; count < 8; count++) {
		RSP_CPU_Message("     Iteration: %i", count);

		el = Indx[RSPOpC.OP.V.element].B[count];
		del = EleSpec[RSPOpC.OP.V.element].B[el];

		sprintf(Reg, "RSP_Vect[%i].HW[%i]", RSPOpC.OP.V.vt, del);
		MoveSxVariableToX86regHalf(&RspRecompPos, &RSP_Vect[RSPOpC.OP.V.vt].HW[del], Reg, x86_EAX);

		sprintf(Reg, "ACCUM_HIGH[%i]", el);
		MoveVariableToX86regHalf(&RspRecompPos, &RSP_ACCUM_HIGH.UHW[el], Reg, x86_EBX);
		ShiftLeftSignImmed(&RspRecompPos, x86_EBX, 16);
		sprintf(Reg, "ACCUM_MID[%i]", el);
		MoveVariableToX86regHalf(&RspRecompPos, &RSP_ACCUM_MID.UHW[el], Reg, x86_EBX);
		if ((RSPOpC.OP.V.vs & 1) == 0) {
			sprintf(Reg, "ACCUM_LOW[%i]", el);
			MoveVariableToX86regHalf(&RspRecompPos, &RSP_ACCUM_LOW.UHW[el], Reg, x86_ECX);
		}

		XorX86RegToX86Reg(&RspRecompPos, x86_EDX, x86_EDX);
		CompConstToX86reg(&RspRecompPos, x86_EBX, 0);
		CondMoveLess(&RspRecompPos, x86_EAX, x86_EDX);

		if ((RSPOpC.OP.V.vs & 1) != 0) {
			AddX86RegToX86Reg(&RspRecompPos, x86_EBX, x86_EAX);
		}
		else {
			Cdq(&RspRecompPos);
			AddX86RegToX86RegHalf(&RspRecompPos, x86_ECX, x86_EAX);
			AdcX86RegToX86Reg(&RspRecompPos, x86_EBX, x86_EDX);

			if (bWriteToAccum == TRUE) {
				sprintf(Reg, "ACCUM_LOW[%i]", el);
				MoveX86regHalfToVariable(&RspRecompPos, x86_ECX, &RSP_ACCUM_LOW.UHW[el], Reg);
			}
		}

		if (bWriteToAccum == TRUE) {
			sprintf(Reg, "ACCUM_MID[%i]", el);
			MoveX86regHalfToVariable(&RspRecompPos, x86_EBX, &RSP_ACCUM_MID.UHW[el], Reg);
			MoveX86RegToX86Reg(&RspRecompPos, x86_EBX, x86_EDX);
			ShiftRightUnsignImmed(&RspRecompPos, x86_EDX, 16);
			sprintf(Reg, "ACCUM_HIGH[%i]", el);
			MoveX86regHalfToVariable(&RspRecompPos, x86_EDX, &RSP_ACCUM_HIGH.UHW[el], Reg);
		}

		if (bWriteToDest == TRUE) {
			CompX86RegToX86Reg(&RspRecompPos, x86_EBX, x86_ESI);
			CondMoveGreater(&RspRecompPos, x86_EBX, x86_ESI);
			CompX86RegToX86Reg(&RspRecompPos, x86_EBX, x86_EDI);
			CondMoveLess(&RspRecompPos, x86_EBX, x86_EDI);

			MoveX86regHalfToVariable(&RspRecompPos, x86_EBX, &RSP_Vect[RSPOpC.OP.V.vd].HW[el], "RSP_Vect[RSPOpC.OP.V.vd].HW[el]");
		}
	}
}

static BOOL Compile_Vector_VMULQ_SSE2(BOOL writeToVectorDest, BOOL writeToAccum) {
	char Reg[256];

	/* Do our SSE checks here */
	if (IsSseEnabled == FALSE || IsSse2Enabled == FALSE)
		return FALSE;

	// load vs and vt
	sprintf(Reg, "RSP_Vect[%i]", RSPOpC.OP.V.vs);
	SseMoveAlignedVariableToReg(&RspRecompPos, &RSP_Vect[RSPOpC.OP.V.vs].UHW[0], Reg, x86_XMM1, SseType_QuadWord, TRUE);

	if ((RSPOpC.OP.V.element & 0xF) < 2) {
		sprintf(Reg, "RSP_Vect[%i]", RSPOpC.OP.V.vt);
		SseMoveAlignedVariableToReg(&RspRecompPos, &RSP_Vect[RSPOpC.OP.V.vt].UHW[0], Reg, x86_XMM2, SseType_QuadWord, TRUE);
	}
	else if ((RSPOpC.OP.V.element & 0xF) >= 8) {
		RSP_Element2Sse(x86_XMM2);
	}
	else {
		RSP_MultiElement2Sse(x86_XMM2);
	}

	// sign extend vs and vt
	Sse2MoveSxWordRegToDWordReg(&RspRecompPos, x86_XMM3, x86_XMM1, IsSse41Enabled);
	Sse2ShuffleDWordsRegToReg(&RspRecompPos, x86_XMM1, x86_XMM1, _MMX_SHUFFLE(3, 2, 3, 2));
	Sse2MoveSxWordRegToDWordReg(&RspRecompPos, x86_XMM1, x86_XMM1, IsSse41Enabled);

	Sse2MoveSxWordRegToDWordReg(&RspRecompPos, x86_XMM4, x86_XMM2, IsSse41Enabled);
	Sse2ShuffleDWordsRegToReg(&RspRecompPos, x86_XMM2, x86_XMM2, _MMX_SHUFFLE(3, 2, 3, 2));
	Sse2MoveSxWordRegToDWordReg(&RspRecompPos, x86_XMM2, x86_XMM2, IsSse41Enabled);

	// multiply
	Sse2PmulldRegToReg(&RspRecompPos, x86_XMM1, x86_XMM2);
	Sse2PmulldRegToReg(&RspRecompPos, x86_XMM3, x86_XMM4);

	// round
	SseMoveRegToReg(&RspRecompPos, x86_XMM2, x86_XMM1, SseType_QuadWord, TRUE);
	SseMoveRegToReg(&RspRecompPos, x86_XMM4, x86_XMM3, SseType_QuadWord, TRUE);
	Sse2PsradImmed(&RspRecompPos, x86_XMM2, 31);
	Sse2PsradImmed(&RspRecompPos, x86_XMM4, 31);
	Sse2PsrldImmed(&RspRecompPos, x86_XMM2, 27); // 0 or 1F
	Sse2PsrldImmed(&RspRecompPos, x86_XMM4, 27); // 0 or 1F
	Sse2PadddRegToReg(&RspRecompPos, x86_XMM1, x86_XMM2);
	Sse2PadddRegToReg(&RspRecompPos, x86_XMM3, x86_XMM4);

	if (writeToAccum) {
		SseMoveRegToReg(&RspRecompPos, x86_XMM6, x86_XMM1, SseType_QuadWord, TRUE);
		SseMoveRegToReg(&RspRecompPos, x86_XMM7, x86_XMM3, SseType_QuadWord, TRUE);
		Sse2PsradImmed(&RspRecompPos, x86_XMM6, 16);
		Sse2PsradImmed(&RspRecompPos, x86_XMM7, 16);
		Sse2PackSignedDWordRegToWordReg(&RspRecompPos, x86_XMM7, x86_XMM6);
		SseMoveAlignedRegToVariable(&RspRecompPos, x86_XMM7, &RSP_ACCUM_HIGH.UHW[0], "RSP_ACCUM_HIGH", SseType_QuadWord, TRUE);

		int reg1 = x86_XMM1;
		int reg2 = x86_XMM3;
		if (writeToVectorDest) {
			reg1 = x86_XMM6;
			reg2 = x86_XMM7;
			SseMoveRegToReg(&RspRecompPos, x86_XMM6, x86_XMM1, SseType_QuadWord, TRUE);
			SseMoveRegToReg(&RspRecompPos, x86_XMM7, x86_XMM3, SseType_QuadWord, TRUE);
		}
		Sse2PslldImmed(&RspRecompPos, reg1, 16);
		Sse2PslldImmed(&RspRecompPos, reg2, 16);
		Sse2PsradImmed(&RspRecompPos, reg1, 16);
		Sse2PsradImmed(&RspRecompPos, reg2, 16);
		Sse2PackSignedDWordRegToWordReg(&RspRecompPos, reg2, reg1);
		SseMoveAlignedRegToVariable(&RspRecompPos, reg2, &RSP_ACCUM_MID.UHW[0], "RSP_ACCUM_MID", SseType_QuadWord, TRUE);

		Sse2PxorRegToReg(&RspRecompPos, x86_XMM0, x86_XMM0);
		SseMoveAlignedRegToVariable(&RspRecompPos, x86_XMM0, &RSP_ACCUM_LOW.UHW[0], "RSP_ACCUM_LOW", SseType_QuadWord, TRUE);
	}

	if (writeToVectorDest) {
		Sse2PsradImmed(&RspRecompPos, x86_XMM1, 1);
		Sse2PsradImmed(&RspRecompPos, x86_XMM3, 1);

		Sse2PackSignedDWordRegToWordReg(&RspRecompPos, x86_XMM3, x86_XMM1);

		Sse2PsrlwImmed(&RspRecompPos, x86_XMM3, 4);
		Sse2PsllwImmed(&RspRecompPos, x86_XMM3, 4); // & ~0xF

		sprintf(Reg, "RSP_Vect[%i]", RSPOpC.OP.V.vd);
		SseMoveAlignedRegToVariable(&RspRecompPos, x86_XMM3, &RSP_Vect[RSPOpC.OP.V.vd].UHW[0], Reg, SseType_QuadWord, TRUE);
	}

	return TRUE;
}

static BOOL Compile_Vector_VMULQ_AVX(BOOL writeToVectorDest, BOOL writeToAccum) {
	char Reg[256];

	/* Do our AVX checks here */
	if (IsAvxEnabled == FALSE || IsAvx2Enabled == FALSE || IsSseEnabled == FALSE || IsSse2Enabled == FALSE)
		return FALSE;

	// load vs and vt
	sprintf(Reg, "RSP_Vect[%i]", RSPOpC.OP.V.vs);
	SseMoveAlignedVariableToReg(&RspRecompPos, &RSP_Vect[RSPOpC.OP.V.vs].UHW[0], Reg, x86_XMM1, SseType_QuadWord, TRUE);

	if ((RSPOpC.OP.V.element & 0xF) < 2) {
		sprintf(Reg, "RSP_Vect[%i]", RSPOpC.OP.V.vt);
		SseMoveAlignedVariableToReg(&RspRecompPos, &RSP_Vect[RSPOpC.OP.V.vt].UHW[0], Reg, x86_XMM2, SseType_QuadWord, TRUE);
	}
	else if ((RSPOpC.OP.V.element & 0xF) >= 8) {
		RSP_Element2Sse(x86_XMM2);
	}
	else {
		RSP_MultiElement2Sse(x86_XMM2);
	}
	
	// sign extend vs and vt
	AvxVPMovesxWordReg128ToDwordReg256(&RspRecompPos, x86_YMM1, x86_XMM1);
	AvxVPMovesxWordReg128ToDwordReg256(&RspRecompPos, x86_YMM2, x86_XMM2);

	// multiply
	AvxVPMulldRegToReg256(&RspRecompPos, x86_YMM1, x86_YMM1, x86_YMM2);

	// round
	AvxVPSradRegToReg256Immed(&RspRecompPos, x86_YMM2, x86_YMM1, 31);
	AvxVPSrldRegToReg256Immed(&RspRecompPos, x86_YMM2, x86_YMM2, 27);
	AvxVPAdddRegToReg256(&RspRecompPos, x86_YMM1, x86_YMM1, x86_YMM2);

	if (writeToAccum) {
		AvxVPSradRegToReg256Immed(&RspRecompPos, x86_YMM2, x86_YMM1, 16);
		AvxVExtracti128RegToReg(&RspRecompPos, x86_XMM3, x86_YMM2, TRUE);
		AvxVPackSignedDWordRegToWordReg128(&RspRecompPos, x86_XMM2, x86_XMM2, x86_XMM3);
		SseMoveAlignedRegToVariable(&RspRecompPos, x86_XMM2, &RSP_ACCUM_HIGH.UHW[0], "RSP_ACCUM_HIGH", SseType_QuadWord, TRUE);

		AvxVPSlldRegToReg256Immed(&RspRecompPos, x86_YMM2, x86_YMM1, 16);
		AvxVPSradRegToReg256Immed(&RspRecompPos, x86_YMM2, x86_YMM2, 16);
		AvxVExtracti128RegToReg(&RspRecompPos, x86_XMM3, x86_YMM2, TRUE);
		AvxVPackSignedDWordRegToWordReg128(&RspRecompPos, x86_XMM2, x86_XMM2, x86_XMM3);
		SseMoveAlignedRegToVariable(&RspRecompPos, x86_XMM2, &RSP_ACCUM_MID.UHW[0], "RSP_ACCUM_MID", SseType_QuadWord, TRUE);

		Sse2PxorRegToReg(&RspRecompPos, x86_XMM0, x86_XMM0);
		SseMoveAlignedRegToVariable(&RspRecompPos, x86_XMM0, &RSP_ACCUM_LOW.UHW[0], "RSP_ACCUM_LOW", SseType_QuadWord, TRUE);
	}

	if (writeToVectorDest) {
		AvxVPSradRegToReg256Immed(&RspRecompPos, x86_YMM1, x86_YMM1, 1);

		AvxVExtracti128RegToReg(&RspRecompPos, x86_XMM2, x86_YMM1, TRUE);
		AvxVPackSignedDWordRegToWordReg128(&RspRecompPos, x86_XMM1, x86_XMM1, x86_XMM2);

		Sse2PsrlwImmed(&RspRecompPos, x86_XMM1, 4);
		Sse2PsllwImmed(&RspRecompPos, x86_XMM1, 4); // & ~0xF

		sprintf(Reg, "RSP_Vect[%i]", RSPOpC.OP.V.vd);
		SseMoveAlignedRegToVariable(&RspRecompPos, x86_XMM1, &RSP_Vect[RSPOpC.OP.V.vd].UHW[0], Reg, SseType_QuadWord, TRUE);
	}

	return TRUE;
}

void CompileRsp_Vector_VMULQ(void) {
	char Reg[256];
	int count, el, del;

	BOOL bOptimize = ((RSPOpC.OP.V.element & 0x0f) >= 8) ? TRUE : FALSE;
	BOOL bWriteToAccum = WriteToAccum(EntireAccum, RspCompilePC);
	BOOL bWriteToDest = WriteToVectorDest(RSPOpC.OP.V.vd, RspCompilePC);

	#ifndef CompileVmulq
	InterpreterFallback((void*)RSP_Vector_VMULQ, "RSP_Vector_VMULQ"); return;
	#endif

	if (bWriteToDest == FALSE && bWriteToAccum == FALSE) {
		return;
	}

	if (TRUE == Compile_Vector_VMULQ_AVX(bWriteToDest, bWriteToAccum)) {
		return;
	}

	if (TRUE == Compile_Vector_VMULQ_SSE2(bWriteToDest, bWriteToAccum)) {
		return;
	}

	if (bOptimize == TRUE) {
		del = (RSPOpC.OP.V.element & 0x07) ^ 7;
		sprintf(Reg, "RSP_Vect[%i].HW[%i]", RSPOpC.OP.V.vt, del);
		MoveSxVariableToX86regHalf(&RspRecompPos, &RSP_Vect[RSPOpC.OP.V.vt].HW[del], Reg, x86_ECX);
	}

	if (bWriteToDest == TRUE) {
		MoveConstToX86reg(&RspRecompPos, 0x7fff, x86_ESI);
		MoveConstToX86reg(&RspRecompPos, 0xffff8000, x86_EDI);
	}

	for (count = 0; count < 8; count++) {
		RSP_CPU_Message("     Iteration: %i", count);

		el = Indx[RSPOpC.OP.V.element].B[count];
		del = EleSpec[RSPOpC.OP.V.element].B[el];

		sprintf(Reg, "RSP_Vect[%i].HW[%i]", RSPOpC.OP.V.vs, el);
		MoveSxVariableToX86regHalf(&RspRecompPos, &RSP_Vect[RSPOpC.OP.V.vs].HW[el], Reg, x86_EAX);

		if (RSPOpC.OP.V.vt == RSPOpC.OP.V.vs && el == del) {
			imulX86reg(&RspRecompPos, x86_EAX);
		}
		else {
			if (bOptimize == FALSE) {
				sprintf(Reg, "RSP_Vect[%i].HW[%i]", RSPOpC.OP.V.vt, del);
				MoveSxVariableToX86regHalf(&RspRecompPos, &RSP_Vect[RSPOpC.OP.V.vt].HW[del], Reg, x86_ECX);
			}
			imulX86reg(&RspRecompPos, x86_ECX);
		}

		MoveX86RegToX86Reg(&RspRecompPos, x86_EAX, x86_EBX);
		ShiftRightSignImmed(&RspRecompPos, x86_EBX, 31);
		AndConstToX86Reg(&RspRecompPos, x86_EBX, 0x1F);
		AddX86RegToX86Reg(&RspRecompPos, x86_EAX, x86_EBX);

		if (bWriteToAccum == TRUE) {
			MoveX86RegToX86Reg(&RspRecompPos, x86_EAX, x86_EBX);
			ShiftRightUnsignImmed(&RspRecompPos, x86_EBX, 16);
			MoveX86regHalfToVariable(&RspRecompPos, x86_EBX, &RSP_ACCUM_HIGH.HW[el], "RSP_ACCUM_HIGH.HW[el]");
			MoveX86regHalfToVariable(&RspRecompPos, x86_EAX, &RSP_ACCUM_MID.HW[el], "RSP_ACCUM_MID.HW[el]");
			MoveConstHalfToVariable(&RspRecompPos, 0, &RSP_ACCUM_LOW.HW[el], "RSP_ACCUM_LOW.HW[el]");
		}

		if (bWriteToDest == TRUE) {
			ShiftRightSignImmed(&RspRecompPos, x86_EAX, 1);

			CompX86RegToX86Reg(&RspRecompPos, x86_EAX, x86_ESI);
			CondMoveGreater(&RspRecompPos, x86_EAX, x86_ESI);
			CompX86RegToX86Reg(&RspRecompPos, x86_EAX, x86_EDI);
			CondMoveLess(&RspRecompPos, x86_EAX, x86_EDI);

			AndConstToX86Reg(&RspRecompPos, x86_EAX, ~0xFU);

			MoveX86regHalfToVariable(&RspRecompPos, x86_EAX, &RSP_Vect[RSPOpC.OP.V.vd].HW[el], "RSP_Vect[RSPOpC.OP.V.vd].HW[el]");
		}
	}
}

static BOOL Compile_Vector_VMUDL_MMX ( void ) {
	char Reg[256];

	/* Do our MMX checks here */
	if (IsMmxEnabled == FALSE)
		return FALSE;
	if (IsMmx2Enabled == FALSE)
		return FALSE;

	sprintf(Reg, "RSP_Vect[%i].UHW[0]", RSPOpC.OP.V.vs);
	MmxMoveQwordVariableToReg(&RspRecompPos, x86_MM0, &RSP_Vect[RSPOpC.OP.V.vs].UHW[0], Reg);
	sprintf(Reg, "RSP_Vect[%i].UHW[4]", RSPOpC.OP.V.vs);
	MmxMoveQwordVariableToReg(&RspRecompPos, x86_MM1, &RSP_Vect[RSPOpC.OP.V.vs].UHW[4], Reg);

	if ((RSPOpC.OP.V.element & 0xF) < 2) {
		sprintf(Reg, "RSP_Vect[%i].UHW[0]", RSPOpC.OP.V.vt);
		MmxMoveQwordVariableToReg(&RspRecompPos, x86_MM2, &RSP_Vect[RSPOpC.OP.V.vt].UHW[0], Reg);
		sprintf(Reg, "RSP_Vect[%i].UHW[4]", RSPOpC.OP.V.vt);
		MmxMoveQwordVariableToReg(&RspRecompPos, x86_MM3, &RSP_Vect[RSPOpC.OP.V.vt].UHW[4], Reg);
		
		MmxPmulhuwRegToReg(&RspRecompPos, x86_MM0, x86_MM2);
		MmxPmulhuwRegToReg(&RspRecompPos, x86_MM1, x86_MM3);
	} else if ((RSPOpC.OP.V.element & 0xF) >= 8) {
		RSP_Element2Mmx(x86_MM2);
		MmxPmulhuwRegToReg(&RspRecompPos, x86_MM0, x86_MM2);
		MmxPmulhuwRegToReg(&RspRecompPos, x86_MM1, x86_MM2);
	} else {
		RSP_MultiElement2Mmx(x86_MM2, x86_MM3);
		MmxPmulhuwRegToReg(&RspRecompPos, x86_MM0, x86_MM2);
		MmxPmulhuwRegToReg(&RspRecompPos, x86_MM1, x86_MM3);
	}

	sprintf(Reg, "RSP_Vect[%i].UHW[0]", RSPOpC.OP.V.vd);
	MmxMoveQwordRegToVariable(&RspRecompPos, x86_MM0, &RSP_Vect[RSPOpC.OP.V.vd].UHW[0], Reg);
	sprintf(Reg, "RSP_Vect[%i].UHW[4]", RSPOpC.OP.V.vd);
	MmxMoveQwordRegToVariable(&RspRecompPos, x86_MM1, &RSP_Vect[RSPOpC.OP.V.vd].UHW[4], Reg);
	
	if (IsNextRspInstructionMmx(RspCompilePC) == FALSE)
		MmxEmptyMultimediaState(&RspRecompPos);

	return TRUE;
}

static BOOL Compile_Vector_VMUDL_SSE2(BOOL writeToVectorDest, BOOL writeToAccum) {
	char Reg[256];

	/* Do our SSE checks here */
	if (IsSseEnabled == FALSE || IsSse2Enabled == FALSE)
		return FALSE;

	sprintf(Reg, "RSP_Vect[%i]", RSPOpC.OP.V.vs);
	SseMoveAlignedVariableToReg(&RspRecompPos, &RSP_Vect[RSPOpC.OP.V.vs].UHW[0], Reg, x86_XMM0, SseType_QuadWord, TRUE);

	if ((RSPOpC.OP.V.element & 0xF) < 2) {
		sprintf(Reg, "RSP_Vect[%i]", RSPOpC.OP.V.vt);
		Sse2PmulhuwVariableToReg(&RspRecompPos, x86_XMM0, &RSP_Vect[RSPOpC.OP.V.vt].UHW[0], Reg);
	}
	else if ((RSPOpC.OP.V.element & 0xF) >= 8) {
		RSP_Element2Sse(x86_XMM1);
		Sse2PmulhuwRegToReg(&RspRecompPos, x86_XMM0, x86_XMM1);
	}
	else {
		RSP_MultiElement2Sse(x86_XMM1);
		Sse2PmulhuwRegToReg(&RspRecompPos, x86_XMM0, x86_XMM1);
	}

	if (writeToVectorDest) {
		sprintf(Reg, "RSP_Vect[%i]", RSPOpC.OP.V.vd);
		SseMoveAlignedRegToVariable(&RspRecompPos, x86_XMM0, &RSP_Vect[RSPOpC.OP.V.vd].UHW[0], Reg, SseType_QuadWord, TRUE);
	}
	if (writeToAccum) {
		Sse2PxorRegToReg(&RspRecompPos, x86_XMM1, x86_XMM1);
		SseMoveAlignedRegToVariable(&RspRecompPos, x86_XMM1, &RSP_ACCUM_HIGH.UHW[0], "RSP_ACCUM_HIGH", SseType_QuadWord, TRUE);
		SseMoveAlignedRegToVariable(&RspRecompPos, x86_XMM1, &RSP_ACCUM_MID.UHW[0], "RSP_ACCUM_MID", SseType_QuadWord, TRUE);
		SseMoveAlignedRegToVariable(&RspRecompPos, x86_XMM0, &RSP_ACCUM_LOW.UHW[0], "RSP_ACCUM_LOW", SseType_QuadWord, TRUE);
	}

	return TRUE;
}

void CompileRsp_Vector_VMUDL ( void ) {
	char Reg[256];
	int count, el, del;
	
	BOOL bOptimize = ((RSPOpC.OP.V.element & 0x0f) >= 8) ? TRUE : FALSE;
	BOOL bWriteToDest = WriteToVectorDest(RSPOpC.OP.V.vd, RspCompilePC);
	BOOL bWriteToAccum = WriteToAccum(EntireAccum, RspCompilePC);

	#ifndef CompileVmudl
	InterpreterFallback((void*)RSP_Vector_VMUDL,"RSP_Vector_VMUDL"); return;
	#endif

	RSP_CPU_Message("  %X %s",RspCompilePC,RSPOpcodeName(RSPOpC.OP.Hex,RspCompilePC));

	if (bWriteToDest == FALSE && bWriteToAccum == FALSE) {
		return;
	}

	if (TRUE == Compile_Vector_VMUDL_SSE2(bWriteToDest, bWriteToAccum)) {
		return;
	}

	if (bWriteToAccum == FALSE) {
		if (TRUE == Compile_Vector_VMUDL_MMX())
			return;
	}
	
	if (bOptimize == TRUE) {
		del = (RSPOpC.OP.V.element & 0x07) ^ 7;
		sprintf(Reg, "RSP_Vect[%i].HW[%i]", RSPOpC.OP.V.vt, del);
		MoveZxVariableToX86regHalf(&RspRecompPos, &RSP_Vect[RSPOpC.OP.V.vt].HW[del], Reg, x86_EBX);
	}

	if (bWriteToAccum == TRUE)
		XorX86RegToX86Reg(&RspRecompPos, x86_EDI, x86_EDI);

	for (count = 0; count < 8; count++) {
		RSP_CPU_Message("     Iteration: %i", count);
		el = Indx[RSPOpC.OP.V.element].B[count];
		del = EleSpec[RSPOpC.OP.V.element].B[el];

		sprintf(Reg, "RSP_Vect[%i].UHW[%i]", RSPOpC.OP.V.vs, el);
		MoveZxVariableToX86regHalf(&RspRecompPos, &RSP_Vect[RSPOpC.OP.V.vs].UHW[el], Reg, x86_EAX);

		if (bOptimize == FALSE) {
			sprintf(Reg, "RSP_Vect[%i].HW[%i]", RSPOpC.OP.V.vt, del);
			MoveZxVariableToX86regHalf(&RspRecompPos, &RSP_Vect[RSPOpC.OP.V.vt].HW[del], Reg, x86_EBX);
		}

		imulX86reg(&RspRecompPos, x86_EBX);
		ShiftRightUnsignImmed(&RspRecompPos, x86_EAX, 16);

		if (bWriteToAccum == TRUE) {
			sprintf(Reg, "RSP_ACCUM_LOW.HW[%i]", el);
			MoveX86regHalfToVariable(&RspRecompPos, x86_EAX, &RSP_ACCUM_LOW.UHW[el], Reg);
			sprintf(Reg, "RSP_ACCUM_HIGH.HW[%i]", el);
			MoveX86regHalfToVariable(&RspRecompPos, x86_EDI, &RSP_ACCUM_HIGH.UHW[el], Reg);
			sprintf(Reg, "RSP_ACCUM_MID.HW[%i]", el);
			MoveX86regHalfToVariable(&RspRecompPos, x86_EDI, &RSP_ACCUM_MID.UHW[el], Reg);
		}

		if (bWriteToDest == TRUE) {
			sprintf(Reg, "RSP_Vect[%i].HW[%i]", RSPOpC.OP.V.vd, el);
			MoveX86regHalfToVariable(&RspRecompPos, x86_EAX, &RSP_Vect[RSPOpC.OP.V.vd].HW[el], Reg);
		}
	}
}

static BOOL Compile_Vector_VMUDM_MMX ( void ) {
	char Reg[256];

	/* Do our MMX checks here */
	if (IsMmxEnabled == FALSE)
		return FALSE;
	if (IsMmx2Enabled == FALSE)
		return FALSE;

	sprintf(Reg, "RSP_Vect[%i].UHW[0]", RSPOpC.OP.V.vs);
	MmxMoveQwordVariableToReg(&RspRecompPos, x86_MM0, &RSP_Vect[RSPOpC.OP.V.vs].UHW[0], Reg);
	sprintf(Reg, "RSP_Vect[%i].UHW[4]", RSPOpC.OP.V.vs);
	MmxMoveQwordVariableToReg(&RspRecompPos, x86_MM1, &RSP_Vect[RSPOpC.OP.V.vs].UHW[4], Reg);

	if ((RSPOpC.OP.V.element & 0xF) < 2) {
		sprintf(Reg, "RSP_Vect[%i].UHW[0]", RSPOpC.OP.V.vt);
		MmxMoveQwordVariableToReg(&RspRecompPos, x86_MM4, &RSP_Vect[RSPOpC.OP.V.vt].UHW[0], Reg);
		sprintf(Reg, "RSP_Vect[%i].UHW[4]", RSPOpC.OP.V.vt);
		MmxMoveQwordVariableToReg(&RspRecompPos, x86_MM5, &RSP_Vect[RSPOpC.OP.V.vt].UHW[4], Reg);

		/* Copy the signed portion */
		MmxMoveRegToReg(&RspRecompPos, x86_MM2, x86_MM0);
		MmxMoveRegToReg(&RspRecompPos, x86_MM3, x86_MM1);

		/* high((u16)a * b) */
		MmxPmulhuwRegToReg(&RspRecompPos, x86_MM0, x86_MM4);
		MmxPmulhuwRegToReg(&RspRecompPos, x86_MM1, x86_MM5);

		/* low((a >> 15) * b) */
		MmxPsrawImmed(&RspRecompPos, x86_MM2, 15);
		MmxPsrawImmed(&RspRecompPos, x86_MM3, 15);
		MmxPmullwRegToReg(&RspRecompPos, x86_MM2, x86_MM4);
		MmxPmullwRegToReg(&RspRecompPos, x86_MM3, x86_MM5);
	} else if ((RSPOpC.OP.V.element & 0xF) >= 8) {
		RSP_Element2Mmx(x86_MM4);

		/* Copy the signed portion */
		MmxMoveRegToReg(&RspRecompPos, x86_MM2, x86_MM0);
		MmxMoveRegToReg(&RspRecompPos, x86_MM3, x86_MM1);

		/* high((u16)a * b) */
		MmxPmulhuwRegToReg(&RspRecompPos, x86_MM0, x86_MM4);
		MmxPmulhuwRegToReg(&RspRecompPos, x86_MM1, x86_MM4);

		/* low((a >> 15) * b) */
		MmxPsrawImmed(&RspRecompPos, x86_MM2, 15);
		MmxPsrawImmed(&RspRecompPos, x86_MM3, 15);
		MmxPmullwRegToReg(&RspRecompPos, x86_MM2, x86_MM4);
		MmxPmullwRegToReg(&RspRecompPos, x86_MM3, x86_MM4);
	} else {
		RSP_MultiElement2Mmx(x86_MM4, x86_MM5);

		/* Copy the signed portion */
		MmxMoveRegToReg(&RspRecompPos, x86_MM2, x86_MM0);
		MmxMoveRegToReg(&RspRecompPos, x86_MM3, x86_MM1);

		/* high((u16)a * b) */
		MmxPmulhuwRegToReg(&RspRecompPos, x86_MM0, x86_MM4);
		MmxPmulhuwRegToReg(&RspRecompPos, x86_MM1, x86_MM5);

		/* low((a >> 15) * b) */
		MmxPsrawImmed(&RspRecompPos, x86_MM2, 15);
		MmxPsrawImmed(&RspRecompPos, x86_MM3, 15);
		MmxPmullwRegToReg(&RspRecompPos, x86_MM2, x86_MM4);
		MmxPmullwRegToReg(&RspRecompPos, x86_MM3, x86_MM5);
	}

	/* Add them up */
	MmxPaddwRegToReg(&RspRecompPos, x86_MM0, x86_MM2);
	MmxPaddwRegToReg(&RspRecompPos, x86_MM1, x86_MM3);

	sprintf(Reg, "RSP_Vect[%i].UHW[0]", RSPOpC.OP.V.vd);
	MmxMoveQwordRegToVariable(&RspRecompPos, x86_MM0, &RSP_Vect[RSPOpC.OP.V.vd].UHW[0], Reg);
	sprintf(Reg, "RSP_Vect[%i].UHW[4]", RSPOpC.OP.V.vd);
	MmxMoveQwordRegToVariable(&RspRecompPos, x86_MM1, &RSP_Vect[RSPOpC.OP.V.vd].UHW[4], Reg);

	if (IsNextRspInstructionMmx(RspCompilePC) == FALSE)
		MmxEmptyMultimediaState(&RspRecompPos);

	return TRUE;
}

static BOOL Compile_Vector_VMUDM_SSE2(BOOL writeToVectorDest, BOOL writeToAccum) {
	char Reg[256];

	/* Do our SSE checks here */
	if (IsSseEnabled == FALSE || IsSse2Enabled == FALSE)
		return FALSE;

	sprintf(Reg, "RSP_Vect[%i]", RSPOpC.OP.V.vs);
	SseMoveAlignedVariableToReg(&RspRecompPos, &RSP_Vect[RSPOpC.OP.V.vs].UHW[0], Reg, x86_XMM0, SseType_QuadWord, TRUE);

	if ((RSPOpC.OP.V.element & 0xF) < 2) {
		sprintf(Reg, "RSP_Vect[%i]", RSPOpC.OP.V.vt);
		SseMoveAlignedVariableToReg(&RspRecompPos, &RSP_Vect[RSPOpC.OP.V.vt].UHW[0], Reg, x86_XMM1, SseType_QuadWord, TRUE);
	}
	else if ((RSPOpC.OP.V.element & 0xF) >= 8) {
		RSP_Element2Sse(x86_MM1);
	}
	else {
		RSP_MultiElement2Sse(x86_MM1);
	}

	if (writeToAccum) {
		SseMoveRegToReg(&RspRecompPos, x86_XMM2, x86_XMM0, SseType_QuadWord, TRUE);
		Sse2PmullwRegToReg(&RspRecompPos, x86_XMM2, x86_XMM1);
		SseMoveAlignedRegToVariable(&RspRecompPos, x86_XMM2, &RSP_ACCUM_LOW.UHW[0], "RSP_ACCUM_LOW", SseType_QuadWord, TRUE);
	}

	/* Copy the signed portion */
	SseMoveRegToReg(&RspRecompPos, x86_XMM2, x86_XMM0, SseType_QuadWord, TRUE);

	/* high((u16)a * b) */
	Sse2PmulhuwRegToReg(&RspRecompPos, x86_XMM0, x86_XMM1);

	/* low((a >> 15) * b) */
	Sse2PsrawImmed(&RspRecompPos, x86_XMM2, 15);
	Sse2PmullwRegToReg(&RspRecompPos, x86_XMM2, x86_XMM1);

	/* Add them up */
	Sse2PaddwRegToReg(&RspRecompPos, x86_XMM0, x86_XMM2);

	if (writeToVectorDest) {
		sprintf(Reg, "RSP_Vect[%i]", RSPOpC.OP.V.vd);
		SseMoveAlignedRegToVariable(&RspRecompPos, x86_XMM0, &RSP_Vect[RSPOpC.OP.V.vd].UHW[0], Reg, SseType_QuadWord, TRUE);
	}

	if (writeToAccum) {
		SseMoveAlignedRegToVariable(&RspRecompPos, x86_XMM0, &RSP_ACCUM_MID.UHW[0], "RSP_ACCUM_MID", SseType_QuadWord, TRUE);
		Sse2PsrawImmed(&RspRecompPos, x86_XMM0, 15);
		SseMoveAlignedRegToVariable(&RspRecompPos, x86_XMM0, &RSP_ACCUM_HIGH.UHW[0], "RSP_ACCUM_HIGH", SseType_QuadWord, TRUE);
	}

	return TRUE;
}

static BOOL Compile_Vector_VMUDM_AVX(BOOL writeToVectorDest, BOOL writeToAccum) {
	char Reg[256];

	/* Do our SSE checks here */
	if (IsSseEnabled == FALSE || IsSse2Enabled == FALSE)
		return FALSE;

	sprintf(Reg, "RSP_Vect[%i]", RSPOpC.OP.V.vs);
	SseMoveAlignedVariableToReg(&RspRecompPos, &RSP_Vect[RSPOpC.OP.V.vs].UHW[0], Reg, x86_XMM0, SseType_QuadWord, TRUE);

	if ((RSPOpC.OP.V.element & 0xF) < 2) {
		sprintf(Reg, "RSP_Vect[%i]", RSPOpC.OP.V.vt);
		SseMoveAlignedVariableToReg(&RspRecompPos, &RSP_Vect[RSPOpC.OP.V.vt].UHW[0], Reg, x86_XMM1, SseType_QuadWord, TRUE);
	}
	else if ((RSPOpC.OP.V.element & 0xF) >= 8) {
		RSP_Element2Sse(x86_MM1);
	}
	else {
		RSP_MultiElement2Sse(x86_MM1);
	}

	if (writeToAccum) {
		AvxVPMullwRegToReg128(&RspRecompPos, x86_XMM2, x86_XMM0, x86_XMM1);
		SseMoveAlignedRegToVariable(&RspRecompPos, x86_XMM2, &RSP_ACCUM_LOW.UHW[0], "RSP_ACCUM_LOW", SseType_QuadWord, TRUE);
	}

	/* low((a >> 15) * b) */
	AvxVPSrawRegToReg128Immed(&RspRecompPos, x86_XMM2, x86_XMM0, 15);
	Sse2PmullwRegToReg(&RspRecompPos, x86_XMM2, x86_XMM1);

	/* high((u16)a * b) */
	Sse2PmulhuwRegToReg(&RspRecompPos, x86_XMM0, x86_XMM1);

	/* Add them up */
	Sse2PaddwRegToReg(&RspRecompPos, x86_XMM0, x86_XMM2);

	if (writeToVectorDest) {
		sprintf(Reg, "RSP_Vect[%i]", RSPOpC.OP.V.vd);
		SseMoveAlignedRegToVariable(&RspRecompPos, x86_XMM0, &RSP_Vect[RSPOpC.OP.V.vd].UHW[0], Reg, SseType_QuadWord, TRUE);
	}

	if (writeToAccum) {
		SseMoveAlignedRegToVariable(&RspRecompPos, x86_XMM0, &RSP_ACCUM_MID.UHW[0], "RSP_ACCUM_MID", SseType_QuadWord, TRUE);
		Sse2PsrawImmed(&RspRecompPos, x86_XMM0, 15);
		SseMoveAlignedRegToVariable(&RspRecompPos, x86_XMM0, &RSP_ACCUM_HIGH.UHW[0], "RSP_ACCUM_HIGH", SseType_QuadWord, TRUE);
	}

	return TRUE;
}

void CompileRsp_Vector_VMUDM ( void ) {
	char Reg[256];
	int count, el, del;

	BOOL bOptimize = ((RSPOpC.OP.V.element & 0x0f) >= 8) ? TRUE : FALSE;
	BOOL bWriteToDest = WriteToVectorDest(RSPOpC.OP.V.vd, RspCompilePC);
	BOOL bWriteToAccum = WriteToAccum(EntireAccum, RspCompilePC);

	#ifndef CompileVmudm
	InterpreterFallback((void*)RSP_Vector_VMUDM,"RSP_Vector_VMUDM"); return;
	#endif

	RSP_CPU_Message("  %X %s",RspCompilePC,RSPOpcodeName(RSPOpC.OP.Hex,RspCompilePC));

	if (bWriteToDest == FALSE && bWriteToAccum == FALSE) {
		return;
	}

	if (TRUE == Compile_Vector_VMUDM_AVX(bWriteToDest, bWriteToAccum)) {
		return;
	}

	if (TRUE == Compile_Vector_VMUDM_SSE2(bWriteToDest, bWriteToAccum)) {
		return;
	}

	if (bWriteToAccum == FALSE) {
		if (TRUE == Compile_Vector_VMUDM_MMX())
			return;
	}

	if (bOptimize == TRUE) {
		del = (RSPOpC.OP.V.element & 0x07) ^ 7;
		sprintf(Reg, "RSP_Vect[%i].HW[%i]", RSPOpC.OP.V.vt, del);
		MoveZxVariableToX86regHalf(&RspRecompPos, &RSP_Vect[RSPOpC.OP.V.vt].HW[del], Reg, x86_EBX);
	}

	sprintf(Reg, "RSP_Vect[%i].HW[0]", RSPOpC.OP.V.vs);
	MoveOffsetToX86reg(&RspRecompPos, (DWORD)&RSP_Vect[RSPOpC.OP.V.vs].HW[0], Reg, x86_ESI);

	if (bWriteToDest) {
		sprintf(Reg, "RSP_Vect[%i].HW[0]", RSPOpC.OP.V.vd);
		MoveOffsetToX86reg(&RspRecompPos, (DWORD)&RSP_Vect[RSPOpC.OP.V.vd].HW[0], Reg, x86_ECX);
	}
	if (!bOptimize) {
		sprintf(Reg, "RSP_Vect[%i].HW[0]", RSPOpC.OP.V.vt);
		MoveOffsetToX86reg(&RspRecompPos, (DWORD)&RSP_Vect[RSPOpC.OP.V.vt].HW[0], Reg, x86_EDI);
	}

	for (count = 0; count < 8; count++) {
		RSP_CPU_Message("     Iteration: %i", count);
		el = Indx[RSPOpC.OP.V.element].B[count];
		del = EleSpec[RSPOpC.OP.V.element].B[el];

		MoveSxX86RegPtrDispToX86RegHalf(&RspRecompPos, x86_ESI, (BYTE)(el * 2), x86_EAX); // vs

		if (bOptimize == FALSE) {
			MoveZxX86RegPtrDispToX86RegHalf(&RspRecompPos, x86_EDI, (BYTE)(del * 2), x86_EBX); // vt
		}

		imulX86reg(&RspRecompPos, x86_EBX);

		if (bWriteToAccum == TRUE) {
			sprintf(Reg, "RSP_ACCUM_LOW.HW[%i]", el);
			MoveX86regHalfToVariable(&RspRecompPos, x86_EAX, &RSP_ACCUM_LOW.UHW[el], Reg);
		}
		ShiftRightSignImmed(&RspRecompPos, x86_EAX, 16);
		if (bWriteToDest == TRUE) {
			MoveX86regHalfToX86regPointerDisp(&RspRecompPos, x86_EAX, x86_ECX, (BYTE)(el * 2));
		}
		if (bWriteToAccum == TRUE) {
			sprintf(Reg, "RSP_ACCUM_MID.HW[%i]", el);
			MoveX86regHalfToVariable(&RspRecompPos, x86_EAX, &RSP_ACCUM_MID.UHW[el], Reg);
			sprintf(Reg, "RSP_ACCUM_HIGH.HW[%i]", el);
			MoveX86regHalfToVariable(&RspRecompPos, x86_EDX, &RSP_ACCUM_HIGH.UHW[el], Reg);
		}
	}
}

static BOOL Compile_Vector_VMUDN_MMX ( void ) {
	char Reg[256];

	/* Do our MMX checks here */
	if (IsMmxEnabled == FALSE)
		return FALSE;
	if ((RSPOpC.OP.V.element & 0x0f) >= 2 && (RSPOpC.OP.V.element & 0x0f) <= 7 && IsMmx2Enabled == FALSE)
		return FALSE;

	sprintf(Reg, "RSP_Vect[%i].UHW[0]", RSPOpC.OP.V.vs);
	MmxMoveQwordVariableToReg(&RspRecompPos, x86_MM0, &RSP_Vect[RSPOpC.OP.V.vs].UHW[0], Reg);
	sprintf(Reg, "RSP_Vect[%i].UHW[4]", RSPOpC.OP.V.vs);
	MmxMoveQwordVariableToReg(&RspRecompPos, x86_MM1, &RSP_Vect[RSPOpC.OP.V.vs].UHW[4], Reg);

	if ((RSPOpC.OP.V.element & 0xF) < 2) {
		sprintf(Reg, "RSP_Vect[%i].UHW[0]", RSPOpC.OP.V.vt);
		MmxPmullwVariableToReg(&RspRecompPos, x86_MM0, &RSP_Vect[RSPOpC.OP.V.vt].UHW[0], Reg);
		sprintf(Reg, "RSP_Vect[%i].UHW[4]", RSPOpC.OP.V.vt);
		MmxPmullwVariableToReg(&RspRecompPos, x86_MM1, &RSP_Vect[RSPOpC.OP.V.vt].UHW[4], Reg);
	} else if ((RSPOpC.OP.V.element & 0xF) >= 8) {
		RSP_Element2Mmx(x86_MM2);
		MmxPmullwRegToReg(&RspRecompPos, x86_MM0, x86_MM2);
		MmxPmullwRegToReg(&RspRecompPos, x86_MM1, x86_MM2);
	} else {
		RSP_MultiElement2Mmx(x86_MM2, x86_MM3);
		MmxPmullwRegToReg(&RspRecompPos, x86_MM0, x86_MM2);
		MmxPmullwRegToReg(&RspRecompPos, x86_MM1, x86_MM3);
	}

	sprintf(Reg, "RSP_Vect[%i].UHW[0]", RSPOpC.OP.V.vd);
	MmxMoveQwordRegToVariable(&RspRecompPos, x86_MM0, &RSP_Vect[RSPOpC.OP.V.vd].UHW[0], Reg);
	sprintf(Reg, "RSP_Vect[%i].UHW[4]", RSPOpC.OP.V.vd);
	MmxMoveQwordRegToVariable(&RspRecompPos, x86_MM1, &RSP_Vect[RSPOpC.OP.V.vd].UHW[4], Reg);

	if (IsNextRspInstructionMmx(RspCompilePC) == FALSE)
		MmxEmptyMultimediaState(&RspRecompPos);

	return TRUE;
}

static BOOL Compile_Vector_VMUDN_NoAccum_SSE2(void) {
	char Reg[256];

	/* Do our SSE checks here */
	if (IsSseEnabled == FALSE || IsSse2Enabled == FALSE)
		return FALSE;

	sprintf(Reg, "RSP_Vect[%i]", RSPOpC.OP.V.vs);
	SseMoveAlignedVariableToReg(&RspRecompPos, &RSP_Vect[RSPOpC.OP.V.vs].UHW[0], Reg, x86_XMM0, SseType_QuadWord, TRUE);

	if ((RSPOpC.OP.V.element & 0xF) < 2) {
		sprintf(Reg, "RSP_Vect[%i]", RSPOpC.OP.V.vt);
		Sse2PmullwVariableToReg(&RspRecompPos, x86_XMM0, &RSP_Vect[RSPOpC.OP.V.vt].UHW[0], Reg);
	} else if ((RSPOpC.OP.V.element & 0xF) >= 8) {
		RSP_Element2Sse(x86_XMM1);
		Sse2PmullwRegToReg(&RspRecompPos, x86_XMM0, x86_XMM1);
	} else {
		RSP_MultiElement2Sse(x86_XMM1);
		Sse2PmullwRegToReg(&RspRecompPos, x86_XMM0, x86_XMM1);
	}

	sprintf(Reg, "RSP_Vect[%i]", RSPOpC.OP.V.vd);
	SseMoveAlignedRegToVariable(&RspRecompPos, x86_XMM0, &RSP_Vect[RSPOpC.OP.V.vd].UHW[0], Reg, SseType_QuadWord, TRUE);

	return TRUE;
}

static BOOL Compile_Vector_VMUDN_SSE2(BOOL writeToVectorDest) {
	char Reg[256];

	/* Do our SSE checks here */
	if (IsSseEnabled == FALSE || IsSse2Enabled == FALSE)
		return FALSE;

	sprintf(Reg, "RSP_Vect[%i]", RSPOpC.OP.V.vs);
	SseMoveAlignedVariableToReg(&RspRecompPos, &RSP_Vect[RSPOpC.OP.V.vs].UHW[0], Reg, x86_XMM0, SseType_QuadWord, TRUE);

	if ((RSPOpC.OP.V.element & 0xF) < 2) {
		sprintf(Reg, "RSP_Vect[%i]", RSPOpC.OP.V.vt);
		SseMoveAlignedVariableToReg(&RspRecompPos, &RSP_Vect[RSPOpC.OP.V.vt].UHW[0], Reg, x86_XMM1, SseType_QuadWord, TRUE);
	} else if ((RSPOpC.OP.V.element & 0xF) >= 8) {
		RSP_Element2Sse(x86_XMM1);
	} else {
		RSP_MultiElement2Sse(x86_XMM1);
	}

	SseMoveRegToReg(&RspRecompPos, x86_XMM2, x86_XMM0, SseType_QuadWord, TRUE);
	Sse2PmullwRegToReg(&RspRecompPos, x86_XMM2, x86_XMM1);
	SseMoveAlignedRegToVariable(&RspRecompPos, x86_XMM2, &RSP_ACCUM_LOW.UHW[0], "RSP_ACCUM_LOW", SseType_QuadWord, TRUE);
	if (writeToVectorDest) {
		sprintf(Reg, "RSP_Vect[%i]", RSPOpC.OP.V.vd);
		SseMoveAlignedRegToVariable(&RspRecompPos, x86_XMM2, &RSP_Vect[RSPOpC.OP.V.vd].UHW[0], Reg, SseType_QuadWord, TRUE);
	}

	/* Copy the signed portion */
	SseMoveRegToReg(&RspRecompPos, x86_XMM2, x86_XMM1, SseType_QuadWord, TRUE);

	/* high((u16)a * b) */
	Sse2PmulhuwRegToReg(&RspRecompPos, x86_XMM1, x86_XMM0);

	/* low((a >> 15) * b) */
	Sse2PsrawImmed(&RspRecompPos, x86_XMM2, 15);
	Sse2PmullwRegToReg(&RspRecompPos, x86_XMM2, x86_XMM0);

	/* Add them up */
	Sse2PaddwRegToReg(&RspRecompPos, x86_XMM1, x86_XMM2);

	SseMoveAlignedRegToVariable(&RspRecompPos, x86_XMM1, &RSP_ACCUM_MID.UHW[0], "RSP_ACCUM_MID", SseType_QuadWord, TRUE);
	Sse2PsrawImmed(&RspRecompPos, x86_XMM1, 15);
	SseMoveAlignedRegToVariable(&RspRecompPos, x86_XMM1, &RSP_ACCUM_HIGH.UHW[0], "RSP_ACCUM_HIGH", SseType_QuadWord, TRUE);

	return TRUE;
}

static BOOL Compile_Vector_VMUDN_AVX(BOOL writeToVectorDest) {
	char Reg[256];

	/* Do our SSE checks here */
	if (IsAvxEnabled == FALSE || IsSseEnabled == FALSE || IsSse2Enabled == FALSE)
		return FALSE;

	sprintf(Reg, "RSP_Vect[%i]", RSPOpC.OP.V.vs);
	SseMoveAlignedVariableToReg(&RspRecompPos, &RSP_Vect[RSPOpC.OP.V.vs].UHW[0], Reg, x86_XMM0, SseType_QuadWord, TRUE);

	if ((RSPOpC.OP.V.element & 0xF) < 2) {
		sprintf(Reg, "RSP_Vect[%i]", RSPOpC.OP.V.vt);
		SseMoveAlignedVariableToReg(&RspRecompPos, &RSP_Vect[RSPOpC.OP.V.vt].UHW[0], Reg, x86_XMM1, SseType_QuadWord, TRUE);
	} else if ((RSPOpC.OP.V.element & 0xF) >= 8) {
		RSP_Element2Sse(x86_XMM1);
	} else {
		RSP_MultiElement2Sse(x86_XMM1);
	}

	AvxVPMullwRegToReg128(&RspRecompPos, x86_XMM2, x86_XMM0, x86_XMM1);
	SseMoveAlignedRegToVariable(&RspRecompPos, x86_XMM2, &RSP_ACCUM_LOW.UHW[0], "RSP_ACCUM_LOW", SseType_QuadWord, TRUE);
	if (writeToVectorDest) {
		sprintf(Reg, "RSP_Vect[%i]", RSPOpC.OP.V.vd);
		SseMoveAlignedRegToVariable(&RspRecompPos, x86_XMM2, &RSP_Vect[RSPOpC.OP.V.vd].UHW[0], Reg, SseType_QuadWord, TRUE);
	}

	/* low((a >> 15) * b) */
	AvxVPSrawRegToReg128Immed(&RspRecompPos, x86_XMM2, x86_XMM1, 15);
	Sse2PmullwRegToReg(&RspRecompPos, x86_XMM2, x86_XMM0);

	/* high((u16)a * b) */
	Sse2PmulhuwRegToReg(&RspRecompPos, x86_XMM1, x86_XMM0);

	/* Add them up */
	Sse2PaddwRegToReg(&RspRecompPos, x86_XMM1, x86_XMM2);

	SseMoveAlignedRegToVariable(&RspRecompPos, x86_XMM1, &RSP_ACCUM_MID.UHW[0], "RSP_ACCUM_MID", SseType_QuadWord, TRUE);
	Sse2PsrawImmed(&RspRecompPos, x86_XMM1, 15);
	SseMoveAlignedRegToVariable(&RspRecompPos, x86_XMM1, &RSP_ACCUM_HIGH.UHW[0], "RSP_ACCUM_HIGH", SseType_QuadWord, TRUE);

	return TRUE;
}

void CompileRsp_Vector_VMUDN ( void ) {
	char Reg[256];
	int count, el, del;
	
	BOOL bOptimize = ((RSPOpC.OP.V.element & 0x0f) >= 8) ? TRUE : FALSE;
	BOOL bWriteToDest = WriteToVectorDest(RSPOpC.OP.V.vd, RspCompilePC);
	BOOL bWriteToAccum = WriteToAccum(EntireAccum, RspCompilePC);

	#ifndef CompileVmudn
	InterpreterFallback((void*)RSP_Vector_VMUDN,"RSP_Vector_VMUDN"); return;
	#endif

	RSP_CPU_Message("  %X %s",RspCompilePC,RSPOpcodeName(RSPOpC.OP.Hex,RspCompilePC));

	if (bWriteToDest == FALSE && bWriteToAccum == FALSE) {
		return;
	}

	if (bWriteToAccum == FALSE) {
		if (TRUE == Compile_Vector_VMUDN_NoAccum_SSE2())
			return;

		if (TRUE == Compile_Vector_VMUDN_MMX())
			return;
	}

	if (TRUE == Compile_Vector_VMUDN_AVX(bWriteToDest)) {
		return;
	}

	if (TRUE == Compile_Vector_VMUDN_SSE2(bWriteToDest)) {
		return;
	}

	if (bOptimize == TRUE) {
		del = (RSPOpC.OP.V.element & 0x07) ^ 7;
		sprintf(Reg, "RSP_Vect[%i].HW[%i]", RSPOpC.OP.V.vt, del);
		MoveSxVariableToX86regHalf(&RspRecompPos, &RSP_Vect[RSPOpC.OP.V.vt].HW[del], Reg, x86_EBX);
	}

	sprintf(Reg, "RSP_Vect[%i].HW[0]", RSPOpC.OP.V.vs);
	MoveOffsetToX86reg(&RspRecompPos, (DWORD)&RSP_Vect[RSPOpC.OP.V.vs].HW[0], Reg, x86_ESI);

	for (count = 0; count < 8; count++) {
		RSP_CPU_Message("     Iteration: %i", count);
		el = Indx[RSPOpC.OP.V.element].B[count];
		del = EleSpec[RSPOpC.OP.V.element].B[el];

		MoveZxX86RegPtrDispToX86RegHalf(&RspRecompPos, x86_ESI, (BYTE)(el * 2), x86_EAX);

		if (bOptimize == FALSE) {
			sprintf(Reg, "RSP_Vect[%i].HW[%i]", RSPOpC.OP.V.vt, del);
			MoveSxVariableToX86regHalf(&RspRecompPos, &RSP_Vect[RSPOpC.OP.V.vt].HW[del], Reg, x86_EBX);
		}

		imulX86reg(&RspRecompPos, x86_EBX);

		if (bWriteToDest == TRUE) {
			sprintf(Reg, "RSP_Vect[%i].HW[%i]", RSPOpC.OP.V.vd, el);
			MoveX86regHalfToVariable(&RspRecompPos, x86_EAX, &RSP_Vect[RSPOpC.OP.V.vd].HW[el], Reg);
		}

		if (bWriteToAccum == TRUE) {
			sprintf(Reg, "RSP_ACCUM_HIGH.HW[%i]", el);
			MoveX86regHalfToVariable(&RspRecompPos, x86_EDX, &RSP_ACCUM_HIGH.UHW[el], Reg);
			sprintf(Reg, "RSP_ACCUM_LOW.HW[%i]", el);
			MoveX86regHalfToVariable(&RspRecompPos, x86_EAX, &RSP_ACCUM_LOW.UHW[el], Reg);
			ShiftRightSignImmed(&RspRecompPos, x86_EAX, 16);
			sprintf(Reg, "RSP_ACCUM_MID.HW[%i]", el);
			MoveX86regHalfToVariable(&RspRecompPos, x86_EAX, &RSP_ACCUM_MID.UHW[el], Reg);
		}
	}
}

static BOOL Compile_Vector_VMUDH_MMX ( void ) {
	char Reg[256];

	/* Do our MMX checks here */
	if (IsMmxEnabled == FALSE)
		return FALSE;
	if ((RSPOpC.OP.V.element & 0x0f) >= 2 && (RSPOpC.OP.V.element & 0x0f) <= 7 && IsMmx2Enabled == FALSE)
		return FALSE;

	sprintf(Reg, "RSP_Vect[%i].HW[0]", RSPOpC.OP.V.vs);
	MmxMoveQwordVariableToReg(&RspRecompPos, x86_MM0, &RSP_Vect[RSPOpC.OP.V.vs].HW[0], Reg);
	sprintf(Reg, "RSP_Vect[%i].HW[4]", RSPOpC.OP.V.vs);
	MmxMoveQwordVariableToReg(&RspRecompPos, x86_MM1, &RSP_Vect[RSPOpC.OP.V.vs].HW[4], Reg);

	/* Registers 4 & 5 are high */
	MmxMoveRegToReg(&RspRecompPos, x86_MM4, x86_MM0);
	MmxMoveRegToReg(&RspRecompPos, x86_MM5, x86_MM1);

	if ((RSPOpC.OP.V.element & 0x0f) < 2) {
		sprintf(Reg, "RSP_Vect[%i].HW[0]", RSPOpC.OP.V.vt);
		MmxMoveQwordVariableToReg(&RspRecompPos, x86_MM2, &RSP_Vect[RSPOpC.OP.V.vt].HW[0], Reg);
		sprintf(Reg, "RSP_Vect[%i].HW[4]", RSPOpC.OP.V.vt);
		MmxMoveQwordVariableToReg(&RspRecompPos, x86_MM3, &RSP_Vect[RSPOpC.OP.V.vt].HW[4], Reg);

		MmxPmullwRegToReg(&RspRecompPos, x86_MM0, x86_MM2);
		MmxPmulhwRegToReg(&RspRecompPos, x86_MM4, x86_MM2);
		MmxPmullwRegToReg(&RspRecompPos, x86_MM1, x86_MM3);
		MmxPmulhwRegToReg(&RspRecompPos, x86_MM5, x86_MM3);
	} else if ((RSPOpC.OP.V.element & 0x0f) >= 8) {
		RSP_Element2Mmx(x86_MM2);

		MmxPmullwRegToReg(&RspRecompPos, x86_MM0, x86_MM2);
		MmxPmulhwRegToReg(&RspRecompPos, x86_MM4, x86_MM2);
		MmxPmullwRegToReg(&RspRecompPos, x86_MM1, x86_MM2);
		MmxPmulhwRegToReg(&RspRecompPos, x86_MM5, x86_MM2);
	} else {
		RSP_MultiElement2Mmx(x86_MM2, x86_MM3);

		MmxPmullwRegToReg(&RspRecompPos, x86_MM0, x86_MM2);
		MmxPmulhwRegToReg(&RspRecompPos, x86_MM4, x86_MM2);
		MmxPmullwRegToReg(&RspRecompPos, x86_MM1, x86_MM3);
		MmxPmulhwRegToReg(&RspRecompPos, x86_MM5, x86_MM3);
	}

	/* 0 & 1 are low, 4 & 5 are high */
	MmxMoveRegToReg(&RspRecompPos, x86_MM6, x86_MM0);
	MmxMoveRegToReg(&RspRecompPos, x86_MM7, x86_MM1);

	MmxUnpackLowWord(&RspRecompPos, x86_MM0, x86_MM4);
	MmxUnpackHighWord(&RspRecompPos, x86_MM6, x86_MM4);
	MmxUnpackLowWord(&RspRecompPos, x86_MM1, x86_MM5);		
	MmxUnpackHighWord(&RspRecompPos, x86_MM7, x86_MM5);

	/* Integrate copies */
	MmxPackSignedDwords(&RspRecompPos, x86_MM0, x86_MM6);
	MmxPackSignedDwords(&RspRecompPos, x86_MM1, x86_MM7);
	
	sprintf(Reg, "RSP_Vect[%i].HW[0]", RSPOpC.OP.V.vd);
	MmxMoveQwordRegToVariable(&RspRecompPos, x86_MM0, &RSP_Vect[RSPOpC.OP.V.vd].HW[0], Reg);
	sprintf(Reg, "RSP_Vect[%i].HW[4]", RSPOpC.OP.V.vd);
	MmxMoveQwordRegToVariable(&RspRecompPos, x86_MM1, &RSP_Vect[RSPOpC.OP.V.vd].HW[4], Reg);

	if (IsNextRspInstructionMmx(RspCompilePC) == FALSE)
		MmxEmptyMultimediaState(&RspRecompPos);

	return TRUE;
}

static BOOL Compile_Vector_VMUDH_SSE2(BOOL writeToVectorDest, BOOL writeToAccum) {
	char Reg[256];

	/* Do our SSE checks here */
	if (IsSseEnabled == FALSE || IsSse2Enabled == FALSE)
		return FALSE;

	sprintf(Reg, "RSP_Vect[%i]", RSPOpC.OP.V.vs);
	SseMoveAlignedVariableToReg(&RspRecompPos, &RSP_Vect[RSPOpC.OP.V.vs].HW[0], Reg, x86_XMM0, SseType_QuadWord, TRUE);

	/* Register 2 is high */
	SseMoveRegToReg(&RspRecompPos, x86_XMM2, x86_XMM0, SseType_QuadWord, TRUE);

	if ((RSPOpC.OP.V.element & 0x0f) < 2) {
		sprintf(Reg, "RSP_Vect[%i]", RSPOpC.OP.V.vt);
		SseMoveAlignedVariableToReg(&RspRecompPos, &RSP_Vect[RSPOpC.OP.V.vt].HW[0], Reg, x86_XMM1, SseType_QuadWord, TRUE);
	} else if ((RSPOpC.OP.V.element & 0x0f) >= 8) {
		RSP_Element2Sse(x86_XMM1);
	} else {
		RSP_MultiElement2Sse(x86_XMM1);
	}

	Sse2PmullwRegToReg(&RspRecompPos, x86_XMM0, x86_XMM1);
	Sse2PmulhwRegToReg(&RspRecompPos, x86_XMM2, x86_XMM1);

	if (writeToAccum) {
		SseMoveAlignedRegToVariable(&RspRecompPos, x86_XMM0, &RSP_ACCUM_MID.UHW[0], "RSP_ACCUM_MID", SseType_QuadWord, TRUE);
		SseMoveAlignedRegToVariable(&RspRecompPos, x86_XMM2, &RSP_ACCUM_HIGH.UHW[0], "RSP_ACCUM_HIGH", SseType_QuadWord, TRUE);
		Sse2PxorRegToReg(&RspRecompPos, x86_XMM7, x86_XMM7);
		SseMoveAlignedRegToVariable(&RspRecompPos, x86_XMM7, &RSP_ACCUM_LOW.UHW[0], "RSP_ACCUM_LOW", SseType_QuadWord, TRUE);
	}

	if (writeToVectorDest) {
		/* 0 is low, 2 is high */
		SseMoveRegToReg(&RspRecompPos, x86_XMM3, x86_XMM0, SseType_QuadWord, TRUE);

		Sse2PunpckLowWordsRegToReg(&RspRecompPos, x86_XMM0, x86_XMM2);
		Sse2PunpckHighWordsRegToReg(&RspRecompPos, x86_XMM3, x86_XMM2);

		/* Integrate copies */
		Sse2PackSignedDWordRegToWordReg(&RspRecompPos, x86_XMM0, x86_XMM3);

		sprintf(Reg, "RSP_Vect[%i]", RSPOpC.OP.V.vd);
		SseMoveAlignedRegToVariable(&RspRecompPos, x86_XMM0, &RSP_Vect[RSPOpC.OP.V.vd].HW[0], Reg, SseType_QuadWord, TRUE);
	}

	return TRUE;
}

static BOOL Compile_Vector_VMUDH_NoAccum_AVX() {
	char Reg[256];

	/* Do our AVX checks here */
	if (IsAvxEnabled == FALSE  || IsAvx2Enabled == FALSE || IsSseEnabled == FALSE)
		return FALSE;

	sprintf(Reg, "RSP_Vect[%i]", RSPOpC.OP.V.vs);
	AvxVPMovesxWordVariableToDWordReg256(&RspRecompPos, x86_YMM0, &RSP_Vect[RSPOpC.OP.V.vs].HW[0], Reg);

	if ((RSPOpC.OP.V.element & 0x0f) < 2) {
		sprintf(Reg, "RSP_Vect[%i]", RSPOpC.OP.V.vt);
		AvxVPMovesxWordVariableToDWordReg256(&RspRecompPos, x86_YMM1, &RSP_Vect[RSPOpC.OP.V.vt].HW[0], Reg);
	}
	else if ((RSPOpC.OP.V.element & 0x0f) >= 8) {
		RSP_Element2Sse(x86_XMM1);
		AvxVPMovesxWordReg128ToDwordReg256(&RspRecompPos, x86_YMM1, x86_XMM1);
	}
	else {
		RSP_MultiElement2Sse(x86_XMM1);
		AvxVPMovesxWordReg128ToDwordReg256(&RspRecompPos, x86_YMM1, x86_XMM1);
	}

	AvxVPMulldRegToReg256(&RspRecompPos, x86_YMM0, x86_YMM0, x86_YMM1);

	AvxVExtracti128RegToReg(&RspRecompPos, x86_XMM1, x86_YMM0, TRUE);
	AvxVPackSignedDWordRegToWordReg128(&RspRecompPos, x86_XMM0, x86_XMM0, x86_XMM1);

	sprintf(Reg, "RSP_Vect[%i]", RSPOpC.OP.V.vd);
	SseMoveAlignedRegToVariable(&RspRecompPos, x86_XMM0, &RSP_Vect[RSPOpC.OP.V.vd].HW[0], Reg, SseType_QuadWord, IsSse2Enabled);

	return TRUE;
}

static BOOL Compile_Vector_VMUDH_AVX(BOOL writeToVectorDest) {
	char Reg[256];

	/* Do our SSE checks here */
	if (IsSseEnabled == FALSE || IsSse2Enabled == FALSE)
		return FALSE;

	sprintf(Reg, "RSP_Vect[%i]", RSPOpC.OP.V.vs);
	SseMoveAlignedVariableToReg(&RspRecompPos, &RSP_Vect[RSPOpC.OP.V.vs].HW[0], Reg, x86_XMM0, SseType_QuadWord, TRUE);

	if ((RSPOpC.OP.V.element & 0x0f) < 2) {
		sprintf(Reg, "RSP_Vect[%i]", RSPOpC.OP.V.vt);
		SseMoveAlignedVariableToReg(&RspRecompPos, &RSP_Vect[RSPOpC.OP.V.vt].HW[0], Reg, x86_XMM1, SseType_QuadWord, TRUE);
	}
	else if ((RSPOpC.OP.V.element & 0x0f) >= 8) {
		RSP_Element2Sse(x86_XMM1);
	}
	else {
		RSP_MultiElement2Sse(x86_XMM1);
	}

	AvxVPMullwRegToReg128(&RspRecompPos, x86_XMM2, x86_XMM0, x86_XMM1);
	Sse2PmulhwRegToReg(&RspRecompPos, x86_XMM1, x86_XMM0);

	SseMoveAlignedRegToVariable(&RspRecompPos, x86_XMM2, &RSP_ACCUM_MID.UHW[0], "RSP_ACCUM_MID", SseType_QuadWord, TRUE);
	SseMoveAlignedRegToVariable(&RspRecompPos, x86_XMM1, &RSP_ACCUM_HIGH.UHW[0], "RSP_ACCUM_HIGH", SseType_QuadWord, TRUE);
	Sse2PxorRegToReg(&RspRecompPos, x86_XMM7, x86_XMM7);
	SseMoveAlignedRegToVariable(&RspRecompPos, x86_XMM7, &RSP_ACCUM_LOW.UHW[0], "RSP_ACCUM_LOW", SseType_QuadWord, TRUE);

	if (writeToVectorDest) {
		AvxVPunpckLowWordsRegToReg128(&RspRecompPos, x86_XMM0, x86_XMM2, x86_XMM1);
		AvxVPunpckHighWordsRegToReg128(&RspRecompPos, x86_XMM1, x86_XMM2, x86_XMM1);

		/* Integrate copies */
		Sse2PackSignedDWordRegToWordReg(&RspRecompPos, x86_XMM0, x86_XMM1);

		sprintf(Reg, "RSP_Vect[%i]", RSPOpC.OP.V.vd);
		SseMoveAlignedRegToVariable(&RspRecompPos, x86_XMM0, &RSP_Vect[RSPOpC.OP.V.vd].HW[0], Reg, SseType_QuadWord, TRUE);
	}

	return TRUE;
}

void CompileRsp_Vector_VMUDH ( void ) {
	char Reg[256];
	int count, el, del;

	BOOL bOptimize = ((RSPOpC.OP.V.element & 0x0f) >= 8) ? TRUE : FALSE;
	BOOL bWriteToDest = WriteToVectorDest(RSPOpC.OP.V.vd, RspCompilePC);
	BOOL bWriteToAccum = WriteToAccum(EntireAccum, RspCompilePC);

	#ifndef CompileVmudh
	InterpreterFallback((void*)RSP_Vector_VMUDH,"RSP_Vector_VMUDH"); return;
	#endif

	RSP_CPU_Message("  %X %s",RspCompilePC,RSPOpcodeName(RSPOpC.OP.Hex,RspCompilePC));

	if (bWriteToDest == FALSE && bWriteToAccum == FALSE) {
		return;
	}
	
	if (bWriteToAccum == FALSE) {
		if (TRUE == Compile_Vector_VMUDH_NoAccum_AVX())
			return;
	}

	if (TRUE == Compile_Vector_VMUDH_AVX(bWriteToDest))
		return;

	if (TRUE == Compile_Vector_VMUDH_SSE2(bWriteToDest, bWriteToAccum))
		return;

	if (bWriteToAccum == FALSE) {
		if (TRUE == Compile_Vector_VMUDH_MMX())
			return;
	}

	if (bWriteToDest == FALSE && bOptimize == TRUE) {
		Push(&RspRecompPos, x86_EBP);

		/* Load source */
		del = (RSPOpC.OP.V.element & 0x07) ^ 7;
		sprintf(Reg, "RSP_Vect[%i].HW[%i]", RSPOpC.OP.V.vt, del);
		MoveSxVariableToX86regHalf(&RspRecompPos, &RSP_Vect[RSPOpC.OP.V.vt].HW[del], Reg, x86_EBX);

		/* 
		 * Pipe lined segment 0
		 */
		
		sprintf(Reg, "RSP_Vect[%i].HW[0]", RSPOpC.OP.V.vs);
		MoveOffsetToX86reg(&RspRecompPos, (DWORD)&RSP_Vect[RSPOpC.OP.V.vs].HW[0], Reg, x86_EBP);

		MoveSxX86RegPtrDispToX86RegHalf(&RspRecompPos, x86_EBP, 0, x86_EAX);
		MoveSxX86RegPtrDispToX86RegHalf(&RspRecompPos, x86_EBP, 2, x86_ECX);
		MoveSxX86RegPtrDispToX86RegHalf(&RspRecompPos, x86_EBP, 4, x86_EDI);
		MoveSxX86RegPtrDispToX86RegHalf(&RspRecompPos, x86_EBP, 6, x86_ESI);

		ImulX86RegToX86Reg(&RspRecompPos, x86_EAX, x86_EBX);
		ImulX86RegToX86Reg(&RspRecompPos, x86_ECX, x86_EBX);
		ImulX86RegToX86Reg(&RspRecompPos, x86_EDI, x86_EBX);
		ImulX86RegToX86Reg(&RspRecompPos, x86_ESI, x86_EBX);
		XorX86RegToX86Reg(&RspRecompPos, x86_EDX, x86_EDX);

		MoveOffsetToX86reg(&RspRecompPos, (DWORD)&RSP_ACCUM_LOW.UHW[0], "RSP_ACCUM_LOW.UHW[0]", x86_EBP);
		MoveX86regHalfToX86regPointerDisp(&RspRecompPos, x86_EDX, x86_EBP, 0);
		MoveX86regHalfToX86regPointerDisp(&RspRecompPos, x86_EDX, x86_EBP, 2);
		MoveX86regHalfToX86regPointerDisp(&RspRecompPos, x86_EDX, x86_EBP, 4);
		MoveX86regHalfToX86regPointerDisp(&RspRecompPos, x86_EDX, x86_EBP, 6);

		MoveOffsetToX86reg(&RspRecompPos, (DWORD)&RSP_ACCUM_MID.UHW[0], "RSP_ACCUM_MID.UHW[0]", x86_EBP);
		MoveX86regHalfToX86regPointerDisp(&RspRecompPos, x86_EAX, x86_EBP, 0);
		MoveX86regHalfToX86regPointerDisp(&RspRecompPos, x86_ECX, x86_EBP, 2);
		MoveX86regHalfToX86regPointerDisp(&RspRecompPos, x86_EDI, x86_EBP, 4);
		MoveX86regHalfToX86regPointerDisp(&RspRecompPos, x86_ESI, x86_EBP, 6);

		MoveOffsetToX86reg(&RspRecompPos, (DWORD)&RSP_ACCUM_HIGH.UHW[0], "RSP_ACCUM_HIGH.UHW[0]", x86_EBP);
		ShiftRightSignImmed(&RspRecompPos, x86_EAX, 16);
		ShiftRightSignImmed(&RspRecompPos, x86_ECX, 16);
		ShiftRightSignImmed(&RspRecompPos, x86_EDI, 16);
		ShiftRightSignImmed(&RspRecompPos, x86_ESI, 16);
		MoveX86regHalfToX86regPointerDisp(&RspRecompPos, x86_EAX, x86_EBP, 0);
		MoveX86regHalfToX86regPointerDisp(&RspRecompPos, x86_ECX, x86_EBP, 2);
		MoveX86regHalfToX86regPointerDisp(&RspRecompPos, x86_EDI, x86_EBP, 4);
		MoveX86regHalfToX86regPointerDisp(&RspRecompPos, x86_ESI, x86_EBP, 6);

		/* 
		 * Pipe lined segment 1
		 */

		sprintf(Reg, "RSP_Vect[%i].HW[0]", RSPOpC.OP.V.vs);
		MoveOffsetToX86reg(&RspRecompPos, (DWORD)&RSP_Vect[RSPOpC.OP.V.vs].HW[0], Reg, x86_EBP);

		MoveSxX86RegPtrDispToX86RegHalf(&RspRecompPos, x86_EBP,  8, x86_EAX);
		MoveSxX86RegPtrDispToX86RegHalf(&RspRecompPos, x86_EBP, 10, x86_ECX);
		MoveSxX86RegPtrDispToX86RegHalf(&RspRecompPos, x86_EBP, 12, x86_EDI);
		MoveSxX86RegPtrDispToX86RegHalf(&RspRecompPos, x86_EBP, 14, x86_ESI);

		ImulX86RegToX86Reg(&RspRecompPos, x86_EAX, x86_EBX);
		ImulX86RegToX86Reg(&RspRecompPos, x86_ECX, x86_EBX);
		ImulX86RegToX86Reg(&RspRecompPos, x86_EDI, x86_EBX);
		ImulX86RegToX86Reg(&RspRecompPos, x86_ESI, x86_EBX);
		XorX86RegToX86Reg(&RspRecompPos, x86_EDX, x86_EDX);

		MoveOffsetToX86reg(&RspRecompPos, (DWORD)&RSP_ACCUM_LOW.UHW[0], "RSP_ACCUM_LOW.UHW[0]", x86_EBP);
		MoveX86regHalfToX86regPointerDisp(&RspRecompPos, x86_EDX, x86_EBP, 8);
		MoveX86regHalfToX86regPointerDisp(&RspRecompPos, x86_EDX, x86_EBP, 10);
		MoveX86regHalfToX86regPointerDisp(&RspRecompPos, x86_EDX, x86_EBP, 12);
		MoveX86regHalfToX86regPointerDisp(&RspRecompPos, x86_EDX, x86_EBP, 14);

		MoveOffsetToX86reg(&RspRecompPos, (DWORD)&RSP_ACCUM_MID.UHW[0], "RSP_ACCUM_MID.UHW[0]", x86_EBP);
		MoveX86regHalfToX86regPointerDisp(&RspRecompPos, x86_EAX, x86_EBP, 8);
		MoveX86regHalfToX86regPointerDisp(&RspRecompPos, x86_ECX, x86_EBP, 10);
		MoveX86regHalfToX86regPointerDisp(&RspRecompPos, x86_EDI, x86_EBP, 12);
		MoveX86regHalfToX86regPointerDisp(&RspRecompPos, x86_ESI, x86_EBP, 14);

		MoveOffsetToX86reg(&RspRecompPos, (DWORD)&RSP_ACCUM_HIGH.UHW[0], "RSP_ACCUM_HIGH.UHW[0]", x86_EBP);
		ShiftRightSignImmed(&RspRecompPos, x86_EAX, 16);
		ShiftRightSignImmed(&RspRecompPos, x86_ECX, 16);
		ShiftRightSignImmed(&RspRecompPos, x86_EDI, 16);
		ShiftRightSignImmed(&RspRecompPos, x86_ESI, 16);
		MoveX86regHalfToX86regPointerDisp(&RspRecompPos, x86_EAX, x86_EBP, 8);
		MoveX86regHalfToX86regPointerDisp(&RspRecompPos, x86_ECX, x86_EBP, 10);
		MoveX86regHalfToX86regPointerDisp(&RspRecompPos, x86_EDI, x86_EBP, 12);
		MoveX86regHalfToX86regPointerDisp(&RspRecompPos, x86_ESI, x86_EBP, 14);

		Pop(&RspRecompPos, x86_EBP);
	} else {
		if (bOptimize == TRUE) {
			del = (RSPOpC.OP.V.element & 0x07) ^ 7;
			sprintf(Reg, "RSP_Vect[%i].HW[%i]", RSPOpC.OP.V.vt, del);
			MoveSxVariableToX86regHalf(&RspRecompPos, &RSP_Vect[RSPOpC.OP.V.vt].HW[del], Reg, x86_EBX);
		}
		if (bWriteToDest == TRUE) {
			/*
			 * Prepare for conditional moves
			 */
			MoveConstToX86reg(&RspRecompPos, 0x00007fff, x86_ESI);
			MoveConstToX86reg(&RspRecompPos, 0xFFFF8000, x86_EDI);
		}

		for (count = 0; count < 8; count++) {
			RSP_CPU_Message("     Iteration: %i", count);
			el = Indx[RSPOpC.OP.V.element].B[count];
			del = EleSpec[RSPOpC.OP.V.element].B[el];
		
			sprintf(Reg, "RSP_Vect[%i].HW[%i]", RSPOpC.OP.V.vs, el);
			MoveSxVariableToX86regHalf(&RspRecompPos, &RSP_Vect[RSPOpC.OP.V.vs].HW[el], Reg, x86_EAX);

			if (bOptimize == FALSE) {
				sprintf(Reg, "RSP_Vect[%i].HW[%i]", RSPOpC.OP.V.vt, del);
				MoveSxVariableToX86regHalf(&RspRecompPos, &RSP_Vect[RSPOpC.OP.V.vt].HW[del], Reg, x86_EBX);
			}
			imulX86reg(&RspRecompPos, x86_EBX);
			
			if (bWriteToAccum == TRUE) {
				MoveX86regHalfToVariable(&RspRecompPos, x86_EAX, &RSP_ACCUM_MID.UHW[el], "RSP_ACCUM_MID.UHW[el]");
				int highReg = x86_EAX;
				if (bWriteToDest == TRUE) {
					highReg = x86_ECX;
					MoveX86RegToX86Reg(&RspRecompPos, x86_EAX, x86_ECX);
				}
				ShiftRightSignImmed(&RspRecompPos, highReg, 16);
				MoveX86regHalfToVariable(&RspRecompPos, highReg, &RSP_ACCUM_HIGH.UHW[el], "RSP_ACCUM_HIGH.UHW[el]");
				MoveConstHalfToVariable(&RspRecompPos, 0, &RSP_ACCUM_LOW.UHW[el], "RSP_ACCUM_LOW.UHW[el]");
			}
			
			if (bWriteToDest == TRUE) {
				CompX86RegToX86Reg(&RspRecompPos, x86_EAX, x86_ESI);
				CondMoveGreater(&RspRecompPos, x86_EAX, x86_ESI);
				CompX86RegToX86Reg(&RspRecompPos, x86_EAX, x86_EDI);
				CondMoveLess(&RspRecompPos, x86_EAX, x86_EDI);

				sprintf(Reg, "RSP_Vect[%i].HW[%i]", RSPOpC.OP.V.vd, el);
				MoveX86regHalfToVariable(&RspRecompPos, x86_EAX, &RSP_Vect[RSPOpC.OP.V.vd].HW[el], Reg);
			}
		}
	}
}

static BOOL Compile_Vector_VMACF_SSE2(BOOL writeToVectorDest, BOOL writeToAccum) {
	char Reg[256];

	/* Do our SSE checks here */
	if (IsSseEnabled == FALSE || IsSse2Enabled == FALSE)
		return FALSE;

	// load vs and vt
	sprintf(Reg, "RSP_Vect[%i]", RSPOpC.OP.V.vs);
	SseMoveAlignedVariableToReg(&RspRecompPos, &RSP_Vect[RSPOpC.OP.V.vs].UHW[0], Reg, x86_XMM1, SseType_QuadWord, TRUE);

	if ((RSPOpC.OP.V.element & 0xF) < 2) {
		sprintf(Reg, "RSP_Vect[%i]", RSPOpC.OP.V.vt);
		SseMoveAlignedVariableToReg(&RspRecompPos, &RSP_Vect[RSPOpC.OP.V.vt].UHW[0], Reg, x86_XMM2, SseType_QuadWord, TRUE);
	}
	else if ((RSPOpC.OP.V.element & 0xF) >= 8) {
		RSP_Element2Sse(x86_XMM2);
	}
	else {
		RSP_MultiElement2Sse(x86_XMM2);
	}

	// 3 is low vs * vt
	SseMoveRegToReg(&RspRecompPos, x86_XMM3, x86_XMM2, SseType_QuadWord, TRUE);
	Sse2PmullwRegToReg(&RspRecompPos, x86_XMM3, x86_XMM1);

	// 4 is low (vs * vt) << 1
	SseMoveRegToReg(&RspRecompPos, x86_XMM4, x86_XMM3, SseType_QuadWord, TRUE);
	Sse2PsllwImmed(&RspRecompPos, x86_XMM4, 1);

	// 5 is unsigned low sum and 6 is signed low sum
	SseMoveAlignedVariableToReg(&RspRecompPos, &RSP_ACCUM_LOW.UHW[0], "RSP_ACCUM_LOW", x86_XMM6, SseType_QuadWord, TRUE);
	SseMoveRegToReg(&RspRecompPos, x86_XMM5, x86_XMM6, SseType_QuadWord, TRUE);
	Sse2PaddwRegToReg(&RspRecompPos, x86_XMM5, x86_XMM4);
	Sse2PadduswRegToReg(&RspRecompPos, x86_XMM6, x86_XMM4);
	if (writeToAccum) {
		SseMoveAlignedRegToVariable(&RspRecompPos, x86_XMM5, &RSP_ACCUM_LOW.UHW[0], "RSP_ACCUM_LOW", SseType_QuadWord, TRUE);
	}

	// compute carry in 5
	Sse2CompareEqualWordRegToReg(&RspRecompPos, x86_XMM5, x86_XMM6);
	Sse2CompareEqualDWordRegToReg(&RspRecompPos, x86_XMM6, x86_XMM6);
	Sse2PandnRegToReg(&RspRecompPos, x86_XMM5, x86_XMM6);
	Sse2PsrlwImmed(&RspRecompPos, x86_XMM5, 15);

	// 2 is high vs * vt
	Sse2PmulhwRegToReg(&RspRecompPos, x86_XMM2, x86_XMM1);

	// 1 and 2 are the high part shifted to add to accum
	SseMoveRegToReg(&RspRecompPos, x86_XMM1, x86_XMM2, SseType_QuadWord, TRUE);
	Sse2PsrlwImmed(&RspRecompPos, x86_XMM3, 15);
	Sse2PsllwImmed(&RspRecompPos, x86_XMM1, 1);
	Sse2PorRegToReg(&RspRecompPos, x86_XMM1, x86_XMM3);

	Sse2PsrawImmed(&RspRecompPos, x86_XMM2, 15);

	// 0 and 1 are merged 32 bits of the high/mid parts of the mult result
	SseMoveRegToReg(&RspRecompPos, x86_XMM0, x86_XMM1, SseType_QuadWord, TRUE);
	Sse2PunpckLowWordsRegToReg(&RspRecompPos, x86_XMM0, x86_XMM2);
	Sse2PunpckHighWordsRegToReg(&RspRecompPos, x86_XMM1, x86_XMM2);

	// 4 and 5 are sign extended carry
	Sse2MoveSxWordRegToDWordReg(&RspRecompPos, x86_XMM4, x86_XMM5, IsSse41Enabled);
	Sse2ShuffleDWordsRegToReg(&RspRecompPos, x86_XMM5, x86_XMM5, _MMX_SHUFFLE(3, 2, 3, 2));
	Sse2MoveSxWordRegToDWordReg(&RspRecompPos, x86_XMM5, x86_XMM5, IsSse41Enabled);

	// add carry
	Sse2PadddRegToReg(&RspRecompPos, x86_XMM0, x86_XMM4);
	Sse2PadddRegToReg(&RspRecompPos, x86_XMM1, x86_XMM5);

	// load in 4 and 5 high|mid accum
	SseMoveAlignedVariableToReg(&RspRecompPos, &RSP_ACCUM_HIGH.UHW[0], "RSP_ACCUM_HIGH", x86_XMM6, SseType_QuadWord, TRUE);
	SseMoveAlignedVariableToReg(&RspRecompPos, &RSP_ACCUM_MID.UHW[0], "RSP_ACCUM_MID", x86_XMM4, SseType_QuadWord, TRUE);
	SseMoveRegToReg(&RspRecompPos, x86_XMM5, x86_XMM4, SseType_QuadWord, TRUE);
	Sse2PunpckLowWordsRegToReg(&RspRecompPos, x86_XMM4, x86_XMM6);
	Sse2PunpckHighWordsRegToReg(&RspRecompPos, x86_XMM5, x86_XMM6);

	// add accum
	Sse2PadddRegToReg(&RspRecompPos, x86_XMM0, x86_XMM4);
	Sse2PadddRegToReg(&RspRecompPos, x86_XMM1, x86_XMM5);

	int destAccumReg = x86_XMM0;

	if (writeToVectorDest) {
		if (writeToAccum) {
			SseMoveRegToReg(&RspRecompPos, x86_XMM2, x86_XMM0, SseType_QuadWord, TRUE);
			destAccumReg = x86_XMM2;
		}
		Sse2PackSignedDWordRegToWordReg(&RspRecompPos, x86_XMM0, x86_XMM1);
		sprintf(Reg, "RSP_Vect[%i]", RSPOpC.OP.V.vd);
		SseMoveAlignedRegToVariable(&RspRecompPos, x86_XMM0, &RSP_Vect[RSPOpC.OP.V.vd].HW[0], Reg, SseType_QuadWord, TRUE);
	}

	if (writeToAccum) {
		SseMoveRegToReg(&RspRecompPos, x86_XMM3, destAccumReg, SseType_QuadWord, TRUE);
		SseMoveRegToReg(&RspRecompPos, x86_XMM4, x86_XMM1, SseType_QuadWord, TRUE);
		Sse2PsradImmed(&RspRecompPos, x86_XMM3, 16);
		Sse2PsradImmed(&RspRecompPos, x86_XMM4, 16);
		Sse2PackSignedDWordRegToWordReg(&RspRecompPos, x86_XMM3, x86_XMM4);
		SseMoveAlignedRegToVariable(&RspRecompPos, x86_XMM3, &RSP_ACCUM_HIGH.UHW[0], "RSP_ACCUM_HIGH", SseType_QuadWord, TRUE);

		Sse2PslldImmed(&RspRecompPos, destAccumReg, 16);
		Sse2PslldImmed(&RspRecompPos, x86_XMM1, 16);
		Sse2PsradImmed(&RspRecompPos, destAccumReg, 16);
		Sse2PsradImmed(&RspRecompPos, x86_XMM1, 16);
		Sse2PackSignedDWordRegToWordReg(&RspRecompPos, destAccumReg, x86_XMM1);
		SseMoveAlignedRegToVariable(&RspRecompPos, destAccumReg, &RSP_ACCUM_MID.UHW[0], "RSP_ACCUM_MID", SseType_QuadWord, TRUE);
	}

	return TRUE;
}

static BOOL Compile_Vector_VMACF_AVX(BOOL writeToVectorDest, BOOL writeToAccum) {
	char Reg[256];

	/* Do our AVX checks here */
	if (IsAvxEnabled == FALSE || IsAvx2Enabled == FALSE || IsSseEnabled == FALSE || IsSse2Enabled == FALSE)
		return FALSE;

	// load vs and vt
	sprintf(Reg, "RSP_Vect[%i]", RSPOpC.OP.V.vs);
	SseMoveAlignedVariableToReg(&RspRecompPos, &RSP_Vect[RSPOpC.OP.V.vs].UHW[0], Reg, x86_XMM1, SseType_QuadWord, TRUE);

	if ((RSPOpC.OP.V.element & 0xF) < 2) {
		sprintf(Reg, "RSP_Vect[%i]", RSPOpC.OP.V.vt);
		SseMoveAlignedVariableToReg(&RspRecompPos, &RSP_Vect[RSPOpC.OP.V.vt].UHW[0], Reg, x86_XMM2, SseType_QuadWord, TRUE);
	}
	else if ((RSPOpC.OP.V.element & 0xF) >= 8) {
		RSP_Element2Sse(x86_XMM2);
	}
	else {
		RSP_MultiElement2Sse(x86_XMM2);
	}

	// 3 is low vs * vt
	AvxVPMullwRegToReg128(&RspRecompPos, x86_XMM3, x86_XMM1, x86_XMM2);

	// 4 is low (vs * vt) << 1
	AvxVPSllwRegToReg128Immed(&RspRecompPos, x86_XMM4, x86_XMM3, 1);

	// 5 is unsigned low sum and 6 is signed low sum
	SseMoveAlignedVariableToReg(&RspRecompPos, &RSP_ACCUM_LOW.UHW[0], "RSP_ACCUM_LOW", x86_XMM6, SseType_QuadWord, TRUE);
	AvxVPAddwRegToReg128(&RspRecompPos, x86_XMM5, x86_XMM6, x86_XMM4);
	Sse2PadduswRegToReg(&RspRecompPos, x86_XMM6, x86_XMM4);
	if (writeToAccum) {
		SseMoveAlignedRegToVariable(&RspRecompPos, x86_XMM5, &RSP_ACCUM_LOW.UHW[0], "RSP_ACCUM_LOW", SseType_QuadWord, TRUE);
	}

	// compute carry in 5
	Sse2CompareEqualWordRegToReg(&RspRecompPos, x86_XMM5, x86_XMM6);
	Sse2CompareEqualDWordRegToReg(&RspRecompPos, x86_XMM6, x86_XMM6);
	Sse2PandnRegToReg(&RspRecompPos, x86_XMM5, x86_XMM6);
	Sse2PsrlwImmed(&RspRecompPos, x86_XMM5, 15);

	// 2 is high vs * vt
	Sse2PmulhwRegToReg(&RspRecompPos, x86_XMM2, x86_XMM1);

	// 0 is the high part shifted to add to accum
	AvxVPMovesxWordReg128ToDwordReg256(&RspRecompPos, x86_YMM0, x86_XMM2);
	AvxVPSlldRegToReg256Immed(&RspRecompPos, x86_YMM0, x86_YMM0, 1);
	Sse2PsrlwImmed(&RspRecompPos, x86_XMM3, 15);
	AvxVPMovesxWordReg128ToDwordReg256(&RspRecompPos, x86_YMM3, x86_XMM3);
	AvxVPorRegToReg256(&RspRecompPos, x86_YMM0, x86_YMM0, x86_YMM3);

	// 4 is sign extended carry
	AvxVPMovesxWordReg128ToDwordReg256(&RspRecompPos, x86_YMM4, x86_XMM5);

	// add carry
	AvxVPAdddRegToReg256(&RspRecompPos, x86_YMM0, x86_YMM0, x86_YMM4);

	// load in 4 high|mid accum
	SseMoveAlignedVariableToReg(&RspRecompPos, &RSP_ACCUM_HIGH.UHW[0], "RSP_ACCUM_HIGH", x86_XMM6, SseType_QuadWord, TRUE);
	SseMoveAlignedVariableToReg(&RspRecompPos, &RSP_ACCUM_MID.UHW[0], "RSP_ACCUM_MID", x86_XMM4, SseType_QuadWord, TRUE);
	AvxVPunpckHighWordsRegToReg256(&RspRecompPos, x86_YMM5, x86_YMM4, x86_YMM6);
	AvxVPunpckLowWordsRegToReg256(&RspRecompPos, x86_YMM4, x86_YMM4, x86_YMM6);
	AvxVInserti128RegToReg(&RspRecompPos, x86_YMM4, x86_YMM4, x86_XMM5, TRUE);

	// add accum
	AvxVPAdddRegToReg256(&RspRecompPos, x86_YMM0, x86_YMM0, x86_YMM4);

	if (writeToVectorDest) {
		AvxVExtracti128RegToReg(&RspRecompPos, x86_XMM2, x86_YMM0, TRUE);
		AvxVPackSignedDWordRegToWordReg128(&RspRecompPos, x86_XMM2, x86_XMM0, x86_XMM2);
		sprintf(Reg, "RSP_Vect[%i]", RSPOpC.OP.V.vd);
		SseMoveAlignedRegToVariable(&RspRecompPos, x86_XMM2, &RSP_Vect[RSPOpC.OP.V.vd].HW[0], Reg, SseType_QuadWord, TRUE);
	}

	if (writeToAccum) {
		AvxVPSradRegToReg256Immed(&RspRecompPos, x86_YMM2, x86_YMM0, 16);
		AvxVExtracti128RegToReg(&RspRecompPos, x86_XMM3, x86_YMM2, TRUE);
		AvxVPackSignedDWordRegToWordReg128(&RspRecompPos, x86_XMM3, x86_XMM2, x86_XMM3);
		SseMoveAlignedRegToVariable(&RspRecompPos, x86_XMM3, &RSP_ACCUM_HIGH.UHW[0], "RSP_ACCUM_HIGH", SseType_QuadWord, TRUE);

		AvxVPSlldRegToReg256Immed(&RspRecompPos, x86_YMM2, x86_YMM0, 16);
		AvxVPSradRegToReg256Immed(&RspRecompPos, x86_YMM2, x86_YMM2, 16);
		AvxVExtracti128RegToReg(&RspRecompPos, x86_XMM3, x86_YMM2, TRUE);
		AvxVPackSignedDWordRegToWordReg128(&RspRecompPos, x86_XMM3, x86_XMM2, x86_XMM3);
		SseMoveAlignedRegToVariable(&RspRecompPos, x86_XMM3, &RSP_ACCUM_MID.UHW[0], "RSP_ACCUM_MID", SseType_QuadWord, TRUE);
	}

	return TRUE;
}

void CompileRsp_Vector_VMACF ( void ) {
	char Reg[256];
	int count, el, del;

	BOOL bOptimize = ((RSPOpC.OP.V.element & 0x0f) >= 8) ? TRUE : FALSE;
	BOOL bWriteToDest = WriteToVectorDest(RSPOpC.OP.V.vd, RspCompilePC);
	BOOL bWriteToAccum = WriteToAccum(EntireAccum, RspCompilePC);

	#ifndef CompileVmacf
	InterpreterFallback((void*)RSP_Vector_VMACF,"RSP_Vector_VMACF"); return;
	#endif

	RSP_CPU_Message("  %X %s",RspCompilePC,RSPOpcodeName(RSPOpC.OP.Hex,RspCompilePC));

	if (bWriteToDest == FALSE && bWriteToAccum == FALSE) {
		return;
	}

	if (TRUE == Compile_Vector_VMACF_AVX(bWriteToDest, bWriteToAccum)) {
		return;
	}

	if (TRUE == Compile_Vector_VMACF_SSE2(bWriteToDest, bWriteToAccum)) {
		return;
	}

	if (bWriteToDest == TRUE) {
		/*
		 * Prepare for conditional moves
		 */
		MoveConstToX86reg(&RspRecompPos, 0x00007fff, x86_ESI);
		MoveConstToX86reg(&RspRecompPos, 0xFFFF8000, x86_EDI);
	}
	if (bOptimize == TRUE) {
		del = (RSPOpC.OP.V.element & 0x07) ^ 7;
		sprintf(Reg, "RSP_Vect[%i].HW[%i]", RSPOpC.OP.V.vt, del);
		MoveSxVariableToX86regHalf(&RspRecompPos, &RSP_Vect[RSPOpC.OP.V.vt].HW[del], Reg, x86_EBX);
	}

	for (count = 0; count < 8; count++) {
		el = Indx[RSPOpC.OP.V.element].B[count];
		del = EleSpec[RSPOpC.OP.V.element].B[el];

		RSP_CPU_Message("     Iteration: %i", count);

		sprintf(Reg, "RSP_Vect[%i].HW[%i]", RSPOpC.OP.V.vs, el);
		MoveSxVariableToX86regHalf(&RspRecompPos, &RSP_Vect[RSPOpC.OP.V.vs].HW[el], Reg, x86_EAX);

		if (bOptimize == FALSE) {
			sprintf(Reg, "RSP_Vect[%i].HW[%i]", RSPOpC.OP.V.vt, del);
			MoveSxVariableToX86regHalf(&RspRecompPos, &RSP_Vect[RSPOpC.OP.V.vt].HW[del], Reg, x86_EBX);
		}

		imulX86reg(&RspRecompPos, x86_EBX);

		MoveX86RegToX86Reg(&RspRecompPos, x86_EAX, x86_EDX);
		ShiftRightSignImmed(&RspRecompPos, x86_EDX, 15);
		ShiftLeftSignImmed(&RspRecompPos, x86_EAX, 1);
		MoveX86RegToX86Reg(&RspRecompPos, x86_EDX, x86_ECX);
		ShiftRightSignImmed(&RspRecompPos, x86_ECX, 16);

		if (bWriteToAccum == TRUE && bWriteToDest == FALSE) {
			AddX86RegToVariableHalf(&RspRecompPos, &RSP_ACCUM_LOW.UHW[el], "RSP_ACCUM_LOW.UHW[el]", x86_EAX);
			AdcX86RegToVariableHalf(&RspRecompPos, &RSP_ACCUM_MID.UHW[el], "RSP_ACCUM_MID.UHW[el]", x86_EDX);
			AdcX86RegToVariableHalf(&RspRecompPos, &RSP_ACCUM_HIGH.UHW[el], "RSP_ACCUM_HIGH.UHW[el]", x86_ECX);
		}
		else {
			AddVariableToX86regHalf(&RspRecompPos, x86_EAX, &RSP_ACCUM_LOW.UHW[el], "RSP_ACCUM_LOW.UHW[el]");
			AdcVariableToX86regHalf(&RspRecompPos, x86_EDX, &RSP_ACCUM_MID.UHW[el], "RSP_ACCUM_MID.UHW[el]");
			AdcVariableToX86regHalf(&RspRecompPos, x86_ECX, &RSP_ACCUM_HIGH.UHW[el], "RSP_ACCUM_HIGH.UHW[el]");

			if (bWriteToAccum == TRUE) {
				MoveX86regHalfToVariable(&RspRecompPos, x86_EAX, &RSP_ACCUM_LOW.UHW[el], "RSP_ACCUM_LOW.UHW[el]");
				MoveX86regHalfToVariable(&RspRecompPos, x86_EDX, &RSP_ACCUM_MID.UHW[el], "RSP_ACCUM_MID.UHW[el]");
				MoveX86regHalfToVariable(&RspRecompPos, x86_ECX, &RSP_ACCUM_HIGH.UHW[el], "RSP_ACCUM_HIGH.UHW[el]");
			}
		}

		if (bWriteToDest == TRUE) {
			AndConstToX86Reg(&RspRecompPos, x86_EDX, 0xFFFF);
			ShiftLeftSignImmed(&RspRecompPos, x86_ECX, 16);
			OrX86RegToX86Reg(&RspRecompPos, x86_EDX, x86_ECX);

			CompX86RegToX86Reg(&RspRecompPos, x86_EDX, x86_ESI);
			CondMoveGreater(&RspRecompPos, x86_EDX, x86_ESI);
			CompX86RegToX86Reg(&RspRecompPos, x86_EDX, x86_EDI);
			CondMoveLess(&RspRecompPos, x86_EDX, x86_EDI);

			sprintf(Reg, "RSP_Vect[%i].HW[%i]", RSPOpC.OP.V.vd, el);
			MoveX86regHalfToVariable(&RspRecompPos, x86_EDX, &RSP_Vect[RSPOpC.OP.V.vd].HW[el], Reg);
		}
	}
}

static BOOL Compile_Vector_VMACU_SSE2(BOOL writeToVectorDest, BOOL writeToAccum) {
	char Reg[256];

	/* Do our SSE checks here */
	if (IsSseEnabled == FALSE || IsSse2Enabled == FALSE)
		return FALSE;

	// load vs and vt
	sprintf(Reg, "RSP_Vect[%i]", RSPOpC.OP.V.vs);
	SseMoveAlignedVariableToReg(&RspRecompPos, &RSP_Vect[RSPOpC.OP.V.vs].UHW[0], Reg, x86_XMM1, SseType_QuadWord, TRUE);

	if ((RSPOpC.OP.V.element & 0xF) < 2) {
		sprintf(Reg, "RSP_Vect[%i]", RSPOpC.OP.V.vt);
		SseMoveAlignedVariableToReg(&RspRecompPos, &RSP_Vect[RSPOpC.OP.V.vt].UHW[0], Reg, x86_XMM2, SseType_QuadWord, TRUE);
	}
	else if ((RSPOpC.OP.V.element & 0xF) >= 8) {
		RSP_Element2Sse(x86_XMM2);
	}
	else {
		RSP_MultiElement2Sse(x86_XMM2);
	}

	// 3 is low vs * vt
	SseMoveRegToReg(&RspRecompPos, x86_XMM3, x86_XMM2, SseType_QuadWord, TRUE);
	Sse2PmullwRegToReg(&RspRecompPos, x86_XMM3, x86_XMM1);

	// 4 is low (vs * vt) << 1
	SseMoveRegToReg(&RspRecompPos, x86_XMM4, x86_XMM3, SseType_QuadWord, TRUE);
	Sse2PsllwImmed(&RspRecompPos, x86_XMM4, 1);

	// 5 is unsigned low sum and 6 is signed low sum
	SseMoveAlignedVariableToReg(&RspRecompPos, &RSP_ACCUM_LOW.UHW[0], "RSP_ACCUM_LOW", x86_XMM6, SseType_QuadWord, TRUE);
	SseMoveRegToReg(&RspRecompPos, x86_XMM5, x86_XMM6, SseType_QuadWord, TRUE);
	Sse2PaddwRegToReg(&RspRecompPos, x86_XMM5, x86_XMM4);
	Sse2PadduswRegToReg(&RspRecompPos, x86_XMM6, x86_XMM4);
	if (writeToAccum) {
		SseMoveAlignedRegToVariable(&RspRecompPos, x86_XMM5, &RSP_ACCUM_LOW.UHW[0], "RSP_ACCUM_LOW", SseType_QuadWord, TRUE);
	}

	// compute carry in 5
	Sse2CompareEqualWordRegToReg(&RspRecompPos, x86_XMM5, x86_XMM6);
	Sse2CompareEqualDWordRegToReg(&RspRecompPos, x86_XMM6, x86_XMM6);
	Sse2PandnRegToReg(&RspRecompPos, x86_XMM5, x86_XMM6);
	Sse2PsrlwImmed(&RspRecompPos, x86_XMM5, 15);

	// 2 is high vs * vt
	Sse2PmulhwRegToReg(&RspRecompPos, x86_XMM2, x86_XMM1);

	// 1 and 2 are the high part shifted to add to accum
	SseMoveRegToReg(&RspRecompPos, x86_XMM1, x86_XMM2, SseType_QuadWord, TRUE);
	Sse2PsrlwImmed(&RspRecompPos, x86_XMM3, 15);
	Sse2PsllwImmed(&RspRecompPos, x86_XMM1, 1);
	Sse2PorRegToReg(&RspRecompPos, x86_XMM1, x86_XMM3);

	Sse2PsrawImmed(&RspRecompPos, x86_XMM2, 15);

	// 0 and 1 are merged 32 bits of the high/mid parts of the mult result
	SseMoveRegToReg(&RspRecompPos, x86_XMM0, x86_XMM1, SseType_QuadWord, TRUE);
	Sse2PunpckLowWordsRegToReg(&RspRecompPos, x86_XMM0, x86_XMM2);
	Sse2PunpckHighWordsRegToReg(&RspRecompPos, x86_XMM1, x86_XMM2);

	// 4 and 5 are sign extended carry
	Sse2MoveSxWordRegToDWordReg(&RspRecompPos, x86_XMM4, x86_XMM5, IsSse41Enabled);
	Sse2ShuffleDWordsRegToReg(&RspRecompPos, x86_XMM5, x86_XMM5, _MMX_SHUFFLE(3, 2, 3, 2));
	Sse2MoveSxWordRegToDWordReg(&RspRecompPos, x86_XMM5, x86_XMM5, IsSse41Enabled);

	// add carry
	Sse2PadddRegToReg(&RspRecompPos, x86_XMM0, x86_XMM4);
	Sse2PadddRegToReg(&RspRecompPos, x86_XMM1, x86_XMM5);

	// load in 4 and 5 high|mid accum
	SseMoveAlignedVariableToReg(&RspRecompPos, &RSP_ACCUM_HIGH.UHW[0], "RSP_ACCUM_HIGH", x86_XMM6, SseType_QuadWord, TRUE);
	SseMoveAlignedVariableToReg(&RspRecompPos, &RSP_ACCUM_MID.UHW[0], "RSP_ACCUM_MID", x86_XMM4, SseType_QuadWord, TRUE);
	SseMoveRegToReg(&RspRecompPos, x86_XMM5, x86_XMM4, SseType_QuadWord, TRUE);
	Sse2PunpckLowWordsRegToReg(&RspRecompPos, x86_XMM4, x86_XMM6);
	Sse2PunpckHighWordsRegToReg(&RspRecompPos, x86_XMM5, x86_XMM6);

	// add accum
	Sse2PadddRegToReg(&RspRecompPos, x86_XMM0, x86_XMM4);
	Sse2PadddRegToReg(&RspRecompPos, x86_XMM1, x86_XMM5);

	int destAccumReg = x86_XMM0;

	if (writeToVectorDest) {
		if (writeToAccum) {
			SseMoveRegToReg(&RspRecompPos, x86_XMM2, x86_XMM0, SseType_QuadWord, TRUE);
			destAccumReg = x86_XMM2;
		}
		Sse2CompareEqualDWordRegToReg(&RspRecompPos, x86_XMM6, x86_XMM6);
		Sse2PsrldImmed(&RspRecompPos, x86_XMM6, 17); // load 0x7FFF in XMM6

		Sse2PxorRegToReg(&RspRecompPos, x86_XMM7, x86_XMM7);
		Sse2CompareGreaterDWordRegToReg(&RspRecompPos, x86_XMM7, x86_XMM0);
		Sse2PandnRegToReg(&RspRecompPos, x86_XMM7, x86_XMM0);
		SseMoveRegToReg(&RspRecompPos, x86_XMM0, x86_XMM7, SseType_QuadWord, TRUE);
		Sse2CompareGreaterDWordRegToReg(&RspRecompPos, x86_XMM7, x86_XMM6);
		Sse2PorRegToReg(&RspRecompPos, x86_XMM0, x86_XMM7);

		Sse2PxorRegToReg(&RspRecompPos, x86_XMM7, x86_XMM7);
		Sse2CompareGreaterDWordRegToReg(&RspRecompPos, x86_XMM7, x86_XMM1);
		Sse2PandnRegToReg(&RspRecompPos, x86_XMM7, x86_XMM1);
		SseMoveRegToReg(&RspRecompPos, x86_XMM5, x86_XMM7, SseType_QuadWord, TRUE);
		Sse2CompareGreaterDWordRegToReg(&RspRecompPos, x86_XMM7, x86_XMM6);
		Sse2PorRegToReg(&RspRecompPos, x86_XMM5, x86_XMM7);

		Sse2PackSignedDWordRegToWordReg(&RspRecompPos, x86_XMM0, x86_XMM5);
		sprintf(Reg, "RSP_Vect[%i]", RSPOpC.OP.V.vd);
		SseMoveAlignedRegToVariable(&RspRecompPos, x86_XMM0, &RSP_Vect[RSPOpC.OP.V.vd].HW[0], Reg, SseType_QuadWord, TRUE);
	}

	if (writeToAccum) {
		SseMoveRegToReg(&RspRecompPos, x86_XMM3, destAccumReg, SseType_QuadWord, TRUE);
		SseMoveRegToReg(&RspRecompPos, x86_XMM4, x86_XMM1, SseType_QuadWord, TRUE);
		Sse2PsradImmed(&RspRecompPos, x86_XMM3, 16);
		Sse2PsradImmed(&RspRecompPos, x86_XMM4, 16);
		Sse2PackSignedDWordRegToWordReg(&RspRecompPos, x86_XMM3, x86_XMM4);
		SseMoveAlignedRegToVariable(&RspRecompPos, x86_XMM3, &RSP_ACCUM_HIGH.UHW[0], "RSP_ACCUM_HIGH", SseType_QuadWord, TRUE);

		Sse2PslldImmed(&RspRecompPos, destAccumReg, 16);
		Sse2PslldImmed(&RspRecompPos, x86_XMM1, 16);
		Sse2PsradImmed(&RspRecompPos, destAccumReg, 16);
		Sse2PsradImmed(&RspRecompPos, x86_XMM1, 16);
		Sse2PackSignedDWordRegToWordReg(&RspRecompPos, destAccumReg, x86_XMM1);
		SseMoveAlignedRegToVariable(&RspRecompPos, destAccumReg, &RSP_ACCUM_MID.UHW[0], "RSP_ACCUM_MID", SseType_QuadWord, TRUE);
	}

	return TRUE;
}

static BOOL Compile_Vector_VMACU_AVX(BOOL writeToVectorDest, BOOL writeToAccum) {
	char Reg[256];

	/* Do our AVX checks here */
	if (IsAvxEnabled == FALSE || IsAvx2Enabled == FALSE || IsSseEnabled == FALSE || IsSse2Enabled == FALSE)
		return FALSE;

	// load vs and vt
	sprintf(Reg, "RSP_Vect[%i]", RSPOpC.OP.V.vs);
	SseMoveAlignedVariableToReg(&RspRecompPos, &RSP_Vect[RSPOpC.OP.V.vs].UHW[0], Reg, x86_XMM1, SseType_QuadWord, TRUE);

	if ((RSPOpC.OP.V.element & 0xF) < 2) {
		sprintf(Reg, "RSP_Vect[%i]", RSPOpC.OP.V.vt);
		SseMoveAlignedVariableToReg(&RspRecompPos, &RSP_Vect[RSPOpC.OP.V.vt].UHW[0], Reg, x86_XMM2, SseType_QuadWord, TRUE);
	}
	else if ((RSPOpC.OP.V.element & 0xF) >= 8) {
		RSP_Element2Sse(x86_XMM2);
	}
	else {
		RSP_MultiElement2Sse(x86_XMM2);
	}

	// 3 is low vs * vt
	AvxVPMullwRegToReg128(&RspRecompPos, x86_XMM3, x86_XMM1, x86_XMM2);

	// 4 is low (vs * vt) << 1
	AvxVPSllwRegToReg128Immed(&RspRecompPos, x86_XMM4, x86_XMM3, 1);

	// 5 is unsigned low sum and 6 is signed low sum
	SseMoveAlignedVariableToReg(&RspRecompPos, &RSP_ACCUM_LOW.UHW[0], "RSP_ACCUM_LOW", x86_XMM6, SseType_QuadWord, TRUE);
	AvxVPAddwRegToReg128(&RspRecompPos, x86_XMM5, x86_XMM6, x86_XMM4);
	Sse2PadduswRegToReg(&RspRecompPos, x86_XMM6, x86_XMM4);
	if (writeToAccum) {
		SseMoveAlignedRegToVariable(&RspRecompPos, x86_XMM5, &RSP_ACCUM_LOW.UHW[0], "RSP_ACCUM_LOW", SseType_QuadWord, TRUE);
	}

	// compute carry in 5
	Sse2CompareEqualWordRegToReg(&RspRecompPos, x86_XMM5, x86_XMM6);
	Sse2CompareEqualDWordRegToReg(&RspRecompPos, x86_XMM6, x86_XMM6);
	Sse2PandnRegToReg(&RspRecompPos, x86_XMM5, x86_XMM6);
	Sse2PsrlwImmed(&RspRecompPos, x86_XMM5, 15);

	// 2 is high vs * vt
	Sse2PmulhwRegToReg(&RspRecompPos, x86_XMM2, x86_XMM1);

	// 0 is the high part shifted to add to accum
	AvxVPMovesxWordReg128ToDwordReg256(&RspRecompPos, x86_YMM0, x86_XMM2);
	AvxVPSlldRegToReg256Immed(&RspRecompPos, x86_YMM0, x86_YMM0, 1);
	Sse2PsrlwImmed(&RspRecompPos, x86_XMM3, 15);
	AvxVPMovesxWordReg128ToDwordReg256(&RspRecompPos, x86_YMM3, x86_XMM3);
	AvxVPorRegToReg256(&RspRecompPos, x86_YMM0, x86_YMM0, x86_YMM3);

	// 4 is sign extended carry
	AvxVPMovesxWordReg128ToDwordReg256(&RspRecompPos, x86_YMM4, x86_XMM5);

	// add carry
	AvxVPAdddRegToReg256(&RspRecompPos, x86_YMM0, x86_YMM0, x86_YMM4);

	// load in 4 high|mid accum
	SseMoveAlignedVariableToReg(&RspRecompPos, &RSP_ACCUM_HIGH.UHW[0], "RSP_ACCUM_HIGH", x86_XMM6, SseType_QuadWord, TRUE);
	SseMoveAlignedVariableToReg(&RspRecompPos, &RSP_ACCUM_MID.UHW[0], "RSP_ACCUM_MID", x86_XMM4, SseType_QuadWord, TRUE);
	AvxVPunpckHighWordsRegToReg256(&RspRecompPos, x86_YMM5, x86_YMM4, x86_YMM6);
	AvxVPunpckLowWordsRegToReg256(&RspRecompPos, x86_YMM4, x86_YMM4, x86_YMM6);
	AvxVInserti128RegToReg(&RspRecompPos, x86_YMM4, x86_YMM4, x86_XMM5, TRUE);

	// add accum
	AvxVPAdddRegToReg256(&RspRecompPos, x86_YMM0, x86_YMM0, x86_YMM4);

	if (writeToVectorDest) {
		AvxCompareEqualDWordRegToReg256(&RspRecompPos, x86_YMM6, x86_YMM6, x86_YMM6);
		AvxVPSrldRegToReg256Immed(&RspRecompPos, x86_YMM6, x86_YMM6, 17); // load 0x7FFF in XMM6

		AvxVPxorRegToReg256(&RspRecompPos, x86_YMM7, x86_YMM7, x86_YMM7);
		AvxCompareGreaterDWordRegToReg256(&RspRecompPos, x86_YMM7, x86_YMM7, x86_YMM0);
		AvxVPandnRegToReg256(&RspRecompPos, x86_YMM7, x86_YMM7, x86_YMM0);
		AvxCompareGreaterDWordRegToReg256(&RspRecompPos, x86_YMM6, x86_YMM7, x86_YMM6);
		AvxVPorRegToReg256(&RspRecompPos, x86_YMM6, x86_YMM6, x86_YMM7);

		AvxVExtracti128RegToReg(&RspRecompPos, x86_XMM2, x86_YMM6, TRUE);
		AvxVPackSignedDWordRegToWordReg128(&RspRecompPos, x86_XMM2, x86_XMM6, x86_XMM2);
		sprintf(Reg, "RSP_Vect[%i]", RSPOpC.OP.V.vd);
		SseMoveAlignedRegToVariable(&RspRecompPos, x86_XMM2, &RSP_Vect[RSPOpC.OP.V.vd].HW[0], Reg, SseType_QuadWord, TRUE);
	}

	if (writeToAccum) {
		AvxVPSradRegToReg256Immed(&RspRecompPos, x86_YMM2, x86_YMM0, 16);
		AvxVExtracti128RegToReg(&RspRecompPos, x86_XMM3, x86_YMM2, TRUE);
		AvxVPackSignedDWordRegToWordReg128(&RspRecompPos, x86_XMM3, x86_XMM2, x86_XMM3);
		SseMoveAlignedRegToVariable(&RspRecompPos, x86_XMM3, &RSP_ACCUM_HIGH.UHW[0], "RSP_ACCUM_HIGH", SseType_QuadWord, TRUE);

		AvxVPSlldRegToReg256Immed(&RspRecompPos, x86_YMM2, x86_YMM0, 16);
		AvxVPSradRegToReg256Immed(&RspRecompPos, x86_YMM2, x86_YMM2, 16);
		AvxVExtracti128RegToReg(&RspRecompPos, x86_XMM3, x86_YMM2, TRUE);
		AvxVPackSignedDWordRegToWordReg128(&RspRecompPos, x86_XMM3, x86_XMM2, x86_XMM3);
		SseMoveAlignedRegToVariable(&RspRecompPos, x86_XMM3, &RSP_ACCUM_MID.UHW[0], "RSP_ACCUM_MID", SseType_QuadWord, TRUE);
	}

	return TRUE;
}

void CompileRsp_Vector_VMACU ( void ) {
	char Reg[256];
	int count, el, del;

	BOOL bOptimize = ((RSPOpC.OP.V.element & 0x0f) >= 8) ? TRUE : FALSE;
	BOOL bWriteToDest = WriteToVectorDest(RSPOpC.OP.V.vd, RspCompilePC);
	BOOL bWriteToAccum = WriteToAccum(EntireAccum, RspCompilePC);

	#ifndef CompileVmacu
	InterpreterFallback((void*)RSP_Vector_VMACU, "RSP_Vector_VMACU"); return;
	#endif

	RSP_CPU_Message("  %X %s", RspCompilePC, RSPOpcodeName(RSPOpC.OP.Hex, RspCompilePC));

	if (bWriteToDest == FALSE && bWriteToAccum == FALSE) {
		return;
	}

	if (TRUE == Compile_Vector_VMACU_AVX(bWriteToDest, bWriteToAccum)) {
		return;
	}

	if (TRUE == Compile_Vector_VMACU_SSE2(bWriteToDest, bWriteToAccum)) {
		return;
	}

	if (bWriteToDest == TRUE) {
		/*
		 * Prepare for conditional moves
		 */
		MoveConstToX86reg(&RspRecompPos, 0x0000ffff, x86_ESI);
		XorX86RegToX86Reg(&RspRecompPos, x86_EDI, x86_EDI);
	}
	if (bOptimize == TRUE) {
		del = (RSPOpC.OP.V.element & 0x07) ^ 7;
		sprintf(Reg, "RSP_Vect[%i].HW[%i]", RSPOpC.OP.V.vt, del);
		MoveSxVariableToX86regHalf(&RspRecompPos, &RSP_Vect[RSPOpC.OP.V.vt].HW[del], Reg, x86_EBX);
	}

	for (count = 0; count < 8; count++) {
		el = Indx[RSPOpC.OP.V.element].B[count];
		del = EleSpec[RSPOpC.OP.V.element].B[el];

		RSP_CPU_Message("     Iteration: %i", count);

		sprintf(Reg, "RSP_Vect[%i].HW[%i]", RSPOpC.OP.V.vs, el);
		MoveSxVariableToX86regHalf(&RspRecompPos, &RSP_Vect[RSPOpC.OP.V.vs].HW[el], Reg, x86_EAX);

		if (bOptimize == FALSE) {
			sprintf(Reg, "RSP_Vect[%i].HW[%i]", RSPOpC.OP.V.vt, del);
			MoveSxVariableToX86regHalf(&RspRecompPos, &RSP_Vect[RSPOpC.OP.V.vt].HW[del], Reg, x86_EBX);
		}

		imulX86reg(&RspRecompPos, x86_EBX);

		MoveX86RegToX86Reg(&RspRecompPos, x86_EAX, x86_EDX);
		ShiftRightSignImmed(&RspRecompPos, x86_EDX, 15);
		ShiftLeftSignImmed(&RspRecompPos, x86_EAX, 1);
		MoveX86RegToX86Reg(&RspRecompPos, x86_EDX, x86_ECX);
		ShiftRightSignImmed(&RspRecompPos, x86_ECX, 16);

		if (bWriteToAccum == TRUE && bWriteToDest == FALSE) {
			AddX86RegToVariableHalf(&RspRecompPos, &RSP_ACCUM_LOW.UHW[el], "RSP_ACCUM_LOW.UHW[el]", x86_EAX);
			AdcX86RegToVariableHalf(&RspRecompPos, &RSP_ACCUM_MID.UHW[el], "RSP_ACCUM_MID.UHW[el]", x86_EDX);
			AdcX86RegToVariableHalf(&RspRecompPos, &RSP_ACCUM_HIGH.UHW[el], "RSP_ACCUM_HIGH.UHW[el]", x86_ECX);
		}
		else {
			AddVariableToX86regHalf(&RspRecompPos, x86_EAX, &RSP_ACCUM_LOW.UHW[el], "RSP_ACCUM_LOW.UHW[el]");
			AdcVariableToX86regHalf(&RspRecompPos, x86_EDX, &RSP_ACCUM_MID.UHW[el], "RSP_ACCUM_MID.UHW[el]");
			AdcVariableToX86regHalf(&RspRecompPos, x86_ECX, &RSP_ACCUM_HIGH.UHW[el], "RSP_ACCUM_HIGH.UHW[el]");

			if (bWriteToAccum == TRUE) {
				MoveX86regHalfToVariable(&RspRecompPos, x86_EAX, &RSP_ACCUM_LOW.UHW[el], "RSP_ACCUM_LOW.UHW[el]");
				MoveX86regHalfToVariable(&RspRecompPos, x86_EDX, &RSP_ACCUM_MID.UHW[el], "RSP_ACCUM_MID.UHW[el]");
				MoveX86regHalfToVariable(&RspRecompPos, x86_ECX, &RSP_ACCUM_HIGH.UHW[el], "RSP_ACCUM_HIGH.UHW[el]");
			}
		}

		if (bWriteToDest == TRUE) {
			AndConstToX86Reg(&RspRecompPos, x86_EDX, 0xFFFF);
			ShiftLeftSignImmed(&RspRecompPos, x86_ECX, 16);
			OrX86RegToX86Reg(&RspRecompPos, x86_EDX, x86_ECX);

			CompConstToX86reg(&RspRecompPos, x86_EDX, 0x7FFF);
			CondMoveGreater(&RspRecompPos, x86_EDX, x86_ESI);
			CompX86RegToX86Reg(&RspRecompPos, x86_EDX, x86_EDI);
			CondMoveLess(&RspRecompPos, x86_EDX, x86_EDI);

			sprintf(Reg, "RSP_Vect[%i].HW[%i]", RSPOpC.OP.V.vd, el);
			MoveX86regHalfToVariable(&RspRecompPos, x86_EDX, &RSP_Vect[RSPOpC.OP.V.vd].HW[el], Reg);
		}
	}
}

static BOOL Compile_Vector_VRNDN_SSE2(BOOL writeToVectorDest, BOOL writeToAccum) {
	char Reg[256];

	/* Do our SSE checks here */
	if (IsSseEnabled == FALSE || IsSse2Enabled == FALSE)
		return FALSE;

	// load vs and vt
	if ((RSPOpC.OP.V.element & 0xF) < 2) {
		sprintf(Reg, "RSP_Vect[%i]", RSPOpC.OP.V.vt);
		SseMoveAlignedVariableToReg(&RspRecompPos, &RSP_Vect[RSPOpC.OP.V.vt].UHW[0], Reg, x86_XMM1, SseType_QuadWord, TRUE);
	}
	else if ((RSPOpC.OP.V.element & 0xF) >= 8) {
		RSP_Element2Sse(x86_XMM1);
	}
	else {
		RSP_MultiElement2Sse(x86_XMM1);
	}

	SseMoveAlignedVariableToReg(&RspRecompPos, &RSP_ACCUM_HIGH.UHW[0], "RSP_ACCUM_HIGH", x86_XMM4, SseType_QuadWord, TRUE);
	SseMoveAlignedVariableToReg(&RspRecompPos, &RSP_ACCUM_MID.UHW[0], "RSP_ACCUM_MID", x86_XMM2, SseType_QuadWord, TRUE);
	SseMoveRegToReg(&RspRecompPos, x86_XMM3, x86_XMM2, SseType_QuadWord, TRUE);
	Sse2PunpckHighWordsRegToReg(&RspRecompPos, x86_XMM2, x86_XMM4);
	Sse2PunpckLowWordsRegToReg(&RspRecompPos, x86_XMM3, x86_XMM4);

	Sse2PxorRegToReg(&RspRecompPos, x86_XMM5, x86_XMM5);
	Sse2CompareGreaterWordRegToReg(&RspRecompPos, x86_XMM5, x86_XMM4);
	Sse2PandRegToReg(&RspRecompPos, x86_XMM5, x86_XMM1);

	if ((RSPOpC.OP.V.vs & 1) != 0) {
		Sse2MoveSxWordRegToDWordReg(&RspRecompPos, x86_XMM0, x86_XMM5, IsSse41Enabled);
		Sse2ShuffleDWordsRegToReg(&RspRecompPos, x86_XMM5, x86_XMM5, _MMX_SHUFFLE(3, 2, 3, 2));
		Sse2MoveSxWordRegToDWordReg(&RspRecompPos, x86_XMM5, x86_XMM5, IsSse41Enabled);
		Sse2PadddRegToReg(&RspRecompPos, x86_XMM2, x86_XMM5);
		Sse2PadddRegToReg(&RspRecompPos, x86_XMM3, x86_XMM0);
	}
	else {
		SseMoveAlignedVariableToReg(&RspRecompPos, &RSP_ACCUM_LOW.UHW[0], "RSP_ACCUM_LOW", x86_XMM4, SseType_QuadWord, TRUE);

		SseMoveRegToReg(&RspRecompPos, x86_XMM1, x86_XMM4, SseType_QuadWord, TRUE);
		Sse2PaddwRegToReg(&RspRecompPos, x86_XMM4, x86_XMM5);
		if (writeToAccum == TRUE) {
			SseMoveAlignedRegToVariable(&RspRecompPos, x86_XMM4, &RSP_ACCUM_LOW.UHW[0], "RSP_ACCUM_LOW", SseType_QuadWord, TRUE);
		}

		// compute carry
		Sse2PadduswRegToReg(&RspRecompPos, x86_XMM1, x86_XMM5);
		Sse2CompareEqualWordRegToReg(&RspRecompPos, x86_XMM4, x86_XMM1);
		Sse2CompareEqualDWordRegToReg(&RspRecompPos, x86_XMM1, x86_XMM1);
		Sse2PandnRegToReg(&RspRecompPos, x86_XMM4, x86_XMM1);
		Sse2PsrlwImmed(&RspRecompPos, x86_XMM4, 15);
		Sse2PsrawImmed(&RspRecompPos, x86_XMM5, 15);
		Sse2PaddwRegToReg(&RspRecompPos, x86_XMM5, x86_XMM4);

		// add to accum mid and high
		Sse2MoveSxWordRegToDWordReg(&RspRecompPos, x86_XMM0, x86_XMM5, IsSse41Enabled);
		Sse2ShuffleDWordsRegToReg(&RspRecompPos, x86_XMM5, x86_XMM5, _MMX_SHUFFLE(3, 2, 3, 2));
		Sse2MoveSxWordRegToDWordReg(&RspRecompPos, x86_XMM5, x86_XMM5, IsSse41Enabled);
		Sse2PadddRegToReg(&RspRecompPos, x86_XMM2, x86_XMM5);
		Sse2PadddRegToReg(&RspRecompPos, x86_XMM3, x86_XMM0);
	}

	if (writeToVectorDest) {
		if (writeToAccum) {
			SseMoveRegToReg(&RspRecompPos, x86_XMM4, x86_XMM3, SseType_QuadWord, TRUE);
		}
		Sse2PackSignedDWordRegToWordReg(&RspRecompPos, x86_XMM3, x86_XMM2);

		sprintf(Reg, "RSP_Vect[%i]", RSPOpC.OP.V.vd);
		SseMoveAlignedRegToVariable(&RspRecompPos, x86_XMM3, &RSP_Vect[RSPOpC.OP.V.vd].UHW[0], Reg, SseType_QuadWord, TRUE);
	}

	if (writeToAccum) {
		int lowReg = writeToVectorDest ? x86_XMM4 : x86_XMM3;

		SseMoveRegToReg(&RspRecompPos, x86_XMM7, lowReg, SseType_QuadWord, TRUE);
		SseMoveRegToReg(&RspRecompPos, x86_XMM6, x86_XMM2, SseType_QuadWord, TRUE);
		Sse2PsradImmed(&RspRecompPos, x86_XMM7, 16);
		Sse2PsradImmed(&RspRecompPos, x86_XMM6, 16);
		Sse2PackSignedDWordRegToWordReg(&RspRecompPos, x86_XMM7, x86_XMM6);

		SseMoveAlignedRegToVariable(&RspRecompPos, x86_XMM7, &RSP_ACCUM_HIGH.UHW[0], "RSP_ACCUM_HIGH", SseType_QuadWord, TRUE);


		Sse2PslldImmed(&RspRecompPos, lowReg, 16);
		Sse2PslldImmed(&RspRecompPos, x86_XMM2, 16);
		Sse2PsradImmed(&RspRecompPos, lowReg, 16);
		Sse2PsradImmed(&RspRecompPos, x86_XMM2, 16);
		Sse2PackSignedDWordRegToWordReg(&RspRecompPos, lowReg, x86_XMM2);

		SseMoveAlignedRegToVariable(&RspRecompPos, lowReg, &RSP_ACCUM_MID.UHW[0], "RSP_ACCUM_MID", SseType_QuadWord, TRUE);
	}

	return TRUE;
}

static BOOL Compile_Vector_VRNDN_AVX(BOOL writeToVectorDest, BOOL writeToAccum) {
	char Reg[256];

	/* Do our AVX checks here */
	if (IsAvxEnabled == FALSE || IsAvx2Enabled == FALSE || IsSse2Enabled == FALSE)
		return FALSE;

	// load vs and vt
	if ((RSPOpC.OP.V.element & 0xF) < 2) {
		sprintf(Reg, "RSP_Vect[%i]", RSPOpC.OP.V.vt);
		SseMoveAlignedVariableToReg(&RspRecompPos, &RSP_Vect[RSPOpC.OP.V.vt].UHW[0], Reg, x86_XMM1, SseType_QuadWord, TRUE);
	}
	else if ((RSPOpC.OP.V.element & 0xF) >= 8) {
		RSP_Element2Sse(x86_XMM1);
	}
	else {
		RSP_MultiElement2Sse(x86_XMM1);
	}

	SseMoveAlignedVariableToReg(&RspRecompPos, &RSP_ACCUM_HIGH.UHW[0], "RSP_ACCUM_HIGH", x86_XMM4, SseType_QuadWord, TRUE);
	SseMoveAlignedVariableToReg(&RspRecompPos, &RSP_ACCUM_MID.UHW[0], "RSP_ACCUM_MID", x86_XMM2, SseType_QuadWord, TRUE);
	AvxVPunpckHighWordsRegToReg256(&RspRecompPos, x86_YMM7, x86_YMM2, x86_YMM4);
	AvxVPunpckLowWordsRegToReg256(&RspRecompPos, x86_YMM2, x86_YMM2, x86_YMM4);
	AvxVInserti128RegToReg(&RspRecompPos, x86_YMM2, x86_YMM2, x86_XMM7, TRUE);

	Sse2PxorRegToReg(&RspRecompPos, x86_XMM5, x86_XMM5);
	AvxCompareGreaterWordRegToReg128(&RspRecompPos, x86_XMM5, x86_XMM5, x86_XMM4);
	AvxVPandRegToReg128(&RspRecompPos, x86_XMM1, x86_XMM5, x86_XMM1);

	if ((RSPOpC.OP.V.vs & 1) != 0) {
		AvxVPMovesxWordReg128ToDwordReg256(&RspRecompPos, x86_YMM1, x86_XMM1);
		AvxVPAdddRegToReg256(&RspRecompPos, x86_YMM2, x86_YMM2, x86_YMM1);
	}
	else {
		SseMoveAlignedVariableToReg(&RspRecompPos, &RSP_ACCUM_LOW.UHW[0], "RSP_ACCUM_LOW", x86_XMM4, SseType_QuadWord, TRUE);

		AvxVPAddwRegToReg128(&RspRecompPos, x86_XMM3, x86_XMM4, x86_XMM1);

		if (writeToAccum == TRUE) {
			SseMoveAlignedRegToVariable(&RspRecompPos, x86_XMM3, &RSP_ACCUM_LOW.UHW[0], "RSP_ACCUM_LOW", SseType_QuadWord, TRUE);
		}

		// compute carry
		Sse2PadduswRegToReg(&RspRecompPos, x86_XMM4, x86_XMM1);
		Sse2CompareEqualWordRegToReg(&RspRecompPos, x86_XMM4, x86_XMM3);
		Sse2CompareEqualDWordRegToReg(&RspRecompPos, x86_XMM5, x86_XMM5);
		Sse2PandnRegToReg(&RspRecompPos, x86_XMM4, x86_XMM5);
		Sse2PsrlwImmed(&RspRecompPos, x86_XMM4, 15);
		Sse2PsrawImmed(&RspRecompPos, x86_XMM1, 15);
		Sse2PaddwRegToReg(&RspRecompPos, x86_XMM1, x86_XMM4);

		// add to accum mid and high
		AvxVPMovesxWordReg128ToDwordReg256(&RspRecompPos, x86_YMM1, x86_XMM1);
		AvxVPAdddRegToReg256(&RspRecompPos, x86_YMM2, x86_YMM2, x86_YMM1);
	}

	if (writeToVectorDest) {
		AvxVExtracti128RegToReg(&RspRecompPos, x86_XMM0, x86_YMM2, TRUE);
		AvxVPackSignedDWordRegToWordReg128(&RspRecompPos, x86_XMM0, x86_XMM2, x86_XMM0);

		sprintf(Reg, "RSP_Vect[%i]", RSPOpC.OP.V.vd);
		SseMoveAlignedRegToVariable(&RspRecompPos, x86_XMM0, &RSP_Vect[RSPOpC.OP.V.vd].UHW[0], Reg, SseType_QuadWord, TRUE);
	}

	if (writeToAccum) {
		AvxVPSlldRegToReg256Immed(&RspRecompPos, x86_YMM0, x86_YMM2, 16);
		AvxVPSradRegToReg256Immed(&RspRecompPos, x86_YMM0, x86_YMM0, 16);
		AvxVExtracti128RegToReg(&RspRecompPos, x86_XMM7, x86_YMM0, TRUE);
		AvxVPackSignedDWordRegToWordReg128(&RspRecompPos, x86_XMM0, x86_XMM0, x86_XMM7);
		SseMoveAlignedRegToVariable(&RspRecompPos, x86_XMM0, &RSP_ACCUM_MID.UHW[0], "RSP_ACCUM_MID", SseType_QuadWord, TRUE);

		AvxVPSradRegToReg256Immed(&RspRecompPos, x86_YMM0, x86_YMM2, 16);
		AvxVExtracti128RegToReg(&RspRecompPos, x86_XMM7, x86_YMM0, TRUE);
		AvxVPackSignedDWordRegToWordReg128(&RspRecompPos, x86_XMM0, x86_XMM0, x86_XMM7);
		SseMoveAlignedRegToVariable(&RspRecompPos, x86_XMM0, &RSP_ACCUM_HIGH.UHW[0], "RSP_ACCUM_HIGH", SseType_QuadWord, TRUE);
	}

	return TRUE;
}

void CompileRsp_Vector_VRNDN(void) {
	char Reg[256];
	int count, el, del;

	BOOL bWriteToAccum = FALSE;
	if ((RSPOpC.OP.V.vs & 1) != 0) bWriteToAccum = WriteToAccum(High16BitAccum | Middle16BitAccum, RspCompilePC);
	else bWriteToAccum = WriteToAccum(EntireAccum, RspCompilePC);
	BOOL bWriteToDest = WriteToVectorDest(RSPOpC.OP.V.vd, RspCompilePC);

	#ifndef CompileVrndn
	InterpreterFallback((void*)RSP_Vector_VRNDN, "RSP_Vector_VRNDN"); return;
	#endif

	RSP_CPU_Message("  %X %s", RspCompilePC, RSPOpcodeName(RSPOpC.OP.Hex, RspCompilePC));

	if (bWriteToDest == FALSE && bWriteToAccum == FALSE) {
		return;
	}

	if (TRUE == Compile_Vector_VRNDN_AVX(bWriteToDest, bWriteToAccum)) {
		return;
	}

	if (TRUE == Compile_Vector_VRNDN_SSE2(bWriteToDest, bWriteToAccum)) {
		return;
	}

	MoveConstToX86reg(&RspRecompPos, 0x7fff, x86_ESI);
	MoveConstToX86reg(&RspRecompPos, 0xffff8000, x86_EDI);

	for (count = 0; count < 8; count++) {
		RSP_CPU_Message("     Iteration: %i", count);

		el = Indx[RSPOpC.OP.V.element].B[count];
		del = EleSpec[RSPOpC.OP.V.element].B[el];

		sprintf(Reg, "RSP_Vect[%i].HW[%i]", RSPOpC.OP.V.vt, del);
		MoveSxVariableToX86regHalf(&RspRecompPos, &RSP_Vect[RSPOpC.OP.V.vt].HW[del], Reg, x86_EAX);

		sprintf(Reg, "ACCUM_HIGH[%i]", el);
		MoveVariableToX86regHalf(&RspRecompPos, &RSP_ACCUM_HIGH.UHW[el], Reg, x86_EBX);
		ShiftLeftSignImmed(&RspRecompPos, x86_EBX, 16);
		sprintf(Reg, "ACCUM_MID[%i]", el);
		MoveVariableToX86regHalf(&RspRecompPos, &RSP_ACCUM_MID.UHW[el], Reg, x86_EBX);
		if ((RSPOpC.OP.V.vs & 1) == 0) {
			sprintf(Reg, "ACCUM_LOW[%i]", el);
			MoveVariableToX86regHalf(&RspRecompPos, &RSP_ACCUM_LOW.UHW[el], Reg, x86_ECX);
		}

		XorX86RegToX86Reg(&RspRecompPos, x86_EDX, x86_EDX);
		CompConstToX86reg(&RspRecompPos, x86_EBX, 0);
		CondMoveGreater(&RspRecompPos, x86_EAX, x86_EDX);

		if ((RSPOpC.OP.V.vs & 1) != 0) {
			AddX86RegToX86Reg(&RspRecompPos, x86_EBX, x86_EAX);
		}
		else {
			Cdq(&RspRecompPos);
			AddX86RegToX86RegHalf(&RspRecompPos, x86_ECX, x86_EAX);
			AdcX86RegToX86Reg(&RspRecompPos, x86_EBX, x86_EDX);

			if (bWriteToAccum == TRUE) {
				sprintf(Reg, "ACCUM_LOW[%i]", el);
				MoveX86regHalfToVariable(&RspRecompPos, x86_ECX, &RSP_ACCUM_LOW.UHW[el], Reg);
			}
		}

		if (bWriteToAccum == TRUE) {
			sprintf(Reg, "ACCUM_MID[%i]", el);
			MoveX86regHalfToVariable(&RspRecompPos, x86_EBX, &RSP_ACCUM_MID.UHW[el], Reg);
			MoveX86RegToX86Reg(&RspRecompPos, x86_EBX, x86_EDX);
			ShiftRightUnsignImmed(&RspRecompPos, x86_EDX, 16);
			sprintf(Reg, "ACCUM_HIGH[%i]", el);
			MoveX86regHalfToVariable(&RspRecompPos, x86_EDX, &RSP_ACCUM_HIGH.UHW[el], Reg);
		}

		if (bWriteToDest == TRUE) {
			CompX86RegToX86Reg(&RspRecompPos, x86_EBX, x86_ESI);
			CondMoveGreater(&RspRecompPos, x86_EBX, x86_ESI);
			CompX86RegToX86Reg(&RspRecompPos, x86_EBX, x86_EDI);
			CondMoveLess(&RspRecompPos, x86_EBX, x86_EDI);

			MoveX86regHalfToVariable(&RspRecompPos, x86_EBX, &RSP_Vect[RSPOpC.OP.V.vd].HW[el], "RSP_Vect[RSPOpC.OP.V.vd].HW[el]");
		}
	}
}

static BOOL Compile_Vector_VMACQ_SSE2(BOOL writeToVectorDest, BOOL writeToAccum) {
	char Reg[256];

	/* Do our SSE checks here */
	if (IsSseEnabled == FALSE || IsSse2Enabled == FALSE)
		return FALSE;

	SseMoveAlignedVariableToReg(&RspRecompPos, &RSP_ACCUM_HIGH.UHW[0], "RSP_ACCUM_HIGH", x86_XMM3, SseType_QuadWord, TRUE);
	SseMoveAlignedVariableToReg(&RspRecompPos, &RSP_ACCUM_MID.UHW[0], "RSP_ACCUM_MID", x86_XMM1, SseType_QuadWord, TRUE);
	SseMoveRegToReg(&RspRecompPos, x86_XMM2, x86_XMM1, SseType_QuadWord, TRUE);
	Sse2PunpckHighWordsRegToReg(&RspRecompPos, x86_XMM2, x86_XMM3);
	Sse2PunpckLowWordsRegToReg(&RspRecompPos, x86_XMM1, x86_XMM3);

	// init the value to add to either 0x20 or -0x20 depending on the accumlator sign (in xmm3 and xmm4)
	SseMoveRegToReg(&RspRecompPos, x86_XMM3, x86_XMM1, SseType_QuadWord, TRUE);
	SseMoveRegToReg(&RspRecompPos, x86_XMM4, x86_XMM2, SseType_QuadWord, TRUE);
	Sse2PsradImmed(&RspRecompPos, x86_XMM3, 30);
	Sse2PsradImmed(&RspRecompPos, x86_XMM4, 30);
	Sse2CompareEqualDWordRegToReg(&RspRecompPos, x86_XMM0, x86_XMM0); // xmm0=~0
	Sse2PxorRegToReg(&RspRecompPos, x86_XMM3, x86_XMM0);
	Sse2PxorRegToReg(&RspRecompPos, x86_XMM4, x86_XMM0);
	Sse2PsrldImmed(&RspRecompPos, x86_XMM0, 31); // xmm0=1
	Sse2PorRegToReg(&RspRecompPos, x86_XMM3, x86_XMM0);
	Sse2PorRegToReg(&RspRecompPos, x86_XMM4, x86_XMM0);
	Sse2PslldImmed(&RspRecompPos, x86_XMM3, 5);
	Sse2PslldImmed(&RspRecompPos, x86_XMM4, 5);

	// make it 0 when accumulator is in the [0,0x20] range (in xmm5 and xmm6)
	SseMoveRegToReg(&RspRecompPos, x86_XMM5, x86_XMM1, SseType_QuadWord, TRUE);
	SseMoveRegToReg(&RspRecompPos, x86_XMM6, x86_XMM2, SseType_QuadWord, TRUE);
	Sse2PsradImmed(&RspRecompPos, x86_XMM5, 6);
	Sse2PsradImmed(&RspRecompPos, x86_XMM6, 6);
	Sse2PxorRegToReg(&RspRecompPos, x86_XMM7, x86_XMM7);
	Sse2CompareEqualDWordRegToReg(&RspRecompPos, x86_XMM5, x86_XMM7);
	Sse2CompareEqualDWordRegToReg(&RspRecompPos, x86_XMM6, x86_XMM7);
	Sse2PandnRegToReg(&RspRecompPos, x86_XMM5, x86_XMM3);
	Sse2PandnRegToReg(&RspRecompPos, x86_XMM6, x86_XMM4);

	// make it 0 too if bit 0x20 is set in accumulator (in xmm3 and xmm4)
	Sse2PslldImmed(&RspRecompPos, x86_XMM0, 5); // xmm0=0x20
	SseMoveRegToReg(&RspRecompPos, x86_XMM3, x86_XMM1, SseType_QuadWord, TRUE);
	SseMoveRegToReg(&RspRecompPos, x86_XMM4, x86_XMM2, SseType_QuadWord, TRUE);
	Sse2PandRegToReg(&RspRecompPos, x86_XMM3, x86_XMM0);
	Sse2PandRegToReg(&RspRecompPos, x86_XMM4, x86_XMM0);
	Sse2CompareEqualDWordRegToReg(&RspRecompPos, x86_XMM3, x86_XMM7);
	Sse2CompareEqualDWordRegToReg(&RspRecompPos, x86_XMM4, x86_XMM7);
	Sse2PandRegToReg(&RspRecompPos, x86_XMM3, x86_XMM5);
	Sse2PandRegToReg(&RspRecompPos, x86_XMM4, x86_XMM6);

	// add result to accumulator
	Sse2PadddRegToReg(&RspRecompPos, x86_XMM1, x86_XMM3);
	Sse2PadddRegToReg(&RspRecompPos, x86_XMM2, x86_XMM4);

	if (writeToAccum) {
		SseMoveRegToReg(&RspRecompPos, x86_XMM3, x86_XMM1, SseType_QuadWord, TRUE);
		SseMoveRegToReg(&RspRecompPos, x86_XMM4, x86_XMM2, SseType_QuadWord, TRUE);
		Sse2PsradImmed(&RspRecompPos, x86_XMM3, 16);
		Sse2PsradImmed(&RspRecompPos, x86_XMM4, 16);
		Sse2PackSignedDWordRegToWordReg(&RspRecompPos, x86_XMM3, x86_XMM4);

		SseMoveAlignedRegToVariable(&RspRecompPos, x86_XMM3, &RSP_ACCUM_HIGH.UHW[0], "RSP_ACCUM_HIGH", SseType_QuadWord, TRUE);

		int reg1 = x86_XMM1;
		int reg2 = x86_XMM2;
		if (WriteToVectorDest) {
			SseMoveRegToReg(&RspRecompPos, x86_XMM3, x86_XMM1, SseType_QuadWord, TRUE);
			SseMoveRegToReg(&RspRecompPos, x86_XMM4, x86_XMM2, SseType_QuadWord, TRUE);
			reg1 = x86_XMM3;
			reg2 = x86_XMM4;
		}
		Sse2PslldImmed(&RspRecompPos, reg1, 16);
		Sse2PslldImmed(&RspRecompPos, reg2, 16);
		Sse2PsradImmed(&RspRecompPos, reg1, 16);
		Sse2PsradImmed(&RspRecompPos, reg2, 16);
		Sse2PackSignedDWordRegToWordReg(&RspRecompPos, reg1, reg2);

		SseMoveAlignedRegToVariable(&RspRecompPos, reg1, &RSP_ACCUM_MID.UHW[0], "RSP_ACCUM_MID", SseType_QuadWord, TRUE);
	}

	if (writeToVectorDest) {
		Sse2PsradImmed(&RspRecompPos, x86_XMM1, 1);
		Sse2PsradImmed(&RspRecompPos, x86_XMM2, 1);
		Sse2PackSignedDWordRegToWordReg(&RspRecompPos, x86_XMM1, x86_XMM2);
		Sse2PsrawImmed(&RspRecompPos, x86_XMM1, 4);
		Sse2PsllwImmed(&RspRecompPos, x86_XMM1, 4);

		sprintf(Reg, "RSP_Vect[%i]", RSPOpC.OP.V.vd);
		SseMoveAlignedRegToVariable(&RspRecompPos, x86_XMM1, &RSP_Vect[RSPOpC.OP.V.vd].UHW[0], Reg, SseType_QuadWord, TRUE);
	}

	return TRUE;
}

static BOOL Compile_Vector_VMACQ_AVX(BOOL writeToVectorDest, BOOL writeToAccum) {
	char Reg[256];

	/* Do our SSE checks here */
	if (IsAvxEnabled == FALSE || IsAvx2Enabled == FALSE || IsSseEnabled == FALSE || IsSse2Enabled == FALSE)
		return FALSE;

	SseMoveAlignedVariableToReg(&RspRecompPos, &RSP_ACCUM_HIGH.UHW[0], "RSP_ACCUM_HIGH", x86_XMM3, SseType_QuadWord, TRUE);
	SseMoveAlignedVariableToReg(&RspRecompPos, &RSP_ACCUM_MID.UHW[0], "RSP_ACCUM_MID", x86_XMM1, SseType_QuadWord, TRUE);
	AvxVPunpckHighWordsRegToReg256(&RspRecompPos, x86_YMM4, x86_YMM1, x86_YMM3);
	AvxVPunpckLowWordsRegToReg256(&RspRecompPos, x86_YMM3, x86_YMM1, x86_YMM3);
	AvxVInserti128RegToReg(&RspRecompPos, x86_YMM1, x86_YMM3, x86_XMM4, TRUE);

	// init the value to add to either 0x20 or -0x20 depending on the accumlator sign (in ymm2)
	AvxVPSradRegToReg256Immed(&RspRecompPos, x86_YMM2, x86_YMM1, 30);
	AvxCompareEqualDWordRegToReg256(&RspRecompPos, x86_YMM0, x86_YMM0, x86_YMM0); // ymm0=~0
	AvxVPxorRegToReg256(&RspRecompPos, x86_YMM2, x86_YMM2, x86_YMM0);
	AvxVPSrldRegToReg256Immed(&RspRecompPos, x86_YMM7, x86_YMM0, 31); // ymm7=1
	AvxVPorRegToReg256(&RspRecompPos, x86_YMM2, x86_YMM2, x86_YMM7);
	AvxVPSlldRegToReg256Immed(&RspRecompPos, x86_YMM2, x86_YMM2, 5);

	// make it 0 when accumulator is in the [0,0x20] range (in ymm2)
	AvxVPSradRegToReg256Immed(&RspRecompPos, x86_YMM3, x86_YMM1, 6);
	AvxVPxorRegToReg256(&RspRecompPos, x86_YMM6, x86_YMM6, x86_YMM6);
	AvxCompareEqualDWordRegToReg256(&RspRecompPos, x86_YMM3, x86_YMM3, x86_YMM6);
	AvxVPandnRegToReg256(&RspRecompPos, x86_YMM2, x86_YMM3, x86_YMM2);

	// make it 0 too if bit 0x20 is set in accumulator (in ymm3)
	AvxVPSlldRegToReg256Immed(&RspRecompPos, x86_YMM7, x86_YMM7, 5); // ymm7=0x20
	AvxVPandRegToReg256(&RspRecompPos, x86_YMM3, x86_YMM1, x86_YMM7);
	AvxCompareEqualDWordRegToReg256(&RspRecompPos, x86_YMM3, x86_YMM3, x86_YMM7);
	AvxVPandnRegToReg256(&RspRecompPos, x86_YMM2, x86_YMM3, x86_YMM2);

	// add result to accumulator
	AvxVPAdddRegToReg256(&RspRecompPos, x86_YMM1, x86_YMM1, x86_YMM2);

	if (writeToAccum) {
		AvxVPSlldRegToReg256Immed(&RspRecompPos, x86_YMM2, x86_YMM1, 16);
		AvxVPSradRegToReg256Immed(&RspRecompPos, x86_YMM2, x86_YMM2, 16);
		AvxVExtracti128RegToReg(&RspRecompPos, x86_XMM3, x86_YMM2, TRUE);
		AvxVPackSignedDWordRegToWordReg128(&RspRecompPos, x86_XMM2, x86_XMM2, x86_XMM3);
		SseMoveAlignedRegToVariable(&RspRecompPos, x86_XMM2, &RSP_ACCUM_MID.UHW[0], "RSP_ACCUM_MID", SseType_QuadWord, TRUE);

		AvxVPSradRegToReg256Immed(&RspRecompPos, x86_YMM2, x86_YMM1, 16);
		AvxVExtracti128RegToReg(&RspRecompPos, x86_XMM3, x86_YMM2, TRUE);
		AvxVPackSignedDWordRegToWordReg128(&RspRecompPos, x86_XMM2, x86_XMM2, x86_XMM3);
		SseMoveAlignedRegToVariable(&RspRecompPos, x86_XMM2, &RSP_ACCUM_HIGH.UHW[0], "RSP_ACCUM_HIGH", SseType_QuadWord, TRUE);
	}

	if (writeToVectorDest) {
		AvxVPSradRegToReg256Immed(&RspRecompPos, x86_YMM1, x86_YMM1, 1);
		AvxVExtracti128RegToReg(&RspRecompPos, x86_XMM2, x86_YMM1, TRUE);
		AvxVPackSignedDWordRegToWordReg128(&RspRecompPos, x86_XMM1, x86_XMM1, x86_XMM2);
		Sse2PsrawImmed(&RspRecompPos, x86_XMM1, 4);
		Sse2PsllwImmed(&RspRecompPos, x86_XMM1, 4);

		sprintf(Reg, "RSP_Vect[%i]", RSPOpC.OP.V.vd);
		SseMoveAlignedRegToVariable(&RspRecompPos, x86_XMM1, &RSP_Vect[RSPOpC.OP.V.vd].UHW[0], Reg, SseType_QuadWord, TRUE);
	}

	return TRUE;
}

void CompileRsp_Vector_VMACQ ( void ) {
	char Reg[256];
	int count, el;

	BOOL bWriteToAccum = WriteToAccum(High16BitAccum | Middle16BitAccum, RspCompilePC);
	BOOL bWriteToDest = WriteToVectorDest(RSPOpC.OP.V.vd, RspCompilePC);

#ifndef CompileVmacq
	InterpreterFallback((void*)RSP_Vector_VMACQ, "RSP_Vector_VMACQ"); return;
#endif

	RSP_CPU_Message("  %X %s", RspCompilePC, RSPOpcodeName(RSPOpC.OP.Hex, RspCompilePC));

	if (bWriteToDest == FALSE && bWriteToAccum == FALSE) {
		return;
	}

	if (TRUE == Compile_Vector_VMACQ_AVX(bWriteToDest, bWriteToAccum)) {
		return;
	}

	if (TRUE == Compile_Vector_VMACQ_SSE2(bWriteToDest, bWriteToAccum)) {
		return;
	}

	XorX86RegToX86Reg(&RspRecompPos, x86_EDX, x86_EDX);
	MoveConstToX86reg(&RspRecompPos, 0x7FFF, x86_ESI);
	MoveConstToX86reg(&RspRecompPos, 0xFFFF8000, x86_EDI);

	for (count = 0; count < 8; count++) {
		RSP_CPU_Message("     Iteration: %i", count);

		el = Indx[RSPOpC.OP.V.element].B[count];

		sprintf(Reg, "ACCUM_HIGH[%i]", el);
		MoveVariableToX86regHalf(&RspRecompPos, &RSP_ACCUM_HIGH.UHW[el], Reg, x86_EAX);
		ShiftLeftSignImmed(&RspRecompPos, x86_EAX, 16);
		sprintf(Reg, "ACCUM_MID[%i]", el);
		MoveVariableToX86regHalf(&RspRecompPos, &RSP_ACCUM_MID.UHW[el], Reg, x86_EAX);

		MoveX86RegToX86Reg(&RspRecompPos, x86_EAX, x86_EBX);
		ShiftRightSignImmed(&RspRecompPos, x86_EBX, 30);
		NotX86Reg(&RspRecompPos, x86_EBX);
		OrConstToX86Reg(&RspRecompPos, 1, x86_EBX);
		ShiftLeftSignImmed(&RspRecompPos, x86_EBX, 5);

		MoveX86RegToX86Reg(&RspRecompPos, x86_EAX, x86_ECX);
		ShiftRightSignImmed(&RspRecompPos, x86_ECX, 6);
		CompConstToX86reg(&RspRecompPos, x86_ECX, 0);
		CondMoveEqual(&RspRecompPos, x86_EBX, x86_EDX);

		TestConstToX86Reg(&RspRecompPos, 0x20, x86_EAX);
		CondMoveNotEqual(&RspRecompPos, x86_EBX, x86_EDX);

		AddX86RegToX86Reg(&RspRecompPos, x86_EAX, x86_EBX);

		if (bWriteToAccum) {
			sprintf(Reg, "ACCUM_MID[%i]", el);
			MoveX86regHalfToVariable(&RspRecompPos, x86_EAX, &RSP_ACCUM_MID.UHW[el], Reg);
			if (bWriteToDest) {
				MoveX86RegToX86Reg(&RspRecompPos, x86_EAX, x86_EBX);
				ShiftRightUnsignImmed(&RspRecompPos, x86_EBX, 16);
				sprintf(Reg, "ACCUM_HIGH[%i]", el);
				MoveX86regHalfToVariable(&RspRecompPos, x86_EBX, &RSP_ACCUM_HIGH.UHW[el], Reg);
			} else {
				ShiftRightUnsignImmed(&RspRecompPos, x86_EAX, 16);
				sprintf(Reg, "ACCUM_HIGH[%i]", el);
				MoveX86regHalfToVariable(&RspRecompPos, x86_EAX, &RSP_ACCUM_HIGH.UHW[el], Reg);
			}
		}

		if (bWriteToDest) {
			ShiftRightSignImmed(&RspRecompPos, x86_EAX, 1);
			CompX86RegToX86Reg(&RspRecompPos, x86_EAX, x86_ESI);
			CondMoveGreater(&RspRecompPos, x86_EAX, x86_ESI);
			CompX86RegToX86Reg(&RspRecompPos, x86_EAX, x86_EDI);
			CondMoveLess(&RspRecompPos, x86_EAX, x86_EDI);

			AndConstToX86Reg(&RspRecompPos, x86_EAX, (DWORD)~0xF);

			MoveX86regHalfToVariable(&RspRecompPos, x86_EAX, &RSP_Vect[RSPOpC.OP.V.vd].HW[el], "RSP_Vect[RSPOpC.OP.V.vd].HW[el]");
		}
	}
}

void CompileRsp_Vector_VMADL ( void ) {
	/*char Reg[256];
	int count, el, del;

	BOOL bOptimize = ((RSPOpC.rs & 0x0f) >= 8) ? TRUE : FALSE;
	BOOL bWriteToDest = WriteToVectorDest(RSPOpC.sa, CompilePC);*/

	#ifndef CompileVmadl
	InterpreterFallback((void*)RSP_Vector_VMADL,"RSP_Vector_VMADL"); return;
	#endif

	/*CPU_Message("  %X %s",CompilePC,RSPOpcodeName(RSPOpC.Hex,CompilePC));

	if (bOptimize == TRUE) {
		del = (RSPOpC.rs & 0x07) ^ 7;
		sprintf(Reg, "RSP_Vect[%i].HW[%i]", RSPOpC.rt, del);
		MoveZxVariableToX86regHalf(&RSP_Vect[RSPOpC.rt].HW[del], Reg, x86_EBX);
	}

	if (bWriteToDest == TRUE) {*/
		/*
		 * Prepare for conditional moves
		 */
/*		MoveConstToX86reg(0x00007FFF, x86_ESI);
		MoveConstToX86reg(0xFFFF8000, x86_EDI);

		Push(x86_EBP);
		MoveConstToX86reg(0x0000FFFF, x86_EBP);
	}

	for (count = 0; count < 8; count++) {
		CPU_Message("     Iteration: %i", count);
		el = Indx[RSPOpC.rs].B[count];
		del = EleSpec[RSPOpC.rs].B[el];

		sprintf(Reg, "RSP_Vect[%i].HW[%i]", RSPOpC.rd, el);
		MoveZxVariableToX86regHalf(&RSP_Vect[RSPOpC.rd].HW[el], Reg, x86_EAX);

		if (bOptimize == FALSE) {
			sprintf(Reg, "RSP_Vect[%i].HW[%i]", RSPOpC.rt, del);
			MoveZxVariableToX86regHalf(&RSP_Vect[RSPOpC.rt].HW[del], Reg, x86_EBX);
		}

		imulX86reg(x86_EBX);
		sprintf(Reg, "RSP_ACCUM[%i].W[0]", el);
		AddX86regToVariable(x86_EAX, &RSP_ACCUM[el].W[0], Reg);
		sprintf(Reg, "RSP_ACCUM[%i].W[1]", el);
		AdcConstToVariable(&RSP_ACCUM[el].W[1], Reg, 0);

		if (bWriteToDest != FALSE) {
			XorX86RegToX86Reg(x86_EDX, x86_EDX);
			MoveVariableToX86reg(&RSP_ACCUM[el].W[1], "RSP_ACCUM[el].W[1]", x86_EAX);
			MoveZxVariableToX86regHalf(&RSP_ACCUM[el].HW[1], "RSP_ACCUM[el].hW[1]", x86_ECX);

			CompX86RegToX86Reg(x86_EAX, x86_ESI);
			CondMoveGreater(x86_ECX, x86_EBP);
			CompX86RegToX86Reg(x86_EAX, x86_EDI);
			CondMoveLess(x86_ECX, x86_EDX);

			sprintf(Reg, "RSP_Vect[%i].HW[%i]", RSPOpC.sa, el);
			MoveX86regHalfToVariable(x86_ECX, &RSP_Vect[RSPOpC.sa].HW[el], Reg);
		}
	}

	if (bWriteToDest == TRUE) {
		Pop(x86_EBP);
	}*/
	LogMessage("TODO: CompileRsp_Vector_VMADL");
}

void CompileRsp_Vector_VMADM ( void ) {
	/*char Reg[256];
	int count, el, del;

	BOOL bOptimize = ((RSPOpC.rs & 0x0f) >= 8) ? TRUE : FALSE;
	BOOL bWriteToDest = WriteToVectorDest(RSPOpC.sa, CompilePC);
	BOOL bWriteToAccum = WriteToAccum(EntireAccum, CompilePC);*/

	#ifndef CompileVmadm
	InterpreterFallback((void*)RSP_Vector_VMADM,"RSP_Vector_VMADM"); return;
	#endif

	/*CPU_Message("  %X %s",CompilePC,RSPOpcodeName(RSPOpC.Hex,CompilePC));

	if (bOptimize == TRUE) {
		del = (RSPOpC.rs & 0x07) ^ 7;
		sprintf(Reg, "RSP_Vect[%i].HW[%i]", RSPOpC.rt, del);
		MoveZxVariableToX86regHalf(&RSP_Vect[RSPOpC.rt].HW[del], Reg, x86_EBX);
	}
	if (bWriteToDest == TRUE) {*/
		/*
		 * Prepare for conditional moves
		 */
/*		MoveConstToX86reg(0x00007fff, x86_ESI);
		MoveConstToX86reg(0xFFFF8000, x86_EDI);
	}

	Push(x86_EBP);
	sprintf(Reg, "RSP_Vect[%i].HW[0]", RSPOpC.rd);
	MoveOffsetToX86reg((DWORD)&RSP_Vect[RSPOpC.rd].HW[0], Reg, x86_EBP);

	if (bWriteToDest) {
		sprintf(Reg, "RSP_Vect[%i].HW[0]", RSPOpC.sa);
		MoveOffsetToX86reg((DWORD)&RSP_Vect[RSPOpC.sa].HW[0], Reg, x86_ECX);
	} else if (!bOptimize) {
		sprintf(Reg, "RSP_Vect[%i].HW[0]", RSPOpC.rt);
		MoveOffsetToX86reg((DWORD)&RSP_Vect[RSPOpC.rt].HW[0], Reg, x86_ECX);
	}

	for (count = 0; count < 8; count++) {
		CPU_Message("     Iteration: %i", count);
		el = Indx[RSPOpC.rs].B[count];
		del = EleSpec[RSPOpC.rs].B[el];*/
		
		/*sprintf(Reg, "RSP_Vect[%i].HW[%i]", RSPOpC.rd, el);
		MoveSxVariableToX86regHalf(&RSP_Vect[RSPOpC.rd].HW[el], Reg, x86_EAX);*/
/*		MoveSxX86RegPtrDispToX86RegHalf(x86_EBP, el * 2, x86_EAX);

		if (bOptimize == FALSE) {
			if (bWriteToDest == TRUE) {
				sprintf(Reg, "RSP_Vect[%i].HW[%i]", RSPOpC.rt, del);
				MoveZxVariableToX86regHalf(&RSP_Vect[RSPOpC.rt].HW[del], "RSP_Vect[RSPOpC.rt].HW[del]", x86_EBX);
			} else {
				MoveZxX86RegPtrDispToX86RegHalf(x86_ECX, del * 2, x86_EBX);
			}
		}

		imulX86reg(x86_EBX);

		MoveX86RegToX86Reg(x86_EAX, x86_EDX);
		ShiftRightSignImmed(x86_EDX, 16);
		ShiftLeftSignImmed(x86_EAX, 16);
		AddX86regToVariable(x86_EAX, &RSP_ACCUM[el].W[0], "RSP_ACCUM[el].W[0]");
		AdcX86regToVariable(x86_EDX, &RSP_ACCUM[el].W[1], "RSP_ACCUM[el].W[1]");

		if (bWriteToDest == TRUE) {*/
			/* For compare */
/*			sprintf(Reg, "RSP_ACCUM[%i].W[1]", el);
			MoveVariableToX86reg(&RSP_ACCUM[el].W[1], "RSP_ACCUM[el].W[1]", x86_EAX);

			CompX86RegToX86Reg(x86_EAX, x86_ESI);
			CondMoveGreater(x86_EAX, x86_ESI);
			CompX86RegToX86Reg(x86_EAX, x86_EDI);
			CondMoveLess(x86_EAX, x86_EDI);*/

			/*sprintf(Reg, "RSP_Vect[%i].HW[%i]", RSPOpC.sa, el);
			MoveX86regHalfToVariable(x86_EAX, &RSP_Vect[RSPOpC.sa].HW[el], Reg);*/
/*			MoveX86regHalfToX86regPointerDisp(x86_EAX, x86_ECX, el * 2);
		}
	}

	Pop(x86_EBP);*/
	LogMessage("TODO: CompileRsp_Vector_VMADM");
}

void CompileRsp_Vector_VMADN ( void ) {
	/*char Reg[256];
	int count, el, del;
	
	BOOL bOptimize = ((RSPOpC.rs & 0x0f) >= 8) ? TRUE : FALSE;
	BOOL bWriteToDest = WriteToVectorDest(RSPOpC.sa, CompilePC);
	BOOL bWriteToAccum = WriteToAccum(EntireAccum, CompilePC);*/

	#ifndef CompileVmadn
	InterpreterFallback((void*)RSP_Vector_VMADN,"RSP_Vector_VMADN"); return;
	#endif
	
	/*CPU_Message("  %X %s",CompilePC,RSPOpcodeName(RSPOpC.Hex,CompilePC));

	if (bOptimize == TRUE) {
		del = (RSPOpC.rs & 0x07) ^ 7;
		sprintf(Reg, "RSP_Vect[%i].HW[%i]", RSPOpC.rt, del);
		MoveSxVariableToX86regHalf(&RSP_Vect[RSPOpC.rt].HW[del], Reg, x86_EBX);
	} 
	if (bWriteToDest == TRUE) {*/
		/*
		 * Prepare for conditional moves
		 */
/*		MoveConstToX86reg(0x0000ffff, x86_ESI);
		MoveConstToX86reg(0x00000000, x86_EDI);
	}

	Push(x86_EBP);
	sprintf(Reg, "RSP_Vect[%i].HW[0]", RSPOpC.rd);
	MoveOffsetToX86reg((DWORD)&RSP_Vect[RSPOpC.rd].HW[0], Reg, x86_EBP);

	for (count = 0; count < 8; count++) {
		CPU_Message("     Iteration: %i", count);
		el = Indx[RSPOpC.rs].B[count];
		del = EleSpec[RSPOpC.rs].B[el];*/

		/*sprintf(Reg, "RSP_Vect[%i].UHW[%i]", RSPOpC.rd, el);
		MoveZxVariableToX86regHalf(&RSP_Vect[RSPOpC.rd].UHW[el], Reg, x86_EAX);*/
/*		MoveZxX86RegPtrDispToX86RegHalf(x86_EBP, el * 2, x86_EAX);

		if (bOptimize == FALSE) {
			sprintf(Reg, "RSP_Vect[%i].HW[%i]", RSPOpC.rt, del);
			MoveSxVariableToX86regHalf(&RSP_Vect[RSPOpC.rt].HW[del], Reg, x86_EBX);
		}

		imulX86reg(x86_EBX);

		MoveX86RegToX86Reg(x86_EAX, x86_EDX);
		ShiftRightSignImmed(x86_EDX, 16);
		ShiftLeftSignImmed(x86_EAX, 16);
		AddX86regToVariable(x86_EAX, &RSP_ACCUM[el].W[0], "RSP_ACCUM[el].HW[0]");
		AdcX86regToVariable(x86_EDX, &RSP_ACCUM[el].W[1], "RSP_ACCUM[el].HW[1]");

		if (bWriteToDest == TRUE) {*/
			/* For compare */
/*			sprintf(Reg, "RSP_ACCUM[%i].W[1]", el);
			MoveVariableToX86reg(&RSP_ACCUM[el].W[1], Reg, x86_EAX);*/

			/* For vector */
/*			sprintf(Reg, "RSP_ACCUM[%i].HW[1]", el);
			MoveVariableToX86regHalf(&RSP_ACCUM[el].HW[1], Reg, x86_ECX);*/

			/* Weird eh */
/*			CompConstToX86reg(x86_EAX, 0x7fff);
			CondMoveGreater(x86_ECX, x86_ESI);
			CompConstToX86reg(x86_EAX, -0x8000);
			CondMoveLess(x86_ECX, x86_EDI);

			sprintf(Reg, "RSP_Vect[%i].HW[%i]", RSPOpC.sa, el);
			MoveX86regHalfToVariable(x86_ECX, &RSP_Vect[RSPOpC.sa].HW[el], Reg);
		}
	}
	Pop(x86_EBP);*/
	LogMessage("TODO: Compile_Vector_VMADN");
}

void CompileRsp_Vector_VMADH ( void ) {
	/*char Reg[256];
	int count, el, del;

	BOOL bOptimize = ((RSPOpC.rs & 0x0f) >= 8) ? TRUE : FALSE;
	BOOL bWriteToDest = WriteToVectorDest(RSPOpC.sa, CompilePC);*/

	#ifndef CompileVmadh
	InterpreterFallback((void*)RSP_Vector_VMADH,"RSP_Vector_VMADH"); return;
	#endif

	/*CPU_Message("  %X %s",CompilePC,RSPOpcodeName(RSPOpC.Hex,CompilePC));

	if (bOptimize == TRUE) {
		del = (RSPOpC.rs & 0x07) ^ 7;
		sprintf(Reg, "RSP_Vect[%i].HW[%i]", RSPOpC.rt, del);
		MoveSxVariableToX86regHalf(&RSP_Vect[RSPOpC.rt].HW[del], Reg, x86_EBX);
	} 
	
	if (bWriteToDest == TRUE) {*/
		/*
		 * Prepare for conditional moves
		 */
/*		MoveConstToX86reg(0x00007fff, x86_ESI);
		MoveConstToX86reg(0xFFFF8000, x86_EDI);
	}

	if (bWriteToDest == FALSE && bOptimize == TRUE) {
		Push(x86_EBP);
		sprintf(Reg, "RSP_Vect[%i].HW[0]", RSPOpC.rd);
		MoveOffsetToX86reg((DWORD)&RSP_Vect[RSPOpC.rd].HW[0], Reg, x86_EBP);*/

		/* 
		 * Pipe lined segment 0
		 */
/*		MoveSxX86RegPtrDispToX86RegHalf(x86_EBP, 0, x86_EAX);
		MoveSxX86RegPtrDispToX86RegHalf(x86_EBP, 2, x86_ECX);
		MoveSxX86RegPtrDispToX86RegHalf(x86_EBP, 4, x86_EDI);
		MoveSxX86RegPtrDispToX86RegHalf(x86_EBP, 6, x86_ESI);

		ImulX86RegToX86Reg(x86_EAX, x86_EBX);
		ImulX86RegToX86Reg(x86_ECX, x86_EBX);
		ImulX86RegToX86Reg(x86_EDI, x86_EBX);
		ImulX86RegToX86Reg(x86_ESI, x86_EBX);

		sprintf(Reg, "RSP_ACCUM[%i].W[1]", 0);
		AddX86regToVariable(x86_EAX, &RSP_ACCUM[0].W[1], Reg);
		sprintf(Reg, "RSP_ACCUM[%i].W[1]", 1);
		AddX86regToVariable(x86_ECX, &RSP_ACCUM[1].W[1], Reg);
		sprintf(Reg, "RSP_ACCUM[%i].W[1]", 2);
		AddX86regToVariable(x86_EDI, &RSP_ACCUM[2].W[1], Reg);
		sprintf(Reg, "RSP_ACCUM[%i].W[1]", 3);
		AddX86regToVariable(x86_ESI, &RSP_ACCUM[3].W[1], Reg);*/

		/* 
		 * Pipe lined segment 1
		 */
/*		MoveSxX86RegPtrDispToX86RegHalf(x86_EBP,  8, x86_EAX);
		MoveSxX86RegPtrDispToX86RegHalf(x86_EBP, 10, x86_ECX);
		MoveSxX86RegPtrDispToX86RegHalf(x86_EBP, 12, x86_EDI);
		MoveSxX86RegPtrDispToX86RegHalf(x86_EBP, 14, x86_ESI);

		ImulX86RegToX86Reg(x86_EAX, x86_EBX);
		ImulX86RegToX86Reg(x86_ECX, x86_EBX);
		ImulX86RegToX86Reg(x86_EDI, x86_EBX);
		ImulX86RegToX86Reg(x86_ESI, x86_EBX);

		sprintf(Reg, "RSP_ACCUM[%i].W[1]", 4);
		AddX86regToVariable(x86_EAX, &RSP_ACCUM[4].W[1], Reg);
		sprintf(Reg, "RSP_ACCUM[%i].W[1]", 5);
		AddX86regToVariable(x86_ECX, &RSP_ACCUM[5].W[1], Reg);
		sprintf(Reg, "RSP_ACCUM[%i].W[1]", 6);
		AddX86regToVariable(x86_EDI, &RSP_ACCUM[6].W[1], Reg);
		sprintf(Reg, "RSP_ACCUM[%i].W[1]", 7);
		AddX86regToVariable(x86_ESI, &RSP_ACCUM[7].W[1], Reg);

		Pop(x86_EBP);
	} else {
		Push(x86_EBP);
		sprintf(Reg, "RSP_Vect[%i].HW[0]", RSPOpC.rd);
		MoveOffsetToX86reg((DWORD)&RSP_Vect[RSPOpC.rd].HW[0], Reg, x86_EBP);

		if (bWriteToDest) {
			sprintf(Reg, "RSP_Vect[%i].HW[0]", RSPOpC.sa);
			MoveOffsetToX86reg((DWORD)&RSP_Vect[RSPOpC.sa].HW[0], Reg, x86_ECX);
		} else if (!bOptimize) {
			sprintf(Reg, "RSP_Vect[%i].HW[0]", RSPOpC.rt);
			MoveOffsetToX86reg((DWORD)&RSP_Vect[RSPOpC.rt].HW[0], Reg, x86_ECX);
		}

		for (count = 0; count < 8; count++) {
			CPU_Message("     Iteration: %i", count);
			el = Indx[RSPOpC.rs].B[count];
			del = EleSpec[RSPOpC.rs].B[el];*/

			/*sprintf(Reg, "RSP_Vect[%i].HW[%i]", RSPOpC.rd, el);
			MoveSxVariableToX86regHalf(&RSP_Vect[RSPOpC.rd].HW[el], Reg, x86_EAX);*/
/*			MoveSxX86RegPtrDispToX86RegHalf(x86_EBP, el * 2, x86_EAX);

			if (bOptimize == FALSE) {
				if (bWriteToDest == TRUE) {
					sprintf(Reg, "RSP_Vect[%i].HW[%i]", RSPOpC.rt, del);
					MoveSxVariableToX86regHalf(&RSP_Vect[RSPOpC.rt].HW[del], Reg, x86_EBX);
				} else {
					MoveSxX86RegPtrDispToX86RegHalf(x86_ECX, del * 2, x86_EBX);
				}
			}

			imulX86reg(x86_EBX);
			sprintf(Reg, "RSP_ACCUM[%i].W[1]", el);
			AddX86regToVariable(x86_EAX, &RSP_ACCUM[el].W[1], Reg);

			if (bWriteToDest == TRUE) {
				MoveVariableToX86reg(&RSP_ACCUM[el].W[1], "RSP_ACCUM[el].W[1]", x86_EAX);

				CompX86RegToX86Reg(x86_EAX, x86_ESI);
				CondMoveGreater(x86_EAX, x86_ESI);
				CompX86RegToX86Reg(x86_EAX, x86_EDI);
				CondMoveLess(x86_EAX, x86_EDI);*/

				/*sprintf(Reg, "RSP_Vect[%i].HW[%i]", RSPOpC.sa, el);
				MoveX86regHalfToVariable(x86_EAX, &RSP_Vect[RSPOpC.sa].HW[el], Reg);*/
/*				MoveX86regHalfToX86regPointerDisp(x86_EAX, x86_ECX, el * 2);
			}
		}
		Pop(x86_EBP);
	}*/
	LogMessage("TODO: CompileRsp_Vector_VMADH");
}

/*BOOL Compile_Vector_VADD_MMX ( void ) {
	char Reg[256];*/

	/* Do our MMX checks here */
/*	if (IsMmxEnabled == FALSE)
		return FALSE;
	if ((RSPOpC.rs & 0x0f) >= 2 && (RSPOpC.rs & 0x0f) <= 7 && IsMmx2Enabled == FALSE)
		return FALSE;

	sprintf(Reg, "RSP_Vect[%i].UHW[0]", RSPOpC.rd);
	MmxMoveQwordVariableToReg(x86_MM0, &RSP_Vect[RSPOpC.rd].UHW[0], Reg);
	sprintf(Reg, "RSP_Vect[%i].UHW[4]", RSPOpC.rd);
	MmxMoveQwordVariableToReg(x86_MM1, &RSP_Vect[RSPOpC.rd].UHW[4], Reg);

	if ((RSPOpC.rs & 15) >= 8) {
		RSP_Element2Mmx(x86_MM2);
		MmxPaddswRegToReg(x86_MM0, x86_MM2);
		MmxPaddswRegToReg(x86_MM1, x86_MM2);
	} else if ((RSPOpC.rs & 15) < 2) {
		sprintf(Reg, "RSP_Vect[%i].HW[0]", RSPOpC.rt);
		MmxPaddswVariableToReg(x86_MM0, &RSP_Vect[RSPOpC.rt].HW[0], Reg);
		sprintf(Reg, "RSP_Vect[%i].HW[4]", RSPOpC.rt);
		MmxPaddswVariableToReg(x86_MM1, &RSP_Vect[RSPOpC.rt].HW[4], Reg);
	} else {
		RSP_MultiElement2Mmx(x86_MM2, x86_MM3);
		MmxPaddswRegToReg(x86_MM0, x86_MM2);
		MmxPaddswRegToReg(x86_MM1, x86_MM3);
	}

	sprintf(Reg, "RSP_Vect[%i].UHW[0]", RSPOpC.sa);
	MmxMoveQwordRegToVariable(x86_MM0, &RSP_Vect[RSPOpC.sa].UHW[0], Reg);
	sprintf(Reg, "RSP_Vect[%i].UHW[4]", RSPOpC.sa);
	MmxMoveQwordRegToVariable(x86_MM1, &RSP_Vect[RSPOpC.sa].UHW[4], Reg);
	MmxEmptyMultimediaState();

	return TRUE;
}*/

void CompileRsp_Vector_VADD ( void ) {
	/*char Reg[256];
	int count, el, del;

	BOOL bWriteToDest = WriteToVectorDest(RSPOpC.sa, CompilePC);
	BOOL bElement = ((RSPOpC.rs & 0x0f) >= 8) ? TRUE : FALSE;
	BOOL bWriteToAccum = WriteToAccum(Low16BitAccum, CompilePC);
	BOOL bFlagUseage = UseRspFlags(CompilePC);*/

	#ifndef CompileVadd
	InterpreterFallback((void*)RSP_Vector_VADD,"RSP_Vector_VADD"); return;
	#endif

	/*CPU_Message("  %X %s",CompilePC,RSPOpcodeName(RSPOpC.Hex,CompilePC));

	if (bWriteToAccum == FALSE && bFlagUseage == FALSE) {
		if (TRUE == Compile_Vector_VADD_MMX())
			return;
	}

	if (bElement == TRUE) {
		del = (RSPOpC.rs & 0x07) ^ 7;
		sprintf(Reg, "RSP_Vect[%i].HW[%i]", RSPOpC.rt, del);
		MoveSxVariableToX86regHalf(&RSP_Vect[RSPOpC.rt].HW[del], Reg, x86_EBX);
	}
	if (bWriteToDest == TRUE) {*/
		/*
		 * Prepare for conditional moves
		 */
/*		MoveConstToX86reg(0x00007fff, x86_ESI);
		MoveConstToX86reg(0xffff8000, x86_EDI);
	}*/
	
	/* Used for involking x86 carry flag */
/*	XorX86RegToX86Reg(x86_ECX, x86_ECX);
	Push(x86_EBP);
	MoveVariableToX86reg(&RSP_Flags[0].UW, "RSP_Flags[0].UW", x86_EBP);

	for (count = 0; count < 8; count++) {
		CPU_Message("     Iteration: %i", count);
		el = Indx[RSPOpC.rs].B[count];
		del = EleSpec[RSPOpC.rs].B[el];
	
		sprintf(Reg, "RSP_Vect[%i].HW[%i]", RSPOpC.rd, el);
		MoveSxVariableToX86regHalf(&RSP_Vect[RSPOpC.rd].HW[el], Reg, x86_EAX);

		if (bElement == FALSE) {
			sprintf(Reg, "RSP_Vect[%i].HW[%i]", RSPOpC.rt, del);
			MoveSxVariableToX86regHalf(&RSP_Vect[RSPOpC.rt].HW[del], Reg, x86_EBX);
		}
		
		MoveX86RegToX86Reg(x86_EBP, x86_EDX);
		AndConstToX86Reg(x86_EDX, 1 << (7 - el));
		CompX86RegToX86Reg(x86_ECX, x86_EDX);

		AdcX86RegToX86Reg(x86_EAX, x86_EBX);

		if (bWriteToAccum == TRUE) {
			MoveX86regHalfToVariable(x86_EAX, &RSP_ACCUM[el].HW[1], "RSP_ACCUM[el].HW[1]");
		}
		if (bWriteToDest == TRUE) {
			CompX86RegToX86Reg(x86_EAX, x86_ESI);
			CondMoveGreater(x86_EAX, x86_ESI);
			CompX86RegToX86Reg(x86_EAX, x86_EDI);
			CondMoveLess(x86_EAX, x86_EDI);

			sprintf(Reg, "RSP_Vect[%i].HW[%i]", RSPOpC.sa, el);
			MoveX86regHalfToVariable(x86_EAX, &RSP_Vect[RSPOpC.sa].HW[el], Reg);
		}
	}
	MoveConstToVariable(0, &RSP_Flags[0].UW, "RSP_Flags[0].UW");
	Pop(x86_EBP);*/
	LogMessage("TODO: CompileRsp_Vector_VADD");
}

void CompileRsp_Vector_VSUB ( void ) {
	/*char Reg[256];
	int count, el, del;

	BOOL bWriteToDest = WriteToVectorDest(RSPOpC.sa, CompilePC);
	BOOL bOptimize = ((RSPOpC.rs & 0x0f) >= 8) ? TRUE : FALSE;
	BOOL bWriteToAccum = WriteToAccum(Low16BitAccum, CompilePC);
	BOOL bZeroReg = ((RSPOpC.rs & 0xF) < 2 && (RSPOpC.rt == RSPOpC.rd)) ? TRUE : FALSE;*/

	#ifndef CompileVsub
	InterpreterFallback((void*)RSP_Vector_VSUB,"RSP_Vector_VSUB"); return;
	#endif

	/*CPU_Message("  %X %s",CompilePC,RSPOpcodeName(RSPOpC.Hex,CompilePC));
	Push(x86_EBP);*/

	/* Used for involking the x86 carry flag */
/*	XorX86RegToX86Reg(x86_ECX, x86_ECX);
	MoveVariableToX86reg(&RSP_Flags[0].UW, "RSP_Flags[0].UW", x86_EBP);

	if (bOptimize == TRUE) {
		del = (RSPOpC.rs & 0x07) ^ 7;
		sprintf(Reg, "RSP_Vect[%i].HW[%i]", RSPOpC.rt, del);
		MoveSxVariableToX86regHalf(&RSP_Vect[RSPOpC.rt].HW[del], Reg, x86_EBX);
	}

	if (bWriteToDest == TRUE) {*/
		/*
		 * Prepare for conditional moves
		 */
/*		MoveConstToX86reg(0x00007fff, x86_ESI);
		MoveConstToX86reg(0xffff8000, x86_EDI);
	}

	for (count = 0; count < 8; count++) {
		CPU_Message("     Iteration: %i", count);
		el = Indx[RSPOpC.rs].B[count];
		del = EleSpec[RSPOpC.rs].B[el];

		MoveSxVariableToX86regHalf(&RSP_Vect[RSPOpC.rd].HW[el], "RSP_Vect[RSPOpC.rd].HW[el]", x86_EAX);
		if (bOptimize == FALSE) {
			sprintf(Reg, "RSP_Vect[%i].HW[%i]", RSPOpC.rt, del);
			MoveSxVariableToX86regHalf(&RSP_Vect[RSPOpC.rt].HW[del], Reg, x86_EBX);
		}
	
		MoveX86RegToX86Reg(x86_EBP, x86_EDX);
		AndConstToX86Reg(x86_EDX, 1 << (7 - el));
		CompX86RegToX86Reg(x86_ECX, x86_EDX);

		SbbX86RegToX86Reg(x86_EAX, x86_EBX);

		if (bWriteToAccum == TRUE) {
			MoveX86regHalfToVariable(x86_EAX, &RSP_ACCUM[el].HW[1], "RSP_ACCUM[el].HW[1]");
		}
		if (bWriteToDest == TRUE) {
			CompX86RegToX86Reg(x86_EAX, x86_ESI);
			CondMoveGreater(x86_EAX, x86_ESI);
			CompX86RegToX86Reg(x86_EAX, x86_EDI);
			CondMoveLess(x86_EAX, x86_EDI);

			sprintf(Reg, "RSP_Vect[%i].HW[%i]", RSPOpC.sa, el);
			MoveX86regHalfToVariable(x86_EAX, &RSP_Vect[RSPOpC.sa].HW[el], Reg);
		}
	}

	MoveConstToVariable(0, &RSP_Flags[0].UW, "RSP_Flags[0].UW");
	Pop(x86_EBP);*/
	LogMessage("TODO: CompileRsp_Vector_VSUB");
}

void CompileRsp_Vector_VSUT ( void ) {
	InterpreterFallback((void*)RSP_Vector_VSUT, "RSP_Vector_VSUT");
}

void CompileRsp_Vector_VABS ( void ) {
	/*int count, el, del;
	char Reg[256];
	
	BOOL bWriteToDest = WriteToVectorDest(RSPOpC.sa, CompilePC);
	BOOL bWriteToAccum = WriteToAccum(Low16BitAccum, CompilePC);*/

	#ifndef CompileVabs
	InterpreterFallback((void*)RSP_Vector_VABS,"RSP_Vector_VABS"); return;
	#endif

	/*CPU_Message("  %X %s",CompilePC,RSPOpcodeName(RSPOpC.Hex,CompilePC));

	for (count = 0; count < 8; count++) {
		CPU_Message("     Iteration: %i", count);
		el = Indx[RSPOpC.rs].B[count];
		del = EleSpec[RSPOpC.rs].B[el];

		if (RSPOpC.rd == RSPOpC.rt && (RSPOpC.rs & 0xF) < 2) {*/
			/**
			** Optimize: EDI/ESI unused, and ECX is const etc
			***/

/*			sprintf(Reg, "RSP_Vect[%i].HW[%i]", RSPOpC.rd, el);
			MoveSxVariableToX86regHalf(&RSP_Vect[RSPOpC.rd].HW[el], Reg, x86_EAX);*/

			/*** Obtain the negative of the source ****/
/*			MoveX86RegToX86Reg(x86_EAX, x86_EBX);
			NegateX86reg(x86_EBX);*/
		
			/**
			** determine negative value, 
			** note: negate(FFFF8000h) == 00008000h 
			***/

/*			MoveConstToX86reg(0x7fff, x86_ECX);
			CompConstToX86reg(x86_EBX, 0x00008000);
			CondMoveEqual(x86_EBX, x86_ECX);*/

			/* sign clamp, dest = (eax >= 0) ? eax : ebx */
/*			CompConstToX86reg(x86_EAX, 0);
			CondMoveLess(x86_EAX, x86_EBX);

			if (bWriteToDest == TRUE) {
				sprintf(Reg, "RSP_Vect[%i].HW[%i]", RSPOpC.sa, el);
				MoveX86regHalfToVariable(x86_EAX, &RSP_Vect[RSPOpC.sa].HW[el], Reg);
			}
			if (bWriteToAccum == TRUE) {
				sprintf(Reg, "RSP_ACCUM[%i].HW[1]", el);
				MoveX86regHalfToVariable(x86_EAX, &RSP_ACCUM[el].HW[1], Reg);
			}
		} else {*/
			/**
			** Optimize: ESI unused, and EDX is const etc
			***/

/*			sprintf(Reg, "RSP_Vect[%i].HW[%i]", RSPOpC.rd, el);
			MoveSxVariableToX86regHalf(&RSP_Vect[RSPOpC.rd].HW[el], Reg, x86_EAX);
			sprintf(Reg, "RSP_Vect[%i].HW[%i]", RSPOpC.rt, del);
			MoveSxVariableToX86regHalf(&RSP_Vect[RSPOpC.rt].HW[del], Reg, x86_EBX);*/

			/*** Obtain the negative of the source ****/
/*			MoveX86RegToX86Reg(x86_EBX, x86_ECX);
			NegateX86reg(x86_EBX);*/

			/**
			** determine negative value, 
			** note: negate(FFFF8000h) == 00008000h 
			***/

/*			MoveConstToX86reg(0x7fff, x86_EDX);
			CompConstToX86reg(x86_EBX, 0x00008000);
			CondMoveEqual(x86_EBX, x86_EDX);*/

			/* sign clamp, dest = (eax >= 0) ? ecx : ebx */
/*			CompConstToX86reg(x86_EAX, 0);
			CondMoveGreaterEqual(x86_EDI, x86_ECX);
			CondMoveLess(x86_EDI, x86_EBX);

			if (bWriteToDest == TRUE) {
				sprintf(Reg, "RSP_Vect[%i].HW[%i]", RSPOpC.sa, el);
				MoveX86regHalfToVariable(x86_EDI, &RSP_Vect[RSPOpC.sa].HW[el], Reg);
			}
			if (bWriteToAccum == TRUE) {
				sprintf(Reg, "RSP_ACCUM[%i].HW[1]", el);
				MoveX86regHalfToVariable(x86_EDI, &RSP_ACCUM[el].HW[1], Reg);	
			}
		}
	}*/
	LogMessage("TODO: CompileRsp_Vector_VABS");
}

void CompileRsp_Vector_VADDC ( void ) {
	/*char Reg[256];
	int count, el, del;

	BOOL bWriteToDest = WriteToVectorDest(RSPOpC.sa, CompilePC);
	BOOL bWriteToAccum = WriteToAccum(Low16BitAccum, CompilePC);
	BOOL bElement = ((RSPOpC.rs & 0x0f) >= 8) ? TRUE : FALSE;*/
	
	#ifndef CompileVaddc
	InterpreterFallback((void*)RSP_Vector_VADDC,"RSP_Vector_VADDC"); return;
	#endif

	/*CPU_Message("  %X %s",CompilePC,RSPOpcodeName(RSPOpC.Hex,CompilePC));

	if (bElement == TRUE) {
		del = (RSPOpC.rs & 0x07) ^ 7;
		sprintf(Reg, "RSP_Vect[%i].HW[%i]", RSPOpC.rt, del);
		MoveZxVariableToX86regHalf(&RSP_Vect[RSPOpC.rt].HW[del], Reg, x86_EBX);
	}*/

	/* Initialize flag register */
/*	XorX86RegToX86Reg(x86_ECX, x86_ECX);

	Push(x86_EBP);
	sprintf(Reg, "RSP_Vect[%i].HW[0]", RSPOpC.rd);
	MoveOffsetToX86reg((DWORD)&RSP_Vect[RSPOpC.rd].HW[0], Reg, x86_EBP);

	for (count = 0; count < 8; count++) {
		CPU_Message("     Iteration: %i", count);
		el = Indx[RSPOpC.rs].B[count];
		del = EleSpec[RSPOpC.rs].B[el];*/
	
		/*sprintf(Reg, "RSP_Vect[%i].HW[%i]", RSPOpC.rd, el);
		MoveZxVariableToX86regHalf(&RSP_Vect[RSPOpC.rd].HW[el], Reg, x86_EAX);*/
/*		MoveZxX86RegPtrDispToX86RegHalf(x86_EBP, el * 2, x86_EAX);

		if (bElement == FALSE) {
			sprintf(Reg, "RSP_Vect[%i].HW[%i]", RSPOpC.rt, del);
			MoveZxVariableToX86regHalf(&RSP_Vect[RSPOpC.rt].HW[del], Reg, x86_EBX);
		}

		AddX86RegToX86Reg(x86_EAX, x86_EBX);

		XorX86RegToX86Reg(x86_EDX, x86_EDX);
		TestConstToX86Reg(0xFFFF0000, x86_EAX);
		Setnz(x86_EDX);
		if ((7 - el) != 0) {
			ShiftLeftSignImmed(x86_EDX, 7 - el);
		}
		OrX86RegToX86Reg(x86_ECX, x86_EDX);

		if (bWriteToAccum == TRUE) {
			MoveX86regHalfToVariable(x86_EAX, &RSP_ACCUM[el].HW[1], "RSP_ACCUM[el].HW[1]");
		}
		if (bWriteToDest == TRUE) {
			sprintf(Reg, "RSP_Vect[%i].HW[%i]", RSPOpC.sa, el);
			MoveX86regHalfToVariable(x86_EAX, &RSP_Vect[RSPOpC.sa].HW[el], Reg);
		}
	}
	MoveX86regToVariable(x86_ECX, &RSP_Flags[0].UW, "RSP_Flags[0].UW");
	Pop(x86_EBP);*/
	LogMessage("TODO: CompileRsp_Vector_VADDC");
}

void CompileRsp_Vector_VSUBC ( void ) {
	/*char Reg[256];
	int count, el, del;

	BOOL bWriteToDest = WriteToVectorDest(RSPOpC.sa, CompilePC);
	BOOL bWriteToAccum = WriteToAccum(Low16BitAccum, CompilePC);
	BOOL bElement = ((RSPOpC.rs & 0x0f) >= 8) ? TRUE : FALSE;*/
	
	#ifndef CompileVsubc
	InterpreterFallback((void*)RSP_Vector_VSUBC,"RSP_Vector_VSUBC"); return;
	#endif

	/*CPU_Message("  %X %s",CompilePC,RSPOpcodeName(RSPOpC.Hex,CompilePC));

	if (bElement == TRUE) {
		del = (RSPOpC.rs & 0x07) ^ 7;
		sprintf(Reg, "RSP_Vect[%i].HW[%i]", RSPOpC.rt, del);
		MoveZxVariableToX86regHalf(&RSP_Vect[RSPOpC.rt].HW[del], Reg, x86_EBX);
	}*/

	/* Initialize flag register */
/*	XorX86RegToX86Reg(x86_ECX, x86_ECX);

	for (count = 0; count < 8; count++) {
		CPU_Message("     Iteration: %i", count);
		el = Indx[RSPOpC.rs].B[count];
		del = EleSpec[RSPOpC.rs].B[el];
	
		sprintf(Reg, "RSP_Vect[%i].HW[%i]", RSPOpC.rd, el);
		MoveZxVariableToX86regHalf(&RSP_Vect[RSPOpC.rd].HW[el], Reg, x86_EAX);

		if (bElement == FALSE) {
			sprintf(Reg, "RSP_Vect[%i].HW[%i]", RSPOpC.rt, del);
			MoveZxVariableToX86regHalf(&RSP_Vect[RSPOpC.rt].HW[del], Reg, x86_EBX);
		}

		SubX86RegToX86Reg(x86_EAX, x86_EBX);

		XorX86RegToX86Reg(x86_EDX, x86_EDX);
		TestConstToX86Reg(0x0000FFFF, x86_EAX);
		Setnz(x86_EDX);
		ShiftLeftSignImmed(x86_EDX, 15 - el);
		OrX86RegToX86Reg(x86_ECX, x86_EDX);

		XorX86RegToX86Reg(x86_EDX, x86_EDX);
		TestConstToX86Reg(0xFFFF0000, x86_EAX);
		Setnz(x86_EDX);
		ShiftLeftSignImmed(x86_EDX, 7 - el);
		OrX86RegToX86Reg(x86_ECX, x86_EDX);

		if (bWriteToAccum == TRUE) {
			MoveX86regHalfToVariable(x86_EAX, &RSP_ACCUM[el].HW[1], "RSP_ACCUM[el].HW[1]");
		}
		if (bWriteToDest == TRUE) {
			sprintf(Reg, "RSP_Vect[%i].HW[%i]", RSPOpC.sa, el);
			MoveX86regHalfToVariable(x86_EAX, &RSP_Vect[RSPOpC.sa].HW[el], Reg);
		}
	}
	MoveX86regToVariable(x86_ECX, &RSP_Flags[0].UW, "RSP_Flags[0].UW");*/
	LogMessage("TODO: CompileRsp_Vector_VSUBC");
}

void CompileRsp_Vector_VADDB ( void ) {
	InterpreterFallback((void*)RSP_Vector_VADDB, "RSP_Vector_VADDB");
}

void CompileRsp_Vector_VSUBB ( void ) {
	InterpreterFallback((void*)RSP_Vector_VSUBB, "RSP_Vector_VSUBB");
}

void CompileRsp_Vector_VACCB(void) {
	InterpreterFallback((void*)RSP_Vector_VACCB, "RSP_Vector_VACCB");
}

void CompileRsp_Vector_VSUCB(void) {
	InterpreterFallback((void*)RSP_Vector_VSUCB, "RSP_Vector_VSUCB");
}

void CompileRsp_Vector_VSAD(void) {
	InterpreterFallback((void*)RSP_Vector_VSAD, "RSP_Vector_VSAD");
}

void CompileRsp_Vector_VSAC(void) {
	InterpreterFallback((void*)RSP_Vector_VSAC, "RSP_Vector_VSAC");
}

void CompileRsp_Vector_VSUM(void) {
	InterpreterFallback((void*)RSP_Vector_VSUM, "RSP_Vector_VSUM");
}

void CompileRsp_Vector_VSAR ( void ) {
	/*char Reg[256];
	DWORD Word;*/

	#ifndef CompileVsar
	InterpreterFallback((void*)RSP_Vector_VSAR,"RSP_Vector_VSAR"); return;
	#endif

	/*CPU_Message("  %X %s",CompilePC,RSPOpcodeName(RSPOpC.Hex,CompilePC));

	switch ((RSPOpC.rs & 0xF)) {
	case 8: Word = 3; break;
	case 9: Word = 2; break;
	case 10: Word = 1; break;
	default:
		MoveConstToVariable(0, &RSP_Vect[RSPOpC.sa].DW[1], "RSP_Vect[RSPOpC.sa].DW[1]");
		MoveConstToVariable(0, &RSP_Vect[RSPOpC.sa].DW[0], "RSP_Vect[RSPOpC.sa].DW[0]");
		return;
	}

	sprintf(Reg, "RSP_ACCUM[1].HW[%i]", Word);
	MoveVariableToX86regHalf(&RSP_ACCUM[1].HW[Word], Reg, x86_EAX);
	sprintf(Reg, "RSP_ACCUM[3].HW[%i]", Word);
	MoveVariableToX86regHalf(&RSP_ACCUM[3].HW[Word], Reg, x86_EBX);
	sprintf(Reg, "RSP_ACCUM[5].HW[%i]", Word);
	MoveVariableToX86regHalf(&RSP_ACCUM[5].HW[Word], Reg, x86_ECX);
	sprintf(Reg, "RSP_ACCUM[7].HW[%i]", Word);
	MoveVariableToX86regHalf(&RSP_ACCUM[7].HW[Word], Reg, x86_EDX);

	ShiftLeftSignImmed(x86_EAX, 16);
	ShiftLeftSignImmed(x86_EBX, 16);
	ShiftLeftSignImmed(x86_ECX, 16);
	ShiftLeftSignImmed(x86_EDX, 16);

	sprintf(Reg, "RSP_ACCUM[0].HW[%i]", Word);
	MoveVariableToX86regHalf(&RSP_ACCUM[0].HW[Word], Reg, x86_EAX);
	sprintf(Reg, "RSP_ACCUM[2].HW[%i]", Word);
	MoveVariableToX86regHalf(&RSP_ACCUM[2].HW[Word], Reg, x86_EBX);
	sprintf(Reg, "RSP_ACCUM[4].HW[%i]", Word);
	MoveVariableToX86regHalf(&RSP_ACCUM[4].HW[Word], Reg, x86_ECX);
	sprintf(Reg, "RSP_ACCUM[6].HW[%i]", Word);
	MoveVariableToX86regHalf(&RSP_ACCUM[6].HW[Word], Reg, x86_EDX);

	sprintf(Reg, "RSP_Vect[%i].HW[0]", RSPOpC.sa);
	MoveX86regToVariable(x86_EAX, &RSP_Vect[RSPOpC.sa].HW[0], Reg);
	sprintf(Reg, "RSP_Vect[%i].HW[2]", RSPOpC.sa);
	MoveX86regToVariable(x86_EBX, &RSP_Vect[RSPOpC.sa].HW[2], Reg);
	sprintf(Reg, "RSP_Vect[%i].HW[4]", RSPOpC.sa);
	MoveX86regToVariable(x86_ECX, &RSP_Vect[RSPOpC.sa].HW[4], Reg);
	sprintf(Reg, "RSP_Vect[%i].HW[6]", RSPOpC.sa);
	MoveX86regToVariable(x86_EDX, &RSP_Vect[RSPOpC.sa].HW[6], Reg);*/
	LogMessage("TODO: CompileRsp_Vector_VSAR");
}

void CompileRsp_Vector_V30(void) {
	InterpreterFallback((void*)RSP_Vector_V30, "RSP_Vector_V30");
}

void CompileRsp_Vector_V31(void) {
	InterpreterFallback((void*)RSP_Vector_V31, "RSP_Vector_V31");
}

void CompileRsp_Vector_VLT ( void ) {
	InterpreterFallback((void*)RSP_Vector_VLT,"RSP_Vector_VLT");
}

void CompileRsp_Vector_VEQ ( void ) {
	InterpreterFallback((void*)RSP_Vector_VEQ,"RSP_Vector_VEQ");
}

void CompileRsp_Vector_VNE ( void ) {
	InterpreterFallback((void*)RSP_Vector_VNE,"RSP_Vector_VNE");
}

/*BOOL Compile_Vector_VGE_MMX(void) {
	char Reg[256];

	if ((RSPOpC.rs & 0xF) >= 2 && (RSPOpC.rs & 0xF) <= 7 && IsMmx2Enabled == FALSE)
		return FALSE;

	CPU_Message("  %X %s",CompilePC,RSPOpcodeName(RSPOpC.Hex,CompilePC));
	MoveConstToVariable(0, &RSP_Flags[1].UW, "RSP_Flags[1].UW");

	sprintf(Reg, "RSP_Vect[%i].HW[0]", RSPOpC.rd);
	MmxMoveQwordVariableToReg(x86_MM0, &RSP_Vect[RSPOpC.rd].HW[0], Reg);
	sprintf(Reg, "RSP_Vect[%i].HW[4]", RSPOpC.rd);
	MmxMoveQwordVariableToReg(x86_MM1, &RSP_Vect[RSPOpC.rd].HW[4], Reg);
	MmxMoveRegToReg(x86_MM2, x86_MM0);
	MmxMoveRegToReg(x86_MM3, x86_MM1);

	if ((RSPOpC.rs & 0x0f) < 2) {
		sprintf(Reg, "RSP_Vect[%i].HW[0]", RSPOpC.rt);
		MmxMoveQwordVariableToReg(x86_MM4, &RSP_Vect[RSPOpC.rt].HW[0], Reg);
		sprintf(Reg, "RSP_Vect[%i].HW[4]", RSPOpC.rt);
		MmxMoveQwordVariableToReg(x86_MM5, &RSP_Vect[RSPOpC.rt].HW[4], Reg);
	} else if ((RSPOpC.rs & 0x0f) >= 8) {
		RSP_Element2Mmx(x86_MM4);
	} else {
		RSP_MultiElement2Mmx(x86_MM4, x86_MM5);
	}

	MmxCompareGreaterWordRegToReg(x86_MM2, x86_MM4);
	MmxCompareGreaterWordRegToReg(x86_MM3, ((RSPOpC.rs & 0x0f) >= 8) ? x86_MM4 : x86_MM5);

	MmxPandRegToReg(x86_MM0, x86_MM2);
	MmxPandRegToReg(x86_MM1, x86_MM3);
	MmxPandnRegToReg(x86_MM2, x86_MM4);
	MmxPandnRegToReg(x86_MM3, ((RSPOpC.rs & 0x0f) >= 8) ? x86_MM4 : x86_MM5);

	MmxPorRegToReg(x86_MM0, x86_MM2);
	MmxPorRegToReg(x86_MM1, x86_MM3);
	MoveConstToVariable(0, &RSP_Flags[0].UW, "RSP_Flags[0].UW");
	return TRUE;
}*/

void CompileRsp_Vector_VGE ( void ) {
/*	BOOL bWriteToAccum = WriteToAccum(Low16BitAccum, CompilePC);*/

	/* FIXME: works ok, but needs careful flag analysis */
/*	#if defined (DLIST)
	if (bWriteToAccum == FALSE && TRUE == Compile_Vector_VGE_MMX()) {
		return;
	}
	#endif
*/
	InterpreterFallback((void*)RSP_Vector_VGE,"RSP_Vector_VGE");
}

void CompileRsp_Vector_VCL ( void ) {
	InterpreterFallback((void*)RSP_Vector_VCL,"RSP_Vector_VCL");
}

void CompileRsp_Vector_VCH ( void ) {
	InterpreterFallback((void*)RSP_Vector_VCH,"RSP_Vector_VCH");
}

void CompileRsp_Vector_VCR ( void ) {
	InterpreterFallback((void*)RSP_Vector_VCR,"RSP_Vector_VCR");
}

void CompileRsp_Vector_VMRG ( void ) {
	/*char Reg[256];
	int count, el, del;*/

	#ifndef CompileVmrg
	InterpreterFallback((void*)RSP_Vector_VMRG,"RSP_Vector_VMRG"); return;
	#endif

	/*CPU_Message("  %X %s",CompilePC,RSPOpcodeName(RSPOpC.Hex,CompilePC));
	MoveVariableToX86reg(&RSP_Flags[1].UW, "RSP_Flags[1].UW", x86_EDX);

	for (count = 0;count < 8; count++) {
		el = Indx[RSPOpC.rs].UB[count];
		del = EleSpec[RSPOpC.rs].UB[el];
		CPU_Message("     Iteration: %i", count);

		sprintf(Reg, "RSP_Vect[%i].HW[%i]", RSPOpC.rd, el);
		MoveZxVariableToX86regHalf(&RSP_Vect[RSPOpC.rd].HW[el], Reg, x86_EAX);
		sprintf(Reg, "RSP_Vect[%i].HW[%i]", RSPOpC.rt, del);
		MoveZxVariableToX86regHalf(&RSP_Vect[RSPOpC.rt].HW[del], Reg, x86_EBX);

		TestConstToX86Reg(1 << (7 - el), x86_EDX);
		CondMoveNotEqual(x86_ECX, x86_EAX);
		CondMoveEqual(x86_ECX, x86_EBX);

		sprintf(Reg, "RSP_Vect[%i].HW[%i]", RSPOpC.sa, el);
		MoveX86regHalfToVariable(x86_ECX, &RSP_Vect[RSPOpC.sa].HW[el], Reg);
	}*/
	LogMessage("TODO: CompileRsp_Vector_VMRG");
}

/*BOOL Compile_Vector_VAND_MMX ( void ) {
	char Reg[256];*/

	/* Do our MMX checks here */
/*	if (IsMmxEnabled == FALSE)
		return FALSE;
	if ((RSPOpC.rs & 0x0f) >= 2 && (RSPOpC.rs & 0x0f) <= 7 && IsMmx2Enabled == FALSE)
		return FALSE;

	sprintf(Reg, "RSP_Vect[%i].UHW[0]", RSPOpC.rd);
	MmxMoveQwordVariableToReg(x86_MM0, &RSP_Vect[RSPOpC.rd].UHW[0], Reg);
	sprintf(Reg, "RSP_Vect[%i].UHW[4]", RSPOpC.rd);
	MmxMoveQwordVariableToReg(x86_MM1, &RSP_Vect[RSPOpC.rd].UHW[4], Reg);

	if ((RSPOpC.rs & 0xF) >= 8) {
		RSP_Element2Mmx(x86_MM2);
		MmxPandRegToReg(x86_MM0, x86_MM2);
		MmxPandRegToReg(x86_MM1, x86_MM2);
	} else if ((RSPOpC.rs & 0xF) < 2) {
		sprintf(Reg, "RSP_Vect[%i].HW[0]", RSPOpC.rt);
		MmxPandVariableToReg(&RSP_Vect[RSPOpC.rt].HW[0], Reg, x86_MM0);
		sprintf(Reg, "RSP_Vect[%i].HW[4]", RSPOpC.rt);
		MmxPandVariableToReg(&RSP_Vect[RSPOpC.rt].HW[4], Reg, x86_MM1);
	} else {
		RSP_MultiElement2Mmx(x86_MM2, x86_MM3);
		MmxPandRegToReg(x86_MM0, x86_MM2);
		MmxPandRegToReg(x86_MM1, x86_MM3);
	}

	sprintf(Reg, "RSP_Vect[%i].UHW[0]", RSPOpC.sa);
	MmxMoveQwordRegToVariable(x86_MM0, &RSP_Vect[RSPOpC.sa].UHW[0], Reg);
	sprintf(Reg, "RSP_Vect[%i].UHW[4]", RSPOpC.sa);
	MmxMoveQwordRegToVariable(x86_MM1, &RSP_Vect[RSPOpC.sa].UHW[4], Reg);

	if (IsNextInstructionMmx(CompilePC) == FALSE)
		MmxEmptyMultimediaState();

	return TRUE;
}*/

void CompileRsp_Vector_VAND ( void ) {
	/*char Reg[256];
	int el, del, count;
	BOOL bElement = ((RSPOpC.rs & 0x0f) >= 8) ? TRUE : FALSE;
	BOOL bWriteToAccum = WriteToAccum(Low16BitAccum, CompilePC);*/

	#ifndef CompileVand
	InterpreterFallback((void*)RSP_Vector_VAND,"RSP_Vector_VAND"); return;
	#endif

	/*CPU_Message("  %X %s",CompilePC,RSPOpcodeName(RSPOpC.Hex,CompilePC));

	if (bWriteToAccum == FALSE) {
		if (TRUE == Compile_Vector_VAND_MMX())
			return;
	}
	
	if (bElement == TRUE) {
		del = (RSPOpC.rs & 0x07) ^ 7;
		sprintf(Reg, "RSP_Vect[%i].HW[%i]", RSPOpC.rt, del);
		MoveVariableToX86regHalf(&RSP_Vect[RSPOpC.rt].HW[del], Reg, x86_EBX);
	}

	for (count = 0; count < 8; count++) {
		el = Indx[RSPOpC.rs].B[count];
		del = EleSpec[RSPOpC.rs].B[el];

		CPU_Message("     Iteration: %i", count);

		sprintf(Reg, "RSP_Vect[%i].HW[%i]", RSPOpC.rd, el);
		MoveVariableToX86regHalf(&RSP_Vect[RSPOpC.rd].HW[el], Reg, x86_EAX);
		
		if (bElement == FALSE) {
			sprintf(Reg, "RSP_Vect[%i].HW[%i]", RSPOpC.rt, del);
			AndVariableToX86regHalf(&RSP_Vect[RSPOpC.rt].HW[del], Reg, x86_EAX);
		} else {
			AndX86RegHalfToX86RegHalf(x86_EAX, x86_EBX);
		}

		sprintf(Reg, "RSP_Vect[%i].HW[%i]", RSPOpC.sa, el);
		MoveX86regHalfToVariable(x86_EAX, &RSP_Vect[RSPOpC.sa].HW[el], Reg);

		if (bWriteToAccum != FALSE) {
			sprintf(Reg, "RSP_ACCUM[el].HW[1]", el);
			MoveX86regHalfToVariable(x86_EAX, &RSP_ACCUM[el].HW[1], Reg);
		}
	}*/
	LogMessage("TODO: CompileRsp_Vector_VAND");
}

void CompileRsp_Vector_VNAND ( void ) {
	InterpreterFallback((void*)RSP_Vector_VNAND,"RSP_Vector_VNAND");
}

/*BOOL Compile_Vector_VOR_MMX ( void ) {
	char Reg[256];*/

	/* Do our MMX checks here */
/*	if (IsMmxEnabled == FALSE)
		return FALSE;
	if ((RSPOpC.rs & 0x0f) >= 2 && (RSPOpC.rs & 0x0f) <= 7 && IsMmx2Enabled == FALSE)
		return FALSE;

	sprintf(Reg, "RSP_Vect[%i].UHW[0]", RSPOpC.rd);
	MmxMoveQwordVariableToReg(x86_MM0, &RSP_Vect[RSPOpC.rd].UHW[0], Reg);
	sprintf(Reg, "RSP_Vect[%i].UHW[4]", RSPOpC.rd);
	MmxMoveQwordVariableToReg(x86_MM1, &RSP_Vect[RSPOpC.rd].UHW[4], Reg);

	if ((RSPOpC.rs & 0xF) >= 8) {
		RSP_Element2Mmx(x86_MM2);
		MmxPorRegToReg(x86_MM0, x86_MM2);
		MmxPorRegToReg(x86_MM1, x86_MM2);
	} else if ((RSPOpC.rs & 0xF) < 2) {
		sprintf(Reg, "RSP_Vect[%i].HW[0]", RSPOpC.rt);
		MmxPorVariableToReg(&RSP_Vect[RSPOpC.rt].HW[0], Reg, x86_MM0);
		sprintf(Reg, "RSP_Vect[%i].HW[4]", RSPOpC.rt);
		MmxPorVariableToReg(&RSP_Vect[RSPOpC.rt].HW[4], Reg, x86_MM1);
	} else {
		RSP_MultiElement2Mmx(x86_MM2, x86_MM3);
		MmxPorRegToReg(x86_MM0, x86_MM2);
		MmxPorRegToReg(x86_MM1, x86_MM3);
	}

	sprintf(Reg, "RSP_Vect[%i].UHW[0]", RSPOpC.sa);
	MmxMoveQwordRegToVariable(x86_MM0, &RSP_Vect[RSPOpC.sa].UHW[0], Reg);
	sprintf(Reg, "RSP_Vect[%i].UHW[4]", RSPOpC.sa);
	MmxMoveQwordRegToVariable(x86_MM1, &RSP_Vect[RSPOpC.sa].UHW[4], Reg);

	if (IsNextInstructionMmx(CompilePC) == FALSE)
		MmxEmptyMultimediaState();

	return TRUE;
}*/

void CompileRsp_Vector_VOR ( void ) {
	/*char Reg[256];
	int el, del, count;
	BOOL bElement = ((RSPOpC.rs & 0x0f) >= 8) ? TRUE : FALSE;
	BOOL bWriteToAccum = WriteToAccum(Low16BitAccum, CompilePC);*/

	#ifndef CompileVor
	InterpreterFallback((void*)RSP_Vector_VOR,"RSP_Vector_VOR"); return;
	#endif

	/*CPU_Message("  %X %s",CompilePC,RSPOpcodeName(RSPOpC.Hex,CompilePC));

	if (bWriteToAccum == FALSE) {
		if (TRUE == Compile_Vector_VOR_MMX())
			return;
	}

	if (bElement == TRUE) {
		del = (RSPOpC.rs & 0x07) ^ 7;
		sprintf(Reg, "RSP_Vect[%i].HW[%i]", RSPOpC.rt, del);
		MoveVariableToX86regHalf(&RSP_Vect[RSPOpC.rt].HW[del], Reg, x86_EBX);
	}

	for (count = 0; count < 8; count++) {
		el = Indx[RSPOpC.rs].B[count];
		del = EleSpec[RSPOpC.rs].B[el];

		CPU_Message("     Iteration: %i", count);

		sprintf(Reg, "RSP_Vect[%i].HW[%i]", RSPOpC.rd, el);
		MoveVariableToX86regHalf(&RSP_Vect[RSPOpC.rd].HW[el], Reg, x86_EAX);
		
		if (bElement == FALSE) {
			sprintf(Reg, "RSP_Vect[%i].HW[%i]", RSPOpC.rt, del);
			OrVariableToX86regHalf(&RSP_Vect[RSPOpC.rt].HW[del], Reg, x86_EAX);
		} else {
			OrX86RegToX86Reg(x86_EAX, x86_EBX);
		}

		if (bWriteToAccum == TRUE) {
			sprintf(Reg, "RSP_ACCUM[el].HW[1]", el);
			MoveX86regHalfToVariable(x86_EAX, &RSP_ACCUM[el].HW[1], Reg);
		}
		sprintf(Reg, "RSP_Vect[%i].HW[%i]", RSPOpC.sa, el);
		MoveX86regHalfToVariable(x86_EAX, &RSP_Vect[RSPOpC.sa].HW[el], Reg);
	}*/
	LogMessage("tODO: CompileRsp_Vector_VOR");
}

void CompileRsp_Vector_VNOR ( void ) {
	InterpreterFallback((void*)RSP_Vector_VNOR,"RSP_Vector_VNOR");
}

/*BOOL Compile_Vector_VXOR_MMX ( void ) {
	char Reg[256];*/

	/* Do our MMX checks here */
/*	if (IsMmxEnabled == FALSE)
		return FALSE;
	if ((RSPOpC.rs & 0x0f) >= 2 && (RSPOpC.rs & 0x0f) <= 7 && IsMmx2Enabled == FALSE)
		return FALSE;

	if ((RSPOpC.rs & 0xF) < 2 && (RSPOpC.rd == RSPOpC.rt)) {
		static DWORD VXOR_DynaRegCount = 0;
		MmxXorRegToReg(VXOR_DynaRegCount, VXOR_DynaRegCount);

		sprintf(Reg, "RSP_Vect[%i].UHW[0]", RSPOpC.sa);
		MmxMoveQwordRegToVariable(VXOR_DynaRegCount, &RSP_Vect[RSPOpC.sa].UHW[0], Reg);
		sprintf(Reg, "RSP_Vect[%i].UHW[4]", RSPOpC.sa);
		MmxMoveQwordRegToVariable(VXOR_DynaRegCount, &RSP_Vect[RSPOpC.sa].UHW[4], Reg);
		VXOR_DynaRegCount = (VXOR_DynaRegCount + 1) & 7;
	} else {		
		sprintf(Reg, "RSP_Vect[%i].UHW[0]", RSPOpC.rd);
		MmxMoveQwordVariableToReg(x86_MM0, &RSP_Vect[RSPOpC.rd].UHW[0], Reg);
		sprintf(Reg, "RSP_Vect[%i].UHW[4]", RSPOpC.rd);
		MmxMoveQwordVariableToReg(x86_MM1, &RSP_Vect[RSPOpC.rd].UHW[4], Reg);

		if ((RSPOpC.rs & 0xF) >= 8) {
			RSP_Element2Mmx(x86_MM2);
			MmxXorRegToReg(x86_MM0, x86_MM2);
			MmxXorRegToReg(x86_MM1, x86_MM2);
		} else if ((RSPOpC.rs & 0xF) < 2) {
			sprintf(Reg, "RSP_Vect[%i].HW[0]", RSPOpC.rt);
			MmxMoveQwordVariableToReg(x86_MM2, &RSP_Vect[RSPOpC.rt].HW[0], Reg);
			sprintf(Reg, "RSP_Vect[%i].HW[4]", RSPOpC.rt);
			MmxMoveQwordVariableToReg(x86_MM3, &RSP_Vect[RSPOpC.rt].HW[4], Reg);

			MmxXorRegToReg(x86_MM0, x86_MM2);
			MmxXorRegToReg(x86_MM1, x86_MM3);
		} else {
			RSP_MultiElement2Mmx(x86_MM2, x86_MM3);
			MmxXorRegToReg(x86_MM0, x86_MM2);
			MmxXorRegToReg(x86_MM1, x86_MM3);
		}

		sprintf(Reg, "RSP_Vect[%i].UHW[0]", RSPOpC.sa);
		MmxMoveQwordRegToVariable(x86_MM0, &RSP_Vect[RSPOpC.sa].UHW[0], Reg);
		sprintf(Reg, "RSP_Vect[%i].UHW[4]", RSPOpC.sa);
		MmxMoveQwordRegToVariable(x86_MM1, &RSP_Vect[RSPOpC.sa].UHW[4], Reg);
	}

	if (IsNextInstructionMmx(CompilePC) == FALSE)
		MmxEmptyMultimediaState();

	return TRUE;
}*/

void CompileRsp_Vector_VXOR ( void ) {	
	#ifdef CompileVxor
	/*char Reg[256];
	DWORD count;
	BOOL bWriteToAccum = WriteToAccum(Low16BitAccum, CompilePC);

	CPU_Message("  %X %s",CompilePC,RSPOpcodeName(RSPOpC.Hex,CompilePC)); 
	
	if (!bWriteToAccum || ((RSPOpC.rs & 0xF) < 2 && RSPOpC.rd == RSPOpC.rt)) {
		if (TRUE == Compile_Vector_VXOR_MMX()) {
			if (bWriteToAccum == TRUE) {
				XorX86RegToX86Reg(x86_EAX, x86_EAX);
				for (count = 0; count < 8; count++) {
					sprintf(Reg, "RSP_ACCUM[%i].HW[1]", count);
					MoveX86regHalfToVariable(x86_EAX, &RSP_ACCUM[count].HW[1], Reg);
				}
			}
			return;
		}
	}*/
	LogMessage("TODO: CompileRsp_Vector_VXOR");
	#endif

	InterpreterFallback((void*)RSP_Vector_VXOR,"RSP_Vector_VXOR");
}

void CompileRsp_Vector_VNXOR ( void ) {
	InterpreterFallback((void*)RSP_Vector_VNXOR,"RSP_Vector_VNXOR");
}

void CompileRsp_Vector_V46(void) {
	InterpreterFallback((void*)RSP_Vector_V46, "RSP_Vector_V46");
}

void CompileRsp_Vector_V47(void) {
	InterpreterFallback((void*)RSP_Vector_V47, "RSP_Vector_V47");
}

void CompileRsp_Vector_VRCP ( void ) {
	InterpreterFallback((void*)RSP_Vector_VRCP,"RSP_Vector_VRCP");
}

void CompileRsp_Vector_VRCPL ( void ) {
	InterpreterFallback((void*)RSP_Vector_VRCPL,"RSP_Vector_VRCPL");
}

void CompileRsp_Vector_VRCPH ( void ) {
	/*char Reg[256];
	int count, el, last = -1;
	BOOL bWriteToAccum = WriteToAccum(Low16BitAccum, CompilePC);*/

	#ifndef CompileVrcph
	InterpreterFallback((void*)RSP_Vector_VRCPH,"RSP_Vector_VRCPH"); return;
	#endif

	/*CPU_Message("  %X %s",CompilePC,RSPOpcodeName(RSPOpC.Hex,CompilePC));

	el = EleSpec[RSPOpC.rs].B[(RSPOpC.rd & 0x7)];
	sprintf(Reg, "RSP_Vect[%i].UHW[%i]", RSPOpC.rt, el);
	MoveVariableToX86regHalf(&RSP_Vect[RSPOpC.rt].UHW[el], Reg, x86_EDX);
	MoveX86regHalfToVariable(x86_EDX, &Recp.UHW[1], "Recp.UHW[1]");
	
	MoveConstHalfToVariable(0, &Recp.UHW[0], "Recp.UHW[0]");
	
	MoveVariableToX86regHalf(&RecpResult.UHW[1], "RecpResult.UHW[1]", x86_ECX);
	el = 7 - (RSPOpC.rd & 0x7);
	sprintf(Reg, "RSP_Vect[%i].UHW[%i]", RSPOpC.sa, el);
	MoveX86regHalfToVariable(x86_ECX, &RSP_Vect[RSPOpC.sa].UHW[el], Reg);

	if (bWriteToAccum == FALSE) return;

	for (count = 0; count < 8; count++) {
		el = EleSpec[RSPOpC.rs].B[count];

		if (el != last) {
			sprintf(Reg, "RSP_Vect[%i].UHW[%i]", RSPOpC.rt, el);
			MoveVariableToX86regHalf(&RSP_Vect[RSPOpC.rt].UHW[el], Reg, x86_EAX);
			last = el;
		}

		sprintf(Reg, "RSP_ACCUM[%i].HW[1]", count);
		MoveX86regHalfToVariable(x86_EAX, &RSP_ACCUM[count].HW[1], Reg);
	}*/
	LogMessage("TODO: CompileRsp_Vector_VRSPH");
}

void CompileRsp_Vector_VMOV ( void ) {
	/*char Reg[256];
	int el;*/

	#ifndef CompileVmov
	InterpreterFallback((void*)RSP_Vector_VMOV,"RSP_Vector_VMOV"); return;
	#endif

	/*CPU_Message("  %X %s",CompilePC,RSPOpcodeName(RSPOpC.Hex,CompilePC));

	el = EleSpec[RSPOpC.rs].B[(RSPOpC.rd & 0x7)];
	sprintf(Reg, "RSP_Vect[%i].UHW[%i]", RSPOpC.rt, el);

	MoveVariableToX86regHalf(&RSP_Vect[RSPOpC.rt].UHW[el], Reg, x86_ECX);

	el = 7 - (RSPOpC.rd & 0x7);
	sprintf(Reg, "RSP_Vect[%i].UHW[%i]", RSPOpC.sa, el);

	MoveX86regHalfToVariable(x86_ECX, &RSP_Vect[RSPOpC.sa].UHW[el], Reg);*/
	LogMessage("TODO: CompileRsp_Vector_VMOV");
}

void CompileRsp_Vector_VRSQ ( void ) {
	RSP_CPU_Message("  %X %s",RspCompilePC,RSPOpcodeName(RSPOpC.OP.Hex,RspCompilePC));
	InterpreterFallback((void*)RSP_Vector_VRSQ,"RSP_Vector_VRSQ");
}

void CompileRsp_Vector_VRSQL ( void ) {
	RSP_CPU_Message("  %X %s",RspCompilePC,RSPOpcodeName(RSPOpC.OP.Hex,RspCompilePC));
	InterpreterFallback((void*)RSP_Vector_VRSQL,"RSP_Vector_VRSQL");
}

void CompileRsp_Vector_VRSQH ( void ) {
	/*char Reg[256];
	int count, el;*/

	#ifndef CompileVrsqh
	InterpreterFallback((void*)RSP_Vector_VRSQH,"RSP_Vector_VRSQH"); return;
	#endif

/*	CPU_Message("  %X %s",CompilePC,RSPOpcodeName(RSPOpC.Hex,CompilePC));
	
	el = EleSpec[RSPOpC.rs].B[(RSPOpC.rd & 0x7)];
	sprintf(Reg, "RSP_Vect[%i].UHW[%i]", RSPOpC.rt, el);
	MoveVariableToX86regHalf(&RSP_Vect[RSPOpC.rt].UHW[el], Reg, x86_EDX);
	MoveX86regHalfToVariable(x86_EDX, &SQroot.UHW[1], "SQroot.UHW[1]");

	MoveVariableToX86regHalf(&SQrootResult.UHW[1], "SQrootResult.UHW[1]", x86_ECX);
	el = 7 - (RSPOpC.rd & 0x7);
	sprintf(Reg, "RSP_Vect[%i].UHW[%i]", RSPOpC.sa, el);
	MoveX86regHalfToVariable(x86_ECX, &RSP_Vect[RSPOpC.sa].UHW[el], Reg);

	for (count = 0; count < 8; count++) {
		el = EleSpec[RSPOpC.rs].B[count];
		sprintf(Reg, "RSP_Vect[%i].UHW[%i]", RSPOpC.rt, el);
		MoveVariableToX86regHalf(&RSP_Vect[RSPOpC.rt].UHW[el], Reg, x86_EAX);

		sprintf(Reg, "RSP_ACCUM[%i].HW[1]", count);
		MoveX86regHalfToVariable(x86_EAX, &RSP_ACCUM[count].HW[1], Reg);
	}*/
	LogMessage("TODO: CompileRsp_Vector_VRSQH");
}

void CompileRsp_Vector_VNOOP ( void ) {

}

void CompileRsp_Vector_VEXTT(void) {
	InterpreterFallback((void*)RSP_Vector_VEXTT, "RSP_Vector_VEXTT");
}

void CompileRsp_Vector_VEXTQ(void) {
	InterpreterFallback((void*)RSP_Vector_VEXTQ, "RSP_Vector_VEXTQ");
}

void CompileRsp_Vector_VEXTN(void) {
	InterpreterFallback((void*)RSP_Vector_VEXTN, "RSP_Vector_VEXTN");
}

void CompileRsp_Vector_V59(void) {
	InterpreterFallback((void*)RSP_Vector_V59, "RSP_Vector_V59");
}

void CompileRsp_Vector_VINST(void) {
	InterpreterFallback((void*)RSP_Vector_VINST, "RSP_Vector_VINST");
}

void CompileRsp_Vector_VINSQ(void) {
	InterpreterFallback((void*)RSP_Vector_VINSQ, "RSP_Vector_VINSQ");
}

void CompileRsp_Vector_VINSN(void) {
	InterpreterFallback((void*)RSP_Vector_VINSN, "RSP_Vector_VINSN");
}

void CompileRsp_Vector_VNULL(void) {
}


/************************** lc2 functions **************************/

void CompileRsp_Opcode_LBV ( void ) {
	InterpreterFallback((void*)RSP_Opcode_LBV,"RSP_Opcode_LBV");
}

void CompileRsp_Opcode_LSV ( void ) {
	/*char Reg[256];
	int offset = (RSPOpC.voffset << 1);

	if (RSPOpC.del > 14) {
		rsp_UnknownOpcode();
		return;
	}*/

	#ifndef CompileLsv
	InterpreterFallback((void*)RSP_Opcode_LSV,"RSP_Opcode_LSV"); return;
	#endif

	/*CPU_Message("  %X %s",CompilePC,RSPOpcodeName(RSPOpC.Hex,CompilePC));
	
	if (IsRegConst(RSPOpC.base) == TRUE) {
		DWORD Addr = (MipsRegConst(RSPOpC.base) + offset) & 0xfff;

		if ((Addr & 1) != 0) {
			sprintf(Reg, "Dmem + %Xh", (Addr + 0) ^ 3);
			MoveVariableToX86regByte(RSPInfo.DMEM + ((Addr + 0) ^ 3), Reg, x86_ECX);
			sprintf(Reg, "Dmem + %Xh", (Addr + 1) ^ 3);
			MoveVariableToX86regByte(RSPInfo.DMEM + ((Addr + 1) ^ 3), Reg, x86_EDX);

			sprintf(Reg, "RSP_Vect[%i].B[%i]", RSPOpC.rt, 15 - (RSPOpC.del + 0));
			MoveX86regByteToVariable(x86_ECX, &RSP_Vect[RSPOpC.rt].B[15 - (RSPOpC.del + 0)], Reg);
			sprintf(Reg, "RSP_Vect[%i].B[%i]", RSPOpC.rt, 15 - (RSPOpC.del + 1));
			MoveX86regByteToVariable(x86_EDX, &RSP_Vect[RSPOpC.rt].B[15 - (RSPOpC.del + 1)], Reg);
		} else {
			sprintf(Reg, "Dmem + %Xh", Addr ^ 2);
			MoveVariableToX86regHalf(RSPInfo.DMEM + (Addr ^ 2), Reg, x86_EDX);
			sprintf(Reg, "RSP_Vect[%i].B[%i]", RSPOpC.rt, 15 - (RSPOpC.del + 1));
			MoveX86regHalfToVariable(x86_EDX, &RSP_Vect[RSPOpC.rt].B[15 - (RSPOpC.del + 1)], Reg);
		}
		return;
	}
	
	MoveVariableToX86reg(&RSP_GPR[RSPOpC.base].UW, GPR_Name(RSPOpC.base), x86_EBX);
	if (offset != 0) AddConstToX86Reg(x86_EBX, offset);
	AndConstToX86Reg(x86_EBX, 0x0FFF);

	if (Compiler.bAlignVector == TRUE) {
		XorConstToX86Reg(x86_EBX, 2);
		MoveN64MemToX86regHalf(x86_ECX, x86_EBX);
		sprintf(Reg, "RSP_Vect[%i].B[%i]", RSPOpC.rt, 15 - (RSPOpC.del + 1));
		MoveX86regHalfToVariable(x86_ECX, &RSP_Vect[RSPOpC.rt].B[15 - (RSPOpC.del + 1)], Reg);
	} else {
		LeaSourceAndOffset(x86_EAX, x86_EBX, 1);
		XorConstToX86Reg(x86_EBX, 3);
		XorConstToX86Reg(x86_EAX, 3);

		MoveN64MemToX86regByte(x86_ECX, x86_EBX);
		MoveN64MemToX86regByte(x86_EDX, x86_EAX);

		sprintf(Reg, "RSP_Vect[%i].B[%i]", RSPOpC.rt, 15 - (RSPOpC.del + 0));
		MoveX86regByteToVariable(x86_ECX, &RSP_Vect[RSPOpC.rt].B[15 - (RSPOpC.del + 0)], Reg);

		sprintf(Reg, "RSP_Vect[%i].B[%i]", RSPOpC.rt, 15 - (RSPOpC.del + 1));
		MoveX86regByteToVariable(x86_EDX, &RSP_Vect[RSPOpC.rt].B[15 - (RSPOpC.del + 1)], Reg);
	}*/
	LogMessage("TODO: CompileRsp_Opcode_LSV");
}

void CompileRsp_Opcode_LLV ( void ) {
	/*char Reg[256];
	int offset = (RSPOpC.voffset << 2);
	BYTE * Jump[2];*/

	#ifndef CompileLlv
	InterpreterFallback((void*)RSP_Opcode_LLV,"RSP_Opcode_LLV"); return;
	#endif
	
	/*CPU_Message("  %X %s",CompilePC,RSPOpcodeName(RSPOpC.Hex,CompilePC));

	if ((RSPOpC.del & 0x3) != 0) {
		rsp_UnknownOpcode();
		return;
	}

 	if (IsRegConst(RSPOpC.base) == TRUE) {
		DWORD Addr = (MipsRegConst(RSPOpC.base) + offset) & 0xfff;

		if ((Addr & 3) != 0) {
			CompilerWarning("Unaligned LLV at constant address");
			Cheat_r4300iOpcodeNoMessage(RSP_Opcode_LLV,"RSP_Opcode_LLV");
			return;
		}

		sprintf(Reg, "Dmem + %Xh", Addr);
		MoveVariableToX86reg(RSPInfo.DMEM + Addr, Reg, x86_EAX);
		sprintf(Reg, "RSP_Vect[%i].B[%i]", RSPOpC.rt, 16 - RSPOpC.del - 4);
		MoveX86regToVariable(x86_EAX, &RSP_Vect[RSPOpC.rt].B[16 - RSPOpC.del - 4], Reg);
		return;
	}
	
	MoveVariableToX86reg(&RSP_GPR[RSPOpC.base].UW, GPR_Name(RSPOpC.base), x86_EBX);
	if (offset != 0) AddConstToX86Reg(x86_EBX, offset);
	
	TestConstToX86Reg(3, x86_EBX);
	JneLabel32("Unaligned", 0);
	Jump[0] = RecompPos - 4;*/

	/*
	 * Unaligned
	 */
/*	CompilerToggleBuffer();

	CPU_Message("   Unaligned:");
	*((DWORD *)(Jump[0]))=(DWORD)(RecompPos - Jump[0] - 4);
	Cheat_r4300iOpcodeNoMessage(RSP_Opcode_LLV,"RSP_Opcode_LLV");
	JmpLabel32("Done", 0);
	Jump[1] = RecompPos - 4;

	CompilerToggleBuffer();*/

	/*
	 * Aligned
	 */
/*	AndConstToX86Reg(x86_EBX, 0x0fff);
	MoveN64MemToX86reg(x86_EAX, x86_EBX);*/
	/* Because of byte swapping this swizzle works nicely */
/*	sprintf(Reg, "RSP_Vect[%i].B[%i]", RSPOpC.rt, 16 - RSPOpC.del - 4);
	MoveX86regToVariable(x86_EAX, &RSP_Vect[RSPOpC.rt].B[16 - RSPOpC.del - 4], Reg);

	CPU_Message("   Done:");
	*((DWORD *)(Jump[1]))=(DWORD)(RecompPos - Jump[1] - 4);*/
	LogMessage("TODO: CompileRsp_Opcode_LLV");
}

void CompileRsp_Opcode_LDV ( void ) {
	/*char Reg[256];
	int offset = (RSPOpC.voffset << 3);
	BYTE * Jump[2], * LoopEntry;*/

	#ifndef CompileLdv
	InterpreterFallback((void*)RSP_Opcode_LDV,"RSP_Opcode_LDV"); return;
	#endif

/*	CPU_Message("  %X %s",CompilePC,RSPOpcodeName(RSPOpC.Hex,CompilePC));*/

	/* FIXME: Conker's hits this */
	//if ((RSPOpC.del & 0x7) != 0) {
	//	rsp_UnknownOpcode();
	//	return;
	//}

/*	if (IsRegConst(RSPOpC.base) == TRUE) {
		DWORD Addr = (MipsRegConst(RSPOpC.base) + offset) & 0xfff;

		if ((Addr & 3) != 0) {
			CompilerWarning("Unaligned LDV at constant address PC = %04X", CompilePC);
			Cheat_r4300iOpcodeNoMessage(RSP_Opcode_LDV,"RSP_Opcode_LDV");
			return;
		}

		sprintf(Reg, "Dmem + %Xh", Addr);
		MoveVariableToX86reg(RSPInfo.DMEM + Addr + 0, Reg, x86_EAX);
		sprintf(Reg, "Dmem + %Xh", Addr + 4);
		MoveVariableToX86reg(RSPInfo.DMEM + Addr + 4, Reg, x86_ECX);

		sprintf(Reg, "RSP_Vect[%i].B[%i]", RSPOpC.rt, 16 - RSPOpC.del - 4);
		MoveX86regToVariable(x86_EAX, &RSP_Vect[RSPOpC.rt].B[16 - RSPOpC.del - 4], Reg);
		sprintf(Reg, "RSP_Vect[%i].B[%i]", RSPOpC.rt, 16 - RSPOpC.del - 8);
		MoveX86regToVariable(x86_ECX, &RSP_Vect[RSPOpC.rt].B[16 - RSPOpC.del - 8], Reg);
		return;
	}
	
	MoveVariableToX86reg(&RSP_GPR[RSPOpC.base].UW, GPR_Name(RSPOpC.base), x86_EBX);
	if (offset != 0) {
		AddConstToX86Reg(x86_EBX, offset);
	}
	AndConstToX86Reg(x86_EBX, 0x0fff);
	TestConstToX86Reg(3, x86_EBX);
	JneLabel32("Unaligned", 0);
	Jump[0] = RecompPos - 4;

	CompilerToggleBuffer();
	CPU_Message("   Unaligned:");
	x86_SetBranch32b(Jump[0], RecompPos);
	sprintf(Reg, "RSP_Vect[%i].UB[%i]", RSPOpC.rt, 15 - RSPOpC.del);
	MoveOffsetToX86reg((DWORD)&RSP_Vect[RSPOpC.rt].UB[15 - RSPOpC.del], Reg, x86_EDI);
	MoveConstToX86reg(8, x86_ECX);*/

/*    mov eax, ebx
      dec edi
      xor eax, 3h
      inc ebx
      mov dl, byte ptr [eax+Dmem]
      dec ecx
      mov byte ptr [edi+1], dl      
      jne $Loop */

/*	LoopEntry = RecompPos;
	CPU_Message("   Loop:");	
	MoveX86RegToX86Reg(x86_EBX, x86_EAX);
	XorConstToX86Reg(x86_EAX, 3);
	MoveN64MemToX86regByte(x86_EDX, x86_EAX);
	MoveX86regByteToX86regPointer(x86_EDX, x86_EDI);
	IncX86reg(x86_EBX);*/ /* address constant */
/*	DecX86reg(x86_EDI);*/ /* vector pointer */
/*	DecX86reg(x86_ECX);*/ /* counter */
/*	JneLabel8("Loop", 0);
	x86_SetBranch8b(RecompPos - 1, LoopEntry);

	JmpLabel32("Done", 0);
	Jump[1] = RecompPos - 4;
	CompilerToggleBuffer();

	MoveN64MemToX86reg(x86_EAX, x86_EBX);
	MoveN64MemDispToX86reg(x86_ECX, x86_EBX, 4);*/
	
	/* Because of byte swapping this swizzle works nicely */
/*	sprintf(Reg, "RSP_Vect[%i].B[%i]", RSPOpC.rt, 16 - RSPOpC.del - 4);
	MoveX86regToVariable(x86_EAX, &RSP_Vect[RSPOpC.rt].B[16 - RSPOpC.del - 4], Reg);
	sprintf(Reg, "RSP_Vect[%i].B[%i]", RSPOpC.rt, 16 - RSPOpC.del - 8);
	MoveX86regToVariable(x86_ECX, &RSP_Vect[RSPOpC.rt].B[16 - RSPOpC.del - 8], Reg);

	CPU_Message("   Done:");
	x86_SetBranch32b(Jump[1], RecompPos);*/
	LogMessage("TODO: CompileRsp_Opcode_LDV");
}

void CompileRsp_Opcode_LQV ( void ) {
	/*char Reg[256];
	int offset = (RSPOpC.voffset << 4);
	BYTE * Jump[2];*/

	#ifndef CompileLqv
	InterpreterFallback((void*)RSP_Opcode_LQV,"RSP_Opcode_LQV"); return;
	#endif
	
	/*CPU_Message("  %X %s",CompilePC,RSPOpcodeName(RSPOpC.Hex,CompilePC));

	if (RSPOpC.del != 0) {
		rsp_UnknownOpcode();
		return;
	}

	if (IsRegConst(RSPOpC.base) == TRUE) {
		DWORD Addr = (MipsRegConst(RSPOpC.base) + offset) & 0xfff;

		if (Addr & 15) {
			CompilerWarning("Unaligned LQV at constant address PC = %04X", CompilePC);
			Cheat_r4300iOpcodeNoMessage(RSP_Opcode_LQV,"RSP_Opcode_LQV");
			return;
		}*/
	
		/*
		 * Aligned store
		 */

/*		if (IsSseEnabled == FALSE) {
			sprintf(Reg, "Dmem+%Xh+0", Addr);
			MoveVariableToX86reg(RSPInfo.DMEM + Addr + 0, Reg, x86_EAX);
			sprintf(Reg, "Dmem+%Xh+4", Addr);
			MoveVariableToX86reg(RSPInfo.DMEM + Addr + 4, Reg, x86_EBX);
			sprintf(Reg, "Dmem+%Xh+8", Addr);
			MoveVariableToX86reg(RSPInfo.DMEM + Addr + 8, Reg, x86_ECX);
			sprintf(Reg, "Dmem+%Xh+C", Addr);
			MoveVariableToX86reg(RSPInfo.DMEM + Addr + 12, Reg, x86_EDX);

			sprintf(Reg, "RSP_Vect[%i].B[12]", RSPOpC.rt);
			MoveX86regToVariable(x86_EAX, &RSP_Vect[RSPOpC.rt].B[12], Reg);
			sprintf(Reg, "RSP_Vect[%i].B[8]", RSPOpC.rt);
			MoveX86regToVariable(x86_EBX, &RSP_Vect[RSPOpC.rt].B[8], Reg);
			sprintf(Reg, "RSP_Vect[%i].B[4]", RSPOpC.rt);
			MoveX86regToVariable(x86_ECX, &RSP_Vect[RSPOpC.rt].B[4], Reg);
			sprintf(Reg, "RSP_Vect[%i].B[0]", RSPOpC.rt);
			MoveX86regToVariable(x86_EDX, &RSP_Vect[RSPOpC.rt].B[0], Reg);
		} else {
			sprintf(Reg, "Dmem+%Xh", Addr);
			SseMoveUnalignedVariableToReg(RSPInfo.DMEM + Addr, Reg, x86_XMM0);
			SseShuffleReg(x86_XMM0, x86_MM0, 0x1b);
			sprintf(Reg, "RSP_Vect[%i].B[0]", RSPOpC.rt);
			SseMoveAlignedRegToVariable(x86_XMM0, &RSP_Vect[RSPOpC.rt].B[0], Reg);
		}
		return;
	}

	MoveVariableToX86reg(&RSP_GPR[RSPOpC.base].UW, GPR_Name(RSPOpC.base), x86_EBX);
	if (offset != 0) {
		AddConstToX86Reg(x86_EBX, offset);
	}
	TestConstToX86Reg(15, x86_EBX);
	JneLabel32("Unaligned", 0);
	Jump[0] = RecompPos - 4;

	CompilerToggleBuffer();
	CPU_Message("   Unaligned:");
	x86_SetBranch32b(Jump[0], RecompPos);

	Cheat_r4300iOpcodeNoMessage(RSP_Opcode_LQV,"RSP_Opcode_LQV");
	JmpLabel32("Done", 0);
	Jump[1] = RecompPos - 4;
	CompilerToggleBuffer();

	AndConstToX86Reg(x86_EBX, 0x0fff);
	if (IsSseEnabled == FALSE) {
		MoveN64MemDispToX86reg(x86_EAX, x86_EBX, 0);
		MoveN64MemDispToX86reg(x86_ECX, x86_EBX, 4);
		MoveN64MemDispToX86reg(x86_EDX, x86_EBX, 8);
		MoveN64MemDispToX86reg(x86_EDI, x86_EBX, 12);

		sprintf(Reg, "RSP_Vect[%i].B[12]", RSPOpC.rt);
		MoveX86regToVariable(x86_EAX, &RSP_Vect[RSPOpC.rt].B[12], Reg);
		sprintf(Reg, "RSP_Vect[%i].B[8]", RSPOpC.rt);
		MoveX86regToVariable(x86_ECX, &RSP_Vect[RSPOpC.rt].B[8], Reg);
		sprintf(Reg, "RSP_Vect[%i].B[4]", RSPOpC.rt);
		MoveX86regToVariable(x86_EDX, &RSP_Vect[RSPOpC.rt].B[4], Reg);
		sprintf(Reg, "RSP_Vect[%i].B[0]", RSPOpC.rt);
		MoveX86regToVariable(x86_EDI, &RSP_Vect[RSPOpC.rt].B[0], Reg);
	} else {
		SseMoveUnalignedN64MemToReg(x86_XMM0, x86_EBX);
		SseShuffleReg(x86_XMM0, x86_MM0, 0x1b);
		sprintf(Reg, "RSP_Vect[%i].B[0]", RSPOpC.rt);
		SseMoveAlignedRegToVariable(x86_XMM0, &RSP_Vect[RSPOpC.rt].B[0], Reg);
	}
	CPU_Message("   Done:");
	x86_SetBranch32b((DWORD*)Jump[1], (DWORD*)RecompPos);*/
	LogMessage("TODO: CompileRsp_Opcode_LQV");
}

void CompileRsp_Opcode_LRV ( void ) {
	/*int offset = (RSPOpC.voffset << 4);
	BYTE * Loop, * Jump[2];*/

	#ifndef CompileLrv
	InterpreterFallback((void*)RSP_Opcode_LRV,"RSP_Opcode_LRV"); return;
	#endif

	/*CPU_Message("  %X %s",CompilePC,RSPOpcodeName(RSPOpC.Hex,CompilePC));

	if (RSPOpC.del != 0) {
		rsp_UnknownOpcode();
		return;
	}

	MoveVariableToX86reg(&RSP_GPR[RSPOpC.base].UW, GPR_Name(RSPOpC.base), x86_EBX);
	if (offset != 0) AddConstToX86Reg(x86_EBX, offset);

	if (Compiler.bAlignVector == FALSE) {
		TestConstToX86Reg(1, x86_EBX);
		JneLabel32("Unaligned", 0);
		Jump[0] = RecompPos - 4;*/

		/* Unaligned */
/*		CompilerToggleBuffer();

		CPU_Message(" Unaligned:");
		x86_SetBranch32b(Jump[0], RecompPos);

		Cheat_r4300iOpcodeNoMessage(RSP_Opcode_LRV,"RSP_Opcode_LRV");
		JmpLabel32("Done", 0);
		Jump[1] = RecompPos - 4;

		CompilerToggleBuffer();
	}*/

	/* Aligned */
/*	MoveX86RegToX86Reg(x86_EBX, x86_EAX);
	AndConstToX86Reg(x86_EAX, 0x0F);
	AndConstToX86Reg(x86_EBX, 0x0ff0);

	MoveX86RegToX86Reg(x86_EAX, x86_ECX);
	ShiftRightUnsignImmed(x86_ECX, 1);

	JeLabel8("Done", 0);
	Jump[0] = RecompPos - 1;*/
/*
	DecX86reg(x86_EAX);
	LeaSourceAndOffset(x86_EAX, x86_EAX, (DWORD) &RSP_Vect[RSPOpC.rt].B[0]);
	DecX86reg(x86_EAX);
*/
/*	AddConstToX86Reg(x86_EAX, ((DWORD)&RSP_Vect[RSPOpC.rt].UB[0]) - 2);

	CPU_Message("   Loop:");
	Loop = RecompPos;

	MoveX86RegToX86Reg(x86_EBX, x86_ESI);
	XorConstToX86Reg(x86_ESI, 2);
	MoveN64MemToX86regHalf(x86_EDX, x86_ESI);
	MoveX86regHalfToX86regPointer(x86_EDX, x86_EAX);

	AddConstToX86Reg(x86_EBX, 2);*/	/* Dmem pointer	*/
/*	SubConstFromX86Reg(x86_EAX, 2);*/	/* Vector pointer */	
/*	DecX86reg(x86_ECX);*/				/* Loop counter	*/
/*	JneLabel8("Loop", 0);
	x86_SetBranch8b(RecompPos - 1, Loop);

	if (Compiler.bAlignVector == FALSE) {
		CPU_Message("   Done:");
		x86_SetBranch32b((DWORD*)Jump[1], (DWORD*)RecompPos);
	}

	x86_SetBranch8b(Jump[0], RecompPos);*/
	LogMessage("TODO: CompileRsp_Opcode_LRV");
}

void CompileRsp_Opcode_LPV ( void ) {
	InterpreterFallback((void*)RSP_Opcode_LPV,"RSP_Opcode_LPV");
}

void CompileRsp_Opcode_LUV ( void ) {
	InterpreterFallback((void*)RSP_Opcode_LUV,"RSP_Opcode_LUV");
}


void CompileRsp_Opcode_LHV ( void ) {
	InterpreterFallback((void*)RSP_Opcode_LHV,"RSP_Opcode_LHV");
}


void CompileRsp_Opcode_LFV ( void ) {
	InterpreterFallback((void*)RSP_Opcode_LFV,"RSP_Opcode_LFV");
}

void CompileRsp_Opcode_LWV ( void ) {
}

void CompileRsp_Opcode_LTV ( void ) {
	InterpreterFallback((void*)RSP_Opcode_LTV,"RSP_Opcode_LTV");
}

/************************** sc2 functions **************************/

void CompileRsp_Opcode_SBV ( void ) {
	InterpreterFallback((void*)RSP_Opcode_SBV,"RSP_Opcode_SBV");
}

void CompileRsp_Opcode_SSV ( void ) {
	/*char Reg[256];
	int offset = (RSPOpC.voffset << 1);

	if (RSPOpC.del > 14) {
		rsp_UnknownOpcode();
		return;
	}*/

	#ifndef CompileSsv
	InterpreterFallback((void*)RSP_Opcode_SSV,"RSP_Opcode_SSV"); return;
	#endif
	
	/*CPU_Message("  %X %s",CompilePC,RSPOpcodeName(RSPOpC.Hex,CompilePC));

	if (IsRegConst(RSPOpC.base) == TRUE) {
		DWORD Addr = (MipsRegConst(RSPOpC.base) + offset) & 0xfff;

		if ((Addr & 1) != 0) {
			sprintf(Reg, "RSP_Vect[%i].B[%i]", RSPOpC.rt, 15 - (RSPOpC.del + 0));
			MoveVariableToX86regByte(&RSP_Vect[RSPOpC.rt].B[15 - (RSPOpC.del + 0)], Reg, x86_ECX);
			sprintf(Reg, "RSP_Vect[%i].B[%i]", RSPOpC.rt, 15 - (RSPOpC.del + 1));
			MoveVariableToX86regByte(&RSP_Vect[RSPOpC.rt].B[15 - (RSPOpC.del + 1)], Reg, x86_EDX);

			sprintf(Reg, "Dmem + %Xh", (Addr + 0) ^ 3);
			MoveX86regByteToVariable(x86_ECX, RSPInfo.DMEM + ((Addr + 0) ^ 3), Reg);
			sprintf(Reg, "Dmem + %Xh", (Addr + 1) ^ 3);
			MoveX86regByteToVariable(x86_EDX, RSPInfo.DMEM + ((Addr + 1) ^ 3), Reg);
		} else {
			sprintf(Reg, "RSP_Vect[%i].B[%i]", RSPOpC.rt, 15 - (RSPOpC.del + 1));
			MoveVariableToX86regHalf(&RSP_Vect[RSPOpC.rt].B[15 - (RSPOpC.del + 1)], Reg, x86_ECX);
			sprintf(Reg, "Dmem + %Xh", Addr ^ 2);
			MoveX86regHalfToVariable(x86_ECX, RSPInfo.DMEM + (Addr ^ 2), Reg);
		}
		return;
	}

	MoveVariableToX86reg(&RSP_GPR[RSPOpC.base].UW, GPR_Name(RSPOpC.base), x86_EBX);
	if (offset != 0) AddConstToX86Reg(x86_EBX, offset);
	AndConstToX86Reg(x86_EBX, 0x0FFF);

	if (Compiler.bAlignVector == TRUE) {
		sprintf(Reg, "RSP_Vect[%i].B[%i]", RSPOpC.rt, 15 - (RSPOpC.del + 1));
		MoveVariableToX86regHalf(&RSP_Vect[RSPOpC.rt].B[15 - (RSPOpC.del + 1)], Reg, x86_ECX);
		XorConstToX86Reg(x86_EBX, 2);
		MoveX86regHalfToN64Mem(x86_ECX, x86_EBX);		
	} else {
		LeaSourceAndOffset(x86_EAX, x86_EBX, 1);
		XorConstToX86Reg(x86_EBX, 3);
		XorConstToX86Reg(x86_EAX, 3);

		sprintf(Reg, "RSP_Vect[%i].B[%i]", RSPOpC.rt, 15 - (RSPOpC.del + 0));
		MoveVariableToX86regByte(&RSP_Vect[RSPOpC.rt].B[15 - (RSPOpC.del + 0)], Reg, x86_ECX);
		sprintf(Reg, "RSP_Vect[%i].B[%i]", RSPOpC.rt, 15 - (RSPOpC.del + 1));
		MoveVariableToX86regByte(&RSP_Vect[RSPOpC.rt].B[15 - (RSPOpC.del + 1)], Reg, x86_EDX);

		MoveX86regByteToN64Mem(x86_ECX, x86_EBX);
		MoveX86regByteToN64Mem(x86_EDX, x86_EAX);
	}*/
	LogMessage("TODO: CompileRsp_Opcode_SSV");
}

void CompileRsp_Opcode_SLV ( void ) {
	/*char Reg[256];
	int offset = (RSPOpC.voffset << 2);
	BYTE * Jump[2];*/

	#ifndef CompileSlv
	InterpreterFallback((void*)RSP_Opcode_SLV,"RSP_Opcode_SLV"); return;
	#endif

/*	CPU_Message("  %X %s",CompilePC,RSPOpcodeName(RSPOpC.Hex,CompilePC));

//	if ((RSPOpC.del & 0x3) != 0) {
//		rsp_UnknownOpcode();
//		return;
//	}

 	if (IsRegConst(RSPOpC.base) == TRUE) {
		DWORD Addr = (MipsRegConst(RSPOpC.base) + offset) & 0xfff;

		if ((Addr & 3) != 0) {
			CompilerWarning("Unaligned SLV at constant address");
			Cheat_r4300iOpcodeNoMessage(RSP_Opcode_SLV,"RSP_Opcode_SLV");
			return;
		}

		sprintf(Reg, "RSP_Vect[%i].B[%i]", RSPOpC.rt, 16 - RSPOpC.del - 4);
		MoveVariableToX86reg(&RSP_Vect[RSPOpC.rt].B[16 - RSPOpC.del - 4], Reg, x86_EAX);
		sprintf(Reg, "Dmem + %Xh", Addr);
		MoveX86regToVariable(x86_EAX, RSPInfo.DMEM + Addr, Reg);
		return;
	}
	
	MoveVariableToX86reg(&RSP_GPR[RSPOpC.base].UW, GPR_Name(RSPOpC.base), x86_EBX);
	if (offset != 0) AddConstToX86Reg(x86_EBX, offset);
	
	TestConstToX86Reg(3, x86_EBX);
	JneLabel32("Unaligned", 0);
	Jump[0] = RecompPos - 4;*/

	/*
	 * Unaligned
	 */
/*	CompilerToggleBuffer();

	CPU_Message("   Unaligned:");
	*((DWORD *)(Jump[0]))=(DWORD)(RecompPos - Jump[0] - 4);
	Cheat_r4300iOpcodeNoMessage(RSP_Opcode_SLV,"RSP_Opcode_SLV");
	JmpLabel32("Done", 0);
	Jump[1] = RecompPos - 4;

	CompilerToggleBuffer();*/

	/*
	 * Aligned
	 */

	/* Because of byte swapping this swizzle works nicely */
/*	sprintf(Reg, "RSP_Vect[%i].B[%i]", RSPOpC.rt, 16 - RSPOpC.del - 4);
	MoveVariableToX86reg(&RSP_Vect[RSPOpC.rt].B[16 - RSPOpC.del - 4], Reg, x86_EAX);
	
	AndConstToX86Reg(x86_EBX, 0x0fff);
	MoveX86regToN64Mem(x86_EAX, x86_EBX);

	CPU_Message("   Done:");
	*((DWORD *)(Jump[1]))=(DWORD)(RecompPos - Jump[1] - 4);*/
	LogMessage("TODO: CompileRsp_Opcode_SLV");
}

void CompileRsp_Opcode_SDV ( void ) {
	/*char Reg[256];
	int offset = (RSPOpC.voffset << 3);
	BYTE * Jump[2], * LoopEntry;*/

	//if ((RSPOpC.del & 0x7) != 0) {
	//	rsp_UnknownOpcode();
	//	return;
	//}

	#ifndef CompileSdv
	InterpreterFallback((void*)RSP_Opcode_SDV,"RSP_Opcode_SDV"); return;
	#endif
	
	/*CPU_Message("  %X %s",CompilePC,RSPOpcodeName(RSPOpC.Hex,CompilePC));

	if (IsRegConst(RSPOpC.base) == TRUE) {
		DWORD Addr = (MipsRegConst(RSPOpC.base) + offset) & 0xfff;

		if ((Addr & 3) != 0) {
			CompilerWarning("Unaligned SDV at constant address PC = %04X", CompilePC);
			Cheat_r4300iOpcodeNoMessage(RSP_Opcode_SDV,"RSP_Opcode_SDV");
			return;
		}

		sprintf(Reg, "RSP_Vect[%i].B[%i]", RSPOpC.rt, 16 - RSPOpC.del - 4);
		MoveVariableToX86reg(&RSP_Vect[RSPOpC.rt].B[16 - RSPOpC.del - 4], Reg, x86_EAX);
		sprintf(Reg, "RSP_Vect[%i].B[%i]", RSPOpC.rt, 16 - RSPOpC.del - 8);
		MoveVariableToX86reg(&RSP_Vect[RSPOpC.rt].B[16 - RSPOpC.del - 8], Reg, x86_EBX);

		sprintf(Reg, "Dmem + %Xh", Addr);
		MoveX86regToVariable(x86_EAX, RSPInfo.DMEM + Addr, Reg);
		sprintf(Reg, "Dmem + %Xh", Addr + 4);
		MoveX86regToVariable(x86_EBX, RSPInfo.DMEM + Addr + 4, Reg);
		return;
	}
		
	MoveVariableToX86reg(&RSP_GPR[RSPOpC.base].UW, GPR_Name(RSPOpC.base), x86_EBX);
	if (offset != 0) {
		AddConstToX86Reg(x86_EBX, offset);
	}
	AndConstToX86Reg(x86_EBX, 0x0fff);
	TestConstToX86Reg(3, x86_EBX);
	JneLabel32("Unaligned", 0);
	Jump[0] = RecompPos - 4;
	
	CompilerToggleBuffer();
	CPU_Message("   Unaligned:");
	x86_SetBranch32b((DWORD*)Jump[0], (DWORD*)RecompPos);
	
	sprintf(Reg, "RSP_Vect[%i].UB[%i]", RSPOpC.rt, 15 - RSPOpC.del);
	MoveOffsetToX86reg((DWORD)&RSP_Vect[RSPOpC.rt].UB[15 - RSPOpC.del], Reg, x86_EDI);
	MoveConstToX86reg(8, x86_ECX);

	CPU_Message("   Loop:");
	LoopEntry = RecompPos;
	MoveX86RegToX86Reg(x86_EBX, x86_EAX);
	XorConstToX86Reg(x86_EAX, 3);
	MoveX86regPointerToX86regByte(x86_EDX, x86_EDI);
	MoveX86regByteToN64Mem(x86_EDX, x86_EAX);
	IncX86reg(x86_EBX);*/ /* address constant */
/*	DecX86reg(x86_EDI);*/ /* vector pointer */
/*	DecX86reg(x86_ECX);*/ /* counter */
/*	JneLabel8("Loop", 0);
	x86_SetBranch8b(RecompPos - 1, LoopEntry);

	JmpLabel32("Done", 0);
	Jump[1] = RecompPos - 4;
	CompilerToggleBuffer();

	sprintf(Reg, "RSP_Vect[%i].B[%i]", RSPOpC.rt, 16 - RSPOpC.del - 4);
	MoveVariableToX86reg(&RSP_Vect[RSPOpC.rt].B[16 - RSPOpC.del - 4], Reg, x86_EAX);
	sprintf(Reg, "RSP_Vect[%i].B[%i]", RSPOpC.rt, 16 - RSPOpC.del - 8);
	MoveVariableToX86reg(&RSP_Vect[RSPOpC.rt].B[16 - RSPOpC.del - 8], Reg, x86_ECX);
	MoveX86regToN64Mem(x86_EAX, x86_EBX);
	MoveX86regToN64MemDisp(x86_ECX, x86_EBX, 4);

	CPU_Message("   Done:");
	x86_SetBranch32b((DWORD*)Jump[1], (DWORD*)RecompPos);*/
	LogMessage("TODO: CompileRsp_Opcode_SDV");
}

void CompileRsp_Opcode_SQV ( void ) {
	/*char Reg[256];
	int offset = (RSPOpC.voffset << 4);
	BYTE * Jump[2];*/

	#ifndef CompileSqv
 	InterpreterFallback((void*)RSP_Opcode_SQV,"RSP_Opcode_SQV"); return;
	#endif

	/*CPU_Message("  %X %s",CompilePC,RSPOpcodeName(RSPOpC.Hex,CompilePC));

	if (RSPOpC.del != 0) {
		rsp_UnknownOpcode();
		return;
	}

	if (IsRegConst(RSPOpC.base) == TRUE) {
		DWORD Addr = (MipsRegConst(RSPOpC.base) + offset) & 0xfff;

		if (Addr & 15) {
			CompilerWarning("Unaligned SQV at constant address %04X", CompilePC);
			Cheat_r4300iOpcodeNoMessage(RSP_Opcode_SQV,"RSP_Opcode_SQV");
			return;
		}*/
	
		/*
		 * Aligned store
		 */

/*		if (IsSseEnabled == FALSE) {
			sprintf(Reg, "RSP_Vect[%i].B[12]", RSPOpC.rt);
			MoveVariableToX86reg(&RSP_Vect[RSPOpC.rt].B[12], Reg, x86_EAX);
			sprintf(Reg, "RSP_Vect[%i].B[8]", RSPOpC.rt);
			MoveVariableToX86reg(&RSP_Vect[RSPOpC.rt].B[8], Reg, x86_EBX);
			sprintf(Reg, "RSP_Vect[%i].B[4]", RSPOpC.rt);
			MoveVariableToX86reg(&RSP_Vect[RSPOpC.rt].B[4], Reg, x86_ECX);
			sprintf(Reg, "RSP_Vect[%i].B[0]", RSPOpC.rt);
			MoveVariableToX86reg(&RSP_Vect[RSPOpC.rt].B[0], Reg, x86_EDX);

			sprintf(Reg, "Dmem+%Xh+0", Addr);
			MoveX86regToVariable(x86_EAX, RSPInfo.DMEM + Addr + 0, Reg);
			sprintf(Reg, "Dmem+%Xh+4", Addr);
			MoveX86regToVariable(x86_EBX, RSPInfo.DMEM + Addr + 4, Reg);
			sprintf(Reg, "Dmem+%Xh+8", Addr);
			MoveX86regToVariable(x86_ECX, RSPInfo.DMEM + Addr + 8, Reg);
			sprintf(Reg, "Dmem+%Xh+C", Addr);
			MoveX86regToVariable(x86_EDX, RSPInfo.DMEM + Addr + 12, Reg);
		} else {
			sprintf(Reg, "RSP_Vect[%i].B[0]", RSPOpC.rt);
			SseMoveAlignedVariableToReg(&RSP_Vect[RSPOpC.rt].B[0], Reg, x86_XMM0);
			SseShuffleReg(x86_XMM0, x86_MM0, 0x1b);
			sprintf(Reg, "Dmem+%Xh", Addr);
			SseMoveUnalignedRegToVariable(x86_XMM0, RSPInfo.DMEM + Addr, Reg);
		}
		return;
	}
	
	MoveVariableToX86reg(&RSP_GPR[RSPOpC.base].UW, GPR_Name(RSPOpC.base), x86_EBX);
	if (offset != 0) {
		AddConstToX86Reg(x86_EBX, offset);
	}
	TestConstToX86Reg(15, x86_EBX);
	JneLabel32("Unaligned", 0);
	Jump[0] = RecompPos - 4;

	CompilerToggleBuffer();
	CPU_Message("   Unaligned:");
	x86_SetBranch32b((DWORD*)Jump[0], (DWORD*)RecompPos);
	Cheat_r4300iOpcodeNoMessage(RSP_Opcode_SQV,"RSP_Opcode_SQV");
	JmpLabel32("Done", 0);
	Jump[1] = RecompPos - 4;
	CompilerToggleBuffer();

	AndConstToX86Reg(x86_EBX, 0x0fff);
	if (IsSseEnabled == FALSE) {
		sprintf(Reg, "RSP_Vect[%i].B[12]", RSPOpC.rt);
		MoveVariableToX86reg(&RSP_Vect[RSPOpC.rt].B[12], Reg, x86_EAX);
		sprintf(Reg, "RSP_Vect[%i].B[8]", RSPOpC.rt);
		MoveVariableToX86reg(&RSP_Vect[RSPOpC.rt].B[8], Reg, x86_ECX);
		sprintf(Reg, "RSP_Vect[%i].B[4]", RSPOpC.rt);
		MoveVariableToX86reg(&RSP_Vect[RSPOpC.rt].B[4], Reg, x86_EDX);
		sprintf(Reg, "RSP_Vect[%i].B[0]", RSPOpC.rt);
		MoveVariableToX86reg(&RSP_Vect[RSPOpC.rt].B[0], Reg, x86_EDI);

		MoveX86regToN64MemDisp(x86_EAX, x86_EBX, 0);
		MoveX86regToN64MemDisp(x86_ECX, x86_EBX, 4);
		MoveX86regToN64MemDisp(x86_EDX, x86_EBX, 8);
		MoveX86regToN64MemDisp(x86_EDI, x86_EBX, 12);
	} else {
		sprintf(Reg, "RSP_Vect[%i].B[0]", RSPOpC.rt);
		SseMoveAlignedVariableToReg(&RSP_Vect[RSPOpC.rt].B[0], Reg, x86_XMM0);
		SseShuffleReg(x86_XMM0, x86_MM0, 0x1b);
		SseMoveUnalignedRegToN64Mem(x86_XMM0, x86_EBX);
	}
	CPU_Message("   Done:");
	x86_SetBranch32b((DWORD*)Jump[1], (DWORD*)RecompPos);*/
	LogMessage("TODO: CompileRsp_Opcode_SQV");
}

void CompileRsp_Opcode_SRV ( void ) {
	InterpreterFallback((void*)RSP_Opcode_SRV,"RSP_Opcode_SRV");
}

void CompileRsp_Opcode_SPV ( void ) {
	InterpreterFallback((void*)RSP_Opcode_SPV,"RSP_Opcode_SPV");
}

void CompileRsp_Opcode_SUV ( void ) {
	InterpreterFallback((void*)RSP_Opcode_SUV,"RSP_Opcode_SUV");
}

void CompileRsp_Opcode_SHV ( void ) {
	InterpreterFallback((void*)RSP_Opcode_SHV,"RSP_Opcode_SHV");
}

void CompileRsp_Opcode_SFV ( void ) {
	InterpreterFallback((void*)RSP_Opcode_SFV,"RSP_Opcode_SFV");
}

void CompileRsp_Opcode_STV ( void ) {
	InterpreterFallback((void*)RSP_Opcode_STV,"RSP_Opcode_STV");
}

void CompileRsp_Opcode_SWV ( void ) {
	InterpreterFallback((void*)RSP_Opcode_SWV,"RSP_Opcode_SWV");
}

/************************** Other functions **************************/

void CompileRsp_WrapToBeginOfImem() {
	JmpLabel32(&RspRecompPos, "000", 0);
	Branch_AddRef(RspCompilePC & 0xFFC, (DWORD*)(RspRecompPos - 4));
}

void CompileRsp_CheckRspIsRunning() {
	CompConstToVariable(&RspRecompPos, 0, &RSP_Running, "RSP_Running");
	JneLabel8(&RspRecompPos, "RSP_Running", 0);
	BYTE* pos = RspRecompPos - 1;
	MoveConstHalfToVariable(&RspRecompPos, (WORD)RspCompilePC, &SP_PC_REG, "RSP PC");
	Ret(&RspRecompPos);
	RSP_CPU_Message("   RSP_Running:");
	x86_SetBranch8b(pos, RspRecompPos);
}

void CompileRsp_SaveBeginOfSubBlock() {
	MoveConstHalfToVariable(&RspRecompPos, (WORD)RspCompilePC, &BeginOfCurrentSubBlock, "BeginOfCurentSubBlock");
}

void CompileRsp_UpdateCycleCounts() {
	MoveConstToX86reg(&RspRecompPos, RspCompilePC+4, x86_EAX);
	SubVariableFromX86reg(&RspRecompPos, x86_EAX, &BeginOfCurrentSubBlock, "BeginOfcurrentSubBlock");
	SubX86regFromVariable(&RspRecompPos, x86_EAX, &RemainingRspCycles, "RemainingRspCycles");
	JgLabel8(&RspRecompPos, "NotYetFinished", 0);
	BYTE* pos = RspRecompPos - 1;
	MoveConstByteToVariable(&RspRecompPos, 0, &RSP_Running, "RSP_Running");
	RSP_CPU_Message("   NotYetFinished:");
	x86_SetBranch8b(pos, RspRecompPos);
}

void CompileRsp_ConsecutiveDelaySlots() {
	RSP_CPU_Message("%X Unhandles conscutive delay slots", RspCompilePC);
	RSP_NextInstruction = FINISH_BLOCK;
	MoveConstToVariable(&RspRecompPos, RspCompilePC, &SP_PC_REG, "RSP PC");
	MoveConstToVariable(&RspRecompPos, RSPOpC.OP.Hex, &RSPOpC.OP.Hex, "RSPOpC.Hex");
	Call_Direct(&RspRecompPos, (void*)rsp_UnknownOpcode, "rsp_UnknownOpcode");
	Ret(&RspRecompPos);
}

void CompileRsp_UnknownOpcode (void) {
	RSP_CPU_Message("  %X Unhandled Opcode: %s",RspCompilePC, RSPOpcodeName(RSPOpC.OP.Hex,RspCompilePC) );	
	RSP_NextInstruction = FINISH_BLOCK;
	MoveConstToVariable(&RspRecompPos, RspCompilePC,&SP_PC_REG,"RSP PC");
	MoveConstToVariable(&RspRecompPos, RSPOpC.OP.Hex,&RSPOpC.OP.Hex, "RSPOpC.Hex");
	Call_Direct(&RspRecompPos, (void*)rsp_UnknownOpcode, "rsp_UnknownOpcode" );
	Ret(&RspRecompPos);
}
