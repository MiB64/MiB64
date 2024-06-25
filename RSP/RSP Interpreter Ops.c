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
#include <math.h>*/
#include "rsp_Cpu.h"
#include "RSP Command.h"
#include "rsp_registers.h"
#include "rsp_config.h"
#include "RSP Interpreter CPU.h"
#include "rsp_memory.h"
#include "../Dma.h"
/*#include "log.h"
#include "x86.h"*/
#include "../Main.h"
#include "../rdp_registers.h"
#include "../mi_registers.h"
#include "../Exception.h"

/************************* OpCode functions *************************/
void RSP_Opcode_SPECIAL ( void ) {
	((void (*)()) RSP_Special[ RSPOpC.OP.R.funct ])();
}

void RSP_Opcode_REGIMM ( void ) {
	((void (*)()) RSP_RegImm[ RSPOpC.OP.B.rt ])();
}

void RSP_Opcode_J ( void ) {
	RSP_NextInstruction = DELAY_SLOT;
	RSP_JumpTo = (RSPOpC.OP.J.target << 2) & 0xFFC;
}

void RSP_Opcode_JAL ( void ) {
	RSP_NextInstruction = DELAY_SLOT;
	RSP_GPR[31].UW = ( SP_PC_REG + 8 ) & 0xFFC;
	RSP_JumpTo = (RSPOpC.OP.J.target << 2) & 0xFFC;
}

void RSP_Opcode_BEQ ( void ) {
	RSP_NextInstruction = DELAY_SLOT;
	if (RSP_GPR[RSPOpC.OP.B.rs].W == RSP_GPR[RSPOpC.OP.B.rt].W) {
		RSP_JumpTo = ( SP_PC_REG + ((short)RSPOpC.OP.B.offset << 2) + 4 ) & 0xFFC;
	} else  {
		RSP_JumpTo = ( SP_PC_REG + 8 ) & 0xFFC;
	}
}

void RSP_Opcode_BNE ( void ) {
	RSP_NextInstruction = DELAY_SLOT;
	if (RSP_GPR[RSPOpC.OP.B.rs].W != RSP_GPR[RSPOpC.OP.B.rt].W) {
		RSP_JumpTo = ( SP_PC_REG + ((short)RSPOpC.OP.B.offset << 2) + 4 ) & 0xFFC;
	} else  {
		RSP_JumpTo = ( SP_PC_REG + 8 ) & 0xFFC;
	}
}

void RSP_Opcode_BLEZ ( void ) {
	RSP_NextInstruction = DELAY_SLOT;
	if (RSP_GPR[RSPOpC.OP.B.rs].W <= 0) {
		RSP_JumpTo = ( SP_PC_REG + ((short)RSPOpC.OP.B.offset << 2) + 4 ) & 0xFFC;
	} else  {
		RSP_JumpTo = ( SP_PC_REG + 8 ) & 0xFFC;
	}
}

void RSP_Opcode_BGTZ ( void ) {
	RSP_NextInstruction = DELAY_SLOT;
	if (RSP_GPR[RSPOpC.OP.B.rs].W > 0) {
		RSP_JumpTo = ( SP_PC_REG + ((short)RSPOpC.OP.B.offset << 2) + 4 ) & 0xFFC;
	} else  {
		RSP_JumpTo = ( SP_PC_REG + 8 ) & 0xFFC;
	}
}

void RSP_Opcode_ADDI ( void ) {
	if (RSPOpC.OP.I.rt != 0) {
		RSP_GPR[RSPOpC.OP.I.rt].W = RSP_GPR[RSPOpC.OP.I.rs].W + (short)RSPOpC.OP.I.immediate;
	}
}

void RSP_Opcode_ADDIU ( void ) {
	if (RSPOpC.OP.I.rt != 0) {
		RSP_GPR[RSPOpC.OP.I.rt].UW = RSP_GPR[RSPOpC.OP.I.rs].UW + (DWORD)((short)RSPOpC.OP.I.immediate);
	}
}

void RSP_Opcode_SLTI (void) {
	if (RSPOpC.OP.I.rt == 0) { return; }
	if (RSP_GPR[RSPOpC.OP.I.rs].W < (short)RSPOpC.OP.I.immediate) {
		RSP_GPR[RSPOpC.OP.I.rt].W = 1;
	} else {
		RSP_GPR[RSPOpC.OP.I.rt].W = 0;
	}
}

void RSP_Opcode_SLTIU (void) {
	if (RSPOpC.OP.I.rt == 0) { return; }
	if (RSP_GPR[RSPOpC.OP.I.rs].UW < (DWORD)(short)RSPOpC.OP.I.immediate) {
		RSP_GPR[RSPOpC.OP.I.rt].W = 1;
	} else {
		RSP_GPR[RSPOpC.OP.I.rt].W = 0;
	}
}

void RSP_Opcode_ANDI ( void ) {
	if (RSPOpC.OP.I.rt != 0) {
		RSP_GPR[RSPOpC.OP.I.rt].W = RSP_GPR[RSPOpC.OP.I.rs].W & RSPOpC.OP.I.immediate;
	}
}

void RSP_Opcode_ORI ( void ) {
	if (RSPOpC.OP.I.rt != 0) {
		RSP_GPR[RSPOpC.OP.I.rt].W = RSP_GPR[RSPOpC.OP.I.rs].W | RSPOpC.OP.I.immediate;
	}
}

void RSP_Opcode_XORI ( void ) {
	if (RSPOpC.OP.I.rt != 0) {
		RSP_GPR[RSPOpC.OP.I.rt].W = RSP_GPR[RSPOpC.OP.I.rs].W ^ RSPOpC.OP.I.immediate;
	}
}

void RSP_Opcode_LUI (void) {
	if (RSPOpC.OP.I.rt != 0) {
		RSP_GPR[RSPOpC.OP.I.rt].W = (short)RSPOpC.OP.I.immediate << 16;
	}
}

void RSP_Opcode_COP0 (void) {
	((void (*)()) RSP_Cop0[ RSPOpC.OP.I.rs ])();
}

void RSP_Opcode_COP2 (void) {
	((void (*)()) RSP_Cop2[ RSPOpC.OP.I.rs ])();
}

void RSP_Opcode_LB ( void ) {
	if (RSPOpC.OP.LS.rt != 0) {
		DWORD Address = ((RSP_GPR[RSPOpC.OP.LS.base].UW + (short)RSPOpC.OP.LS.offset) & 0xFFF);
		RSP_LB_DMEM(Address, &RSP_GPR[RSPOpC.OP.LS.rt].UB[0]);
		RSP_GPR[RSPOpC.OP.LS.rt].W = RSP_GPR[RSPOpC.OP.LS.rt].B[0];
	}
}

void RSP_Opcode_LH ( void ) {
	if (RSPOpC.OP.LS.rt != 0) {
		DWORD Address = ((RSP_GPR[RSPOpC.OP.LS.base].UW + (short)RSPOpC.OP.LS.offset) & 0xFFF);
		RSP_LH_DMEM(Address, &RSP_GPR[RSPOpC.OP.LS.rt].UHW[0]);
		RSP_GPR[RSPOpC.OP.LS.rt].W = RSP_GPR[RSPOpC.OP.LS.rt].HW[0];
	}
}

void RSP_Opcode_LW ( void ) {
	if (RSPOpC.OP.LS.rt != 0) {
		DWORD Address = ((RSP_GPR[RSPOpC.OP.LS.base].UW + (short)RSPOpC.OP.LS.offset) & 0xFFF);
		RSP_LW_DMEM(Address, &RSP_GPR[RSPOpC.OP.LS.rt].UW);
	}
}

void RSP_Opcode_LBU ( void ) {
	if (RSPOpC.OP.LS.rt != 0) {
		DWORD Address = ((RSP_GPR[RSPOpC.OP.LS.base].UW + (short)RSPOpC.OP.LS.offset) & 0xFFF);
		RSP_LB_DMEM(Address, &RSP_GPR[RSPOpC.OP.LS.rt].UB[0]);
		RSP_GPR[RSPOpC.OP.LS.rt].UW = RSP_GPR[RSPOpC.OP.LS.rt].UB[0];
	}
}

void RSP_Opcode_LHU ( void ) {
	if (RSPOpC.OP.LS.rt != 0) {
		DWORD Address = ((RSP_GPR[RSPOpC.OP.LS.base].UW + (short)RSPOpC.OP.LS.offset) & 0xFFF);
		RSP_LH_DMEM(Address, &RSP_GPR[RSPOpC.OP.LS.rt].UHW[0]);
		RSP_GPR[RSPOpC.OP.LS.rt].UW = RSP_GPR[RSPOpC.OP.LS.rt].UHW[0];
	}
}

void RSP_Opcode_LWU ( void ) {
	if (RSPOpC.OP.LS.rt != 0) {
		DWORD Address = ((RSP_GPR[RSPOpC.OP.LS.base].UW + (short)RSPOpC.OP.LS.offset) & 0xFFF);
		RSP_LW_DMEM(Address, &RSP_GPR[RSPOpC.OP.LS.rt].UW);
	}
}

void RSP_Opcode_SB ( void ) {
	DWORD Address = ((RSP_GPR[RSPOpC.OP.LS.base].UW + (short)RSPOpC.OP.LS.offset) & 0xFFF);
	RSP_SB_DMEM( Address, RSP_GPR[RSPOpC.OP.LS.rt].UB[0] );
}

void RSP_Opcode_SH ( void ) {
	DWORD Address = ((RSP_GPR[RSPOpC.OP.LS.base].UW + (short)RSPOpC.OP.LS.offset) & 0xFFF);
	RSP_SH_DMEM( Address, RSP_GPR[RSPOpC.OP.LS.rt].UHW[0] );
}

void RSP_Opcode_SW ( void ) {
	DWORD Address = ((RSP_GPR[RSPOpC.OP.LS.base].UW + (short)RSPOpC.OP.LS.offset) & 0xFFF);
	RSP_SW_DMEM( Address, RSP_GPR[RSPOpC.OP.LS.rt].UW );
}

void RSP_Opcode_LC2 (void) {
	((void (*)()) RSP_Lc2 [ RSPOpC.OP.R.rd ])();
}

void RSP_Opcode_SC2 (void) {
	((void (*)()) RSP_Sc2 [ RSPOpC.OP.R.rd ])();
}
/********************** R4300i OpCodes: Special **********************/
void RSP_Special_SLL ( void ) {
	if (RSPOpC.OP.R.rd != 0) {
		RSP_GPR[RSPOpC.OP.R.rd].W = RSP_GPR[RSPOpC.OP.R.rt].W << RSPOpC.OP.R.sa;
	}
}

void RSP_Special_SRL ( void ) {
	if (RSPOpC.OP.R.rd != 0) {
		RSP_GPR[RSPOpC.OP.R.rd].UW = RSP_GPR[RSPOpC.OP.R.rt].UW >> RSPOpC.OP.R.sa;
	}
}

void RSP_Special_SRA ( void ) {
	if (RSPOpC.OP.R.rd != 0) {
		RSP_GPR[RSPOpC.OP.R.rd].W = RSP_GPR[RSPOpC.OP.R.rt].W >> RSPOpC.OP.R.sa;
	}
}

void RSP_Special_SLLV (void) {
	if (RSPOpC.OP.R.rd != 0) {
		RSP_GPR[RSPOpC.OP.R.rd].W = RSP_GPR[RSPOpC.OP.R.rt].W << (RSP_GPR[RSPOpC.OP.R.rs].W & 0x1F);
	}
}

void RSP_Special_SRLV (void) {
	if (RSPOpC.OP.R.rd != 0) {
		RSP_GPR[RSPOpC.OP.R.rd].UW = RSP_GPR[RSPOpC.OP.R.rt].UW >> (RSP_GPR[RSPOpC.OP.R.rs].W & 0x1F);
	}
}

void RSP_Special_SRAV (void) {
	if (RSPOpC.OP.R.rd != 0) {
		RSP_GPR[RSPOpC.OP.R.rd].W = RSP_GPR[RSPOpC.OP.R.rt].W >> (RSP_GPR[RSPOpC.OP.R.rs].W & 0x1F);
	}
}

void RSP_Special_JR (void) {
	RSP_NextInstruction = DELAY_SLOT;
	RSP_JumpTo = (RSP_GPR[RSPOpC.OP.R.rs].W & 0xFFC);
}

void RSP_Special_JALR (void) {
	RSP_NextInstruction = DELAY_SLOT;
	RSP_JumpTo = (RSP_GPR[RSPOpC.OP.R.rs].W & 0xFFC);
	RSP_GPR[RSPOpC.OP.R.rd].W = (SP_PC_REG + 8) & 0xFFC;
}

void RSP_Special_BREAK ( void ) {
	RSP_Running = FALSE;
	SP_STATUS_REG |= (SP_STATUS_HALT | SP_STATUS_BROKE );
	if ((SP_STATUS_REG & SP_STATUS_INTR_BREAK) != 0 ) {
		MI_INTR_REG |= MI_INTR_SP;
		CheckInterrupts();
	}
}

void RSP_Special_ADD (void) {
	if (RSPOpC.OP.R.rd != 0) {
		RSP_GPR[RSPOpC.OP.R.rd].W = RSP_GPR[RSPOpC.OP.R.rs].W + RSP_GPR[RSPOpC.OP.R.rt].W;
	}
}

void RSP_Special_ADDU (void) {
	if (RSPOpC.OP.R.rd != 0) {
		RSP_GPR[RSPOpC.OP.R.rd].UW = RSP_GPR[RSPOpC.OP.R.rs].UW + RSP_GPR[RSPOpC.OP.R.rt].UW;
	}
}

void RSP_Special_SUB (void) {
	if (RSPOpC.OP.R.rd != 0) {
		RSP_GPR[RSPOpC.OP.R.rd].W = RSP_GPR[RSPOpC.OP.R.rs].W - RSP_GPR[RSPOpC.OP.R.rt].W;
	}
}

void RSP_Special_SUBU (void) {
	if (RSPOpC.OP.R.rd != 0) {
		RSP_GPR[RSPOpC.OP.R.rd].UW = RSP_GPR[RSPOpC.OP.R.rs].UW - RSP_GPR[RSPOpC.OP.R.rt].UW;
	}
}

void RSP_Special_AND (void) {
	if (RSPOpC.OP.R.rd != 0) {
		RSP_GPR[RSPOpC.OP.R.rd].UW = RSP_GPR[RSPOpC.OP.R.rs].UW & RSP_GPR[RSPOpC.OP.R.rt].UW;
	}
}

void RSP_Special_OR (void) {
	if (RSPOpC.OP.R.rd != 0) {
		RSP_GPR[RSPOpC.OP.R.rd].UW = RSP_GPR[RSPOpC.OP.R.rs].UW | RSP_GPR[RSPOpC.OP.R.rt].UW;
	}
}

void RSP_Special_XOR (void) {
	if (RSPOpC.OP.R.rd != 0) {
		RSP_GPR[RSPOpC.OP.R.rd].UW = RSP_GPR[RSPOpC.OP.R.rs].UW ^ RSP_GPR[RSPOpC.OP.R.rt].UW;
	}
}

void RSP_Special_NOR (void) {
	if (RSPOpC.OP.R.rd != 0) {
		RSP_GPR[RSPOpC.OP.R.rd].UW = ~(RSP_GPR[RSPOpC.OP.R.rs].UW | RSP_GPR[RSPOpC.OP.R.rt].UW);
	}
}

void RSP_Special_SLT (void) {
	if (RSPOpC.OP.R.rd == 0) { return; }
	if (RSP_GPR[RSPOpC.OP.R.rs].W < RSP_GPR[RSPOpC.OP.R.rt].W) {
		RSP_GPR[RSPOpC.OP.R.rd].UW = 1;
	} else {
		RSP_GPR[RSPOpC.OP.R.rd].UW = 0;
	}
}

void RSP_Special_SLTU (void) {
	if (RSPOpC.OP.R.rd == 0) { return; }
	if (RSP_GPR[RSPOpC.OP.R.rs].UW < RSP_GPR[RSPOpC.OP.R.rt].UW) {
		RSP_GPR[RSPOpC.OP.R.rd].UW = 1;
	} else {
		RSP_GPR[RSPOpC.OP.R.rd].UW = 0;
	}
}
/********************** R4300i OpCodes: RegImm **********************/
void RSP_Opcode_BLTZ ( void ) {
	RSP_NextInstruction = DELAY_SLOT;
	if (RSP_GPR[RSPOpC.OP.B.rs].W < 0) {
		RSP_JumpTo = ( SP_PC_REG + ((short)RSPOpC.OP.B.offset << 2) + 4 ) & 0xFFC;
	} else  {
		RSP_JumpTo = ( SP_PC_REG + 8 ) & 0xFFC;
	}
}

void RSP_Opcode_BGEZ ( void ) {
	RSP_NextInstruction = DELAY_SLOT;
	if (RSP_GPR[RSPOpC.OP.B.rs].W >= 0) {
		RSP_JumpTo = ( SP_PC_REG + ((short)RSPOpC.OP.B.offset << 2) + 4 ) & 0xFFC;
	} else  {
		RSP_JumpTo = ( SP_PC_REG + 8 ) & 0xFFC;
	}
}

void RSP_Opcode_BLTZAL ( void ) {
	RSP_NextInstruction = DELAY_SLOT;
	if (RSP_GPR[RSPOpC.OP.B.rs].W < 0) {
		RSP_JumpTo = ( SP_PC_REG + ((short)RSPOpC.OP.B.offset << 2) + 4 ) & 0xFFC;
	} else  {
		RSP_JumpTo = ( SP_PC_REG + 8 ) & 0xFFC;
	}
	RSP_GPR[31].UW = (SP_PC_REG + 8) & 0xFFC;
}

void RSP_Opcode_BGEZAL ( void ) {
	RSP_NextInstruction = DELAY_SLOT;
	if (RSP_GPR[RSPOpC.OP.B.rs].W >= 0) {
		RSP_JumpTo = ( SP_PC_REG + ((short)RSPOpC.OP.B.offset << 2) + 4 ) & 0xFFC;
	} else  {
		RSP_JumpTo = ( SP_PC_REG + 8 ) & 0xFFC;
	}
	RSP_GPR[31].UW = (SP_PC_REG + 8) & 0xFFC;
}
/************************** Cop0 functions *************************/
void RSP_Cop0_MF (void) {
	switch (RSPOpC.OP.R.rd) {
	case 0: RSP_GPR[RSPOpC.OP.R.rt].UW = SP_MEM_ADDR_REG; break;
	case 1: RSP_GPR[RSPOpC.OP.R.rt].UW = SP_DRAM_ADDR_REG; break;
	case 2: RSP_GPR[RSPOpC.OP.R.rt].UW = SP_RD_LEN_REG; break;
	case 3: RSP_GPR[RSPOpC.OP.R.rt].UW = SP_WR_LEN_REG; break;
	case 4: RSP_GPR[RSPOpC.OP.R.rt].UW = SP_STATUS_REG; break;
	case 5: RSP_GPR[RSPOpC.OP.R.rt].UW = SP_DMA_FULL_REG; break;
	case 6: RSP_GPR[RSPOpC.OP.R.rt].UW = SP_DMA_BUSY_REG; break;
	case 7: 
		RSP_GPR[RSPOpC.OP.R.rt].W = SP_SEMAPHORE_REG;
		SP_SEMAPHORE_REG = 1;
		break;
	/*case 8: RSP_GPR[RSPOpC.rt].UW = *RSPInfo.DPC_START_REG ; break;*/
	case 9: RSP_GPR[RSPOpC.OP.R.rt].UW = DPC_END_REG; break;
	case 10: RSP_GPR[RSPOpC.OP.R.rt].UW = DPC_CURRENT_REG; break;
	case 11: RSP_GPR[RSPOpC.OP.R.rt].W = DPC_STATUS_REG; break;
	case 12: RSP_GPR[RSPOpC.OP.R.rt].W = DPC_CLOCK_REG; break;
	default:
		DisplayError("have not implemented RSP MF CP0 reg %s (%d)",RspCOP0_Name(RSPOpC.OP.R.rd),RSPOpC.OP.R.rd);
		LogMessage("have not implemented RSP MF CP0 reg %s (%d)", RspCOP0_Name(RSPOpC.OP.R.rd), RSPOpC.OP.R.rd);
	}
}

void RSP_Cop0_MT (void) {
	switch (RSPOpC.OP.R.rd) {
	case 0: SP_MEM_ADDR_REGW  = RSP_GPR[RSPOpC.OP.R.rt].UW & 0x1FF8; break;
	case 1: SP_DRAM_ADDR_REGW = RSP_GPR[RSPOpC.OP.R.rt].UW & 0xFFFFF8; break;
	case 2:
		SP_RD_LEN_REG = RSP_GPR[RSPOpC.OP.R.rt].UW & 0xFF8FFFF8;
		SP_DMA_READ();		
		break;
	case 3:
		SP_WR_LEN_REG = RSP_GPR[RSPOpC.OP.R.rt].UW & 0xFF8FFFF8;
		SP_DMA_WRITE();
		break;
	case 4:
		WriteRspStatusRegister(RSP_GPR[RSPOpC.OP.R.rt].W);
		if (SP_STATUS_REG & SP_STATUS_HALT) {
			RSP_Running = FALSE;
		}
		break;
	case 7: SP_SEMAPHORE_REG = 0; break;
	case 8:
		if ((DPC_STATUS_REG & DPC_STATUS_START_VALID) == 0) {
			DPC_START_REG = RSP_GPR[RSPOpC.OP.R.rt].UW & 0xFFFFF8;
			DPC_STATUS_REG |= DPC_STATUS_START_VALID;
		}
		break;
	case 9:
		DPC_END_REG = RSP_GPR[RSPOpC.OP.R.rt].UW & 0xFFFFF8;
		if (DPC_STATUS_REG & DPC_STATUS_START_VALID) {
			DPC_CURRENT_REG = DPC_START_REG;
			DPC_STATUS_REG &= ~DPC_STATUS_START_VALID;
			DPC_STATUS_REG |= (DPC_STATUS_PIPE_BUSY | DPC_STATUS_START_GCLK);
		}
		if ((DPC_STATUS_REG & DPC_STATUS_FREEZE) == 0) {
			if ((DPC_STATUS_REG & DPC_STATUS_START_VALID) == 0 && DPC_END_REG > DPC_CURRENT_REG && ProcessRDPList) { ProcessRDPList(); }
		}
		break;
	/*case 10: *RSPInfo.DPC_CURRENT_REG = RSP_GPR[RSPOpC.rt].UW; break;*/
	case 11:
		WriteDPCStatusRegister(RSP_GPR[RSPOpC.OP.R.rt].W);
		break;
	default:
		DisplayError("have not implemented RSP MT CP0 reg %s (%d)",RspCOP0_Name(RSPOpC.OP.R.rd),RSPOpC.OP.R.rd);
		LogMessage("TODO: have not implemented RSP MT CP0 reg % s(% d)", RspCOP0_Name(RSPOpC.OP.R.rd), RSPOpC.OP.R.rd);
	}
}

/************************** Cop2 functions *************************/
void RSP_Cop2_MF (void) {
	if (RSPOpC.OP.R.rt != 0) {
		int element = RSPOpC.OP.LSV.element;
		RSP_GPR[RSPOpC.OP.R.rt].B[1] = RSP_Vect[RSPOpC.OP.R.rd].B[15 - element];
		RSP_GPR[RSPOpC.OP.R.rt].B[0] = RSP_Vect[RSPOpC.OP.R.rd].B[15 - ((element + 1) % 16)];
		RSP_GPR[RSPOpC.OP.R.rt].W = RSP_GPR[RSPOpC.OP.R.rt].HW[0];
	}
}

void RSP_Cop2_CF (void) {
	switch ((RSPOpC.OP.R.rd & 0x03)) {
	case 0: RSP_GPR[RSPOpC.OP.R.rt].W = RspVCO; break;
	case 1: RSP_GPR[RSPOpC.OP.R.rt].W = RspVCC; break;
	case 2: RSP_GPR[RSPOpC.OP.R.rt].W = (BYTE)RspVCE; break;
	case 3: RSP_GPR[RSPOpC.OP.R.rt].W = (BYTE)RspVCE; break;
	}
}

void RSP_Cop2_MT (void) {
	int element = 15 - RSPOpC.OP.MV.element;
	RSP_Vect[RSPOpC.OP.MV.vs].B[element] = RSP_GPR[RSPOpC.OP.MV.rt].B[1];
	if (element != 0) {
		RSP_Vect[RSPOpC.OP.MV.vs].B[element - 1] = RSP_GPR[RSPOpC.OP.MV.rt].B[0];
	}
}

void RSP_Cop2_CT (void) {
	switch ((RSPOpC.OP.R.rd & 0x03)) {
	case 0: RspVCO = RSP_GPR[RSPOpC.OP.R.rt].HW[0]; break;
	case 1: RspVCC = RSP_GPR[RSPOpC.OP.R.rt].HW[0]; break;
	case 2: RspVCE = RSP_GPR[RSPOpC.OP.R.rt].B[0]; break;
	case 3: RspVCE = RSP_GPR[RSPOpC.OP.R.rt].B[0]; break;
	}
}

void RSP_COP2_VECTOR (void) {
	((void (*)()) RSP_Vector[ RSPOpC.OP.V.funct ])();
}
/************************** Vect functions **************************/
void RSP_Vector_VMULF (void) {
	int count, el, del;
	MIPS_WORD temp;

	for (count = 0; count < 8; count++) {
		el = Indx[RSPOpC.OP.V.element].B[count];
		del = EleSpec[RSPOpC.OP.V.element].B[el];

		if (RSP_Vect[RSPOpC.OP.V.vs].UHW[el] != 0x8000 || RSP_Vect[RSPOpC.OP.V.vt].UHW[del] != 0x8000) {
			temp.W = ((long)RSP_Vect[RSPOpC.OP.V.vs].HW[el] * (long)RSP_Vect[RSPOpC.OP.V.vt].HW[del]) << 1;
			temp.UW += 0x8000;
			RSP_ACCUM[el].HW[2] = temp.HW[1];
			RSP_ACCUM[el].HW[1] = temp.HW[0];
			RSP_ACCUM[el].HW[3] = (RSP_ACCUM[el].HW[2] >> 15);
			RSP_Vect[RSPOpC.OP.V.vd].HW[el] = RSP_ACCUM[el].HW[2];
		}
		else {
			temp.W = 0x80000000;
			RSP_ACCUM[el].UHW[3] = 0;
			RSP_ACCUM[el].UHW[2] = 0x8000;
			RSP_ACCUM[el].UHW[1] = 0x8000;
			RSP_Vect[RSPOpC.OP.V.vd].HW[el] = 0x7FFF;
		}
	}
}

void RSP_Vector_VMULU (void) {
	int count, el, del;

	for (count = 0; count < 8; count ++ ) {
		el = Indx[RSPOpC.OP.V.element].B[count];
		del = EleSpec[RSPOpC.OP.V.element].B[el];
		RSP_ACCUM[el].DW = (__int64)(RSP_Vect[RSPOpC.OP.V.vs].HW[el] * RSP_Vect[RSPOpC.OP.V.vt].HW[del]) << 17;
		RSP_ACCUM[el].DW += 0x80000000;
		if (RSP_ACCUM[el].DW < 0) {
			RSP_Vect[RSPOpC.OP.V.vd].HW[el] = 0;
		} else if ((short)(RSP_ACCUM[el].UHW[3] ^ RSP_ACCUM[el].UHW[2]) < 0) {
			RSP_Vect[RSPOpC.OP.V.vd].HW[el] = -1;
		} else {
			RSP_Vect[RSPOpC.OP.V.vd].HW[el] = RSP_ACCUM[el].HW[2];
		}
	}
}

void RSP_Vector_VRNDP(void) {
	MIPS_WORD temp;

	for (int count = 0; count < 8; count++) {
		int el = Indx[RSPOpC.OP.V.element].B[count];
		int del = EleSpec[RSPOpC.OP.V.element].B[el];

		if ((RSPOpC.OP.V.vs & 1) != 0) {
			temp.HW[1] = RSP_Vect[RSPOpC.OP.V.vt].HW[del];
			temp.HW[0] = 0;
		}
		else {
			temp.HW[1] = RSP_Vect[RSPOpC.OP.V.vt].HW[del] >> 15;
			temp.HW[0] = RSP_Vect[RSPOpC.OP.V.vt].HW[del];
		}

		if (RSP_ACCUM[el].DW >= 0) {
			RSP_ACCUM[el].DW += ((long long)temp.W) << 16;
		}

		if (RSP_ACCUM[el].W[1] > 0x7FFF) {
			RSP_Vect[RSPOpC.OP.V.vd].HW[el] = 0x7FFF;
		}
		else if (RSP_ACCUM[el].W[1] < 0 && RSP_ACCUM[el].UHW[3] != 0xFFFF) {
			RSP_Vect[RSPOpC.OP.V.vd].HW[el] = 0x8000;
		}
		else {
			RSP_Vect[RSPOpC.OP.V.vd].HW[el] = RSP_ACCUM[el].HW[2];
		}
	}
}

void RSP_Vector_VMULQ(void) {
	int count, el, del;

	for (count = 0; count < 8; count++) {
		el = Indx[RSPOpC.OP.V.element].B[count];
		del = EleSpec[RSPOpC.OP.V.element].B[el];

		RSP_ACCUM[el].W[1] = ((long)RSP_Vect[RSPOpC.OP.V.vs].HW[el] * (long)RSP_Vect[RSPOpC.OP.V.vt].HW[del]);
		if (RSP_ACCUM[el].W[1] < 0) RSP_ACCUM[el].W[1] += 31;

		RSP_ACCUM[el].W[0] = 0;

		if (RSP_ACCUM[el].W[1] > 0xFFFF) {
			RSP_Vect[RSPOpC.OP.V.vd].HW[el] = 0x7FFF;
		}
		else if (RSP_ACCUM[el].W[1] < 0 && (RSP_ACCUM[el].HW[3] & 0xFFFE) != 0xFFFE) {
			RSP_Vect[RSPOpC.OP.V.vd].HW[el] = 0x8000;
		}
		else {
			RSP_Vect[RSPOpC.OP.V.vd].HW[el] = (short)(RSP_ACCUM[el].W[1] >> 1);
		}
		RSP_Vect[RSPOpC.OP.V.vd].HW[el] &= ~0xF;
	}
}

void RSP_Vector_VMUDL (void) {
	int count, el, del;
	MIPS_WORD temp;

	for (count = 0; count < 8; count ++ ) {
		el = Indx[RSPOpC.OP.V.element].B[count];
		del = EleSpec[RSPOpC.OP.V.element].B[el];

		temp.UW = (DWORD)RSP_Vect[RSPOpC.OP.V.vs].UHW[el] * (DWORD)RSP_Vect[RSPOpC.OP.V.vt].UHW[del];
		RSP_ACCUM[el].W[1] = 0;
		RSP_ACCUM[el].HW[1] = temp.HW[1];
		RSP_Vect[RSPOpC.OP.V.vd].HW[el] = RSP_ACCUM[el].HW[1];
		
	}
}

void RSP_Vector_VMUDM (void) {
	int count, el, del;
	MIPS_WORD temp;

	for (count = 0; count < 8; count ++ ) {
		el = Indx[RSPOpC.OP.V.element].B[count];
		del = EleSpec[RSPOpC.OP.V.element].B[el];

		temp.UW = (DWORD)((long)RSP_Vect[RSPOpC.OP.V.vs].HW[el]) * (DWORD)RSP_Vect[RSPOpC.OP.V.vt].UHW[del];
		if (temp.W < 0) {
			RSP_ACCUM[el].HW[3] = -1;
		} else {
			RSP_ACCUM[el].HW[3] = 0;
		}
		RSP_ACCUM[el].HW[2] = temp.HW[1];
		RSP_ACCUM[el].HW[1] = temp.HW[0];
		RSP_Vect[RSPOpC.OP.V.vd].HW[el] = RSP_ACCUM[el].HW[2];
	}
}

void RSP_Vector_VMUDN (void) {
	int count, el, del;
	MIPS_WORD temp;

	for (count = 0; count < 8; count ++ ) {
		el = Indx[RSPOpC.OP.V.element].B[count];
		del = EleSpec[RSPOpC.OP.V.element].B[el];

		temp.UW = (DWORD)RSP_Vect[RSPOpC.OP.V.vs].UHW[el] * (DWORD)(long)(RSP_Vect[RSPOpC.OP.V.vt].HW[del]);
		if (temp.W < 0) {
			RSP_ACCUM[el].HW[3] = -1;
		} else {
			RSP_ACCUM[el].HW[3] = 0;
		}
		RSP_ACCUM[el].HW[2] = temp.HW[1];
		RSP_ACCUM[el].HW[1] = temp.HW[0];
		RSP_Vect[RSPOpC.OP.V.vd].HW[el] = RSP_ACCUM[el].HW[1];
	}
}

void RSP_Vector_VMUDH (void) {
	int count, el, del;

	for (count = 0; count < 8; count ++ ) {
		el = Indx[RSPOpC.OP.V.element].B[count];
		del = EleSpec[RSPOpC.OP.V.element].B[el];
		
		RSP_ACCUM[el].W[1] = (long)RSP_Vect[RSPOpC.OP.V.vs].HW[el] * (long)RSP_Vect[RSPOpC.OP.V.vt].HW[del]; 
		RSP_ACCUM[el].HW[1] = 0;
		if (RSP_ACCUM[el].W[1] < (long)0xFFFF8000) {
			RSP_Vect[RSPOpC.OP.V.vd].HW[el] = (WORD)0x8000;
		}
		else if (RSP_ACCUM[el].W[1] > 0X7FFF) {
			RSP_Vect[RSPOpC.OP.V.vd].HW[el] = 0x7FFF;
		}
		else {
			RSP_Vect[RSPOpC.OP.V.vd].HW[el] = RSP_ACCUM[el].HW[2];
		}
	}
}

void RSP_Vector_VMACF (void) {
	int count, el, del;
	MIPS_WORD temp;

	for (count = 0; count < 8; count ++ ) {
		el = Indx[RSPOpC.OP.V.element].B[count];
		del = EleSpec[RSPOpC.OP.V.element].B[el];

		temp.W = (long)RSP_Vect[RSPOpC.OP.V.vs].HW[el] * (long)(DWORD)RSP_Vect[RSPOpC.OP.V.vt].HW[del];
		RSP_ACCUM[el].DW += ((__int64)temp.W) << 17;
		if (RSP_ACCUM[el].W[1] < (long)0xFFFF8000) {
			RSP_Vect[RSPOpC.OP.V.vd].HW[el] = (WORD)0x8000;
		}
		else if(RSP_ACCUM[el].W[1] > 0X7FFF) {
			RSP_Vect[RSPOpC.OP.V.vd].HW[el] = 0x7FFF;
		}
		else {
			RSP_Vect[RSPOpC.OP.V.vd].HW[el] = RSP_ACCUM[el].HW[2];
		}
	}
}

void RSP_Vector_VMACU (void) {
	int count, el, del;
	MIPS_WORD temp, temp2;

	for (count = 0; count < 8; count ++ ) {
		el = Indx[RSPOpC.OP.V.element].B[count];
		del = EleSpec[RSPOpC.OP.V.element].B[el];

		temp.W = (long)RSP_Vect[RSPOpC.OP.V.vs].HW[el] * (long)(DWORD)RSP_Vect[RSPOpC.OP.V.vt].HW[del];
		RSP_ACCUM[el].UHW[3] += (WORD)(temp.W >> 31);
		temp.UW = temp.UW << 1;
		temp2.UW = temp.UHW[0] + RSP_ACCUM[el].UHW[1];
		RSP_ACCUM[el].HW[1] = temp2.HW[0];
		temp2.UW = temp.UHW[1] + RSP_ACCUM[el].UHW[2] + temp2.UHW[1];
		RSP_ACCUM[el].HW[2] = temp2.HW[0];
		RSP_ACCUM[el].HW[3] += temp2.HW[1];
		if (RSP_ACCUM[el].HW[3] < 0) {
			RSP_Vect[RSPOpC.OP.V.vd].HW[el] = 0;
		} else {
			if (RSP_ACCUM[el].UHW[3] != 0) { 
				RSP_Vect[RSPOpC.OP.V.vd].UHW[el] = 0xFFFF; 
			} else {
				if (RSP_ACCUM[el].HW[2] < 0) {
					RSP_Vect[RSPOpC.OP.V.vd].UHW[el] = 0xFFFF; 
				} else {
					RSP_Vect[RSPOpC.OP.V.vd].HW[el] = RSP_ACCUM[el].HW[2];
				}
			}
		}
	}
}

void RSP_Vector_VRNDN(void) {
	MIPS_WORD temp;

	for (int count = 0; count < 8; count++) {
		int el = Indx[RSPOpC.OP.V.element].B[count];
		int del = EleSpec[RSPOpC.OP.V.element].B[el];

		if ((RSPOpC.OP.V.vs & 1) != 0) {
			temp.HW[1] = RSP_Vect[RSPOpC.OP.V.vt].HW[del];
			temp.HW[0] = 0;
		}
		else {
			temp.HW[1] = RSP_Vect[RSPOpC.OP.V.vt].HW[del] >> 15;
			temp.HW[0] = RSP_Vect[RSPOpC.OP.V.vt].HW[del];
		}

		if (RSP_ACCUM[el].DW < 0) {
			RSP_ACCUM[el].DW += ((long long)temp.W) << 16;
		}

		if (RSP_ACCUM[el].W[1] > 0x7FFF) {
			RSP_Vect[RSPOpC.OP.V.vd].HW[el] = 0x7FFF;
		}
		else if (RSP_ACCUM[el].W[1] < 0 && RSP_ACCUM[el].UHW[3] != 0xFFFF) {
			RSP_Vect[RSPOpC.OP.V.vd].HW[el] = 0x8000;
		}
		else {
			RSP_Vect[RSPOpC.OP.V.vd].HW[el] = RSP_ACCUM[el].HW[2];
		}
	}
}

void RSP_Vector_VMACQ (void) {
	int count, el, del;
	MIPS_WORD temp;

	for (count = 0; count < 8; count ++ ) {
		el = Indx[RSPOpC.OP.V.element].B[count];
		del = EleSpec[RSPOpC.OP.V.element].B[el];

		if (RSP_ACCUM[el].W[1] > 0x20) {
			if ((RSP_ACCUM[el].W[1] & 0x20) == 0) {
				RSP_ACCUM[el].W[1] -= 0x20;
			}
		} else if (RSP_ACCUM[el].W[1] < -0x20) {
			if ((RSP_ACCUM[el].W[1] & 0x20) == 0) {
				RSP_ACCUM[el].W[1] += 0x20;
			}
		}
		temp.W = RSP_ACCUM[el].W[1] >> 1;
		if (temp.HW[1] < 0) {
			if (temp.UHW[1] != 0xFFFF) { 
				RSP_Vect[RSPOpC.OP.V.vd].HW[el] = (WORD)0x8000;
			} else {
				if (temp.HW[0] >= 0) {
					RSP_Vect[RSPOpC.OP.V.vd].HW[el] = (WORD)0x8000;
				} else {					
					RSP_Vect[RSPOpC.OP.V.vd].HW[el] = (WORD)(temp.UW & 0xFFF0);
				}
			}
		} else {
			if (temp.UHW[1] != 0) { 
				RSP_Vect[RSPOpC.OP.V.vd].HW[el] = 0x7FF0;
			} else {
				if (temp.HW[0] < 0) {
					RSP_Vect[RSPOpC.OP.V.vd].HW[el] = 0x7FF0;
				} else {
					RSP_Vect[RSPOpC.OP.V.vd].HW[el] = (WORD)(temp.UW & 0xFFF0);
				}
			}
		}
	}
}

void RSP_Vector_VMADL (void) {
	int count, el, del;
	MIPS_WORD temp, temp2;

	for (count = 0; count < 8; count ++ ) {
		el = Indx[RSPOpC.OP.V.element].B[count];
		del = EleSpec[RSPOpC.OP.V.element].B[el];

		temp.UW = (DWORD)RSP_Vect[RSPOpC.OP.V.vs].UHW[el] * (DWORD)RSP_Vect[RSPOpC.OP.V.vt].UHW[del];
		temp2.UW = temp.UHW[1] + RSP_ACCUM[el].UHW[1];
		RSP_ACCUM[el].HW[1] = temp2.HW[0];
		temp2.UW = RSP_ACCUM[el].UHW[2] + temp2.UHW[1];
		RSP_ACCUM[el].HW[2] = temp2.HW[0];
		RSP_ACCUM[el].HW[3] += temp2.HW[1];
		if (RSP_ACCUM[el].HW[3] < 0) {
			if (RSP_ACCUM[el].UHW[3] != 0xFFFF) { 
				RSP_Vect[RSPOpC.OP.V.vd].HW[el] = 0;
			} else {
				if (RSP_ACCUM[el].HW[2] >= 0) {
					RSP_Vect[RSPOpC.OP.V.vd].HW[el] = 0;
				} else {
					RSP_Vect[RSPOpC.OP.V.vd].HW[el] = RSP_ACCUM[el].HW[1];
				}
			}
		} else {
			if (RSP_ACCUM[el].UHW[3] != 0) { 
				RSP_Vect[RSPOpC.OP.V.vd].UHW[el] = 0xFFFF;
			} else {
				if (RSP_ACCUM[el].HW[2] < 0) {
					RSP_Vect[RSPOpC.OP.V.vd].UHW[el] = 0xFFFF;
				} else {
					RSP_Vect[RSPOpC.OP.V.vd].HW[el] = RSP_ACCUM[el].HW[1];
				}
			}
		}
	}
}

void RSP_Vector_VMADM (void) {
	int count, el, del;
	MIPS_WORD temp, temp2;

	for (count = 0; count < 8; count ++ ) {
		el = Indx[RSPOpC.OP.V.element].B[count];
		del = EleSpec[RSPOpC.OP.V.element].B[el];

		temp.UW = (DWORD)((long)RSP_Vect[RSPOpC.OP.V.vs].HW[el]) * (DWORD)RSP_Vect[RSPOpC.OP.V.vt].UHW[del];
		temp2.UW = temp.UHW[0] + RSP_ACCUM[el].UHW[1];
		RSP_ACCUM[el].HW[1] = temp2.HW[0];
		temp2.UW = temp.UHW[1] + RSP_ACCUM[el].UHW[2] + temp2.UHW[1];
		RSP_ACCUM[el].HW[2] = temp2.HW[0];
		RSP_ACCUM[el].HW[3] += temp2.HW[1];
		if (temp.W < 0) { 
			RSP_ACCUM[el].HW[3] -= 1; 
		}
		if (RSP_ACCUM[el].W[1] < (long)0xFFFF8000) {
			RSP_Vect[RSPOpC.OP.V.vd].HW[el] = (WORD)0x8000;
		}
		else if (RSP_ACCUM[el].W[1] > 0X7FFF) {
			RSP_Vect[RSPOpC.OP.V.vd].HW[el] = 0x7FFF;
		}
		else {
			RSP_Vect[RSPOpC.OP.V.vd].HW[el] = RSP_ACCUM[el].HW[2];
		}
	}
}

void RSP_Vector_VMADN (void) {
	int count, el, del;
	MIPS_WORD temp, temp2;

	for (count = 0; count < 8; count ++ ) {
		el = Indx[RSPOpC.OP.V.element].B[count];
		del = EleSpec[RSPOpC.OP.V.element].B[el];

		temp.UW = (DWORD)RSP_Vect[RSPOpC.OP.V.vs].UHW[el] * (DWORD)((long)RSP_Vect[RSPOpC.OP.V.vt].HW[del]);
		temp2.UW = temp.UHW[0] + RSP_ACCUM[el].UHW[1];
		RSP_ACCUM[el].HW[1] = temp2.HW[0];
		temp2.UW = temp.UHW[1] + RSP_ACCUM[el].UHW[2] + temp2.UHW[1];
		RSP_ACCUM[el].HW[2] = temp2.HW[0];
		RSP_ACCUM[el].HW[3] += temp2.HW[1];
		if (temp.W < 0) { 
			RSP_ACCUM[el].HW[3] -= 1; 
		}
		if (RSP_ACCUM[el].HW[3] < 0) {
			if (RSP_ACCUM[el].UHW[3] != 0xFFFF) { 
				RSP_Vect[RSPOpC.OP.V.vd].HW[el] = 0;
			} else {
				if (RSP_ACCUM[el].HW[2] >= 0) {
					RSP_Vect[RSPOpC.OP.V.vd].HW[el] = 0;
				} else {
					RSP_Vect[RSPOpC.OP.V.vd].HW[el] = RSP_ACCUM[el].HW[1];
				}
			}
		} else {
			if (RSP_ACCUM[el].UHW[3] != 0) { 
				RSP_Vect[RSPOpC.OP.V.vd].UHW[el] = 0xFFFF;
			} else {
				if (RSP_ACCUM[el].HW[2] < 0) {
					RSP_Vect[RSPOpC.OP.V.vd].UHW[el] = 0xFFFF;
				} else {
					RSP_Vect[RSPOpC.OP.V.vd].HW[el] = RSP_ACCUM[el].HW[1];
				}
			}
		}
	}
}

void RSP_Vector_VMADH (void) {
	int count, el, del;

	for (count = 0; count < 8; count ++ ) {
		el = Indx[RSPOpC.OP.V.element].B[count];
		del = EleSpec[RSPOpC.OP.V.element].B[el];
		
		RSP_ACCUM[el].W[1] += (long)RSP_Vect[RSPOpC.OP.V.vs].HW[el] * (long)RSP_Vect[RSPOpC.OP.V.vt].HW[del];
		if (RSP_ACCUM[el].W[1] < (long)0xFFFF8000) {
			RSP_Vect[RSPOpC.OP.V.vd].HW[el] = (WORD)0x8000;
		}
		else if (RSP_ACCUM[el].W[1] > 0X7FFF) {
			RSP_Vect[RSPOpC.OP.V.vd].HW[el] = 0x7FFF;
		}
		else {
			RSP_Vect[RSPOpC.OP.V.vd].HW[el] = RSP_ACCUM[el].HW[2];
		}
	}
}

void RSP_Vector_VADD (void) {
	int count, el, del;
	MIPS_WORD temp;
	
	for ( count = 0; count < 8; count++ ) {
		el = Indx[RSPOpC.OP.V.element].B[count];
		del = EleSpec[RSPOpC.OP.V.element].B[el];
        
		temp.W = (int)RSP_Vect[RSPOpC.OP.V.vs].HW[el] + (int)RSP_Vect[RSPOpC.OP.V.vt].HW[del] +
			 ((RspVCO >> (7 - el)) & 0x1);
		RSP_ACCUM[el].HW[1] = temp.HW[0];
		if ((temp.HW[0] & 0x8000) == 0) {
			if (temp.HW[1] != 0) {
				RSP_Vect[RSPOpC.OP.V.vd].HW[el] = (WORD)0x8000;
			} else {
				RSP_Vect[RSPOpC.OP.V.vd].HW[el] = temp.HW[0];
			}
		} else {
			if (temp.HW[1] != -1 ) {
				RSP_Vect[RSPOpC.OP.V.vd].HW[el] = 0x7FFF;
			} else {
				RSP_Vect[RSPOpC.OP.V.vd].HW[el] = temp.HW[0];
			}
		}
	}
	RspVCO = 0;
}

void RSP_Vector_VSUB (void) {
	int count, el, del;
	MIPS_WORD temp;
	
	for ( count = 0; count < 8; count++ ) {
		el = Indx[RSPOpC.OP.V.element].B[count];
		del = EleSpec[RSPOpC.OP.V.element].B[el];
        
		temp.W = (int)RSP_Vect[RSPOpC.OP.V.vs].HW[el] - (int)RSP_Vect[RSPOpC.OP.V.vt].HW[del] -
			 ((RspVCO>> (7 - el)) & 0x1);
		RSP_ACCUM[el].HW[1] = temp.HW[0];
		if ((temp.HW[0] & 0x8000) == 0) {
			if (temp.HW[1] != 0) {
				RSP_Vect[RSPOpC.OP.V.vd].HW[el] = (WORD)0x8000;
			} else {
				RSP_Vect[RSPOpC.OP.V.vd].HW[el] = temp.HW[0];
			}
		} else {
			if (temp.HW[1] != -1 ) {
				RSP_Vect[RSPOpC.OP.V.vd].HW[el] = 0x7FFF;
			} else {
				RSP_Vect[RSPOpC.OP.V.vd].HW[el] = temp.HW[0];
			}
		}
	}
	RspVCO = 0;
}

static void AddAndClear() {
	for (int count = 0; count < 8; ++count) {
		int el = Indx[RSPOpC.OP.V.element].B[count];
		int del = EleSpec[RSPOpC.OP.V.element].B[el];

		RSP_ACCUM[el].HW[1] = RSP_Vect[RSPOpC.OP.V.vs].UHW[el] + RSP_Vect[RSPOpC.OP.V.vt].UHW[del];
		RSP_Vect[RSPOpC.OP.V.vd].UHW[el] = 0;
	}
}

void RSP_Vector_VSUT(void) {
	AddAndClear();
}

void RSP_Vector_VABS (void) {
	int count, el, del;

	for ( count = 0; count < 8; count++ ) {
		el = Indx[RSPOpC.OP.V.element].B[count];
		del = EleSpec[RSPOpC.OP.V.element].B[el];

		if (RSP_Vect[RSPOpC.OP.V.vs].HW[el] > 0) {
			RSP_Vect[RSPOpC.OP.V.vd].HW[el] = RSP_Vect[RSPOpC.OP.V.vt].UHW[del];
			RSP_ACCUM[el].HW[1] = RSP_Vect[RSPOpC.OP.V.vd].HW[el];
		} else if (RSP_Vect[RSPOpC.OP.V.vs].HW[el] < 0) {
			if (RSP_Vect[RSPOpC.OP.V.vt].UHW[del] == 0x8000) {
				RSP_Vect[RSPOpC.OP.V.vd].HW[el] = 0x7FFF;
				RSP_ACCUM[el].HW[1] = 0x8000;
			} else {
				RSP_Vect[RSPOpC.OP.V.vd].HW[el] = RSP_Vect[RSPOpC.OP.V.vt].HW[del] * -1;
				RSP_ACCUM[el].HW[1] = RSP_Vect[RSPOpC.OP.V.vd].HW[el];
			}
		} else {
			RSP_Vect[RSPOpC.OP.V.vd].HW[el] = 0;
			RSP_ACCUM[el].HW[1] = RSP_Vect[RSPOpC.OP.V.vd].HW[el];
		}
	}
}

void RSP_Vector_VADDC (void) {
	int count, el, del;
	MIPS_WORD temp;
	
	RspVCO = 0;
	for ( count = 0; count < 8; count++ ) {
		el = Indx[RSPOpC.OP.V.element].B[count];
		del = EleSpec[RSPOpC.OP.V.element].B[el];
        
		temp.UW = (int)RSP_Vect[RSPOpC.OP.V.vs].UHW[el] + (int)RSP_Vect[RSPOpC.OP.V.vt].UHW[del];
		RSP_ACCUM[el].HW[1] = temp.HW[0];
		RSP_Vect[RSPOpC.OP.V.vd].HW[el] = temp.HW[0];
		if (temp.UW & 0xffff0000) {
			RspVCO |= ( 1 << (7 - el) );
		}
	}
}

void RSP_Vector_VSUBC (void) {
	int count, el, del;
	MIPS_WORD temp;
	
	RspVCO = 0x0;
	for ( count = 0; count < 8; count++ ) {
		el = Indx[RSPOpC.OP.V.element].B[count];
		del = EleSpec[RSPOpC.OP.V.element].B[el];
        
		temp.UW = (int)RSP_Vect[RSPOpC.OP.V.vs].UHW[el] - (int)RSP_Vect[RSPOpC.OP.V.vt].UHW[del];
		RSP_ACCUM[el].HW[1] = temp.HW[0];
		RSP_Vect[RSPOpC.OP.V.vd].HW[el] = temp.HW[0];
		if (temp.HW[0] != 0) {
			RspVCO |= ( 0x1 << (15 - el) );
		}
		if (temp.UW & 0xffff0000) {
			RspVCO |= ( 0x1 << (7 - el) );
		}
	}
}

void RSP_Vector_VADDB(void) {
	AddAndClear();
}

void RSP_Vector_VSUBB(void) {
	AddAndClear();
}

void RSP_Vector_VACCB(void) {
	AddAndClear();
}

void RSP_Vector_VSUCB(void) {
	AddAndClear();
}

void RSP_Vector_VSAD(void) {
	AddAndClear();
}

void RSP_Vector_VSAC(void) {
	AddAndClear();

}

void RSP_Vector_VSUM(void) {
	AddAndClear();
}

void RSP_Vector_VSAR (void) {
	switch (RSPOpC.OP.V.element) {
	case 8:
		RSP_Vect[RSPOpC.OP.V.vd].HW[0] = RSP_ACCUM[0].HW[3];
		RSP_Vect[RSPOpC.OP.V.vd].HW[1] = RSP_ACCUM[1].HW[3];
		RSP_Vect[RSPOpC.OP.V.vd].HW[2] = RSP_ACCUM[2].HW[3];
		RSP_Vect[RSPOpC.OP.V.vd].HW[3] = RSP_ACCUM[3].HW[3];
		RSP_Vect[RSPOpC.OP.V.vd].HW[4] = RSP_ACCUM[4].HW[3];
		RSP_Vect[RSPOpC.OP.V.vd].HW[5] = RSP_ACCUM[5].HW[3];
		RSP_Vect[RSPOpC.OP.V.vd].HW[6] = RSP_ACCUM[6].HW[3];
		RSP_Vect[RSPOpC.OP.V.vd].HW[7] = RSP_ACCUM[7].HW[3];
		break;
	case 9:
		RSP_Vect[RSPOpC.OP.V.vd].HW[0] = RSP_ACCUM[0].HW[2];
		RSP_Vect[RSPOpC.OP.V.vd].HW[1] = RSP_ACCUM[1].HW[2];
		RSP_Vect[RSPOpC.OP.V.vd].HW[2] = RSP_ACCUM[2].HW[2];
		RSP_Vect[RSPOpC.OP.V.vd].HW[3] = RSP_ACCUM[3].HW[2];
		RSP_Vect[RSPOpC.OP.V.vd].HW[4] = RSP_ACCUM[4].HW[2];
		RSP_Vect[RSPOpC.OP.V.vd].HW[5] = RSP_ACCUM[5].HW[2];
		RSP_Vect[RSPOpC.OP.V.vd].HW[6] = RSP_ACCUM[6].HW[2];
		RSP_Vect[RSPOpC.OP.V.vd].HW[7] = RSP_ACCUM[7].HW[2];
		break;
	case 10:
		RSP_Vect[RSPOpC.OP.V.vd].HW[0] = RSP_ACCUM[0].HW[1];
		RSP_Vect[RSPOpC.OP.V.vd].HW[1] = RSP_ACCUM[1].HW[1];
		RSP_Vect[RSPOpC.OP.V.vd].HW[2] = RSP_ACCUM[2].HW[1];
		RSP_Vect[RSPOpC.OP.V.vd].HW[3] = RSP_ACCUM[3].HW[1];
		RSP_Vect[RSPOpC.OP.V.vd].HW[4] = RSP_ACCUM[4].HW[1];
		RSP_Vect[RSPOpC.OP.V.vd].HW[5] = RSP_ACCUM[5].HW[1];
		RSP_Vect[RSPOpC.OP.V.vd].HW[6] = RSP_ACCUM[6].HW[1];
		RSP_Vect[RSPOpC.OP.V.vd].HW[7] = RSP_ACCUM[7].HW[1];
		break;
	default:
		RSP_Vect[RSPOpC.OP.V.vd].UDW[1] = 0;
		RSP_Vect[RSPOpC.OP.V.vd].UDW[0] = 0;
	}
}

void RSP_Vector_V30(void) {
	AddAndClear();
}

void RSP_Vector_V31(void) {
	AddAndClear();
}

void RSP_Vector_VLT (void) {
	int count, el, del;
	
	RspVCC = 0;
	for ( count = 0; count < 8; count++ ) {
		el = Indx[RSPOpC.OP.V.element].B[count];
		del = EleSpec[RSPOpC.OP.V.element].B[el];

		if (RSP_Vect[RSPOpC.OP.V.vs].HW[el] < RSP_Vect[RSPOpC.OP.V.vt].HW[del]) {
			RSP_Vect[RSPOpC.OP.V.vd].HW[el] = RSP_Vect[RSPOpC.OP.V.vs].UHW[el];
			RspVCC |= ( 1 << (7 - el) );
		} else if (RSP_Vect[RSPOpC.OP.V.vs].HW[el] != RSP_Vect[RSPOpC.OP.V.vt].HW[del]) {
			RSP_Vect[RSPOpC.OP.V.vd].HW[el] = RSP_Vect[RSPOpC.OP.V.vt].UHW[del];
			RspVCC &= ~( 1 << (7 - el) );
		} else {
			RSP_Vect[RSPOpC.OP.V.vd].HW[el] = RSP_Vect[RSPOpC.OP.V.vs].UHW[el];
			if ( (RspVCO & (0x101 << (7 - el))) == (WORD)(0x101 << (7 - el))) {
				RspVCC |= ( 1 << (7 - el) );
			} else {	
				RspVCC &= ~( 1 << (7 - el) );
			}
		}
		RSP_ACCUM[el].HW[1] = RSP_Vect[RSPOpC.OP.V.vd].HW[el];			
	}
	RspVCO = 0;
}

void RSP_Vector_VEQ (void) {
	int count, el, del;
	
	RspVCC = 0;
	for ( count = 0; count < 8; count++ ) {
		el = Indx[RSPOpC.OP.V.element].B[count];
		del = EleSpec[RSPOpC.OP.V.element].B[el];
			
		if (RSP_Vect[RSPOpC.OP.V.vs].UHW[el] == RSP_Vect[RSPOpC.OP.V.vt].UHW[del]) {
			if ( (RspVCO & (1 << (15 - el))) == 0) {
				RspVCC |= ( 1 << (7 - el));
			}
		}

		RSP_Vect[RSPOpC.OP.V.vd].HW[el] = RSP_Vect[RSPOpC.OP.V.vt].UHW[del];
		RSP_ACCUM[el].HW[1] = RSP_Vect[RSPOpC.OP.V.vt].UHW[del];
	}
	RspVCO = 0;
}

void RSP_Vector_VNE (void) {
	int count, el, del;
	
	RspVCC = 0;
	for ( count = 0; count < 8; count++ ) {
		el = Indx[RSPOpC.OP.V.element].B[count];
		del = EleSpec[RSPOpC.OP.V.element].B[el];

   		if (RSP_Vect[RSPOpC.OP.V.vs].UHW[el] != RSP_Vect[RSPOpC.OP.V.vt].UHW[del]) {
			RspVCC |= ( 1 << (7 - el) );
		} else {
			if ( (RspVCO & (1 << (15 - el))) != 0) {
				RspVCC |= ( 1 << (7 - el) );
			}
		}

		RSP_Vect[RSPOpC.OP.V.vd].HW[el] = RSP_Vect[RSPOpC.OP.V.vs].UHW[el];
		RSP_ACCUM[el].HW[1] = RSP_Vect[RSPOpC.OP.V.vs].UHW[el];
	}
	RspVCO = 0;
}

void RSP_Vector_VGE (void) {
	int count, el, del;
	
	RspVCC = 0;
	for ( count = 0; count < 8; count++ ) {
		el = Indx[RSPOpC.OP.V.element].B[count];
		del = EleSpec[RSPOpC.OP.V.element].B[el];

		if (RSP_Vect[RSPOpC.OP.V.vs].HW[el] == RSP_Vect[RSPOpC.OP.V.vt].HW[del]) {
			RSP_Vect[RSPOpC.OP.V.vd].HW[el] = RSP_Vect[RSPOpC.OP.V.vs].UHW[el];
			if ( (RspVCO & (0x101 << (7 - el))) == (WORD)(0x101 << (7 - el))) {
				RspVCC &= ~( 1 << (7 - el) );
			} else {	
				RspVCC |= ( 1 << (7 - el) );
			}
		} else if (RSP_Vect[RSPOpC.OP.V.vs].HW[el] > RSP_Vect[RSPOpC.OP.V.vt].HW[del]) {
			RSP_Vect[RSPOpC.OP.V.vd].HW[el] = RSP_Vect[RSPOpC.OP.V.vs].UHW[el];
			RspVCC |= ( 1 << (7 - el) );
		} else {
			RSP_Vect[RSPOpC.OP.V.vd].HW[el] = RSP_Vect[RSPOpC.OP.V.vt].UHW[del];
			RspVCC &= ~( 1 << (7 - el) );
		}
		RSP_ACCUM[el].HW[1] = RSP_Vect[RSPOpC.OP.V.vd].HW[el];			
	}
	RspVCO = 0;
}

void RSP_Vector_VCL (void) {
	int count, el, del;

	for (count = 0;count < 8; count++) {
		el = Indx[RSPOpC.OP.V.element].B[count];
		del = EleSpec[RSPOpC.OP.V.element].B[el];

		if ((RspVCO & ( 1 << (7 - el))) != 0 ) {
			if ((RspVCO & ( 1 << (15 - el))) != 0 ) {
				if ((RspVCC & ( 1 << (7 - el))) != 0 ) {
					RSP_ACCUM[el].HW[1] = -RSP_Vect[RSPOpC.OP.V.vt].UHW[del];
				} else {
					RSP_ACCUM[el].HW[1] = RSP_Vect[RSPOpC.OP.V.vs].HW[el];
				}
			} else {
				if ((RspVCE & ( 1 << (7 - el)))) {
					if ( RSP_Vect[RSPOpC.OP.V.vs].UHW[el] + RSP_Vect[RSPOpC.OP.V.vt].UHW[del] > 0x10000) {
						RSP_ACCUM[el].HW[1] = RSP_Vect[RSPOpC.OP.V.vs].HW[el];
						RspVCC &= ~(1 << (7 - el));
					} else {
						RSP_ACCUM[el].HW[1] = -RSP_Vect[RSPOpC.OP.V.vt].UHW[del];
						RspVCC |= (1 << (7 - el));
					}
				} else {
					if (RSP_Vect[RSPOpC.OP.V.vt].UHW[del] + RSP_Vect[RSPOpC.OP.V.vs].UHW[el] != 0) {
						RSP_ACCUM[el].HW[1] = RSP_Vect[RSPOpC.OP.V.vs].HW[el];
						RspVCC &= ~(1 << (7 - el));
					} else {
						RSP_ACCUM[el].HW[1] = -RSP_Vect[RSPOpC.OP.V.vt].UHW[del];
						RspVCC |= (1 << (7 - el));
					}
				}
			}
		} else {
			if ((RspVCO & ( 1 << (15 - el))) != 0 ) {
				if ((RspVCC & ( 1 << (15 - el))) != 0 ) {
					RSP_ACCUM[el].HW[1] = RSP_Vect[RSPOpC.OP.V.vt].HW[del];
				} else {
					RSP_ACCUM[el].HW[1] = RSP_Vect[RSPOpC.OP.V.vs].HW[el];
				}
			} else {			
				if ( RSP_Vect[RSPOpC.OP.V.vs].UHW[el] - RSP_Vect[RSPOpC.OP.V.vt].UHW[del] >= 0) {
					RSP_ACCUM[el].HW[1] = RSP_Vect[RSPOpC.OP.V.vt].UHW[del];
					RspVCC |= (1 << (15 - el));
				} else {
					RSP_ACCUM[el].HW[1] = RSP_Vect[RSPOpC.OP.V.vs].HW[el];
					RspVCC &= ~(1 << (15 - el));
				}				
			}
		}
		RSP_Vect[RSPOpC.OP.V.vd].HW[el] = RSP_ACCUM[el].HW[1];
	}
	RspVCO = 0;
	RspVCE = 0;
}

void RSP_Vector_VCH (void) {
	int count, el, del;
	
	RspVCO = 0;
	RspVCC = 0;
	RspVCE = 0;

	for (count = 0;count < 8; count++) {
		el = Indx[RSPOpC.OP.V.element].B[count];
		del = EleSpec[RSPOpC.OP.V.element].B[el];

		if ((RSP_Vect[RSPOpC.OP.V.vs].HW[el] ^ RSP_Vect[RSPOpC.OP.V.vt].HW[del]) < 0) {
			RspVCO |= ( 1 << (7 - el));
			
			if (RSP_Vect[RSPOpC.OP.V.vt].HW[del] < 0) {
				RspVCC |= ( 1 << (15 - el));
			}
			if (RSP_Vect[RSPOpC.OP.V.vs].HW[el] + RSP_Vect[RSPOpC.OP.V.vt].HW[del] != 0) {
				if (RSP_Vect[RSPOpC.OP.V.vs].HW[el] != ~RSP_Vect[RSPOpC.OP.V.vt].HW[del]) {
					RspVCO |= (1 << (15 - el));
				}
			}
			if (RSP_Vect[RSPOpC.OP.V.vs].HW[el] + RSP_Vect[RSPOpC.OP.V.vt].HW[del] <= 0) {
				if (RSP_Vect[RSPOpC.OP.V.vs].HW[el] + RSP_Vect[RSPOpC.OP.V.vt].HW[del] == -1) {
					RspVCE |= ( 1 << (7 - el));
				}
				RspVCC |= ( 1 << (7 - el));
				RSP_Vect[RSPOpC.OP.V.vd].HW[el] = -RSP_Vect[RSPOpC.OP.V.vt].UHW[del];
			} else {
				RSP_Vect[RSPOpC.OP.V.vd].HW[el] = RSP_Vect[RSPOpC.OP.V.vs].HW[el];
			}
		} else {
			if (RSP_Vect[RSPOpC.OP.V.vt].HW[del] < 0) {
				RspVCC |= ( 1 << (7 - el));
			}
			
			if (RSP_Vect[RSPOpC.OP.V.vs].HW[el] - RSP_Vect[RSPOpC.OP.V.vt].HW[del] != 0) {
				if (RSP_Vect[RSPOpC.OP.V.vs].HW[el] != ~RSP_Vect[RSPOpC.OP.V.vt].HW[del]) {
					RspVCO |= (1 << (15 - el));
				}
			}
			if (RSP_Vect[RSPOpC.OP.V.vs].HW[el] - RSP_Vect[RSPOpC.OP.V.vt].HW[del] >= 0) {
				RspVCC |= ( 1 << (15 - el));
				RSP_Vect[RSPOpC.OP.V.vd].HW[el] = RSP_Vect[RSPOpC.OP.V.vt].UHW[del];
			} else {
				RSP_Vect[RSPOpC.OP.V.vd].HW[el] = RSP_Vect[RSPOpC.OP.V.vs].HW[el];
			}
		}
		RSP_ACCUM[el].HW[1] = RSP_Vect[RSPOpC.OP.V.vd].HW[el];
	}
}

void RSP_Vector_VCR (void) {
	int count, el, del;

	RspVCO = 0;
	RspVCC = 0;
	RspVCE = 0;
	for (count = 0;count < 8; count++) {
		el = Indx[RSPOpC.OP.V.element].B[count];
		del = EleSpec[RSPOpC.OP.V.element].B[el];
		
		if ((RSP_Vect[RSPOpC.OP.V.vs].HW[el] ^ RSP_Vect[RSPOpC.OP.V.vt].HW[del]) < 0) {
			if (RSP_Vect[RSPOpC.OP.V.vt].HW[del] < 0) {
				RspVCC |= ( 1 << (15 - el));
			}
			if ((RSP_Vect[RSPOpC.OP.V.vs].HW[el] + RSP_Vect[RSPOpC.OP.V.vt].HW[del] + 1) <= 0) {
				RSP_ACCUM[el].HW[1] = ~RSP_Vect[RSPOpC.OP.V.vt].UHW[del];
				RspVCC |= ( 1 << (7 - el));
			} else {
				RSP_ACCUM[el].HW[1] = RSP_Vect[RSPOpC.OP.V.vs].HW[el];
			}
		} else {
			if (RSP_Vect[RSPOpC.OP.V.vt].HW[del] < 0) {
				RspVCC |= ( 1 << (7 - el));
			}
			if (RSP_Vect[RSPOpC.OP.V.vs].HW[el] - RSP_Vect[RSPOpC.OP.V.vt].HW[del] >= 0) {
				RSP_ACCUM[el].HW[1] = RSP_Vect[RSPOpC.OP.V.vt].UHW[del];
				RspVCC |= ( 1 << (15 - el));
			} else {
				RSP_ACCUM[el].HW[1] = RSP_Vect[RSPOpC.OP.V.vs].HW[el];
			}
		}
		RSP_Vect[RSPOpC.OP.V.vd].HW[el] = RSP_ACCUM[el].HW[1];
	}
}

void RSP_Vector_VMRG (void) {
	int count, el, del;

	for ( count = 0; count < 8; count ++ ){
		el = Indx[RSPOpC.OP.V.element].B[count];
		del = EleSpec[RSPOpC.OP.V.element].B[el];

		if ((RspVCC & ( 1 << (7 - el))) != 0) {
			RSP_Vect[RSPOpC.OP.V.vd].HW[el] = RSP_Vect[RSPOpC.OP.V.vs].HW[el];
		} else {
			RSP_Vect[RSPOpC.OP.V.vd].HW[el] = RSP_Vect[RSPOpC.OP.V.vt].HW[del];
		}
		RSP_ACCUM[el].HW[1] = RSP_Vect[RSPOpC.OP.V.vd].HW[el];
	}
	RspVCO = 0;
}

void RSP_Vector_VAND (void) {
	int count, el, del;

	for ( count = 0; count < 8; count ++ ){
		el = Indx[RSPOpC.OP.V.element].B[count];
		del = EleSpec[RSPOpC.OP.V.element].B[el];
		RSP_Vect[RSPOpC.OP.V.vd].HW[el] = RSP_Vect[RSPOpC.OP.V.vs].HW[el] & RSP_Vect[RSPOpC.OP.V.vt].HW[del];
		RSP_ACCUM[el].HW[1] = RSP_Vect[RSPOpC.OP.V.vd].HW[el];
	}	
}

void RSP_Vector_VNAND (void) {
	int count, el, del;

	for ( count = 0; count < 8; count ++ ){
		el = Indx[RSPOpC.OP.V.element].B[count];
		del = EleSpec[RSPOpC.OP.V.element].B[el];
		RSP_Vect[RSPOpC.OP.V.vd].HW[el] = ~(RSP_Vect[RSPOpC.OP.V.vs].HW[el] & RSP_Vect[RSPOpC.OP.V.vt].HW[del]);
		RSP_ACCUM[el].HW[1] = RSP_Vect[RSPOpC.OP.V.vd].HW[el];
	}
}

void RSP_Vector_VOR (void) {
	int count, el, del;

	for ( count = 0; count < 8; count ++ ){
		el = Indx[RSPOpC.OP.V.element].B[count];
		del = EleSpec[RSPOpC.OP.V.element].B[el];
		RSP_Vect[RSPOpC.OP.V.vd].HW[el] = RSP_Vect[RSPOpC.OP.V.vs].HW[el] | RSP_Vect[RSPOpC.OP.V.vt].HW[del];
		RSP_ACCUM[el].HW[1] = RSP_Vect[RSPOpC.OP.V.vd].HW[el];
	}	
}

void RSP_Vector_VNOR (void) {
	int count, el, del;

	for ( count = 0; count < 8; count ++ ){
		el = Indx[RSPOpC.OP.V.element].B[count];
		del = EleSpec[RSPOpC.OP.V.element].B[el];
		RSP_Vect[RSPOpC.OP.V.vd].HW[el] = ~(RSP_Vect[RSPOpC.OP.V.vs].HW[el] | RSP_Vect[RSPOpC.OP.V.vt].HW[del]);
		RSP_ACCUM[el].HW[1] = RSP_Vect[RSPOpC.OP.V.vd].HW[el];
	}	
}

void RSP_Vector_VXOR (void) {
	int count, el, del;

	for ( count = 0; count < 8; count ++ ){
		el = Indx[RSPOpC.OP.V.element].B[count];
		del = EleSpec[RSPOpC.OP.V.element].B[el];
		RSP_Vect[RSPOpC.OP.V.vd].HW[el] = RSP_Vect[RSPOpC.OP.V.vs].HW[el] ^ RSP_Vect[RSPOpC.OP.V.vt].HW[del];
		RSP_ACCUM[el].HW[1] = RSP_Vect[RSPOpC.OP.V.vd].HW[el];
	}
}

void RSP_Vector_VNXOR (void) {
	int count, el, del;

	for ( count = 0; count < 8; count ++ ){
		el = Indx[RSPOpC.OP.V.element].B[count];
		del = EleSpec[RSPOpC.OP.V.element].B[el];
		RSP_Vect[RSPOpC.OP.V.vd].HW[el] = ~(RSP_Vect[RSPOpC.OP.V.vs].HW[el] ^ RSP_Vect[RSPOpC.OP.V.vt].HW[del]);
		RSP_ACCUM[el].HW[1] = RSP_Vect[RSPOpC.OP.V.vd].HW[el];
	}	
}

void RSP_Vector_V46(void) {
	AddAndClear();
}

void RSP_Vector_V47(void) {
	AddAndClear();
}

static DWORD rcp(int index) { // TOCHECK: it may be faster to compute each time than doing a lookup in a precomputed table
	if (index == 0) return 0xFFFF;
	QWORD a = index + 512;
	QWORD b = (1LL << 34) / a;
	return (DWORD)((b + 1) >> 8);
}

void RSP_Vector_VRCP (void) {
	int count, neg;

	DivOut.W = RSP_Vect[RSPOpC.OP.V.vt].HW[7 - (RSPOpC.OP.V.element & 7)];
	PendingDivIn = 0;
	if (DivOut.UW == 0) {
		DivOut.UW = 0x7FFFFFFF;
	} else {
		if (DivOut.W < 0) {
			neg = TRUE;
			DivOut.W = ~DivOut.W + 1;
		} else {
			neg = FALSE;
		}

		int index = 0;
		for (count = 15; count > 0; count--) {
			if ((DivOut.UW & (1 << count))) {
				int shift = 15 - count;
				index = ((DivOut.UW << shift ) & 0x7FC0) >> 6;
				break;
			}
		}
		DivOut.UW = ((0x10000 | rcp(index)) << 14) >> count;

		if (neg == TRUE) {
			DivOut.W = ~DivOut.W;
		}
	}
	for ( count = 0; count < 8; count++ ) {
		RSP_ACCUM[count].HW[1] = RSP_Vect[RSPOpC.OP.V.vt].UHW[EleSpec[RSPOpC.OP.V.element].B[count]];
	}
	RSP_Vect[RSPOpC.OP.V.vd].HW[7 - (RSPOpC.OP.R.rd & 0x7)] = DivOut.UHW[0];
}

void RSP_Vector_VRCPL (void) {
	int count, neg;

	if (PendingDivIn) {
		DivOut.UW = RSP_Vect[RSPOpC.OP.V.vt].UHW[7 - (RSPOpC.OP.V.element & 7)] | DivIn.W;
	}
	else {
		DivOut.W = RSP_Vect[RSPOpC.OP.V.vt].HW[7 - (RSPOpC.OP.V.element & 7)];
	}
	PendingDivIn = 0;
	if (DivOut.UW == 0) {
		DivOut.UW = 0x7FFFFFFF;
	} else {
		if (DivOut.W < 0) {
			neg = TRUE;
			if (DivOut.UHW[1] == 0xFFFF && DivOut.HW[0] < 0) {
				DivOut.W = ~DivOut.W + 1;
			} else {
				DivOut.W = ~DivOut.W;
			}
		} else {
			neg = FALSE;
		}
		for (count = 31; count > 0; count--) {
			if ((DivOut.W & (1 << count))) {
				DivOut.W &= (0xFFC00000 >> (31 - count) );
				count = 0;
			}
		}	
		DivOut.W = 0x7FFFFFFF / DivOut.W;
		for (count = 31; count > 0; count--) {
			if ((DivOut.W & (1 << count))) {
				DivOut.W &= (0xFFFF8000 >> (31 - count) );
				count = 0;
			}
		}
		/*int index = 0;
		for (count = 15; count > 0; count--) {
			if ((DivOut.UW & (1 << count))) {
				int shift = 15 - count;
				index = ((DivOut.UW << shift) & 0x7FC0) >> 6;
				break;
			}
		}
		DivOut.UW = ((0x10000 | rcp(index)) << 14) >> count;*/

		if (neg == TRUE) {
			DivOut.W = ~DivOut.W;
		}
	}
	for ( count = 0; count < 8; count++ ) {
		RSP_ACCUM[count].HW[1] = RSP_Vect[RSPOpC.OP.V.vt].UHW[EleSpec[RSPOpC.OP.V.element].B[count]];
	}
	RSP_Vect[RSPOpC.OP.V.vd].HW[7 - (RSPOpC.OP.R.rd & 0x7)] = DivOut.UHW[0];
}

void RSP_Vector_VRCPH (void) {
	int count;

	DivIn.UHW[1] = RSP_Vect[RSPOpC.OP.V.vt].UHW[7 - (RSPOpC.OP.V.element & 7)];
	PendingDivIn = 1;
	for ( count = 0; count < 8; count++ ) {
		RSP_ACCUM[count].HW[1] = RSP_Vect[RSPOpC.OP.V.vt].UHW[EleSpec[RSPOpC.OP.V.element].B[count]];
	}
	RSP_Vect[RSPOpC.OP.V.vd].UHW[7 - (RSPOpC.OP.R.rd & 0x7)] = DivOut.UHW[1];
}

void RSP_Vector_VMOV (void) {
	for (int count = 0; count < 8; count++) {
		RSP_ACCUM[count].HW[1] = RSP_Vect[RSPOpC.OP.V.vt].UHW[EleSpec[RSPOpC.OP.V.element].B[count]];
	}
	RSP_Vect[RSPOpC.OP.V.vd].UHW[7 - (RSPOpC.OP.R.rd & 0x7)] =
		RSP_Vect[RSPOpC.OP.V.vt].UHW[EleSpec[RSPOpC.OP.V.element].B[7 - (RSPOpC.OP.V.vs & 0x7)]];
}

void RSP_Vector_VRSQ (void) {
	int count, neg;

	DivOut.W = RSP_Vect[RSPOpC.OP.V.vt].HW[7 - (RSPOpC.OP.V.element & 7)];
	PendingDivIn = 0;
	if (DivOut.UW == 0) {
		DivOut.UW = 0x7FFFFFFF;
	} else if (DivOut.UW == 0xFFFF8000) {
		DivOut.UW = 0xFFFF0000;
	} else {
		if (DivOut.W < 0) {
			neg = TRUE;
			DivOut.W = ~DivOut.W + 1;
		} else {
			neg = FALSE;
		}
		for (count = 15; count > 0; count--) {
			if ((DivOut.W & (1 << count))) {
				DivOut.W &= (0xFF80 >> (15 - count) );
				count = 0;
			}
		}	
		//SQrootResult.W = sqrt(SQrootResult.W);
		DivOut.W = (long)(0x7FFFFFFF / sqrt(DivOut.W));
		for (count = 31; count > 0; count--) {
			if ((DivOut.W & (1 << count))) {
				DivOut.W &= (0xFFFF8000 >> (31 - count) );
				count = 0;
			}
		}		
		if (neg == TRUE) {
			DivOut.W = ~DivOut.W;
		}
	}
	for ( count = 0; count < 8; count++ ) {
		RSP_ACCUM[count].HW[1] = RSP_Vect[RSPOpC.OP.V.vt].UHW[EleSpec[RSPOpC.OP.V.element].B[count]];
	}
	RSP_Vect[RSPOpC.OP.V.vd].HW[7 - (RSPOpC.OP.R.rd & 0x7)] = DivOut.UHW[0];
}

void RSP_Vector_VRSQL (void) {
	int count, neg;

	if (PendingDivIn) {
		DivOut.UW = RSP_Vect[RSPOpC.OP.V.vt].UHW[EleSpec[RSPOpC.OP.V.element].B[(RSPOpC.OP.R.rd & 0x7)]] | DivIn.W;
	}
	else {
		DivOut.W = RSP_Vect[RSPOpC.OP.V.vt].HW[7 - (RSPOpC.OP.V.element & 7)];
	}
	PendingDivIn = 0;
	if (DivOut.UW == 0) {
		DivOut.UW = 0x7FFFFFFF;
	} else if (DivOut.UW == 0xFFFF8000) {
		DivOut.UW = 0xFFFF0000;
	} else {
		if (DivOut.W < 0) {
			neg = TRUE;
			if (DivOut.UHW[1] == 0xFFFF && DivOut.HW[0] < 0) {				
				DivOut.W = ~DivOut.W + 1;
			} else {
				DivOut.W = ~DivOut.W;
			}
		} else {
			neg = FALSE;
		}
		for (count = 31; count > 0; count--) {
			if ((DivOut.W & (1 << count))) {
				DivOut.W &= (0xFF800000 >> (31 - count) );
				count = 0;
			}
		}	
		DivOut.W = (long)(0x7FFFFFFF / sqrt(DivOut.W));
		for (count = 31; count > 0; count--) {
			if ((DivOut.W & (1 << count))) {
				DivOut.W &= (0xFFFF8000 >> (31 - count) );
				count = 0;
			}
		}		
		if (neg == TRUE) {
			DivOut.W = ~DivOut.W;
		}
	}
	for ( count = 0; count < 8; count++ ) {
		RSP_ACCUM[count].HW[1] = RSP_Vect[RSPOpC.OP.V.vt].UHW[EleSpec[RSPOpC.OP.V.element].B[count]];
	}
	RSP_Vect[RSPOpC.OP.V.vd].HW[7 - (RSPOpC.OP.R.rd & 0x7)] = DivOut.UHW[0];
}

void RSP_Vector_VRSQH (void) {
	int count;

	DivIn.UHW[1] = RSP_Vect[RSPOpC.OP.V.vt].UHW[EleSpec[RSPOpC.OP.V.element].B[(RSPOpC.OP.R.rd & 0x7)]];
	PendingDivIn = 1;
	for ( count = 0; count < 8; count++ ) {
		RSP_ACCUM[count].HW[1] = RSP_Vect[RSPOpC.OP.V.vt].UHW[EleSpec[RSPOpC.OP.V.element].B[count]];
	}
	RSP_Vect[RSPOpC.OP.V.vd].UHW[7 - (RSPOpC.OP.R.rd & 0x7)] = DivOut.UHW[1];
}

void RSP_Vector_VNOOP(void) {}

void RSP_Vector_VEXTT(void) {
	AddAndClear();
}

void RSP_Vector_VEXTQ(void) {
	AddAndClear();
}

void RSP_Vector_VEXTN(void) {
	AddAndClear();
}

void RSP_Vector_V59(void) {
	AddAndClear();
}

void RSP_Vector_VINST(void) {
	AddAndClear();
}

void RSP_Vector_VINSQ(void) {
	AddAndClear();
}

void RSP_Vector_VINSN(void) {
	AddAndClear();
}

void RSP_Vector_VNULL(void) {
}

/************************** lc2 functions **************************/
void RSP_Opcode_LBV ( void ) {
	DWORD Address = ((RSP_GPR[RSPOpC.OP.LSV.base].UW + (DWORD)(RSPOpC.OP.LSV.offset)) &0xFFF);
	RSP_LBV_DMEM( Address, RSPOpC.OP.LSV.vt, RSPOpC.OP.LSV.element);
}

void RSP_Opcode_LSV ( void ) {
	DWORD Address = ((RSP_GPR[RSPOpC.OP.LSV.base].UW + (DWORD)(RSPOpC.OP.LSV.offset << 1)) &0xFFF);
	RSP_LSV_DMEM( Address, RSPOpC.OP.LSV.vt, RSPOpC.OP.LSV.element);
}

void RSP_Opcode_LLV ( void ) {
	DWORD Address = ((RSP_GPR[RSPOpC.OP.LSV.base].UW + (DWORD)(RSPOpC.OP.LSV.offset << 2)) &0xFFF);
	RSP_LLV_DMEM( Address, RSPOpC.OP.LSV.vt, RSPOpC.OP.LSV.element);
}

void RSP_Opcode_LDV ( void ) {
	DWORD Address = ((RSP_GPR[RSPOpC.OP.LSV.base].UW + (DWORD)(RSPOpC.OP.LSV.offset << 3)) &0xFFF);
	RSP_LDV_DMEM( Address, RSPOpC.OP.LSV.vt, RSPOpC.OP.LSV.element);
}

void RSP_Opcode_LQV ( void ) {
	DWORD Address = ((RSP_GPR[RSPOpC.OP.LSV.base].UW + (DWORD)(RSPOpC.OP.LSV.offset << 4)) &0xFFF);
	RSP_LQV_DMEM( Address, RSPOpC.OP.LSV.vt, RSPOpC.OP.LSV.element);
}

void RSP_Opcode_LRV ( void ) {
	DWORD Address = ((RSP_GPR[RSPOpC.OP.LSV.base].UW + (DWORD)(RSPOpC.OP.LSV.offset << 4)) &0xFFF);
	RSP_LRV_DMEM( Address, RSPOpC.OP.LSV.vt, RSPOpC.OP.LSV.element);
}

void RSP_Opcode_LPV ( void ) {
	DWORD Address = ((RSP_GPR[RSPOpC.OP.LSV.base].UW + (DWORD)(RSPOpC.OP.LSV.offset << 3)) &0xFFF);
	RSP_LPV_DMEM( Address, RSPOpC.OP.LSV.vt, RSPOpC.OP.LSV.element);
}

void RSP_Opcode_LUV ( void ) {
	DWORD Address = ((RSP_GPR[RSPOpC.OP.LSV.base].UW + (DWORD)(RSPOpC.OP.LSV.offset << 3)) &0xFFF);
	RSP_LUV_DMEM( Address, RSPOpC.OP.LSV.vt, RSPOpC.OP.LSV.element);
}

void RSP_Opcode_LHV ( void ) {
	DWORD Address = ((RSP_GPR[RSPOpC.OP.LSV.base].UW + (DWORD)(RSPOpC.OP.LSV.offset << 4)) &0xFFF);
	RSP_LHV_DMEM( Address, RSPOpC.OP.LSV.vt, RSPOpC.OP.LSV.element);
}

void RSP_Opcode_LFV ( void ) {
	DWORD Address = ((RSP_GPR[RSPOpC.OP.LSV.base].UW + (DWORD)(RSPOpC.OP.LSV.offset << 4)) &0xFFF);
	RSP_LFV_DMEM( Address, RSPOpC.OP.LSV.vt, RSPOpC.OP.LSV.element);
}

void RSP_Opcode_LWV( void ) {
}

void RSP_Opcode_LTV ( void ) {
	DWORD Address = ((RSP_GPR[RSPOpC.OP.LSV.base].UW + (DWORD)(RSPOpC.OP.LSV.offset << 4)) &0xFFF);
	RSP_LTV_DMEM( Address, RSPOpC.OP.LSV.vt, RSPOpC.OP.LSV.element);
}

/************************** sc2 functions **************************/
void RSP_Opcode_SBV ( void ) {
	DWORD Address = ((RSP_GPR[RSPOpC.OP.LSV.base].UW + (DWORD)(RSPOpC.OP.LSV.offset)) &0xFFF);
	RSP_SBV_DMEM( Address, RSPOpC.OP.LSV.vt, RSPOpC.OP.LSV.element);
}

void RSP_Opcode_SSV ( void ) {
	DWORD Address = ((RSP_GPR[RSPOpC.OP.LSV.base].UW + (DWORD)(RSPOpC.OP.LSV.offset << 1)) &0xFFF);
	RSP_SSV_DMEM( Address, RSPOpC.OP.LSV.vt, RSPOpC.OP.LSV.element);
}

void RSP_Opcode_SLV ( void ) {
	DWORD Address = ((RSP_GPR[RSPOpC.OP.LSV.base].UW + (DWORD)(RSPOpC.OP.LSV.offset << 2)) &0xFFF);
	RSP_SLV_DMEM( Address, RSPOpC.OP.LSV.vt, RSPOpC.OP.LSV.element);
}

void RSP_Opcode_SDV ( void ) {
	DWORD Address = ((RSP_GPR[RSPOpC.OP.LSV.base].UW + (DWORD)(RSPOpC.OP.LSV.offset << 3)) &0xFFF);
	RSP_SDV_DMEM( Address, RSPOpC.OP.LSV.vt, RSPOpC.OP.LSV.element);
}

void RSP_Opcode_SQV ( void ) {
	DWORD Address = ((RSP_GPR[RSPOpC.OP.LSV.base].UW + (DWORD)(RSPOpC.OP.LSV.offset << 4)) &0xFFF);
	RSP_SQV_DMEM( Address, RSPOpC.OP.LSV.vt, RSPOpC.OP.LSV.element);
}

void RSP_Opcode_SRV ( void ) {
	DWORD Address = ((RSP_GPR[RSPOpC.OP.LSV.base].UW + (DWORD)(RSPOpC.OP.LSV.offset << 4)) &0xFFF);
	RSP_SRV_DMEM( Address, RSPOpC.OP.LSV.vt, RSPOpC.OP.LSV.element);
}

void RSP_Opcode_SPV ( void ) {
	DWORD Address = ((RSP_GPR[RSPOpC.OP.LSV.base].UW + (DWORD)(RSPOpC.OP.LSV.offset << 3)) &0xFFF);
	RSP_SPV_DMEM( Address, RSPOpC.OP.LSV.vt, RSPOpC.OP.LSV.element);
}

void RSP_Opcode_SUV ( void ) {
	DWORD Address = ((RSP_GPR[RSPOpC.OP.LSV.base].UW + (DWORD)(RSPOpC.OP.LSV.offset << 3)) &0xFFF);
	RSP_SUV_DMEM( Address, RSPOpC.OP.LSV.vt, RSPOpC.OP.LSV.element);
}

void RSP_Opcode_SHV ( void ) {
	DWORD Address = ((RSP_GPR[RSPOpC.OP.LSV.base].UW + (DWORD)(RSPOpC.OP.LSV.offset << 4)) &0xFFF);
	RSP_SHV_DMEM( Address, RSPOpC.OP.LSV.vt, RSPOpC.OP.LSV.element);
}

void RSP_Opcode_SFV ( void ) {
	DWORD Address = ((RSP_GPR[RSPOpC.OP.LSV.base].UW + (DWORD)(RSPOpC.OP.LSV.offset << 4)) &0xFFF);
	RSP_SFV_DMEM( Address, RSPOpC.OP.LSV.vt, RSPOpC.OP.LSV.element);
}

void RSP_Opcode_STV ( void ) {
	DWORD Address = ((RSP_GPR[RSPOpC.OP.LSV.base].UW + (DWORD)(RSPOpC.OP.LSV.offset << 4)) &0xFFF);
	RSP_STV_DMEM( Address, RSPOpC.OP.LSV.vt, RSPOpC.OP.LSV.element);
}

void RSP_Opcode_SWV ( void ) {
	DWORD Address = ((RSP_GPR[RSPOpC.OP.LSV.base].UW + (DWORD)(RSPOpC.OP.LSV.offset << 4)) &0xFFF);
	RSP_SWV_DMEM( Address, RSPOpC.OP.LSV.vt, RSPOpC.OP.LSV.element);
}

/************************** Other functions **************************/

void rsp_UnknownOpcode (void) {
	char Message[200];
	int response;

	if (InRSPCommandsWindow) {
		SetRSPCommandViewto( SP_PC_REG );
		DisplayError("Unhandled RSP Opcode\n%s\n\nStoping Emulation!", RSPOpcodeName(RSPOpC.OP.Hex,SP_PC_REG));
	} else {
		sprintf(Message,"Unhandled RSP Opcode\n%s\n\nStoping Emulation!\n\nDo you wish to enter the debugger ?", 
			RSPOpcodeName(RSPOpC.OP.Hex,SP_PC_REG));
		response = MessageBox(NULL,Message,"Error", MB_YESNO | MB_ICONERROR );	
		if (response == IDYES) {		
			Enter_RSP_Commands_Window ();
		}
	}
	ExitThread(0);
}
