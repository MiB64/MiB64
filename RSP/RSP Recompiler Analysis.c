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
/*#include "CPU.h"*/
#include "RSP Recompiler CPU.h"
#include "RSP Command.h"
#include "rsp_memory.h"
#include "RSP_OpCode.h"
/*#include "log.h"*/
#include "rsp_config.h"
#include "../Main.h"

#define RSP_SAFE_ANALYSIS /* makes optimization less risky (dlist useful) */

/************************************************************
** IsRspOpcodeNop
**
** Output: BOOLean whether opcode at PC is a NOP
** Input: PC
*************************************************************/

BOOL IsRspOpcodeNop(DWORD PC) {
	OPCODE RspOp;
	RSP_LW_IMEM(PC, &RspOp.OP.Hex);

	if (RspOp.OP.I.op == RSP_SPECIAL && RspOp.OP.R.funct == RSP_SPECIAL_SLL) {
		return (RspOp.OP.R.rd == 0) ? TRUE : FALSE;
	}

	return FALSE;
}

/************************************************************
** IsNextInstructionMmx
**
** Output: determines EMMS status
** Input: PC
*************************************************************/

/*BOOL IsNextInstructionMmx(DWORD PC) {
	OPCODE RspOp;
	
	if (IsMmxEnabled == FALSE)
		return FALSE;

	PC += 4;
	if (PC >= 0x1000) return FALSE;
	RSP_LW_IMEM(PC, &RspOp.Hex);

	if (RspOp.op != RSP_CP2)
		return FALSE;

	if ((RspOp.rs & 0x10) != 0) {
		switch (RspOp.funct) {
		case RSP_VECTOR_VMULF:
		case RSP_VECTOR_VMUDL:*/ /* Warning: Not all handled? */
/*		case RSP_VECTOR_VMUDM:
		case RSP_VECTOR_VMUDN:
		case RSP_VECTOR_VMUDH:
			if (TRUE == WriteToAccum(7, PC)) {
				return FALSE;
			} else if ((RspOp.rs & 0x0f) >= 2 && (RspOp.rs & 0x0f) <= 7 && IsMmx2Enabled == FALSE) {
				return FALSE;
			} else 
				return TRUE;

		case RSP_VECTOR_VAND:
		case RSP_VECTOR_VOR:
		case RSP_VECTOR_VXOR:
			if (TRUE == WriteToAccum(Low16BitAccum, PC)) {
				return FALSE;
			} else if ((RspOp.rs & 0x0f) >= 2 && (RspOp.rs & 0x0f) <= 7 && IsMmx2Enabled == FALSE) {
				return FALSE;
			} else
				return TRUE;

		case RSP_VECTOR_VADD:*/
			/* Requires no accumulator write! & No flags! */
/*			if (WriteToAccum(Low16BitAccum, CompilePC) == TRUE) {
				return FALSE;
			} else if (UseRspFlags(CompilePC) == TRUE) {
				return FALSE;
			} else if ((RspOp.rs & 0x0f) >= 2 && (RspOp.rs & 0x0f) <= 7 && IsMmx2Enabled == FALSE) {
				return FALSE;
			} else
				return TRUE;

		default:
			return FALSE;
		}
	} else 
		return FALSE;
}*/

/************************************************************
** WriteToAccum2
**
** Output:
**	TRUE: Accumulation series
**	FALSE: Accumulator is reset after this op
**
** Input: PC, Location in accumulator
*************************************************************/

/*#define HIT_BRANCH	0x2

DWORD WriteToAccum2 (int Location, int PC, BOOL RecursiveCall) {
	OPCODE RspOp;
	DWORD BranchTarget = 0;
	signed int BranchImmed = 0;
	DWORD JumpTarget = 0;
	BOOL JumpUncond = FALSE;

	int Instruction_State = NextInstruction;

	if (Compiler.bAccum == FALSE) return TRUE;

	if (Instruction_State == DELAY_SLOT) { 
		return TRUE; 
	}

	do {
		PC += 4;
		if (PC >= 0x1000) { return TRUE; }
		RSP_LW_IMEM(PC, &RspOp.Hex);

		switch (RspOp.op) {
 		case RSP_REGIMM:
			switch (RspOp.rt) {
			case RSP_REGIMM_BLTZ:
			case RSP_REGIMM_BGEZ:
			case RSP_REGIMM_BLTZAL:
			case RSP_REGIMM_BGEZAL:
				Instruction_State = DO_DELAY_SLOT;
				break;
			default:
				CompilerWarning("Unkown opcode in WriteToAccum\n%s",RSPOpcodeName(RspOp.Hex,PC));
				return TRUE;
			}
			break;
		case RSP_SPECIAL:
			switch (RspOp.funct) {
			case RSP_SPECIAL_SLL:
			case RSP_SPECIAL_SRL:
			case RSP_SPECIAL_SRA:
			case RSP_SPECIAL_SLLV:
			case RSP_SPECIAL_SRLV:
			case RSP_SPECIAL_SRAV:
			case RSP_SPECIAL_ADD:
			case RSP_SPECIAL_ADDU:
			case RSP_SPECIAL_SUB:
			case RSP_SPECIAL_SUBU:
			case RSP_SPECIAL_AND:
			case RSP_SPECIAL_OR:
			case RSP_SPECIAL_XOR:
			case RSP_SPECIAL_NOR:
			case RSP_SPECIAL_SLT:
			case RSP_SPECIAL_SLTU:
			case RSP_SPECIAL_BREAK:
				break;

			case RSP_SPECIAL_JR:
				Instruction_State = DO_DELAY_SLOT;
				break;

			default:
				CompilerWarning("Unkown opcode in WriteToAccum\n%s",RSPOpcodeName(RspOp.Hex,PC));
				return TRUE;
			}
			break;
		case RSP_J:
		case RSP_JAL:
			if (!JumpTarget) {
				JumpUncond = TRUE;
				JumpTarget = (RspOp.target << 2) & 0xFFC;
			}
			Instruction_State = DO_DELAY_SLOT;
			break;

		case RSP_BEQ:
		case RSP_BNE:
		case RSP_BLEZ:
		case RSP_BGTZ:
			Instruction_State = DO_DELAY_SLOT;
			BranchTarget = (PC + ((short)RspOp.offset << 2) + 4) & 0xFFC;
			BranchImmed = (short)RspOp.offset;
			break;
		case RSP_ADDI:
		case RSP_ADDIU:
		case RSP_SLTI:
		case RSP_SLTIU:
		case RSP_ANDI:
		case RSP_ORI:
		case RSP_XORI:
		case RSP_LUI:
		case RSP_CP0:			
			break;

		case RSP_CP2:
			if ((RspOp.rs & 0x10) != 0) {
				switch (RspOp.funct) {
				case RSP_VECTOR_VMULF:
				case RSP_VECTOR_VMUDL:
				case RSP_VECTOR_VMUDM:
				case RSP_VECTOR_VMUDN:
				case RSP_VECTOR_VMUDH:
					return FALSE;
				case RSP_VECTOR_VMACF:
				case RSP_VECTOR_VMADL:
				case RSP_VECTOR_VMADM:
				case RSP_VECTOR_VMADN:
				case RSP_VECTOR_VMADH:
					return TRUE;
				case RSP_VECTOR_VABS: // hope this is ok
				case RSP_VECTOR_VADD:
				case RSP_VECTOR_VADDC:
				case RSP_VECTOR_VSUB:
				case RSP_VECTOR_VSUBC:
				case RSP_VECTOR_VAND:
				case RSP_VECTOR_VOR:
				case RSP_VECTOR_VXOR:
				case RSP_VECTOR_VNXOR:
				case RSP_VECTOR_VCR:
				case RSP_VECTOR_VCH:
				case RSP_VECTOR_VCL:
				case RSP_VECTOR_VRCP:
				case RSP_VECTOR_VRCPL: // hope this is ok
				case RSP_VECTOR_VRCPH:
				case RSP_VECTOR_VRSQL:
				case RSP_VECTOR_VRSQH: // hope this is ok
				case RSP_VECTOR_VLT:
				case RSP_VECTOR_VEQ:
				case RSP_VECTOR_VGE:
					if (Location == Low16BitAccum) { return FALSE; }
					break;

				case RSP_VECTOR_VMOV:
				case RSP_VECTOR_VMRG:
					break;
				case RSP_VECTOR_VSAW:
					return TRUE;
				default:
					CompilerWarning("Unkown opcode in WriteToVectorDest\n%s",RSPOpcodeName(RspOp.Hex,PC));
					return TRUE;
				}
			} else {
				switch (RspOp.rs) {				
				case RSP_COP2_CF:
				case RSP_COP2_CT:
				case RSP_COP2_MT:
				case RSP_COP2_MF:
					break;
				default:
					CompilerWarning("Unkown opcode in WriteToVectorDest\n%s",RSPOpcodeName(RspOp.Hex,PC));
					return TRUE;
				}
			}
			break;
		case RSP_LB:
		case RSP_LH:
		case RSP_LW:
		case RSP_LBU:
		case RSP_LHU:
		case RSP_SB:
		case RSP_SH:
		case RSP_SW:
			break;
		case RSP_LC2:
			switch (RspOp.rd) {
			case RSP_LSC2_SV:
			case RSP_LSC2_DV:
			case RSP_LSC2_RV:
			case RSP_LSC2_QV:
			case RSP_LSC2_LV:
			case RSP_LSC2_UV:
			case RSP_LSC2_PV:
			case RSP_LSC2_TV:
				break;
			default:
				CompilerWarning("Unkown opcode in WriteToAccum\n%s",RSPOpcodeName(RspOp.Hex,PC));
				return TRUE;
			}
			break;
		case RSP_SC2:
			switch (RspOp.rd) {
			case RSP_LSC2_BV:
			case RSP_LSC2_SV:
			case RSP_LSC2_LV:
			case RSP_LSC2_DV:
			case RSP_LSC2_QV:
			case RSP_LSC2_RV:
			case RSP_LSC2_PV:
			case RSP_LSC2_UV:
			case RSP_LSC2_HV:
			case RSP_LSC2_FV:
			case RSP_LSC2_WV:
			case RSP_LSC2_TV:
				break;
			default:
				CompilerWarning("Unkown opcode in WriteToAccum\n%s",RSPOpcodeName(RspOp.Hex,PC));
				return TRUE;
			}
			break;
		default:
			CompilerWarning("Unkown opcode in WriteToAccum\n%s",RSPOpcodeName(RspOp.Hex,PC));
			return TRUE;
		}
		switch (Instruction_State) {
		case NORMAL: break;
		case DO_DELAY_SLOT: 
			Instruction_State = DELAY_SLOT;
			break;
		case DELAY_SLOT: 
			if (JumpUncond) {
				PC = JumpTarget - 0x04;
				Instruction_State = NORMAL;
			} else {
				Instruction_State = FINISH_BLOCK; 
			}
			JumpUncond = FALSE;
			break;
		}
	} while (Instruction_State != FINISH_BLOCK);*/

	/***
	** This is a tricky situation because most of the 
	** microcode does loops, so looping back and checking
	** can prove effective, but it's still a branch..
	***/

/*	if (BranchTarget != 0 && RecursiveCall == FALSE) {
		DWORD BranchTaken, BranchFall;*/

		/* analysis of branch taken */
/*		BranchTaken = WriteToAccum2(Location, BranchTarget - 4, TRUE);*/
		/* analysis of branch as nop */
/*		BranchFall = WriteToAccum2(Location, PC, TRUE);

		if (BranchImmed < 0) {
			if (BranchTaken != FALSE) {*/
				/*
				** took this back branch and couldnt find a place 
				** that resets the accum or hit a branch etc 
				**/
/*				return TRUE;
			} else if (BranchFall == HIT_BRANCH) {*/
				/* risky? the loop ended, hit another branch after loop-back */
			
/*#if !defined(RSP_SAFE_ANALYSIS)
				CPU_Message("WriteToDest: Backward branch hit, BranchFall = Hit branch (returning FALSE)");
				return FALSE;
#endif
				return TRUE;
			} else {*/
				/* otherwise this is completely valid */
/*				return BranchFall;
			}
		} else {	
			if (BranchFall != FALSE) {*/
				/*
				** took this forward branch and couldnt find a place 
				** that resets the accum or hit a branch etc 
				**/
/*				return TRUE;
			} else if (BranchTaken == HIT_BRANCH) {*/
				/* risky? jumped forward, hit another branch */
				
/*#if !defined(RSP_SAFE_ANALYSIS)
				CPU_Message("WriteToDest: Forward branch hit, BranchTaken = Hit branch (returning FALSE)");
				return FALSE;
#endif

				return TRUE;
			} else {*/
				/* otherwise this is completely valid */
/*				return BranchTaken;
			}
		}
	} else {
		return HIT_BRANCH;
	}
}

BOOL WriteToAccum (int Location, int PC) {
	DWORD value = WriteToAccum2(Location, PC, FALSE);

	if (value == HIT_BRANCH) {
		return TRUE;*/ /* ??? */
/*	} else
		return value;
}*/

/************************************************************
** WriteToVectorDest
**
** Output:
**	TRUE: Destination is over-written later
**	FALSE: Destination is used as a source later
**
** Input: PC, Register
*************************************************************/

/*BOOL WriteToVectorDest2 (DWORD DestReg, int PC, BOOL RecursiveCall) {
	OPCODE RspOp;
	DWORD BranchTarget = 0;
	signed int BranchImmed = 0;
	DWORD JumpTarget = 0;
	BOOL JumpUncond = FALSE;

	int Instruction_State = NextInstruction;

	if (Compiler.bDest == FALSE) return TRUE;

	if (Instruction_State == DELAY_SLOT) { 
		return TRUE; 
	}
	
	do {
		PC += 4;
		if (PC >= 0x1000) { return TRUE; }
		RSP_LW_IMEM(PC, &RspOp.Hex);

		switch (RspOp.op) {

		case RSP_REGIMM:
			switch (RspOp.rt) {
			case RSP_REGIMM_BLTZ:
			case RSP_REGIMM_BGEZ:
			case RSP_REGIMM_BLTZAL:
			case RSP_REGIMM_BGEZAL:
				Instruction_State = DO_DELAY_SLOT;
				break;
			default:
				CompilerWarning("Unkown opcode in WriteToVectorDest\n%s",RSPOpcodeName(RspOp.Hex,PC));
				return TRUE;
			}
			break;
		case RSP_SPECIAL:
			switch (RspOp.funct) {
			case RSP_SPECIAL_SLL:
			case RSP_SPECIAL_SRL:
			case RSP_SPECIAL_SRA:
			case RSP_SPECIAL_SLLV:
			case RSP_SPECIAL_SRLV:
			case RSP_SPECIAL_SRAV:
			case RSP_SPECIAL_ADD:
			case RSP_SPECIAL_ADDU:
			case RSP_SPECIAL_SUB:
			case RSP_SPECIAL_SUBU:
			case RSP_SPECIAL_AND:
			case RSP_SPECIAL_OR:
			case RSP_SPECIAL_XOR:
			case RSP_SPECIAL_NOR:
			case RSP_SPECIAL_SLT:
			case RSP_SPECIAL_SLTU:
			case RSP_SPECIAL_BREAK:
				break;

			case RSP_SPECIAL_JR:
				Instruction_State = DO_DELAY_SLOT;
				break;

			default:
				CompilerWarning("Unkown opcode in WriteToVectorDest\n%s",RSPOpcodeName(RspOp.Hex,PC));
				return TRUE;
			}
			break;
		case RSP_J:
		case RSP_JAL:
			if (!JumpTarget) {
				JumpUncond = TRUE;
				JumpTarget = (RspOp.target << 2) & 0xFFC;
			}
			Instruction_State = DO_DELAY_SLOT;
			break;
		case RSP_BEQ:
		case RSP_BNE:
		case RSP_BLEZ:
		case RSP_BGTZ:
			Instruction_State = DO_DELAY_SLOT;
			BranchTarget = (PC + ((short)RspOp.offset << 2) + 4) & 0xFFC;
			BranchImmed = (short)RspOp.offset;
			break;
		case RSP_ADDI:
		case RSP_ADDIU:
		case RSP_SLTI:
		case RSP_SLTIU:
		case RSP_ANDI:
		case RSP_ORI:
		case RSP_XORI:
		case RSP_LUI:
		case RSP_CP0:			
			break;

		case RSP_CP2:
			if ((RspOp.rs & 0x10) != 0) {
				switch (RspOp.funct) {
				case RSP_VECTOR_VMULF:
				case RSP_VECTOR_VMUDL:
				case RSP_VECTOR_VMUDM:
				case RSP_VECTOR_VMUDN:
				case RSP_VECTOR_VMUDH:
				case RSP_VECTOR_VMACF:
				case RSP_VECTOR_VMADL:
				case RSP_VECTOR_VMADM:
				case RSP_VECTOR_VMADN:
				case RSP_VECTOR_VMADH:
				case RSP_VECTOR_VADD:
				case RSP_VECTOR_VADDC:
				case RSP_VECTOR_VSUB:
				case RSP_VECTOR_VSUBC:
				case RSP_VECTOR_VAND:
				case RSP_VECTOR_VOR:
				case RSP_VECTOR_VXOR:
				case RSP_VECTOR_VNXOR:
				case RSP_VECTOR_VABS:
					if (DestReg == RspOp.rd) { return TRUE; }
					if (DestReg == RspOp.rt) { return TRUE; }
					if (DestReg == RspOp.sa) { return FALSE; }
					break;

				case RSP_VECTOR_VMOV:
					if (DestReg == RspOp.rt) { return TRUE; }
					break;

				case RSP_VECTOR_VCR:
				case RSP_VECTOR_VRCP:
				case RSP_VECTOR_VRCPH:
				case RSP_VECTOR_VRSQH:
				case RSP_VECTOR_VCH:
				case RSP_VECTOR_VCL:
					return TRUE;

				case RSP_VECTOR_VMRG:
				case RSP_VECTOR_VLT:
				case RSP_VECTOR_VEQ:
				case RSP_VECTOR_VGE:
					if (DestReg == RspOp.rd) { return TRUE; }
					if (DestReg == RspOp.rt) { return TRUE; }
					if (DestReg == RspOp.sa) { return FALSE; }
					break;
				case RSP_VECTOR_VSAW:
					if (DestReg == RspOp.sa) { return FALSE; }
					break;
				default:
					CompilerWarning("Unkown opcode in WriteToVectorDest\n%s",RSPOpcodeName(RspOp.Hex,PC));
					return TRUE;
				}
			} else {
				switch (RspOp.rs) {				
				case RSP_COP2_CF:
				case RSP_COP2_CT:
					break;
				case RSP_COP2_MT:*/
				/*	if (DestReg == RspOp.rd) { return FALSE; } */
/*					break;
				case RSP_COP2_MF:
					if (DestReg == RspOp.rd) { return TRUE; }
					break;
				default:
					CompilerWarning("Unkown opcode in WriteToVectorDest\n%s",RSPOpcodeName(RspOp.Hex,PC));
					return TRUE;
				}
			}
			break;
		case RSP_LB:
		case RSP_LH:
		case RSP_LW:
		case RSP_LBU:
		case RSP_LHU:
		case RSP_SB:
		case RSP_SH:
		case RSP_SW:
			break;
		case RSP_LC2:
			switch (RspOp.rd) {
			case RSP_LSC2_SV:
			case RSP_LSC2_DV:
			case RSP_LSC2_RV:
				break;

			case RSP_LSC2_QV:
			case RSP_LSC2_LV:
			case RSP_LSC2_UV:
			case RSP_LSC2_PV:
			case RSP_LSC2_TV:			
				break;

			default:
				CompilerWarning("Unkown opcode in WriteToVectorDest\n%s",RSPOpcodeName(RspOp.Hex,PC));
				return TRUE;
			}
			break;
		case RSP_SC2:
			switch (RspOp.rd) {
			case RSP_LSC2_BV:
			case RSP_LSC2_SV:
			case RSP_LSC2_LV:
			case RSP_LSC2_DV:
			case RSP_LSC2_QV:
			case RSP_LSC2_RV:
			case RSP_LSC2_PV:
			case RSP_LSC2_UV:
			case RSP_LSC2_HV:
			case RSP_LSC2_FV:
			case RSP_LSC2_WV:
			case RSP_LSC2_TV:
				if (DestReg == RspOp.rt) { return TRUE; }
				break;
			default:
				CompilerWarning("Unkown opcode in WriteToVectorDest\n%s",RSPOpcodeName(RspOp.Hex,PC));
				return TRUE;
			}
			break;
		default:
			CompilerWarning("Unkown opcode in WriteToVectorDest\n%s",RSPOpcodeName(RspOp.Hex,PC));
			return TRUE;
		}
		switch (Instruction_State) {
		case NORMAL: break;
		case DO_DELAY_SLOT: 
			Instruction_State = DELAY_SLOT;
			break;
		case DELAY_SLOT: 
			if (JumpUncond) {
				PC = JumpTarget - 0x04;
				Instruction_State = NORMAL;
			} else {
				Instruction_State = FINISH_BLOCK; 
			}
			JumpUncond = FALSE;
			break;
		}
	} while (Instruction_State != FINISH_BLOCK);*/
	
	/***
	** This is a tricky situation because most of the 
	** microcode does loops, so looping back and checking
	** can prove effective, but it's still a branch..
	***/

/*	if (BranchTarget != 0 && RecursiveCall == FALSE) {
		DWORD BranchTaken, BranchFall;*/

		/* analysis of branch taken */
/*		BranchTaken = WriteToVectorDest2(DestReg, BranchTarget - 4, TRUE);*/
		/* analysis of branch as nop */
/*		BranchFall = WriteToVectorDest2(DestReg, PC, TRUE);

		if (BranchImmed < 0) {
			if (BranchTaken != FALSE) {*/
				/*
				** took this back branch and found a place
				** that needs this vector as a source
				**/
/*				return TRUE;
			} else if (BranchFall == HIT_BRANCH) {*/
				/* (dlist) risky? the loop ended, hit another branch after loop-back */
			
/*#if !defined(RSP_SAFE_ANALYSIS)
				CPU_Message("WriteToDest: Backward branch hit, BranchFall = Hit branch (returning FALSE)");
				return FALSE;
#endif

				return TRUE;
			} else {*/
				/* otherwise this is completely valid */
/*				return BranchFall;
			}
		} else {	
			if (BranchFall != FALSE) {*/
				/*
				** took this forward branch and found a place
				** that needs this vector as a source
				**/
/*				return TRUE;
			} else if (BranchTaken == HIT_BRANCH) {*/
				/* (dlist) risky? jumped forward, hit another branch */

/*#if !defined(RSP_SAFE_ANALYSIS)
				CPU_Message("WriteToDest: Forward branch hit, BranchTaken = Hit branch (returning FALSE)");
				return FALSE;
#endif

				return TRUE;
			} else {*/
				/* otherwise this is completely valid */
/*				return BranchTaken;
			}
		}
	} else {
		return HIT_BRANCH;
	}
}

BOOL WriteToVectorDest (DWORD DestReg, int PC) {
	DWORD value = WriteToVectorDest2(DestReg, PC, FALSE);

	if (value == HIT_BRANCH) {
		return TRUE;*/ /* ??? */
/*	} else
		return value;
}*/

/************************************************************
** UseRspFlags
**
** Output:
**	TRUE: Flags are determined not in use
**	FALSE: Either unable to determine or are in use
**
** Input: PC
*************************************************************/

/* TODO: consider delay slots and such in a branch? */
/*BOOL UseRspFlags (int PC) {
	OPCODE RspOp;
	int Instruction_State = NextInstruction;

	if (Compiler.bFlags == FALSE) return TRUE;

	if (Instruction_State == DELAY_SLOT) { 
		return TRUE; 
	}

	do {
		PC -= 4;
		if (PC < 0) { return TRUE; }
		RSP_LW_IMEM(PC, &RspOp.Hex);

		switch (RspOp.op) {

		case RSP_REGIMM:
			switch (RspOp.rt) {
			case RSP_REGIMM_BLTZ:
			case RSP_REGIMM_BGEZ:
				Instruction_State = DO_DELAY_SLOT;
				break;
			default:
				CompilerWarning("Unkown opcode in WriteToVectorDest\n%s",RSPOpcodeName(RspOp.Hex,PC));
				return TRUE;
			}
			break;
		case RSP_SPECIAL:
			switch (RspOp.funct) {
			case RSP_SPECIAL_SLL:
			case RSP_SPECIAL_SRL:
			case RSP_SPECIAL_SRA:
			case RSP_SPECIAL_SLLV:
			case RSP_SPECIAL_SRLV:
			case RSP_SPECIAL_SRAV:
			case RSP_SPECIAL_ADD:
			case RSP_SPECIAL_ADDU:
			case RSP_SPECIAL_SUB:
			case RSP_SPECIAL_SUBU:
			case RSP_SPECIAL_AND:
			case RSP_SPECIAL_OR:
			case RSP_SPECIAL_XOR:
			case RSP_SPECIAL_NOR:
			case RSP_SPECIAL_SLT:
			case RSP_SPECIAL_SLTU:
			case RSP_SPECIAL_BREAK:
				break;

			case RSP_SPECIAL_JR:
				Instruction_State = DO_DELAY_SLOT;
				break;

			default:
				CompilerWarning("Unkown opcode in WriteToVectorDest\n%s",RSPOpcodeName(RspOp.Hex,PC));
				return TRUE;
			}
			break;
		case RSP_J:
		case RSP_JAL:
		case RSP_BEQ:
		case RSP_BNE:
		case RSP_BLEZ:
		case RSP_BGTZ:
			Instruction_State = DO_DELAY_SLOT;
			break;
		case RSP_ADDI:
		case RSP_ADDIU:
		case RSP_SLTI:
		case RSP_SLTIU:
		case RSP_ANDI:
		case RSP_ORI:
		case RSP_XORI:
		case RSP_LUI:
		case RSP_CP0:			
			break;

		case RSP_CP2:
			if ((RspOp.rs & 0x10) != 0) {
				switch (RspOp.funct) {
				case RSP_VECTOR_VMULF:
				case RSP_VECTOR_VMUDL:
				case RSP_VECTOR_VMUDM:
				case RSP_VECTOR_VMUDN:
				case RSP_VECTOR_VMUDH:
					break;
				case RSP_VECTOR_VMACF:
				case RSP_VECTOR_VMADL:
				case RSP_VECTOR_VMADM:
				case RSP_VECTOR_VMADN:
				case RSP_VECTOR_VMADH:
					break;

				case RSP_VECTOR_VSUB:
				case RSP_VECTOR_VADD:
					return FALSE;
				case RSP_VECTOR_VSUBC:
				case RSP_VECTOR_VADDC:
					return TRUE;

				case RSP_VECTOR_VABS:				
				case RSP_VECTOR_VAND:
				case RSP_VECTOR_VOR:
				case RSP_VECTOR_VXOR:
				case RSP_VECTOR_VRCPH:
				case RSP_VECTOR_VRSQL:
				case RSP_VECTOR_VRSQH:
				case RSP_VECTOR_VRCPL:
				case RSP_VECTOR_VRCP:
					break;

				case RSP_VECTOR_VCR:
				case RSP_VECTOR_VCH:
				case RSP_VECTOR_VCL:				
				case RSP_VECTOR_VLT:
				case RSP_VECTOR_VEQ:
				case RSP_VECTOR_VGE:
				case RSP_VECTOR_VMRG:
					return TRUE;

				case RSP_VECTOR_VSAW:
				case RSP_VECTOR_VMOV:				
					break;

				default:
					CompilerWarning("Unkown opcode in UseRspFlags\n%s",RSPOpcodeName(RspOp.Hex,PC));
					return TRUE;
				}
			} else {
				switch (RspOp.rs) {
				case RSP_COP2_CT:
					return TRUE;

				case RSP_COP2_CF:
				case RSP_COP2_MT:
				case RSP_COP2_MF:
					break;
				default:
					CompilerWarning("Unkown opcode in UseRspFlags\n%s",RSPOpcodeName(RspOp.Hex,PC));
					return TRUE;
				}
			}
			break;
		case RSP_LB:
		case RSP_LH:
		case RSP_LW:
		case RSP_LBU:
		case RSP_LHU:
		case RSP_SB:
		case RSP_SH:
		case RSP_SW:
			break;
		case RSP_LC2:
			switch (RspOp.rd) {
			case RSP_LSC2_SV:
			case RSP_LSC2_DV:
			case RSP_LSC2_RV:
			case RSP_LSC2_QV:
			case RSP_LSC2_LV:
			case RSP_LSC2_UV:
			case RSP_LSC2_PV:
			case RSP_LSC2_TV:
				break;
			default:
				CompilerWarning("Unkown opcode in UseRspFlags\n%s",RSPOpcodeName(RspOp.Hex,PC));
				return TRUE;
			}
			break;
		case RSP_SC2:
			switch (RspOp.rd) {
			case RSP_LSC2_BV:
			case RSP_LSC2_SV:
			case RSP_LSC2_LV:
			case RSP_LSC2_DV:
			case RSP_LSC2_QV:
			case RSP_LSC2_RV:
			case RSP_LSC2_PV:
			case RSP_LSC2_UV:
			case RSP_LSC2_HV:
			case RSP_LSC2_FV:
			case RSP_LSC2_WV:
			case RSP_LSC2_TV:
				break;
			default:
				CompilerWarning("Unkown opcode in UseRspFlags\n%s",RSPOpcodeName(RspOp.Hex,PC));
				return TRUE;
			}
			break;
		default:
			CompilerWarning("Unkown opcode in UseRspFlags\n%s",RSPOpcodeName(RspOp.Hex,PC));
			return TRUE;
		}
		switch (Instruction_State) {
		case NORMAL: break;
		case DO_DELAY_SLOT: 
			Instruction_State = DELAY_SLOT;
			break;
		case DELAY_SLOT: 
			Instruction_State = FINISH_BLOCK; 
			break;
		}
	} while ( Instruction_State != FINISH_BLOCK);
	return TRUE;
}*/

/************************************************************
** IsRspRegisterConstant
**
** Output:
**	TRUE: Register is constant throughout
**	FALSE: Register is not constant at all
**
** Input: PC, Pointer to constant to fill
*************************************************************/

BOOL IsRspRegisterConstant (DWORD Reg, DWORD * Constant) {
	DWORD PC = 0;
	DWORD References = 0;
	DWORD Const = 0;
	OPCODE RspOp;

	if (RspCompiler.bGPRConstants == FALSE) 
		return FALSE;

	while (PC < 0x1000) {
		
		RSP_LW_IMEM(PC, &RspOp.OP.Hex);
		PC += 4;

		switch (RspOp.OP.I.op) {
		case RSP_REGIMM:
			break;

		case RSP_SPECIAL:
			switch (RspOp.OP.R.funct) {
			case RSP_SPECIAL_SLL:
			case RSP_SPECIAL_SRL:
			case RSP_SPECIAL_SRA:
			case RSP_SPECIAL_SLLV:
			case RSP_SPECIAL_SRLV:
			case RSP_SPECIAL_SRAV:
			case RSP_SPECIAL_JALR:
			case RSP_SPECIAL_ADD:
			case RSP_SPECIAL_ADDU:
			case RSP_SPECIAL_SUB:
			case RSP_SPECIAL_SUBU:
			case RSP_SPECIAL_AND:
			case RSP_SPECIAL_OR:
			case RSP_SPECIAL_XOR:
			case RSP_SPECIAL_NOR:
			case RSP_SPECIAL_SLT:
			case RSP_SPECIAL_SLTU:
				if (RspOp.OP.R.rd == Reg) { return FALSE; }
				break;

			case RSP_SPECIAL_BREAK:
			case RSP_SPECIAL_JR:
				break;

			default:
				//RspCompilerWarning("Unkown opcode in IsRspRegisterConstant\n%s",RSPOpcodeName(RspOp.OP.Hex,PC));
				//return FALSE;
				break;
			}
			break;

		case RSP_J:
		case RSP_JAL:
		case RSP_BEQ:
		case RSP_BNE:
		case RSP_BLEZ:
		case RSP_BGTZ:
			break;
		
		case RSP_ADDI:
		case RSP_ADDIU:
			if (RspOp.OP.I.rt == Reg) {
				if (!RspOp.OP.I.rs) {
					if (References > 0) { return FALSE; }
					Const = (short)RspOp.OP.I.immediate;
					References++;
				} else
					return FALSE;
			}
			break;
		case RSP_ORI:
			if (RspOp.OP.I.rt == Reg) {
				if (!RspOp.OP.I.rs) {
					if (References > 0) { return FALSE;	}
					Const = (WORD)(short)RspOp.OP.I.immediate;
					References++;
				} else
					return FALSE;
			}
			break;

		case RSP_LUI:
			if (RspOp.OP.I.rt == Reg) {
				if (References > 0) { return FALSE; }
				Const = (short)RspOp.OP.I.immediate << 16;
				References++;
			}
			break;

		case RSP_ANDI:
		case RSP_XORI:
		case RSP_SLTI:
		case RSP_SLTIU:
			if (RspOp.OP.I.rt == Reg) { return FALSE; }
			break;

		case RSP_CP0:
			switch (RspOp.OP.I.rs) {
			case RSP_COP0_MF:
				if (RspOp.OP.I.rt == Reg) { return FALSE; }
			case RSP_COP0_MT:
				break;
			}			
			break;

		case RSP_CP2:
			if ((RspOp.OP.I.rs & 0x10) == 0) {
				switch (RspOp.OP.I.rs) {
				case RSP_COP2_CT:
				case RSP_COP2_MT:
					break;

				case RSP_COP2_CF:
				case RSP_COP2_MF:
					if (RspOp.OP.R.rt == Reg) { return FALSE; }
					break;

				default:
					RspCompilerWarning("Unkown opcode in IsRspRegisterConstant\n%s",RSPOpcodeName(RspOp.OP.Hex,PC));
					break;
				}
			}
			break;

		case RSP_LB:
		case RSP_LH:
		case RSP_LW:
		case RSP_LBU:
		case RSP_LHU:
		case RSP_LWU:
			if (RspOp.OP.LS.rt == Reg) { return FALSE; }
			break;

		case RSP_SB:
		case RSP_SH:
		case RSP_SW:
			break;
		case RSP_LC2:
			break;
		case RSP_SC2:
			break;
		default:
			//RspCompilerWarning("Unkown opcode in IsRspRegisterConstant\n%s",RSPOpcodeName(RspOp.OP.Hex,PC));
			//return FALSE;
			break;
		}
	}

	if (References > 0) {
		*Constant = Const;
		return TRUE;
	} else {
		*Constant = 0;
		return FALSE;
	}
}

/************************************************************
** IsRspOpcodeBranch
**
** Output:
**	TRUE: opcode is a branch
**	FALSE: opcode is not a branch
**
** Input: PC
*************************************************************/

BOOL IsRspOpcodeBranch(DWORD PC, OPCODE RspOp) {
	switch (RspOp.OP.I.op) {
	case RSP_REGIMM:
		switch (RspOp.OP.B.rt) {
		case RSP_REGIMM_BLTZ:
		case RSP_REGIMM_BGEZ:
		/*case RSP_REGIMM_BLTZAL:
		case RSP_REGIMM_BGEZAL:*/
			return TRUE;
		default:
			RspCompilerWarning("Unknown opcode in IsRspOpcodeBranch\n%s",RSPOpcodeName(RspOp.OP.Hex,PC));
			LogMessage("Unknown opcode in IsRspOpcodeBranch REGIMM:%d", RspOp.OP.B.rt);
			break;
		}
		break;
	case RSP_SPECIAL:
		switch (RspOp.OP.R.funct) {
		case RSP_SPECIAL_SLL:
		case RSP_SPECIAL_SRL:
		case RSP_SPECIAL_SRA:
		case RSP_SPECIAL_SLLV:
		case RSP_SPECIAL_SRLV:
		case RSP_SPECIAL_SRAV:
		case RSP_SPECIAL_ADD:
		case RSP_SPECIAL_ADDU:
		case RSP_SPECIAL_SUB:
		case RSP_SPECIAL_SUBU:
		case RSP_SPECIAL_AND:
		case RSP_SPECIAL_OR:
		case RSP_SPECIAL_XOR:
		case RSP_SPECIAL_NOR:
		case RSP_SPECIAL_SLT:
		case RSP_SPECIAL_SLTU:
		case RSP_SPECIAL_BREAK:
			break;

		case RSP_SPECIAL_JALR:
		case RSP_SPECIAL_JR:
			return TRUE;

		default:
			//RspCompilerWarning("Unknown opcode in IsRspOpcodeBranch\n%s",RSPOpcodeName(RspOp.OP.Hex,PC));
			break;
		}
		break;
	case RSP_J:
	case RSP_JAL:
	case RSP_BEQ:
	case RSP_BNE:
	case RSP_BLEZ:
	case RSP_BGTZ:
		return TRUE;

	case RSP_ADDI:
	case RSP_ADDIU:
	case RSP_SLTI:
	case RSP_SLTIU:
	case RSP_ANDI:
	case RSP_ORI:
	case RSP_XORI:
	case RSP_LUI:

	case RSP_CP0:
	case RSP_CP2:
		break;

	case RSP_LB:
	case RSP_LH:
	case RSP_LW:
	case RSP_LBU:
	case RSP_LHU:
	case RSP_LWU:
	case RSP_SB:
	case RSP_SH:
	case RSP_SW:
		break;

	case RSP_LC2:
	case RSP_SC2:
		break;

	default:
		//RspCompilerWarning("Unknown opcode in IsRspOpcodeBranch\n%s",RSPOpcodeName(RspOp.OP.Hex,PC));
		break;
	}

	return FALSE;
}

/************************************************************
** IsRspDelaySlotBranch
**
** Output:
**	TRUE: next opcode is a branch
**	FALSE: next opcode is not a branch
**
** Input: branch PC
*************************************************************/

BOOL IsRspDelaySlotBranch(DWORD PC) {
	DWORD delaySlotPC = (PC + 4) & 0xFFC;
	OPCODE opcode;
	RSP_LW_IMEM(delaySlotPC, &opcode.OP.Hex);
	return IsRspOpcodeBranch(delaySlotPC, opcode);
}

/************************************************************
** GetInstructionInfo
**
** Output: None in regard to return value
**
** Input:
**	pointer to info structure, fills this
**  with valid opcode data
*************************************************************/

/* 3 possible values, GPR, VEC, VEC & GPR, NOOP is zero */
#define GPR_Instruction		0x0001 /* GPR Instruction flag */
#define VEC_Instruction		0x0002 /* Vec Instruction flag */
#define Instruction_Mask	(GPR_Instruction | VEC_Instruction)

/* 3 possible values, one flag must be set only */
#define Load_Operation		0x0004 /* Load Instruction flag */
#define Store_Operation		0x0008 /* Store Instruction flag */
#define Accum_Operation		0x0010 /* Vector op uses accum - loads & stores dont */
#define MemOperation_Mask	(Load_Operation | Store_Operation)
/*#define Operation_Mask		(MemOperation_Mask | Accum_Operation)*/

/* Per situation basis flags */
#define VEC_ResetAccum		0x0000 /* Vector op resets acc */
#define VEC_Accumulate		0x0020 /* Vector op accumulates */

#define InvalidOpcode		0x0040

typedef struct {
	union {
		DWORD DestReg;
		DWORD StoredReg;
	} DS;
	union {
		DWORD SourceReg0;
		DWORD IndexReg;
	} S0;
	DWORD SourceReg1;
	DWORD flags;
} OPCODE_INFO;

static void GetInstructionInfo(DWORD PC, OPCODE * RspOp, OPCODE_INFO * info) {
	switch (RspOp->OP.I.op) {
	case RSP_REGIMM:
		switch (RspOp->OP.B.rt) {
		case RSP_REGIMM_BLTZ:
		/*case RSP_REGIMM_BLTZAL:*/
		case RSP_REGIMM_BGEZ:
		/*case RSP_REGIMM_BGEZAL:*/
			info->flags = InvalidOpcode;
			info->S0.SourceReg0 = RspOp->OP.B.rs;
			info->SourceReg1 = (DWORD)-1;
			break;

		default:
			RspCompilerWarning("Unkown opcode in GetInstructionInfo\n%s",RSPOpcodeName(RspOp->OP.Hex,PC));
			info->flags = InvalidOpcode;
			break;
		}
		break;
	case RSP_SPECIAL:
		switch (RspOp->OP.R.funct) {
		case RSP_SPECIAL_BREAK:
			info->DS.DestReg = (DWORD)-1;
			info->S0.SourceReg0 = (DWORD)-1;
			info->SourceReg1 = (DWORD)-1;
			info->flags = GPR_Instruction;
			break;

		case RSP_SPECIAL_SLL:
		case RSP_SPECIAL_SRL:
		/*case RSP_SPECIAL_SRA:*/
			info->DS.DestReg = RspOp->OP.R.rd;
			info->S0.SourceReg0 = RspOp->OP.R.rt;
			info->SourceReg1 = (DWORD)-1;
			info->flags = GPR_Instruction;
			break;
		/*case RSP_SPECIAL_SLLV:*/
		case RSP_SPECIAL_SRLV:
		/*case RSP_SPECIAL_SRAV:*/
		case RSP_SPECIAL_ADD:
		/*case RSP_SPECIAL_ADDU:*/
		case RSP_SPECIAL_SUB:
		/*case RSP_SPECIAL_SUBU:*/
		case RSP_SPECIAL_AND:
		/*case RSP_SPECIAL_OR:
		case RSP_SPECIAL_XOR:
		case RSP_SPECIAL_NOR:
		case RSP_SPECIAL_SLT:
		case RSP_SPECIAL_SLTU:*/
			info->DS.DestReg = RspOp->OP.R.rd;
			info->S0.SourceReg0 = RspOp->OP.R.rs;
			info->SourceReg1 = RspOp->OP.R.rt;
			info->flags = GPR_Instruction;
			break;

		/*case RSP_SPECIAL_JR:
			info->flags = InvalidOpcode;
			info->SourceReg0 = -1;
			info->SourceReg1 = -1;
			break;*/

		default:
			RspCompilerWarning("Unkown opcode in GetInstructionInfo\n%s",RSPOpcodeName(RspOp->OP.Hex,PC));
			info->flags = InvalidOpcode;
			break;
		}
		break;
	/*case RSP_J:
	case RSP_JAL:
		info->flags = InvalidOpcode;
		info->SourceReg0 = -1;
		info->SourceReg1 = -1;
		break;*/
	case RSP_BEQ:
	case RSP_BNE:
		info->flags = InvalidOpcode;
		info->S0.SourceReg0 = RspOp->OP.B.rt;
		info->SourceReg1 = RspOp->OP.B.rs;
		break;
	case RSP_BLEZ:
	case RSP_BGTZ:
		info->flags = InvalidOpcode;
		info->S0.SourceReg0 = RspOp->OP.B.rs;
		info->SourceReg1 = (DWORD)-1;
		break;
		
	case RSP_ADDI:
	case RSP_ADDIU:
	/*case RSP_SLTI:
	case RSP_SLTIU:*/
	case RSP_ANDI:
	case RSP_ORI:
	/*case RSP_XORI:*/
		info->DS.DestReg = RspOp->OP.I.rt;
		info->S0.SourceReg0 = RspOp->OP.I.rs;
		info->SourceReg1 = (DWORD)-1;
		info->flags = GPR_Instruction;
		break;

	case RSP_LUI:
		info->DS.DestReg = RspOp->OP.I.rt;
		info->S0.SourceReg0 = (DWORD)-1;
		info->SourceReg1 = (DWORD)-1;
		info->flags = GPR_Instruction;
		break;

	case RSP_CP0:
		switch (RspOp->OP.I.rs) {
		case RSP_COP0_MF:			
			info->DS.DestReg = RspOp->OP.R.rt;
			info->S0.SourceReg0 = (DWORD)-1;
			info->SourceReg1 = (DWORD)-1;
			info->flags = GPR_Instruction | Load_Operation;
			break;

		case RSP_COP0_MT:
			info->DS.StoredReg = RspOp->OP.I.rt;
			info->S0.SourceReg0 = (DWORD)-1;
			info->SourceReg1 = (DWORD)-1;
			info->flags = GPR_Instruction | Store_Operation;
			break;
		}
		break;

	case RSP_CP2:
		if ((RspOp->OP.I.rs & 0x10) != 0) {
			switch (RspOp->OP.V.funct) {
			/*case RSP_VECTOR_VNOOP:
				info->DestReg = -1;
				info->SourceReg0 = -1;
				info->SourceReg1 = -1;
				info->flags = VEC_Instruction;
				break;*/

			case RSP_VECTOR_VMULF:
			case RSP_VECTOR_VMUDL:
			case RSP_VECTOR_VMUDM:
			case RSP_VECTOR_VMUDN:
			case RSP_VECTOR_VMUDH:
				info->DS.DestReg = RspOp->OP.V.vd;
				info->S0.SourceReg0 = RspOp->OP.V.vs;
				info->SourceReg1 = RspOp->OP.V.vt;
				info->flags = VEC_Instruction | VEC_ResetAccum | Accum_Operation;
				break;
			case RSP_VECTOR_VMACF:
			/*case RSP_VECTOR_VMADL:*/
			case RSP_VECTOR_VMADM:
			case RSP_VECTOR_VMADN:
			case RSP_VECTOR_VMADH:
				info->DS.DestReg = RspOp->OP.V.vd;
				info->S0.SourceReg0 = RspOp->OP.V.vs;
				info->SourceReg1 = RspOp->OP.V.vt;
				info->flags = VEC_Instruction | VEC_Accumulate | Accum_Operation;
				break;
			/*case RSP_VECTOR_VABS:*/
			case RSP_VECTOR_VADD:
			/*case RSP_VECTOR_VADDC:*/
			case RSP_VECTOR_VSUB:
			case RSP_VECTOR_VSUBC:
			case RSP_VECTOR_VAND:
			/*case RSP_VECTOR_VOR:*/
			case RSP_VECTOR_VXOR:
			/*case RSP_VECTOR_VNXOR:
			case RSP_VECTOR_VCR:
			case RSP_VECTOR_VCH:*/
			case RSP_VECTOR_VCL:
			/*case RSP_VECTOR_VRCP:
			case RSP_VECTOR_VRCPL:
			case RSP_VECTOR_VRCPH:
			case RSP_VECTOR_VRSQL:
			case RSP_VECTOR_VRSQH:
			case RSP_VECTOR_VLT:
			case RSP_VECTOR_VEQ:*/
			case RSP_VECTOR_VGE:
				info->DS.DestReg = RspOp->OP.V.vd;
				info->S0.SourceReg0 = RspOp->OP.V.vs;
				info->SourceReg1 = RspOp->OP.V.vt;
				info->flags = VEC_Instruction | VEC_ResetAccum | Accum_Operation;
				break;

			/*case RSP_VECTOR_VMOV:
				info->flags = InvalidOpcode;*/
			/*	info->DestReg = RspOp->sa;
				info->SourceReg0 = RspOp->rt;
				info->SourceReg1 = -1;
				info->flags = VEC_Instruction; /* Assume reset? */
/*				break;

			case RSP_VECTOR_VMRG:
				info->flags = InvalidOpcode;*/
			/*	info->DestReg = RspOp->sa;
				info->SourceReg0 = RspOp->rt;
				info->SourceReg1 = RspOp->rd;
				info->flags = VEC_Instruction; /* Assum reset? */
/*				break;*/

			case RSP_VECTOR_VSAR:
				info->DS.DestReg = RspOp->OP.V.vd;
				info->S0.SourceReg0 = (DWORD)-1;
				info->SourceReg1 = (DWORD)-1;
				info->flags = VEC_Instruction | Accum_Operation | VEC_Accumulate;
				break;

			default:
				RspCompilerWarning("Unkown opcode in GetInstructionInfo\n%s",RSPOpcodeName(RspOp->OP.Hex,PC));
				info->flags = InvalidOpcode;
				break;
			}
		} else {
			switch (RspOp->OP.I.rs) {				
			/*case RSP_COP2_CT:
				info->StoredReg = RspOp->rt;
				info->SourceReg0 = -1;
				info->SourceReg1 = -1;
				info->flags = GPR_Instruction | Store_Operation;
				break;
			case RSP_COP2_CF:
				info->DestReg = RspOp->rt;
				info->SourceReg0 = -1;
				info->SourceReg1 = -1;
				info->flags = GPR_Instruction | Load_Operation;
				break;*/

			/* RD is always the vector register, RT is always GPR */
			case RSP_COP2_MT:
				info->DS.DestReg = RspOp->OP.MV.vs;
				info->S0.SourceReg0 = RspOp->OP.MV.rt;
				info->SourceReg1 = (DWORD)-1;
				info->flags = VEC_Instruction | GPR_Instruction | Load_Operation;
				break;
			case RSP_COP2_MF:
				info->DS.DestReg = RspOp->OP.R.rt;
				info->S0.SourceReg0 = RspOp->OP.R.rd;
				info->SourceReg1 = (DWORD)-1;
				info->flags = VEC_Instruction | GPR_Instruction | Store_Operation;
				break;
			default:
				RspCompilerWarning("Unkown opcode in GetInstructionInfo\n%s",RSPOpcodeName(RspOp->OP.Hex,PC));
				info->flags = InvalidOpcode;
				break;
			}
		}
		break;
	case RSP_LB:
	case RSP_LH:
	case RSP_LW:
	case RSP_LBU:
	case RSP_LHU:
		info->DS.DestReg = RspOp->OP.LS.rt;
		info->S0.IndexReg = RspOp->OP.LS.base;
		info->SourceReg1 = (DWORD)-1;
		info->flags = Load_Operation | GPR_Instruction;
		break;
	/*case RSP_SB:*/
	case RSP_SH:
	case RSP_SW:
		info->DS.StoredReg = RspOp->OP.LS.rt;
		info->S0.IndexReg = RspOp->OP.LS.base;
		info->SourceReg1 = (DWORD)-1;
		info->flags = Store_Operation | GPR_Instruction;		
		break;
	case RSP_LC2:
		switch (RspOp->OP.R.rd) {
		case RSP_LSC2_SV:
		case RSP_LSC2_DV:
		case RSP_LSC2_RV:
		case RSP_LSC2_QV:
		/*case RSP_LSC2_LV:
		case RSP_LSC2_UV:
		case RSP_LSC2_PV:*/
			info->DS.DestReg = RspOp->OP.LSV.vt;
			info->S0.IndexReg = RspOp->OP.LSV.base;
			info->SourceReg1 = (DWORD)-1;
			info->flags = Load_Operation | VEC_Instruction;
			break;

		/*case RSP_LSC2_TV:
			info->flags = InvalidOpcode;
			break;*/
		default:
			RspCompilerWarning("Unkown opcode in GetInstructionInfo\n%s",RSPOpcodeName(RspOp->OP.Hex,PC));
			info->flags = InvalidOpcode;
			break;
		}
		break;
	case RSP_SC2:
		switch (RspOp->OP.R.rd) {
		/*case RSP_LSC2_BV:*/
		case RSP_LSC2_SV:
		case RSP_LSC2_LV:
		case RSP_LSC2_DV:
		case RSP_LSC2_QV:
		/*case RSP_LSC2_RV:
		case RSP_LSC2_PV:
		case RSP_LSC2_UV:
		case RSP_LSC2_HV:
		case RSP_LSC2_FV:
		case RSP_LSC2_WV:*/
			info->DS.DestReg = RspOp->OP.LSV.vt;
			info->S0.IndexReg = RspOp->OP.LSV.base;
			info->SourceReg1 = (DWORD)-1;
			info->flags = Store_Operation | VEC_Instruction;
			break;
		/*case RSP_LSC2_TV:
			info->flags = InvalidOpcode;
			break;*/
		default:
			RspCompilerWarning("Unkown opcode in GetInstructionInfo\n%s",RSPOpcodeName(RspOp->OP.Hex,PC));
			info->flags = InvalidOpcode;
			break;
		}
		break;
	default:
		RspCompilerWarning("Unkown opcode in GetInstructionInfo\n%s",RSPOpcodeName(RspOp->OP.Hex,PC));
		info->flags = InvalidOpcode;
		break;
	}
}

/************************************************************
** RspDelaySlotAffectBranch
**
** Output:
**	TRUE: Delay slot does affect the branch
**	FALSE: Registers do not affect each other
**
** Input: PC
*************************************************************/

BOOL RspDelaySlotAffectBranch(DWORD PC) {
	OPCODE Branch, Delay;
	OPCODE_INFO infoBranch, infoDelay;

	if (IsRspOpcodeNop((PC + 4) & 0xFFC) == TRUE) {
		return FALSE;
	}

	RSP_LW_IMEM(PC, &Branch.OP.Hex);
	RSP_LW_IMEM((PC+4)&0xFFC, &Delay.OP.Hex);

	memset(&infoDelay,0,sizeof(infoDelay));
	memset(&infoBranch,0,sizeof(infoBranch));
	
	GetInstructionInfo(PC, &Branch, &infoBranch);
	GetInstructionInfo((PC+4)&0xFFC, &Delay, &infoDelay);

	if ((infoDelay.flags & Instruction_Mask) == VEC_Instruction) {
		return FALSE;
	}

	if (infoBranch.S0.SourceReg0 == infoDelay.DS.DestReg) { return TRUE; }
	if (infoBranch.SourceReg1 == infoDelay.DS.DestReg) { return TRUE; }
	
	return FALSE;
}

/************************************************************
** RspCompareInstructions
**
** Output:
**	TRUE: The opcodes are fine, no dependency
**	FALSE: Watch it, these ops cant be touched
**
** Input:
**	Top, not the current operation, the one above
**	Bottom: the current opcode for re-ordering bubble style
*************************************************************/

BOOL RspCompareInstructions(DWORD PC, OPCODE * Top, OPCODE * Bottom) {

	OPCODE_INFO info0, info1;
	DWORD InstructionType;

	GetInstructionInfo(PC - 4, Top, &info0);
	GetInstructionInfo(PC, Bottom, &info1);

	/* usually branches and such */
	if ((info0.flags & InvalidOpcode) != 0) /*return FALSE;*/LogMessage("TODO RspCompareInstructions: invalid top");
	if ((info1.flags & InvalidOpcode) != 0) /*return FALSE;*/LogMessage("TODO RspCompareInstructions: invalid bottom");

	InstructionType = (info0.flags & Instruction_Mask) << 2;
	InstructionType |= info1.flags & Instruction_Mask;

	/* 4 bit range, 16 possible combinations */
	switch (InstructionType) {
	/*
	** Detect noop instruction, 7 cases, (see flags) */
/*	case 0x01: case 0x02: case 0x03:*/ /* First is a noop */
/*		return TRUE;
	case 0x00:*/						 /* Both ??? */
/*	case 0x10: case 0x20: case 0x30:*/ /* Second is a noop */
/*		return FALSE;*/

	case 0x06: /* GPR than Vector - 01,10 */
		if ((info0.flags & MemOperation_Mask) != 0 && (info1.flags & MemOperation_Mask) != 0) {
			/* TODO: We have a vector & GPR memory operation */
			return FALSE;
		} else if ((info1.flags & MemOperation_Mask) != 0) {
			/* We have a vector memory operation */
			return (info1.S0.IndexReg == info0.DS.DestReg) ? FALSE : TRUE;
		} else {
			/* We could have memory or normal gpr instruction here
			** paired with some kind of vector operation
			*/
			return TRUE;
		}
		break;

	case 0x0A: /* Vector than Vector - 10,10 */

		/*
		** Check for Vector Store than Vector multiply (VMULF)
		**
		** This basically gives preferences to putting stores
		** as close to the finish of an operation as possible
		*/
		if ((info0.flags & Store_Operation) != 0 && (info1.flags & Accum_Operation) != 0
			&& !(info1.flags & VEC_Accumulate)) { return FALSE; }

		/*
		** Look for loads and than some kind of vector operation
		** that does no accumulating, there is no reason to reorder
		*/
		if ((info0.flags & Load_Operation) != 0 && (info1.flags & Accum_Operation) != 0
			&& !(info1.flags & VEC_Accumulate)) { return FALSE; }

		if ((info0.flags & MemOperation_Mask) != 0 && (info1.flags & MemOperation_Mask) != 0) {
			/*
			** TODO: This is a bitch, its best to leave it alone
			**/
			return FALSE;
		} else if ((info1.flags & MemOperation_Mask) != 0) {
			/* Remember stored reg & loaded reg are the same */
			if (info0.DS.DestReg == info1.DS.DestReg) { return FALSE; }

			if (info1.flags & Load_Operation) {
				if (info0.S0.SourceReg0 == info1.DS.DestReg) { return FALSE; }
				if (info0.SourceReg1 == info1.DS.DestReg) { /*return FALSE;*/LogMessage("TODO:RspCompareInstructions vec and vec, 2nd is mem, load, src1 is dest of load"); }
			} else if (info1.flags & Store_Operation) {
				/* It can store source regs */
				return TRUE;
			}

			return TRUE;
		} else if ((info0.flags & MemOperation_Mask) != 0) {
			/* Remember stored reg & loaded reg are the same */
			if (info0.DS.DestReg == info1.DS.DestReg) { /*return FALSE;*/LogMessage("TODO:RspCompareInstructions vec and vec, 1st is mem"); }

			if (info0.flags & Load_Operation) {
				if (info1.S0.SourceReg0 == info0.DS.DestReg) { /*return FALSE;*/LogMessage("TODO:RspCompareInstructions vec and vec, 1st is mem, load, src0 is dest of load"); }
				if (info1.SourceReg1 == info0.DS.DestReg) { /*return FALSE;*/LogMessage("TODO:RspCompareInstructions vec and vec, 1st is mem, load, src1 is dest of load"); }
			} else if (info0.flags & Store_Operation) {
				/* It can store source regs */
				return TRUE;
			}

			return TRUE;
		} else if ((info0.flags & VEC_Accumulate) != 0) {
			/*
			** Example:
			**		VMACF
			**		VMUDH or VMADH or VADD
			*/

			return FALSE;
		} else if ((info1.flags & VEC_Accumulate) != 0) {
			/*
			** Example:
			**		VMULF
			**		VMADH
			*/
			
			return FALSE;
		} else {
			/*
			** Example:
			**		VMULF or VADDC
			**		VADD or VMUDH
			*/

			return FALSE;
		}
		break;

	case 0x09: /* Vector than GPR - 10,01 */
		/**********
		** this is where the bias comes into play, otherwise
		** we can sit here all day swapping these 2 types
		***********/
		return FALSE;

	case 0x05: /* GPR than GPR - 01,01 */
	case 0x07: /* GPR than Cop2 - 01, 11 */
	case 0x0D: /* Cop2 than GPR - 11, 01 */
	case 0x0F: /* Cop2 than Cop2 - 11, 11 */
		return FALSE;

	case 0x0B: /* Vector than Cop2 - 10, 11 */
		if (info1.flags & Load_Operation) {
			/* Move To Cop2 (dest) from GPR (source) */
			if (info1.DS.DestReg == info0.DS.DestReg) { /*return FALSE;*/ LogMessage("TODO:RspCompareInstructions vec and cop2, load, same destination"); }
			if (info1.DS.DestReg == info0.S0.SourceReg0) { /*return FALSE;*/ LogMessage("TODO:RspCompareInstructions vec and cop2, load, dest is source0 of vec"); }
			if (info1.DS.DestReg == info0.SourceReg1) { /*return FALSE;*/ LogMessage("TODO:RspCompareInstructions vec and cop2, load, dest is source1 of vec"); }
		} else if (info1.flags & Store_Operation) {
			/* Move From Cop2 (source) to GPR (dest) */
/*			if (info1.SourceReg0 == info0.DestReg) { return FALSE; }
			if (info1.SourceReg0 == info0.SourceReg0) { return FALSE; }
			if (info1.SourceReg0 == info0.SourceReg1) { return FALSE; }*/
			LogMessage("TODO:RspCompareInstructions vec and cop2, store");
		} else {
			RspCompilerWarning("ReOrder: Unhandled Vector than Cop2");
		}
		// we want vectors on top
		return FALSE;

	case 0x0E: /* Cop2 than Vector - 11, 10 */
		if (info0.flags & Load_Operation) {
			/* Move To Cop2 (dest) from GPR (source) */
			if (info0.DS.DestReg == info1.DS.DestReg) { /*return FALSE;*/ LogMessage("TODO:RspCompareInstructions cop2 and vec, load, same destination"); }
			if (info0.DS.DestReg == info1.S0.SourceReg0) { /*return FALSE;*/ LogMessage("TODO:RspCompareInstructions cop2 and vec, load, dest is source0 of vec"); }
			if (info0.DS.DestReg == info1.SourceReg1) { /*return FALSE;*/ LogMessage("TODO:RspCompareInstructions cop2 and vec, load, dest is source0 of vec"); }
		} else if (info0.flags & Store_Operation) {
			/* Move From Cop2 (source) to GPR (dest) */
/*			if (info0.SourceReg0 == info1.DestReg) { return FALSE; }
			if (info0.SourceReg0 == info1.SourceReg0) { return FALSE; }
			if (info0.SourceReg0 == info1.SourceReg1) { return FALSE; }*/
			LogMessage("TODO:RspCompareInstructions cop2 and vec, store");
		} else {
			RspCompilerWarning("ReOrder: Unhandled Cop2 than Vector");
		}
		// we want this at the top
		return TRUE;

	default:
		RspCompilerWarning("ReOrder: Unhandled instruction type: %x", InstructionType);
	}
	return FALSE;
}
