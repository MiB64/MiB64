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
#include "RSP_breakpoint.h"
#include "rsp_Cpu.h"
#include "RSP Interpreter Ops.h"
#include "RSP Interpreter CPU.h"
#include "rsp_registers.h"
#include "RSP Command.h"
#include "rsp_config.h"
#include "rsp_registers.h"
#include "rsp_memory.h"
#include "RSP_OpCode.h"
/*#include "log.h"*/
#include "../Main.h"

DWORD RSP_NextInstruction, RSP_JumpTo;

void BuildInterpreterRspCPU(void) {
	RSP_Opcode[ 0] = (void*)RSP_Opcode_SPECIAL;
	RSP_Opcode[ 1] = (void*)RSP_Opcode_REGIMM;
	RSP_Opcode[ 2] = (void*)RSP_Opcode_J;
	RSP_Opcode[ 3] = (void*)RSP_Opcode_JAL;
	RSP_Opcode[ 4] = (void*)RSP_Opcode_BEQ;
	RSP_Opcode[ 5] = (void*)RSP_Opcode_BNE;
	RSP_Opcode[ 6] = (void*)RSP_Opcode_BLEZ;
	RSP_Opcode[ 7] = (void*)RSP_Opcode_BGTZ;
	RSP_Opcode[ 8] = (void*)RSP_Opcode_ADDI;
	RSP_Opcode[ 9] = (void*)RSP_Opcode_ADDIU;
	RSP_Opcode[10] = (void*)RSP_Opcode_SLTI;
	RSP_Opcode[11] = (void*)RSP_Opcode_SLTIU;
	RSP_Opcode[12] = (void*)RSP_Opcode_ANDI;
	RSP_Opcode[13] = (void*)RSP_Opcode_ORI;
	RSP_Opcode[14] = (void*)RSP_Opcode_XORI;
	RSP_Opcode[15] = (void*)RSP_Opcode_LUI;
	RSP_Opcode[16] = (void*)RSP_Opcode_COP0;
	RSP_Opcode[17] = (void*)rsp_UnknownOpcode;
	RSP_Opcode[18] = (void*)RSP_Opcode_COP2;
	RSP_Opcode[19] = (void*)rsp_UnknownOpcode;
	RSP_Opcode[20] = (void*)rsp_UnknownOpcode;
	RSP_Opcode[21] = (void*)rsp_UnknownOpcode;
	RSP_Opcode[22] = (void*)rsp_UnknownOpcode;
	RSP_Opcode[23] = (void*)rsp_UnknownOpcode;
	RSP_Opcode[24] = (void*)rsp_UnknownOpcode;
	RSP_Opcode[25] = (void*)rsp_UnknownOpcode;
	RSP_Opcode[26] = (void*)rsp_UnknownOpcode;
	RSP_Opcode[27] = (void*)rsp_UnknownOpcode;
	RSP_Opcode[28] = (void*)rsp_UnknownOpcode;
	RSP_Opcode[29] = (void*)rsp_UnknownOpcode;
	RSP_Opcode[30] = (void*)rsp_UnknownOpcode;
	RSP_Opcode[31] = (void*)rsp_UnknownOpcode;
	RSP_Opcode[32] = (void*)RSP_Opcode_LB;
	RSP_Opcode[33] = (void*)RSP_Opcode_LH;
	RSP_Opcode[34] = (void*)rsp_UnknownOpcode;
	RSP_Opcode[35] = (void*)RSP_Opcode_LW;
	RSP_Opcode[36] = (void*)RSP_Opcode_LBU;
	RSP_Opcode[37] = (void*)RSP_Opcode_LHU;
	RSP_Opcode[38] = (void*)rsp_UnknownOpcode;
	RSP_Opcode[39] = (void*)RSP_Opcode_LWU;
	RSP_Opcode[40] = (void*)RSP_Opcode_SB;
	RSP_Opcode[41] = (void*)RSP_Opcode_SH;
	RSP_Opcode[42] = (void*)rsp_UnknownOpcode;
	RSP_Opcode[43] = (void*)RSP_Opcode_SW;
	RSP_Opcode[44] = (void*)rsp_UnknownOpcode;
	RSP_Opcode[45] = (void*)rsp_UnknownOpcode;
	RSP_Opcode[46] = (void*)rsp_UnknownOpcode;
	RSP_Opcode[47] = (void*)rsp_UnknownOpcode;
	RSP_Opcode[48] = (void*)rsp_UnknownOpcode;
	RSP_Opcode[49] = (void*)rsp_UnknownOpcode;
	RSP_Opcode[50] = (void*)RSP_Opcode_LC2;
	RSP_Opcode[51] = (void*)rsp_UnknownOpcode;
	RSP_Opcode[52] = (void*)rsp_UnknownOpcode;
	RSP_Opcode[53] = (void*)rsp_UnknownOpcode;
	RSP_Opcode[54] = (void*)rsp_UnknownOpcode;
	RSP_Opcode[55] = (void*)rsp_UnknownOpcode;
	RSP_Opcode[56] = (void*)rsp_UnknownOpcode;
	RSP_Opcode[57] = (void*)rsp_UnknownOpcode;
	RSP_Opcode[58] = (void*)RSP_Opcode_SC2;
	RSP_Opcode[59] = (void*)rsp_UnknownOpcode;
	RSP_Opcode[60] = (void*)rsp_UnknownOpcode;
	RSP_Opcode[61] = (void*)rsp_UnknownOpcode;
	RSP_Opcode[62] = (void*)rsp_UnknownOpcode;
	RSP_Opcode[63] = (void*)rsp_UnknownOpcode;

	RSP_Special[ 0] = (void*)RSP_Special_SLL;
	RSP_Special[ 1] = (void*)rsp_UnknownOpcode;
	RSP_Special[ 2] = (void*)RSP_Special_SRL;
	RSP_Special[ 3] = (void*)RSP_Special_SRA;
	RSP_Special[ 4] = (void*)RSP_Special_SLLV;
	RSP_Special[ 5] = (void*)rsp_UnknownOpcode;
	RSP_Special[ 6] = (void*)RSP_Special_SRLV;
	RSP_Special[ 7] = (void*)RSP_Special_SRAV;
	RSP_Special[ 8] = (void*)RSP_Special_JR;
	RSP_Special[ 9] = (void*)RSP_Special_JALR;
	RSP_Special[10] = (void*)rsp_UnknownOpcode;
	RSP_Special[11] = (void*)rsp_UnknownOpcode;
	RSP_Special[12] = (void*)rsp_UnknownOpcode;
	RSP_Special[13] = (void*)RSP_Special_BREAK;
	RSP_Special[14] = (void*)rsp_UnknownOpcode;
	RSP_Special[15] = (void*)rsp_UnknownOpcode;
	RSP_Special[16] = (void*)rsp_UnknownOpcode;
	RSP_Special[17] = (void*)rsp_UnknownOpcode;
	RSP_Special[18] = (void*)rsp_UnknownOpcode;
	RSP_Special[19] = (void*)rsp_UnknownOpcode;
	RSP_Special[20] = (void*)rsp_UnknownOpcode;
	RSP_Special[21] = (void*)rsp_UnknownOpcode;
	RSP_Special[22] = (void*)rsp_UnknownOpcode;
	RSP_Special[23] = (void*)rsp_UnknownOpcode;
	RSP_Special[24] = (void*)rsp_UnknownOpcode;
	RSP_Special[25] = (void*)rsp_UnknownOpcode;
	RSP_Special[26] = (void*)rsp_UnknownOpcode;
	RSP_Special[27] = (void*)rsp_UnknownOpcode;
	RSP_Special[28] = (void*)rsp_UnknownOpcode;
	RSP_Special[29] = (void*)rsp_UnknownOpcode;
	RSP_Special[30] = (void*)rsp_UnknownOpcode;
	RSP_Special[31] = (void*)rsp_UnknownOpcode;
	RSP_Special[32] = (void*)RSP_Special_ADD;
	RSP_Special[33] = (void*)RSP_Special_ADDU;
	RSP_Special[34] = (void*)RSP_Special_SUB;
	RSP_Special[35] = (void*)RSP_Special_SUBU;
	RSP_Special[36] = (void*)RSP_Special_AND;
	RSP_Special[37] = (void*)RSP_Special_OR;
	RSP_Special[38] = (void*)RSP_Special_XOR;
	RSP_Special[39] = (void*)RSP_Special_NOR;
	RSP_Special[40] = (void*)rsp_UnknownOpcode;
	RSP_Special[41] = (void*)rsp_UnknownOpcode;
	RSP_Special[42] = (void*)RSP_Special_SLT;
	RSP_Special[43] = (void*)RSP_Special_SLTU;
	RSP_Special[44] = (void*)rsp_UnknownOpcode;
	RSP_Special[45] = (void*)rsp_UnknownOpcode;
	RSP_Special[46] = (void*)rsp_UnknownOpcode;
	RSP_Special[47] = (void*)rsp_UnknownOpcode;
	RSP_Special[48] = (void*)rsp_UnknownOpcode;
	RSP_Special[49] = (void*)rsp_UnknownOpcode;
	RSP_Special[50] = (void*)rsp_UnknownOpcode;
	RSP_Special[51] = (void*)rsp_UnknownOpcode;
	RSP_Special[52] = (void*)rsp_UnknownOpcode;
	RSP_Special[53] = (void*)rsp_UnknownOpcode;
	RSP_Special[54] = (void*)rsp_UnknownOpcode;
	RSP_Special[55] = (void*)rsp_UnknownOpcode;
	RSP_Special[56] = (void*)rsp_UnknownOpcode;
	RSP_Special[57] = (void*)rsp_UnknownOpcode;
	RSP_Special[58] = (void*)rsp_UnknownOpcode;
	RSP_Special[59] = (void*)rsp_UnknownOpcode;
	RSP_Special[60] = (void*)rsp_UnknownOpcode;
	RSP_Special[61] = (void*)rsp_UnknownOpcode;
	RSP_Special[62] = (void*)rsp_UnknownOpcode;
	RSP_Special[63] = (void*)rsp_UnknownOpcode;

	RSP_RegImm[ 0] = (void*)RSP_Opcode_BLTZ;
	RSP_RegImm[ 1] = (void*)RSP_Opcode_BGEZ;
	RSP_RegImm[ 2] = (void*)rsp_UnknownOpcode;
	RSP_RegImm[ 3] = (void*)rsp_UnknownOpcode;
	RSP_RegImm[ 4] = (void*)rsp_UnknownOpcode;
	RSP_RegImm[ 5] = (void*)rsp_UnknownOpcode;
	RSP_RegImm[ 6] = (void*)rsp_UnknownOpcode;
	RSP_RegImm[ 7] = (void*)rsp_UnknownOpcode;
	RSP_RegImm[ 8] = (void*)rsp_UnknownOpcode;
	RSP_RegImm[ 9] = (void*)rsp_UnknownOpcode;
	RSP_RegImm[10] = (void*)rsp_UnknownOpcode;
	RSP_RegImm[11] = (void*)rsp_UnknownOpcode;
	RSP_RegImm[12] = (void*)rsp_UnknownOpcode;
	RSP_RegImm[13] = (void*)rsp_UnknownOpcode;
	RSP_RegImm[14] = (void*)rsp_UnknownOpcode;
	RSP_RegImm[15] = (void*)rsp_UnknownOpcode;
	RSP_RegImm[16] = (void*)RSP_Opcode_BLTZAL;
	RSP_RegImm[17] = (void*)RSP_Opcode_BGEZAL;
	RSP_RegImm[18] = (void*)rsp_UnknownOpcode;
	RSP_RegImm[19] = (void*)rsp_UnknownOpcode;
	RSP_RegImm[20] = (void*)rsp_UnknownOpcode;
	RSP_RegImm[21] = (void*)rsp_UnknownOpcode;
	RSP_RegImm[22] = (void*)rsp_UnknownOpcode;
	RSP_RegImm[23] = (void*)rsp_UnknownOpcode;
	RSP_RegImm[24] = (void*)rsp_UnknownOpcode;
	RSP_RegImm[25] = (void*)rsp_UnknownOpcode;
	RSP_RegImm[26] = (void*)rsp_UnknownOpcode;
	RSP_RegImm[27] = (void*)rsp_UnknownOpcode;
	RSP_RegImm[28] = (void*)rsp_UnknownOpcode;
	RSP_RegImm[29] = (void*)rsp_UnknownOpcode;
	RSP_RegImm[30] = (void*)rsp_UnknownOpcode;
	RSP_RegImm[31] = (void*)rsp_UnknownOpcode;

	RSP_Cop0[ 0] = (void*)RSP_Cop0_MF;
	RSP_Cop0[ 1] = (void*)rsp_UnknownOpcode;
	RSP_Cop0[ 2] = (void*)rsp_UnknownOpcode;
	RSP_Cop0[ 3] = (void*)rsp_UnknownOpcode;
	RSP_Cop0[ 4] = (void*)RSP_Cop0_MT;
	RSP_Cop0[ 5] = (void*)rsp_UnknownOpcode;
	RSP_Cop0[ 6] = (void*)rsp_UnknownOpcode;
	RSP_Cop0[ 7] = (void*)rsp_UnknownOpcode;
	RSP_Cop0[ 8] = (void*)rsp_UnknownOpcode;
	RSP_Cop0[ 9] = (void*)rsp_UnknownOpcode;
	RSP_Cop0[10] = (void*)rsp_UnknownOpcode;
	RSP_Cop0[11] = (void*)rsp_UnknownOpcode;
	RSP_Cop0[12] = (void*)rsp_UnknownOpcode;
	RSP_Cop0[13] = (void*)rsp_UnknownOpcode;
	RSP_Cop0[14] = (void*)rsp_UnknownOpcode;
	RSP_Cop0[15] = (void*)rsp_UnknownOpcode;
	RSP_Cop0[16] = (void*)rsp_UnknownOpcode;
	RSP_Cop0[17] = (void*)rsp_UnknownOpcode;
	RSP_Cop0[18] = (void*)rsp_UnknownOpcode;
	RSP_Cop0[19] = (void*)rsp_UnknownOpcode;
	RSP_Cop0[20] = (void*)rsp_UnknownOpcode;
	RSP_Cop0[21] = (void*)rsp_UnknownOpcode;
	RSP_Cop0[22] = (void*)rsp_UnknownOpcode;
	RSP_Cop0[23] = (void*)rsp_UnknownOpcode;
	RSP_Cop0[24] = (void*)rsp_UnknownOpcode;
	RSP_Cop0[25] = (void*)rsp_UnknownOpcode;
	RSP_Cop0[26] = (void*)rsp_UnknownOpcode;
	RSP_Cop0[27] = (void*)rsp_UnknownOpcode;
	RSP_Cop0[28] = (void*)rsp_UnknownOpcode;
	RSP_Cop0[29] = (void*)rsp_UnknownOpcode;
	RSP_Cop0[30] = (void*)rsp_UnknownOpcode;
	RSP_Cop0[31] = (void*)rsp_UnknownOpcode;
	
	RSP_Cop2[ 0] = (void*)RSP_Cop2_MF;
	RSP_Cop2[ 1] = (void*)rsp_UnknownOpcode;
	RSP_Cop2[ 2] = (void*)RSP_Cop2_CF;
	RSP_Cop2[ 3] = (void*)rsp_UnknownOpcode;
	RSP_Cop2[ 4] = (void*)RSP_Cop2_MT;
	RSP_Cop2[ 5] = (void*)rsp_UnknownOpcode;
	RSP_Cop2[ 6] = (void*)RSP_Cop2_CT;
	RSP_Cop2[ 7] = (void*)rsp_UnknownOpcode;
	RSP_Cop2[ 8] = (void*)rsp_UnknownOpcode;
	RSP_Cop2[ 9] = (void*)rsp_UnknownOpcode;
	RSP_Cop2[10] = (void*)rsp_UnknownOpcode;
	RSP_Cop2[11] = (void*)rsp_UnknownOpcode;
	RSP_Cop2[12] = (void*)rsp_UnknownOpcode;
	RSP_Cop2[13] = (void*)rsp_UnknownOpcode;
	RSP_Cop2[14] = (void*)rsp_UnknownOpcode;
	RSP_Cop2[15] = (void*)rsp_UnknownOpcode;
	RSP_Cop2[16] = (void*)RSP_COP2_VECTOR;
	RSP_Cop2[17] = (void*)RSP_COP2_VECTOR;
	RSP_Cop2[18] = (void*)RSP_COP2_VECTOR;
	RSP_Cop2[19] = (void*)RSP_COP2_VECTOR;
	RSP_Cop2[20] = (void*)RSP_COP2_VECTOR;
	RSP_Cop2[21] = (void*)RSP_COP2_VECTOR;
	RSP_Cop2[22] = (void*)RSP_COP2_VECTOR;
	RSP_Cop2[23] = (void*)RSP_COP2_VECTOR;
	RSP_Cop2[24] = (void*)RSP_COP2_VECTOR;
	RSP_Cop2[25] = (void*)RSP_COP2_VECTOR;
	RSP_Cop2[26] = (void*)RSP_COP2_VECTOR;
	RSP_Cop2[27] = (void*)RSP_COP2_VECTOR;
	RSP_Cop2[28] = (void*)RSP_COP2_VECTOR;
	RSP_Cop2[29] = (void*)RSP_COP2_VECTOR;
	RSP_Cop2[30] = (void*)RSP_COP2_VECTOR;
	RSP_Cop2[31] = (void*)RSP_COP2_VECTOR;

	RSP_Vector[ 0] = (void*)RSP_Vector_VMULF;
	RSP_Vector[ 1] = (void*)RSP_Vector_VMULU;
	RSP_Vector[ 2] = (void*)RSP_Vector_VRNDP;
	RSP_Vector[ 3] = (void*)RSP_Vector_VMULQ;
	RSP_Vector[ 4] = (void*)RSP_Vector_VMUDL;
	RSP_Vector[ 5] = (void*)RSP_Vector_VMUDM;
	RSP_Vector[ 6] = (void*)RSP_Vector_VMUDN;
	RSP_Vector[ 7] = (void*)RSP_Vector_VMUDH;
	RSP_Vector[ 8] = (void*)RSP_Vector_VMACF;
	RSP_Vector[ 9] = (void*)RSP_Vector_VMACU;
	RSP_Vector[10] = (void*)RSP_Vector_VRNDN;
	RSP_Vector[11] = (void*)RSP_Vector_VMACQ;
	RSP_Vector[12] = (void*)RSP_Vector_VMADL;
	RSP_Vector[13] = (void*)RSP_Vector_VMADM;
	RSP_Vector[14] = (void*)RSP_Vector_VMADN;
	RSP_Vector[15] = (void*)RSP_Vector_VMADH;
	RSP_Vector[16] = (void*)RSP_Vector_VADD;
	RSP_Vector[17] = (void*)RSP_Vector_VSUB;
	RSP_Vector[18] = (void*)RSP_Vector_VSUT;
	RSP_Vector[19] = (void*)RSP_Vector_VABS;
	RSP_Vector[20] = (void*)RSP_Vector_VADDC;
	RSP_Vector[21] = (void*)RSP_Vector_VSUBC;
	RSP_Vector[22] = (void*)RSP_Vector_VADDB;
	RSP_Vector[23] = (void*)RSP_Vector_VSUBB;
	RSP_Vector[24] = (void*)RSP_Vector_VACCB;
	RSP_Vector[25] = (void*)RSP_Vector_VSUCB;
	RSP_Vector[26] = (void*)RSP_Vector_VSAD;
	RSP_Vector[27] = (void*)RSP_Vector_VSAC;
	RSP_Vector[28] = (void*)RSP_Vector_VSUM;
	RSP_Vector[29] = (void*)RSP_Vector_VSAR;
	RSP_Vector[30] = (void*)RSP_Vector_V30;
	RSP_Vector[31] = (void*)RSP_Vector_V31;
	RSP_Vector[32] = (void*)RSP_Vector_VLT;
	RSP_Vector[33] = (void*)RSP_Vector_VEQ;
	RSP_Vector[34] = (void*)RSP_Vector_VNE;
	RSP_Vector[35] = (void*)RSP_Vector_VGE;
	RSP_Vector[36] = (void*)RSP_Vector_VCL;
	RSP_Vector[37] = (void*)RSP_Vector_VCH;
	RSP_Vector[38] = (void*)RSP_Vector_VCR;
	RSP_Vector[39] = (void*)RSP_Vector_VMRG;
	RSP_Vector[40] = (void*)RSP_Vector_VAND;
	RSP_Vector[41] = (void*)RSP_Vector_VNAND;
	RSP_Vector[42] = (void*)RSP_Vector_VOR;
	RSP_Vector[43] = (void*)RSP_Vector_VNOR;
	RSP_Vector[44] = (void*)RSP_Vector_VXOR;
	RSP_Vector[45] = (void*)RSP_Vector_VNXOR;
	RSP_Vector[46] = (void*)RSP_Vector_V46;
	RSP_Vector[47] = (void*)RSP_Vector_V47;
	RSP_Vector[48] = (void*)RSP_Vector_VRCP;
	RSP_Vector[49] = (void*)RSP_Vector_VRCPL;
	RSP_Vector[50] = (void*)RSP_Vector_VRCPH;
	RSP_Vector[51] = (void*)RSP_Vector_VMOV;
	RSP_Vector[52] = (void*)RSP_Vector_VRSQ;
	RSP_Vector[53] = (void*)RSP_Vector_VRSQL;
	RSP_Vector[54] = (void*)RSP_Vector_VRSQH;
	RSP_Vector[55] = (void*)RSP_Vector_VNOOP;
	RSP_Vector[56] = (void*)RSP_Vector_VEXTT;
	RSP_Vector[57] = (void*)RSP_Vector_VEXTQ;
	RSP_Vector[58] = (void*)RSP_Vector_VEXTN;
	RSP_Vector[59] = (void*)RSP_Vector_V59;
	RSP_Vector[60] = (void*)RSP_Vector_VINST;
	RSP_Vector[61] = (void*)RSP_Vector_VINSQ;
	RSP_Vector[62] = (void*)RSP_Vector_VINSN;
	RSP_Vector[63] = (void*)RSP_Vector_VNULL;

	RSP_Lc2[ 0] = (void*)RSP_Opcode_LBV;
	RSP_Lc2[ 1] = (void*)RSP_Opcode_LSV;
	RSP_Lc2[ 2] = (void*)RSP_Opcode_LLV;
	RSP_Lc2[ 3] = (void*)RSP_Opcode_LDV;
	RSP_Lc2[ 4] = (void*)RSP_Opcode_LQV;
	RSP_Lc2[ 5] = (void*)RSP_Opcode_LRV;
	RSP_Lc2[ 6] = (void*)RSP_Opcode_LPV;
	RSP_Lc2[ 7] = (void*)RSP_Opcode_LUV;
	RSP_Lc2[ 8] = (void*)RSP_Opcode_LHV;
	RSP_Lc2[ 9] = (void*)RSP_Opcode_LFV;
	RSP_Lc2[10] = (void*)RSP_Opcode_LWV;
	RSP_Lc2[11] = (void*)RSP_Opcode_LTV;
	RSP_Lc2[12] = (void*)rsp_UnknownOpcode;
	RSP_Lc2[13] = (void*)rsp_UnknownOpcode;
	RSP_Lc2[14] = (void*)rsp_UnknownOpcode;
	RSP_Lc2[15] = (void*)rsp_UnknownOpcode;
	RSP_Lc2[16] = (void*)rsp_UnknownOpcode;
	RSP_Lc2[17] = (void*)rsp_UnknownOpcode;
	RSP_Lc2[18] = (void*)rsp_UnknownOpcode;
	RSP_Lc2[19] = (void*)rsp_UnknownOpcode;
	RSP_Lc2[20] = (void*)rsp_UnknownOpcode;
	RSP_Lc2[21] = (void*)rsp_UnknownOpcode;
	RSP_Lc2[22] = (void*)rsp_UnknownOpcode;
	RSP_Lc2[23] = (void*)rsp_UnknownOpcode;
	RSP_Lc2[24] = (void*)rsp_UnknownOpcode;
	RSP_Lc2[25] = (void*)rsp_UnknownOpcode;
	RSP_Lc2[26] = (void*)rsp_UnknownOpcode;
	RSP_Lc2[27] = (void*)rsp_UnknownOpcode;
	RSP_Lc2[28] = (void*)rsp_UnknownOpcode;
	RSP_Lc2[29] = (void*)rsp_UnknownOpcode;
	RSP_Lc2[30] = (void*)rsp_UnknownOpcode;
	RSP_Lc2[31] = (void*)rsp_UnknownOpcode;

	RSP_Sc2[ 0] = (void*)RSP_Opcode_SBV;
	RSP_Sc2[ 1] = (void*)RSP_Opcode_SSV;
	RSP_Sc2[ 2] = (void*)RSP_Opcode_SLV;
	RSP_Sc2[ 3] = (void*)RSP_Opcode_SDV;
	RSP_Sc2[ 4] = (void*)RSP_Opcode_SQV;
	RSP_Sc2[ 5] = (void*)RSP_Opcode_SRV;
	RSP_Sc2[ 6] = (void*)RSP_Opcode_SPV;
	RSP_Sc2[ 7] = (void*)RSP_Opcode_SUV;
	RSP_Sc2[ 8] = (void*)RSP_Opcode_SHV;
	RSP_Sc2[ 9] = (void*)RSP_Opcode_SFV;
	RSP_Sc2[10] = (void*)RSP_Opcode_SWV;
	RSP_Sc2[11] = (void*)RSP_Opcode_STV;
	RSP_Sc2[12] = (void*)rsp_UnknownOpcode;
	RSP_Sc2[13] = (void*)rsp_UnknownOpcode;
	RSP_Sc2[14] = (void*)rsp_UnknownOpcode;
	RSP_Sc2[15] = (void*)rsp_UnknownOpcode;
	RSP_Sc2[16] = (void*)rsp_UnknownOpcode;
	RSP_Sc2[17] = (void*)rsp_UnknownOpcode;
	RSP_Sc2[18] = (void*)rsp_UnknownOpcode;
	RSP_Sc2[19] = (void*)rsp_UnknownOpcode;
	RSP_Sc2[20] = (void*)rsp_UnknownOpcode;
	RSP_Sc2[21] = (void*)rsp_UnknownOpcode;
	RSP_Sc2[22] = (void*)rsp_UnknownOpcode;
	RSP_Sc2[23] = (void*)rsp_UnknownOpcode;
	RSP_Sc2[24] = (void*)rsp_UnknownOpcode;
	RSP_Sc2[25] = (void*)rsp_UnknownOpcode;
	RSP_Sc2[26] = (void*)rsp_UnknownOpcode;
	RSP_Sc2[27] = (void*)rsp_UnknownOpcode;
	RSP_Sc2[28] = (void*)rsp_UnknownOpcode;
	RSP_Sc2[29] = (void*)rsp_UnknownOpcode;
	RSP_Sc2[30] = (void*)rsp_UnknownOpcode;
	RSP_Sc2[31] = (void*)rsp_UnknownOpcode;

	RSP_NextInstruction = NORMAL;
}

DWORD RunInterpreterRspCPU(DWORD Cycles) {
	RSP_Running = TRUE;
	Enable_RSP_Commands_Window();
	DWORD remainingCycles = Cycles;

	while (RSP_Running) {
		if (NoOfRspBpoints != 0) {
			if (CheckForRSPBPoint(SP_PC_REG)) {
				if (InRSPCommandsWindow) {
					Enter_RSP_Commands_Window();
					if (Stepping_RspCommands) {
						DisplayError ( "Encounted a RSP Breakpoint" );
					} else {
						DisplayError ( "Encounted a RSP Breakpoint\n\nNow Stepping" );
						SetRSPCommandViewto( SP_PC_REG );
						SetRSPCommandToStepping();
					}
				} else {
					DisplayError ( "Encounted a RSP Breakpoint\n\nEntering Command Window" );
					Enter_RSP_Commands_Window();
				}
			}
		}
		
		if (Stepping_RspCommands) {
			WaitingForRspStep = TRUE;
			SetRSPCommandViewto( SP_PC_REG );
			UpdateRSPRegistersScreen();
			while ( WaitingForRspStep == TRUE ){ 
				Sleep(20);						
				if (!Stepping_RspCommands) {
					WaitingForRspStep = FALSE;
				}
			}
		}

		RSP_LW_IMEM(SP_PC_REG, &RSPOpC.OP.Hex);
		((void (*)()) RSP_Opcode[ RSPOpC.OP.I.op ])();

		switch (RSP_NextInstruction) {
		case NORMAL: 
			SP_PC_REG = (SP_PC_REG + 4) & 0xFFC; 
			break;
		case DELAY_SLOT:
			RSP_NextInstruction = JUMP;
			SP_PC_REG = (SP_PC_REG + 4) & 0xFFC; 
			break;
		case JUMP:
			RSP_NextInstruction = NORMAL;
			SP_PC_REG = RSP_JumpTo;
			break;
		}

		if ((--remainingCycles) == 0) {
			RSP_Running = FALSE;
		}
	}

	return Cycles - remainingCycles;
}

