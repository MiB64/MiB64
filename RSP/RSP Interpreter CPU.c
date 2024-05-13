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
	RSP_Opcode[ 0] = /*RSP_Opcode_SPECIAL*/(void*)rsp_UnknownOpcode;
	RSP_Opcode[ 1] = /*RSP_Opcode_REGIMM*/(void*)rsp_UnknownOpcode;
	RSP_Opcode[ 2] = /*RSP_Opcode_J*/(void*)rsp_UnknownOpcode;
	RSP_Opcode[ 3] = /*RSP_Opcode_JAL*/(void*)rsp_UnknownOpcode;
	RSP_Opcode[ 4] = /*RSP_Opcode_BEQ*/(void*)rsp_UnknownOpcode;
	RSP_Opcode[ 5] = /*RSP_Opcode_BNE*/(void*)rsp_UnknownOpcode;
	RSP_Opcode[ 6] = /*RSP_Opcode_BLEZ*/(void*)rsp_UnknownOpcode;
	RSP_Opcode[ 7] = /*RSP_Opcode_BGTZ*/(void*)rsp_UnknownOpcode;
	RSP_Opcode[ 8] = /*RSP_Opcode_ADDI*/(void*)rsp_UnknownOpcode;
	RSP_Opcode[ 9] = /*RSP_Opcode_ADDIU*/(void*)rsp_UnknownOpcode;
	RSP_Opcode[10] = /*RSP_Opcode_SLTI*/(void*)rsp_UnknownOpcode;
	RSP_Opcode[11] = /*RSP_Opcode_SLTIU*/(void*)rsp_UnknownOpcode;
	RSP_Opcode[12] = /*RSP_Opcode_ANDI*/(void*)rsp_UnknownOpcode;
	RSP_Opcode[13] = (void*)RSP_Opcode_ORI; // TOCHECK
	RSP_Opcode[14] = /*RSP_Opcode_XORI*/(void*)rsp_UnknownOpcode;
	RSP_Opcode[15] = /*RSP_Opcode_LUI*/(void*)rsp_UnknownOpcode;
	RSP_Opcode[16] = /*RSP_Opcode_COP0*/(void*)rsp_UnknownOpcode;
	RSP_Opcode[17] = (void*)rsp_UnknownOpcode;
	RSP_Opcode[18] = /*RSP_Opcode_COP2*/(void*)rsp_UnknownOpcode;
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
	RSP_Opcode[32] = /*RSP_Opcode_LB*/(void*)rsp_UnknownOpcode;
	RSP_Opcode[33] = /*RSP_Opcode_LH*/(void*)rsp_UnknownOpcode;
	RSP_Opcode[34] = (void*)rsp_UnknownOpcode;
	RSP_Opcode[35] = /*RSP_Opcode_LW*/(void*)rsp_UnknownOpcode;
	RSP_Opcode[36] = /*RSP_Opcode_LBU*/(void*)rsp_UnknownOpcode;
	RSP_Opcode[37] = /*RSP_Opcode_LHU*/(void*)rsp_UnknownOpcode;
	RSP_Opcode[38] = (void*)rsp_UnknownOpcode;
	RSP_Opcode[39] = (void*)rsp_UnknownOpcode;
	RSP_Opcode[40] = /*RSP_Opcode_SB*/(void*)rsp_UnknownOpcode;
	RSP_Opcode[41] = /*RSP_Opcode_SH*/(void*)rsp_UnknownOpcode;
	RSP_Opcode[42] = (void*)rsp_UnknownOpcode;
	RSP_Opcode[43] = /*RSP_Opcode_SW*/(void*)rsp_UnknownOpcode;
	RSP_Opcode[44] = (void*)rsp_UnknownOpcode;
	RSP_Opcode[45] = (void*)rsp_UnknownOpcode;
	RSP_Opcode[46] = (void*)rsp_UnknownOpcode;
	RSP_Opcode[47] = (void*)rsp_UnknownOpcode;
	RSP_Opcode[48] = (void*)rsp_UnknownOpcode;
	RSP_Opcode[49] = (void*)rsp_UnknownOpcode;
	RSP_Opcode[50] = /*RSP_Opcode_LC2*/(void*)rsp_UnknownOpcode;
	RSP_Opcode[51] = (void*)rsp_UnknownOpcode;
	RSP_Opcode[52] = (void*)rsp_UnknownOpcode;
	RSP_Opcode[53] = (void*)rsp_UnknownOpcode;
	RSP_Opcode[54] = (void*)rsp_UnknownOpcode;
	RSP_Opcode[55] = (void*)rsp_UnknownOpcode;
	RSP_Opcode[56] = (void*)rsp_UnknownOpcode;
	RSP_Opcode[57] = (void*)rsp_UnknownOpcode;
	RSP_Opcode[58] = /*RSP_Opcode_SC2*/(void*)rsp_UnknownOpcode;
	RSP_Opcode[59] = (void*)rsp_UnknownOpcode;
	RSP_Opcode[60] = (void*)rsp_UnknownOpcode;
	RSP_Opcode[61] = (void*)rsp_UnknownOpcode;
	RSP_Opcode[62] = (void*)rsp_UnknownOpcode;
	RSP_Opcode[63] = (void*)rsp_UnknownOpcode;

//	RSP_Special[ 0] = /*RSP_Special_SLL*/rsp_UnknownOpcode;
//	RSP_Special[ 1] = rsp_UnknownOpcode;
//	RSP_Special[ 2] = /*RSP_Special_SRL*/rsp_UnknownOpcode;
//	RSP_Special[ 3] = /*RSP_Special_SRA*/rsp_UnknownOpcode;
//	RSP_Special[ 4] = /*RSP_Special_SLLV*/rsp_UnknownOpcode;
//	RSP_Special[ 5] = rsp_UnknownOpcode;
//	RSP_Special[ 6] = /*RSP_Special_SRLV*/rsp_UnknownOpcode;
//	RSP_Special[ 7] = /*RSP_Special_SRAV*/rsp_UnknownOpcode;
//	RSP_Special[ 8] = /*RSP_Special_JR*/rsp_UnknownOpcode;
//	RSP_Special[ 9] = /*RSP_Special_JALR*/rsp_UnknownOpcode;
/*	RSP_Special[10] = rsp_UnknownOpcode;
	RSP_Special[11] = rsp_UnknownOpcode;
	RSP_Special[12] = rsp_UnknownOpcode;*/
//	RSP_Special[13] = /*RSP_Special_BREAK*/rsp_UnknownOpcode;
/*	RSP_Special[14] = rsp_UnknownOpcode;
	RSP_Special[15] = rsp_UnknownOpcode;
	RSP_Special[16] = rsp_UnknownOpcode;
	RSP_Special[17] = rsp_UnknownOpcode;
	RSP_Special[18] = rsp_UnknownOpcode;
	RSP_Special[19] = rsp_UnknownOpcode;
	RSP_Special[20] = rsp_UnknownOpcode;
	RSP_Special[21] = rsp_UnknownOpcode;
	RSP_Special[22] = rsp_UnknownOpcode;
	RSP_Special[23] = rsp_UnknownOpcode;
	RSP_Special[24] = rsp_UnknownOpcode;
	RSP_Special[25] = rsp_UnknownOpcode;
	RSP_Special[26] = rsp_UnknownOpcode;
	RSP_Special[27] = rsp_UnknownOpcode;
	RSP_Special[28] = rsp_UnknownOpcode;
	RSP_Special[29] = rsp_UnknownOpcode;
	RSP_Special[30] = rsp_UnknownOpcode;
	RSP_Special[31] = rsp_UnknownOpcode;*/
//	RSP_Special[32] = /*RSP_Special_ADD*/rsp_UnknownOpcode;
//	RSP_Special[33] = /*RSP_Special_ADDU*/rsp_UnknownOpcode;
//	RSP_Special[34] = /*RSP_Special_SUB*/rsp_UnknownOpcode;
//	RSP_Special[35] = /*RSP_Special_SUBU*/rsp_UnknownOpcode;
//	RSP_Special[36] = /*RSP_Special_AND*/rsp_UnknownOpcode;
//	RSP_Special[37] = /*RSP_Special_OR*/rsp_UnknownOpcode;
//	RSP_Special[38] = /*RSP_Special_XOR*/rsp_UnknownOpcode;
//	RSP_Special[39] = /*RSP_Special_NOR*/rsp_UnknownOpcode;
/*	RSP_Special[40] = rsp_UnknownOpcode;
	RSP_Special[41] = rsp_UnknownOpcode;*/
//	RSP_Special[42] = /*RSP_Special_SLT*/rsp_UnknownOpcode;
//	RSP_Special[43] = /*RSP_Special_SLTU*/rsp_UnknownOpcode;
/*	RSP_Special[44] = rsp_UnknownOpcode;
	RSP_Special[45] = rsp_UnknownOpcode;
	RSP_Special[46] = rsp_UnknownOpcode;
	RSP_Special[47] = rsp_UnknownOpcode;
	RSP_Special[48] = rsp_UnknownOpcode;
	RSP_Special[49] = rsp_UnknownOpcode;
	RSP_Special[50] = rsp_UnknownOpcode;
	RSP_Special[51] = rsp_UnknownOpcode;
	RSP_Special[52] = rsp_UnknownOpcode;
	RSP_Special[53] = rsp_UnknownOpcode;
	RSP_Special[54] = rsp_UnknownOpcode;
	RSP_Special[55] = rsp_UnknownOpcode;
	RSP_Special[56] = rsp_UnknownOpcode;
	RSP_Special[57] = rsp_UnknownOpcode;
	RSP_Special[58] = rsp_UnknownOpcode;
	RSP_Special[59] = rsp_UnknownOpcode;
	RSP_Special[60] = rsp_UnknownOpcode;
	RSP_Special[61] = rsp_UnknownOpcode;
	RSP_Special[62] = rsp_UnknownOpcode;
	RSP_Special[63] = rsp_UnknownOpcode;*/

//	RSP_RegImm[ 0] = /*RSP_Opcode_BLTZ*/rsp_UnknownOpcode;
//	RSP_RegImm[ 1] = /*RSP_Opcode_BGEZ*/rsp_UnknownOpcode;
/*	RSP_RegImm[ 2] = rsp_UnknownOpcode;
	RSP_RegImm[ 3] = rsp_UnknownOpcode;
	RSP_RegImm[ 4] = rsp_UnknownOpcode;
	RSP_RegImm[ 5] = rsp_UnknownOpcode;
	RSP_RegImm[ 6] = rsp_UnknownOpcode;
	RSP_RegImm[ 7] = rsp_UnknownOpcode;
	RSP_RegImm[ 8] = rsp_UnknownOpcode;
	RSP_RegImm[ 9] = rsp_UnknownOpcode;
	RSP_RegImm[10] = rsp_UnknownOpcode;
	RSP_RegImm[11] = rsp_UnknownOpcode;
	RSP_RegImm[12] = rsp_UnknownOpcode;
	RSP_RegImm[13] = rsp_UnknownOpcode;
	RSP_RegImm[14] = rsp_UnknownOpcode;
	RSP_RegImm[15] = rsp_UnknownOpcode;*/
//	RSP_RegImm[16] = /*RSP_Opcode_BLTZAL*/rsp_UnknownOpcode;
//	RSP_RegImm[17] = /*RSP_Opcode_BGEZAL*/rsp_UnknownOpcode;
/*	RSP_RegImm[18] = rsp_UnknownOpcode;
	RSP_RegImm[19] = rsp_UnknownOpcode;
	RSP_RegImm[20] = rsp_UnknownOpcode;
	RSP_RegImm[21] = rsp_UnknownOpcode;
	RSP_RegImm[22] = rsp_UnknownOpcode;
	RSP_RegImm[23] = rsp_UnknownOpcode;
	RSP_RegImm[24] = rsp_UnknownOpcode;
	RSP_RegImm[25] = rsp_UnknownOpcode;
	RSP_RegImm[26] = rsp_UnknownOpcode;
	RSP_RegImm[27] = rsp_UnknownOpcode;
	RSP_RegImm[28] = rsp_UnknownOpcode;
	RSP_RegImm[29] = rsp_UnknownOpcode;
	RSP_RegImm[30] = rsp_UnknownOpcode;
	RSP_RegImm[31] = rsp_UnknownOpcode;*/

//	RSP_Cop0[ 0] = /*RSP_Cop0_MF*/rsp_UnknownOpcode;
/*	RSP_Cop0[ 1] = rsp_UnknownOpcode;
	RSP_Cop0[ 2] = rsp_UnknownOpcode;
	RSP_Cop0[ 3] = rsp_UnknownOpcode;*/
//	RSP_Cop0[ 4] = /*RSP_Cop0_MT*/rsp_UnknownOpcode;
/*	RSP_Cop0[ 5] = rsp_UnknownOpcode;
	RSP_Cop0[ 6] = rsp_UnknownOpcode;
	RSP_Cop0[ 7] = rsp_UnknownOpcode;
	RSP_Cop0[ 8] = rsp_UnknownOpcode;
	RSP_Cop0[ 9] = rsp_UnknownOpcode;
	RSP_Cop0[10] = rsp_UnknownOpcode;
	RSP_Cop0[11] = rsp_UnknownOpcode;
	RSP_Cop0[12] = rsp_UnknownOpcode;
	RSP_Cop0[13] = rsp_UnknownOpcode;
	RSP_Cop0[14] = rsp_UnknownOpcode;
	RSP_Cop0[15] = rsp_UnknownOpcode;
	RSP_Cop0[16] = rsp_UnknownOpcode;
	RSP_Cop0[17] = rsp_UnknownOpcode;
	RSP_Cop0[18] = rsp_UnknownOpcode;
	RSP_Cop0[19] = rsp_UnknownOpcode;
	RSP_Cop0[20] = rsp_UnknownOpcode;
	RSP_Cop0[21] = rsp_UnknownOpcode;
	RSP_Cop0[22] = rsp_UnknownOpcode;
	RSP_Cop0[23] = rsp_UnknownOpcode;
	RSP_Cop0[24] = rsp_UnknownOpcode;
	RSP_Cop0[25] = rsp_UnknownOpcode;
	RSP_Cop0[26] = rsp_UnknownOpcode;
	RSP_Cop0[27] = rsp_UnknownOpcode;
	RSP_Cop0[28] = rsp_UnknownOpcode;
	RSP_Cop0[29] = rsp_UnknownOpcode;
	RSP_Cop0[30] = rsp_UnknownOpcode;
	RSP_Cop0[31] = rsp_UnknownOpcode;*/
	
//	RSP_Cop2[ 0] = /*RSP_Cop2_MF*/rsp_UnknownOpcode;
//	RSP_Cop2[ 1] = rsp_UnknownOpcode;
//	RSP_Cop2[ 2] = /*RSP_Cop2_CF*/rsp_UnknownOpcode;
//	RSP_Cop2[ 3] = rsp_UnknownOpcode;
//	RSP_Cop2[ 4] = /*RSP_Cop2_MT*/rsp_UnknownOpcode;
//	RSP_Cop2[ 5] = rsp_UnknownOpcode;
//	RSP_Cop2[ 6] = /*RSP_Cop2_CT*/rsp_UnknownOpcode;
/*	RSP_Cop2[ 7] = rsp_UnknownOpcode;
	RSP_Cop2[ 8] = rsp_UnknownOpcode;
	RSP_Cop2[ 9] = rsp_UnknownOpcode;
	RSP_Cop2[10] = rsp_UnknownOpcode;
	RSP_Cop2[11] = rsp_UnknownOpcode;
	RSP_Cop2[12] = rsp_UnknownOpcode;
	RSP_Cop2[13] = rsp_UnknownOpcode;
	RSP_Cop2[14] = rsp_UnknownOpcode;
	RSP_Cop2[15] = rsp_UnknownOpcode;*/
//	RSP_Cop2[16] = /*RSP_COP2_VECTOR*/rsp_UnknownOpcode;
//	RSP_Cop2[17] = /*RSP_COP2_VECTOR*/rsp_UnknownOpcode;
//	RSP_Cop2[18] = /*RSP_COP2_VECTOR*/rsp_UnknownOpcode;
//	RSP_Cop2[19] = /*RSP_COP2_VECTOR*/rsp_UnknownOpcode;
//	RSP_Cop2[20] = /*RSP_COP2_VECTOR*/rsp_UnknownOpcode;
//	RSP_Cop2[21] = /*RSP_COP2_VECTOR*/rsp_UnknownOpcode;
//	RSP_Cop2[22] = /*RSP_COP2_VECTOR*/rsp_UnknownOpcode;
//	RSP_Cop2[23] = /*RSP_COP2_VECTOR*/rsp_UnknownOpcode;
//	RSP_Cop2[24] = /*RSP_COP2_VECTOR*/rsp_UnknownOpcode;
//	RSP_Cop2[25] = /*RSP_COP2_VECTOR*/rsp_UnknownOpcode;
//	RSP_Cop2[26] = /*RSP_COP2_VECTOR*/rsp_UnknownOpcode;
//	RSP_Cop2[27] = /*RSP_COP2_VECTOR*/rsp_UnknownOpcode;
//	RSP_Cop2[28] = /*RSP_COP2_VECTOR*/rsp_UnknownOpcode;
//	RSP_Cop2[29] = /*RSP_COP2_VECTOR*/rsp_UnknownOpcode;
//	RSP_Cop2[30] = /*RSP_COP2_VECTOR*/rsp_UnknownOpcode;
//	RSP_Cop2[31] = /*RSP_COP2_VECTOR*/rsp_UnknownOpcode;

//	RSP_Vector[ 0] = /*RSP_Vector_VMULF*/rsp_UnknownOpcode;
//	RSP_Vector[ 1] = /*RSP_Vector_VMULU*/rsp_UnknownOpcode;
/*	RSP_Vector[ 2] = rsp_UnknownOpcode;
	RSP_Vector[ 3] = rsp_UnknownOpcode;*/
//	RSP_Vector[ 4] = /*RSP_Vector_VMUDL*/rsp_UnknownOpcode;
//	RSP_Vector[ 5] = /*RSP_Vector_VMUDM*/rsp_UnknownOpcode;
//	RSP_Vector[ 6] = /*RSP_Vector_VMUDN*/rsp_UnknownOpcode;
//	RSP_Vector[ 7] = /*RSP_Vector_VMUDH*/rsp_UnknownOpcode;
//	RSP_Vector[ 8] = /*RSP_Vector_VMACF*/rsp_UnknownOpcode;
//	RSP_Vector[ 9] = /*RSP_Vector_VMACU*/rsp_UnknownOpcode;
//	RSP_Vector[10] = rsp_UnknownOpcode;
//	RSP_Vector[11] = /*RSP_Vector_VMACQ*/rsp_UnknownOpcode;
//	RSP_Vector[12] = /*RSP_Vector_VMADL*/rsp_UnknownOpcode;
//	RSP_Vector[13] = /*RSP_Vector_VMADM*/rsp_UnknownOpcode;
//	RSP_Vector[14] = /*RSP_Vector_VMADN*/rsp_UnknownOpcode;
//	RSP_Vector[15] = /*RSP_Vector_VMADH*/rsp_UnknownOpcode;
//	RSP_Vector[16] = /*RSP_Vector_VADD*/rsp_UnknownOpcode;
//	RSP_Vector[17] = /*RSP_Vector_VSUB*/rsp_UnknownOpcode;
//	RSP_Vector[18] = rsp_UnknownOpcode;
//	RSP_Vector[19] = /*RSP_Vector_VABS*/rsp_UnknownOpcode;
//	RSP_Vector[20] = /*RSP_Vector_VADDC*/rsp_UnknownOpcode;
//	RSP_Vector[21] = /*RSP_Vector_VSUBC*/rsp_UnknownOpcode;
/*	RSP_Vector[22] = rsp_UnknownOpcode;
	RSP_Vector[23] = rsp_UnknownOpcode;
	RSP_Vector[24] = rsp_UnknownOpcode;
	RSP_Vector[25] = rsp_UnknownOpcode;
	RSP_Vector[26] = rsp_UnknownOpcode;
	RSP_Vector[27] = rsp_UnknownOpcode;
	RSP_Vector[28] = rsp_UnknownOpcode;*/
//	RSP_Vector[29] = /*RSP_Vector_VSAW*/rsp_UnknownOpcode;
/*	RSP_Vector[30] = rsp_UnknownOpcode;
	RSP_Vector[31] = rsp_UnknownOpcode;*/
//	RSP_Vector[32] = /*RSP_Vector_VLT*/rsp_UnknownOpcode;
//	RSP_Vector[33] = /*RSP_Vector_VEQ*/rsp_UnknownOpcode;
//	RSP_Vector[34] = /*RSP_Vector_VNE*/rsp_UnknownOpcode;
//	RSP_Vector[35] = /*RSP_Vector_VGE*/rsp_UnknownOpcode;
//	RSP_Vector[36] = /*RSP_Vector_VCL*/rsp_UnknownOpcode;
//	RSP_Vector[37] = /*RSP_Vector_VCH*/rsp_UnknownOpcode;
//	RSP_Vector[38] = /*RSP_Vector_VCR*/rsp_UnknownOpcode;
//	RSP_Vector[39] = /*RSP_Vector_VMRG*/rsp_UnknownOpcode;
//	RSP_Vector[40] = /*RSP_Vector_VAND*/rsp_UnknownOpcode;
//	RSP_Vector[41] = /*RSP_Vector_VNAND*/rsp_UnknownOpcode;
//	RSP_Vector[42] = /*RSP_Vector_VOR*/rsp_UnknownOpcode;
//	RSP_Vector[43] = /*RSP_Vector_VNOR*/rsp_UnknownOpcode;
//	RSP_Vector[44] = /*RSP_Vector_VXOR*/rsp_UnknownOpcode;
//	RSP_Vector[45] = /*RSP_Vector_VNXOR*/rsp_UnknownOpcode;
/*	RSP_Vector[46] = rsp_UnknownOpcode;
	RSP_Vector[47] = rsp_UnknownOpcode;*/
//	RSP_Vector[48] = /*RSP_Vector_VRCP*/rsp_UnknownOpcode;
//	RSP_Vector[49] = /*RSP_Vector_VRCPL*/rsp_UnknownOpcode;
//	RSP_Vector[50] = /*RSP_Vector_VRCPH*/rsp_UnknownOpcode;
//	RSP_Vector[51] = /*RSP_Vector_VMOV*/rsp_UnknownOpcode;
//	RSP_Vector[52] = /*RSP_Vector_VRSQ*/rsp_UnknownOpcode;
//	RSP_Vector[53] = /*RSP_Vector_VRSQL*/rsp_UnknownOpcode;
//	RSP_Vector[54] = /*RSP_Vector_VRSQH*/rsp_UnknownOpcode;
//	RSP_Vector[55] = /*RSP_Vector_VNOOP*/rsp_UnknownOpcode;
/*	RSP_Vector[56] = rsp_UnknownOpcode;
	RSP_Vector[57] = rsp_UnknownOpcode;
	RSP_Vector[58] = rsp_UnknownOpcode;
	RSP_Vector[59] = rsp_UnknownOpcode;
	RSP_Vector[60] = rsp_UnknownOpcode;
	RSP_Vector[61] = rsp_UnknownOpcode;
	RSP_Vector[62] = rsp_UnknownOpcode;
	RSP_Vector[63] = rsp_UnknownOpcode;*/

//	RSP_Lc2[ 0] = /*RSP_Opcode_LBV*/rsp_UnknownOpcode;
//	RSP_Lc2[ 1] = /*RSP_Opcode_LSV*/rsp_UnknownOpcode;
//	RSP_Lc2[ 2] = /*RSP_Opcode_LLV*/rsp_UnknownOpcode;
//	RSP_Lc2[ 3] = /*RSP_Opcode_LDV*/rsp_UnknownOpcode;
//	RSP_Lc2[ 4] = /*RSP_Opcode_LQV*/rsp_UnknownOpcode;
//	RSP_Lc2[ 5] = /*RSP_Opcode_LRV*/rsp_UnknownOpcode;
//	RSP_Lc2[ 6] = /*RSP_Opcode_LPV*/rsp_UnknownOpcode;
//	RSP_Lc2[ 7] = /*RSP_Opcode_LUV*/rsp_UnknownOpcode;
//	RSP_Lc2[ 8] = /*RSP_Opcode_LHV*/rsp_UnknownOpcode;
/*	RSP_Lc2[ 9] = RSP_Opcode_LFV;
	RSP_Lc2[10] = rsp_UnknownOpcode;
	RSP_Lc2[11] = RSP_Opcode_LTV;
	RSP_Lc2[12] = rsp_UnknownOpcode;
	RSP_Lc2[13] = rsp_UnknownOpcode;
	RSP_Lc2[14] = rsp_UnknownOpcode;
	RSP_Lc2[15] = rsp_UnknownOpcode;
	RSP_Lc2[16] = rsp_UnknownOpcode;
	RSP_Lc2[17] = rsp_UnknownOpcode;
	RSP_Lc2[18] = rsp_UnknownOpcode;
	RSP_Lc2[19] = rsp_UnknownOpcode;
	RSP_Lc2[20] = rsp_UnknownOpcode;
	RSP_Lc2[21] = rsp_UnknownOpcode;
	RSP_Lc2[22] = rsp_UnknownOpcode;
	RSP_Lc2[23] = rsp_UnknownOpcode;
	RSP_Lc2[24] = rsp_UnknownOpcode;
	RSP_Lc2[25] = rsp_UnknownOpcode;
	RSP_Lc2[26] = rsp_UnknownOpcode;
	RSP_Lc2[27] = rsp_UnknownOpcode;
	RSP_Lc2[28] = rsp_UnknownOpcode;
	RSP_Lc2[29] = rsp_UnknownOpcode;
	RSP_Lc2[30] = rsp_UnknownOpcode;
	RSP_Lc2[31] = rsp_UnknownOpcode;

	RSP_Sc2[ 0] = RSP_Opcode_SBV;
	RSP_Sc2[ 1] = RSP_Opcode_SSV;
	RSP_Sc2[ 2] = RSP_Opcode_SLV;
	RSP_Sc2[ 3] = RSP_Opcode_SDV;
	RSP_Sc2[ 4] = RSP_Opcode_SQV;
	RSP_Sc2[ 5] = RSP_Opcode_SRV;
	RSP_Sc2[ 6] = RSP_Opcode_SPV;
	RSP_Sc2[ 7] = RSP_Opcode_SUV;
	RSP_Sc2[ 8] = RSP_Opcode_SHV;
	RSP_Sc2[ 9] = RSP_Opcode_SFV;
	RSP_Sc2[10] = RSP_Opcode_SWV;
	RSP_Sc2[11] = RSP_Opcode_STV;
	RSP_Sc2[12] = rsp_UnknownOpcode;
	RSP_Sc2[13] = rsp_UnknownOpcode;
	RSP_Sc2[14] = rsp_UnknownOpcode;
	RSP_Sc2[15] = rsp_UnknownOpcode;
	RSP_Sc2[16] = rsp_UnknownOpcode;
	RSP_Sc2[17] = rsp_UnknownOpcode;
	RSP_Sc2[18] = rsp_UnknownOpcode;
	RSP_Sc2[19] = rsp_UnknownOpcode;
	RSP_Sc2[20] = rsp_UnknownOpcode;
	RSP_Sc2[21] = rsp_UnknownOpcode;
	RSP_Sc2[22] = rsp_UnknownOpcode;
	RSP_Sc2[23] = rsp_UnknownOpcode;
	RSP_Sc2[24] = rsp_UnknownOpcode;
	RSP_Sc2[25] = rsp_UnknownOpcode;
	RSP_Sc2[26] = rsp_UnknownOpcode;
	RSP_Sc2[27] = rsp_UnknownOpcode;
	RSP_Sc2[28] = rsp_UnknownOpcode;
	RSP_Sc2[29] = rsp_UnknownOpcode;
	RSP_Sc2[30] = rsp_UnknownOpcode;
	RSP_Sc2[31] = rsp_UnknownOpcode;*/

	RSP_NextInstruction = NORMAL;
}

DWORD RunInterpreterRspCPU(DWORD Cycles) {
	RSP_Running = TRUE;
	Enable_RSP_Commands_Window();

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
	}
	SP_PC_REG -= 4;

	return Cycles;
}

