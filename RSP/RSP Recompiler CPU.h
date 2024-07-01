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

#pragma once

#include "RSP_OpCode.h"

#ifdef __cplusplus
extern "C" {
#endif

#define NORMAL							0
#define DO_DELAY_SLOT 					1
#define DELAY_SLOT 						2
#define DELAY_SLOT_DONE					3
#define FINISH_BLOCK					4
#define FINISH_SUB_BLOCK				5

	extern DWORD RspCompilePC, RSP_NextInstruction, RemainingRspCycles;

#define RspCompilerWarning if (RspShowErrors) DisplayError

	/*#define High16BitAccum		1
	#define Middle16BitAccum	2
	#define Low16BitAccum		4
	#define EntireAccum			(Low16BitAccum|Middle16BitAccum|High16BitAccum)

	BOOL WriteToAccum (int Location, int PC);
	BOOL WriteToVectorDest (DWORD DestReg, int PC);
	BOOL UseRspFlags (int PC);*/

	BOOL RspDelaySlotAffectBranch(DWORD PC);
	BOOL RspCompareInstructions(DWORD PC, OPCODE* Top, OPCODE* Bottom);
	BOOL IsRspOpcodeBranch(DWORD PC, OPCODE RspOp);
	BOOL IsRspOpcodeNop(DWORD PC);

	/*BOOL IsNextInstructionMmx(DWORD PC);*/
	BOOL IsRspRegisterConstant(DWORD Reg, DWORD* Constant);

	/*void RSP_Element2Mmx(int MmxReg);
	void RSP_MultiElement2Mmx(int MmxReg1, int MmxReg2);

	#define MainBuffer			0
	#define SecondaryBuffer		1*/

	DWORD RunRecompilerRspCPU(DWORD Cycles);
	void BuildRecompilerCPU(void);

	/*void CompilerRSPBlock ( void );
	void CompilerToggleBuffer (void);*/
	BOOL RSP_DoSections(void);

	typedef struct {
		DWORD StartPC, CurrPC;		/* block start */

		struct {
			DWORD TargetPC;			/* Target for this unknown branch */
			DWORD* X86JumpLoc;		/* Our x86 dword to fill */
		} BranchesToResolve[200];	/* Branches inside or outside block */

		DWORD ResolveCount;			/* Branches with NULL jump table */
		BYTE IMEM[0x1000];			/* Saved off for re-order */
	} RSP_BLOCK;

	extern RSP_BLOCK RspCurrentBlock;

	typedef struct {
		BOOL bIsRegConst[32];		/* BOOLean toggle for constant */
		DWORD MipsRegConst[32];		/* Value of register 32-bit */
		DWORD BranchLabels[200];
		DWORD LabelCount;
		DWORD BranchLocations[200];
		DWORD BranchCount;
	} RSP_CODE;

	/*extern RSP_CODE RspCode;

	#define IsRegConst(i)	(RspCode.bIsRegConst[i])
	#define MipsRegConst(i) (RspCode.MipsRegConst[i])*/

	typedef struct {
		/*	BOOL mmx, mmx2, sse;*/	/* CPU specs and compiling */
		/*	BOOL bFlags;*/			/* RSP Flag Analysis */
		BOOL bReOrdering;		/* Instruction reordering */
		BOOL bSections;			/* Microcode sections */
	/*	BOOL bDest;*/				/* Vector destionation toggle */
	/*	BOOL bAccum;*/			/* Accumulator toggle */
		BOOL bGPRConstants;		/* Analyze GPR constants */
	/*	BOOL bAlignVector;*/		/* Align known vector loads */
	/*	BOOL bAlignGPR;*/			/* Align known gpr loads */
	} RSP_COMPILER;

	extern RSP_COMPILER RspCompiler;

	/*#define IsMmxEnabled	(Compiler.mmx)
	#define IsMmx2Enabled	(Compiler.mmx2)
	#define IsSseEnabled	(Compiler.sse)*/

	extern BOOL IMEMIsUpdated;

#ifdef __cplusplus
}
#endif
