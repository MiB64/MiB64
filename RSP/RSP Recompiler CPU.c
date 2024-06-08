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
#include "rsp_Cpu.h"
#include "RSP Recompiler CPU.h"
#include "RSP Recompiler Ops.h"
#include "rsp_registers.h"
/*#include "RSP Command.h"*/
#include "rsp_config.h"
#include "rsp_memory.h"
/*#include "opcode.h"
#include "log.h"
#include "Profiling.h"
#include "x86.h"*/
#include "../Main.h"

/* #define REORDER_BLOCK_VERBOSE */
/*#define LINK_BRANCHES_VERBOSE*/ /* no choice really */
/*#define X86_RECOMP_VERBOSE
#define BUILD_BRANCHLABELS_VERBOSE*/

DWORD RspCompilePC/*, BlockID = 0*/;
/*DWORD dwBuffer = MainBuffer;

RSP_BLOCK CurrentBlock;*/
static RSP_CODE RspCode;

/*BYTE * pLastSecondary = NULL, * pLastPrimary = NULL;*/

BOOL IMEMIsUpdated = TRUE;

void BuildRecompilerRspCPU ( void ) {
	RSP_Opcode[ 0] = /*Compile_SPECIAL*/(void*)CompileRsp_UnknownOpcode;
	RSP_Opcode[ 1] = /*Compile_REGIMM*/(void*)CompileRsp_UnknownOpcode;
	RSP_Opcode[ 2] = /*Compile_J*/(void*)CompileRsp_UnknownOpcode;
	RSP_Opcode[ 3] = /*Compile_JAL*/(void*)CompileRsp_UnknownOpcode;
	RSP_Opcode[ 4] = /*Compile_BEQ*/(void*)CompileRsp_UnknownOpcode;
	RSP_Opcode[ 5] = /*Compile_BNE*/(void*)CompileRsp_UnknownOpcode;
	RSP_Opcode[ 6] = /*Compile_BLEZ*/(void*)CompileRsp_UnknownOpcode;
	RSP_Opcode[ 7] = /*Compile_BGTZ*/(void*)CompileRsp_UnknownOpcode;
	RSP_Opcode[ 8] = /*Compile_ADDI*/(void*)CompileRsp_UnknownOpcode;
	RSP_Opcode[ 9] = /*Compile_ADDIU*/(void*)CompileRsp_UnknownOpcode;
	RSP_Opcode[10] = /*Compile_SLTI*/(void*)CompileRsp_UnknownOpcode;
	RSP_Opcode[11] = /*Compile_SLTIU*/(void*)CompileRsp_UnknownOpcode;
	RSP_Opcode[12] = /*Compile_ANDI*/(void*)CompileRsp_UnknownOpcode;
	RSP_Opcode[13] = /*Compile_ORI*/(void*)CompileRsp_UnknownOpcode;
	RSP_Opcode[14] = /*Compile_XORI*/(void*)CompileRsp_UnknownOpcode;
	RSP_Opcode[15] = /*Compile_LUI*/(void*)CompileRsp_UnknownOpcode;
	RSP_Opcode[16] = /*Compile_COP0*/(void*)CompileRsp_UnknownOpcode;
	RSP_Opcode[17] = (void*)CompileRsp_UnknownOpcode;
	RSP_Opcode[18] = /*Compile_COP2*/(void*)CompileRsp_UnknownOpcode;
	RSP_Opcode[19] = (void*)CompileRsp_UnknownOpcode;
	RSP_Opcode[20] = (void*)CompileRsp_UnknownOpcode;
	RSP_Opcode[21] = (void*)CompileRsp_UnknownOpcode;
	RSP_Opcode[22] = (void*)CompileRsp_UnknownOpcode;
	RSP_Opcode[23] = (void*)CompileRsp_UnknownOpcode;
	RSP_Opcode[24] = (void*)CompileRsp_UnknownOpcode;
	RSP_Opcode[25] = (void*)CompileRsp_UnknownOpcode;
	RSP_Opcode[26] = (void*)CompileRsp_UnknownOpcode;
	RSP_Opcode[27] = (void*)CompileRsp_UnknownOpcode;
	RSP_Opcode[28] = (void*)CompileRsp_UnknownOpcode;
	RSP_Opcode[29] = (void*)CompileRsp_UnknownOpcode;
	RSP_Opcode[30] = (void*)CompileRsp_UnknownOpcode;
	RSP_Opcode[31] = (void*)CompileRsp_UnknownOpcode;
	RSP_Opcode[32] = /*Compile_LB*/(void*)CompileRsp_UnknownOpcode;
	RSP_Opcode[33] = /*Compile_LH*/(void*)CompileRsp_UnknownOpcode;
	RSP_Opcode[34] = (void*)CompileRsp_UnknownOpcode;
	RSP_Opcode[35] = /*Compile_LW*/(void*)CompileRsp_UnknownOpcode;
	RSP_Opcode[36] = /*Compile_LBU*/(void*)CompileRsp_UnknownOpcode;
	RSP_Opcode[37] = /*Compile_LHU*/(void*)CompileRsp_UnknownOpcode;
	RSP_Opcode[38] = (void*)CompileRsp_UnknownOpcode;
	RSP_Opcode[39] = (void*)CompileRsp_UnknownOpcode;
	RSP_Opcode[40] = /*Compile_SB*/(void*)CompileRsp_UnknownOpcode;
	RSP_Opcode[41] = /*Compile_SH*/(void*)CompileRsp_UnknownOpcode;
	RSP_Opcode[42] = (void*)CompileRsp_UnknownOpcode;
	RSP_Opcode[43] = /*Compile_SW*/(void*)CompileRsp_UnknownOpcode;
	RSP_Opcode[44] = (void*)CompileRsp_UnknownOpcode;
	RSP_Opcode[45] = (void*)CompileRsp_UnknownOpcode;
	RSP_Opcode[46] = (void*)CompileRsp_UnknownOpcode;
	RSP_Opcode[47] = (void*)CompileRsp_UnknownOpcode;
	RSP_Opcode[48] = (void*)CompileRsp_UnknownOpcode;
	RSP_Opcode[49] = (void*)CompileRsp_UnknownOpcode;
	RSP_Opcode[50] = /*Compile_LC2*/(void*)CompileRsp_UnknownOpcode;
	RSP_Opcode[51] = (void*)CompileRsp_UnknownOpcode;
	RSP_Opcode[52] = (void*)CompileRsp_UnknownOpcode;
	RSP_Opcode[53] = (void*)CompileRsp_UnknownOpcode;
	RSP_Opcode[54] = (void*)CompileRsp_UnknownOpcode;
	RSP_Opcode[55] = (void*)CompileRsp_UnknownOpcode;
	RSP_Opcode[56] = (void*)CompileRsp_UnknownOpcode;
	RSP_Opcode[57] = (void*)CompileRsp_UnknownOpcode;
	RSP_Opcode[58] = /*Compile_SC2*/(void*)CompileRsp_UnknownOpcode;
	RSP_Opcode[59] = (void*)CompileRsp_UnknownOpcode;
	RSP_Opcode[60] = (void*)CompileRsp_UnknownOpcode;
	RSP_Opcode[61] = (void*)CompileRsp_UnknownOpcode;
	RSP_Opcode[62] = (void*)CompileRsp_UnknownOpcode;
	RSP_Opcode[63] = (void*)CompileRsp_UnknownOpcode;

	RSP_Special[ 0] = /*Compile_Special_SLL*/(void*)CompileRsp_UnknownOpcode;
	RSP_Special[ 1] = (void*)CompileRsp_UnknownOpcode;
	RSP_Special[ 2] = /*Compile_Special_SRL*/(void*)CompileRsp_UnknownOpcode;
	RSP_Special[ 3] = /*Compile_Special_SRA*/(void*)CompileRsp_UnknownOpcode;
	RSP_Special[ 4] = /*Compile_Special_SLLV*/(void*)CompileRsp_UnknownOpcode;
	RSP_Special[ 5] = (void*)CompileRsp_UnknownOpcode;
	RSP_Special[ 6] = /*Compile_Special_SRLV*/(void*)CompileRsp_UnknownOpcode;
	RSP_Special[ 7] = /*Compile_Special_SRAV*/(void*)CompileRsp_UnknownOpcode;
	RSP_Special[ 8] = /*Compile_Special_JR*/(void*)CompileRsp_UnknownOpcode;
	RSP_Special[ 9] = /*Compile_Special_JALR*/(void*)CompileRsp_UnknownOpcode;
	RSP_Special[10] = (void*)CompileRsp_UnknownOpcode;
	RSP_Special[11] = (void*)CompileRsp_UnknownOpcode;
	RSP_Special[12] = (void*)CompileRsp_UnknownOpcode;
	RSP_Special[13] = /*Compile_Special_BREAK*/(void*)CompileRsp_UnknownOpcode;
	RSP_Special[14] = (void*)CompileRsp_UnknownOpcode;
	RSP_Special[15] = (void*)CompileRsp_UnknownOpcode;
	RSP_Special[16] = (void*)CompileRsp_UnknownOpcode;
	RSP_Special[17] = (void*)CompileRsp_UnknownOpcode;
	RSP_Special[18] = (void*)CompileRsp_UnknownOpcode;
	RSP_Special[19] = (void*)CompileRsp_UnknownOpcode;
	RSP_Special[20] = (void*)CompileRsp_UnknownOpcode;
	RSP_Special[21] = (void*)CompileRsp_UnknownOpcode;
	RSP_Special[22] = (void*)CompileRsp_UnknownOpcode;
	RSP_Special[23] = (void*)CompileRsp_UnknownOpcode;
	RSP_Special[24] = (void*)CompileRsp_UnknownOpcode;
	RSP_Special[25] = (void*)CompileRsp_UnknownOpcode;
	RSP_Special[26] = (void*)CompileRsp_UnknownOpcode;
	RSP_Special[27] = (void*)CompileRsp_UnknownOpcode;
	RSP_Special[28] = (void*)CompileRsp_UnknownOpcode;
	RSP_Special[29] = (void*)CompileRsp_UnknownOpcode;
	RSP_Special[30] = (void*)CompileRsp_UnknownOpcode;
	RSP_Special[31] = (void*)CompileRsp_UnknownOpcode;
	RSP_Special[32] = /*Compile_Special_ADD*/(void*)CompileRsp_UnknownOpcode;
	RSP_Special[33] = /*Compile_Special_ADDU*/(void*)CompileRsp_UnknownOpcode;
	RSP_Special[34] = /*Compile_Special_SUB*/(void*)CompileRsp_UnknownOpcode;
	RSP_Special[35] = /*Compile_Special_SUBU*/(void*)CompileRsp_UnknownOpcode;
	RSP_Special[36] = /*Compile_Special_AND*/(void*)CompileRsp_UnknownOpcode;
	RSP_Special[37] = /*Compile_Special_OR*/(void*)CompileRsp_UnknownOpcode;
	RSP_Special[38] = /*Compile_Special_XOR*/(void*)CompileRsp_UnknownOpcode;
	RSP_Special[39] = /*Compile_Special_NOR*/(void*)CompileRsp_UnknownOpcode;
	RSP_Special[40] = (void*)CompileRsp_UnknownOpcode;
	RSP_Special[41] = (void*)CompileRsp_UnknownOpcode;
	RSP_Special[42] = /*Compile_Special_SLT*/(void*)CompileRsp_UnknownOpcode;
	RSP_Special[43] = /*Compile_Special_SLTU*/(void*)CompileRsp_UnknownOpcode;
	RSP_Special[44] = (void*)CompileRsp_UnknownOpcode;
	RSP_Special[45] = (void*)CompileRsp_UnknownOpcode;
	RSP_Special[46] = (void*)CompileRsp_UnknownOpcode;
	RSP_Special[47] = (void*)CompileRsp_UnknownOpcode;
	RSP_Special[48] = (void*)CompileRsp_UnknownOpcode;
	RSP_Special[49] = (void*)CompileRsp_UnknownOpcode;
	RSP_Special[50] = (void*)CompileRsp_UnknownOpcode;
	RSP_Special[51] = (void*)CompileRsp_UnknownOpcode;
	RSP_Special[52] = (void*)CompileRsp_UnknownOpcode;
	RSP_Special[53] = (void*)CompileRsp_UnknownOpcode;
	RSP_Special[54] = (void*)CompileRsp_UnknownOpcode;
	RSP_Special[55] = (void*)CompileRsp_UnknownOpcode;
	RSP_Special[56] = (void*)CompileRsp_UnknownOpcode;
	RSP_Special[57] = (void*)CompileRsp_UnknownOpcode;
	RSP_Special[58] = (void*)CompileRsp_UnknownOpcode;
	RSP_Special[59] = (void*)CompileRsp_UnknownOpcode;
	RSP_Special[60] = (void*)CompileRsp_UnknownOpcode;
	RSP_Special[61] = (void*)CompileRsp_UnknownOpcode;
	RSP_Special[62] = (void*)CompileRsp_UnknownOpcode;
	RSP_Special[63] = (void*)CompileRsp_UnknownOpcode;

	RSP_RegImm[ 0] = /*Compile_RegImm_BLTZ*/(void*)CompileRsp_UnknownOpcode;
	RSP_RegImm[ 1] = /*Compile_RegImm_BGEZ*/(void*)CompileRsp_UnknownOpcode;
	RSP_RegImm[ 2] = (void*)CompileRsp_UnknownOpcode;
	RSP_RegImm[ 3] = (void*)CompileRsp_UnknownOpcode;
	RSP_RegImm[ 4] = (void*)CompileRsp_UnknownOpcode;
	RSP_RegImm[ 5] = (void*)CompileRsp_UnknownOpcode;
	RSP_RegImm[ 6] = (void*)CompileRsp_UnknownOpcode;
	RSP_RegImm[ 7] = (void*)CompileRsp_UnknownOpcode;
	RSP_RegImm[ 8] = (void*)CompileRsp_UnknownOpcode;
	RSP_RegImm[ 9] = (void*)CompileRsp_UnknownOpcode;
	RSP_RegImm[10] = (void*)CompileRsp_UnknownOpcode;
	RSP_RegImm[11] = (void*)CompileRsp_UnknownOpcode;
	RSP_RegImm[12] = (void*)CompileRsp_UnknownOpcode;
	RSP_RegImm[13] = (void*)CompileRsp_UnknownOpcode;
	RSP_RegImm[14] = (void*)CompileRsp_UnknownOpcode;
	RSP_RegImm[15] = (void*)CompileRsp_UnknownOpcode;
	RSP_RegImm[16] = /*Compile_RegImm_BLTZAL*/(void*)CompileRsp_UnknownOpcode;
	RSP_RegImm[17] = /*Compile_RegImm_BGEZAL*/(void*)CompileRsp_UnknownOpcode;
	RSP_RegImm[18] = (void*)CompileRsp_UnknownOpcode;
	RSP_RegImm[19] = (void*)CompileRsp_UnknownOpcode;
	RSP_RegImm[20] = (void*)CompileRsp_UnknownOpcode;
	RSP_RegImm[21] = (void*)CompileRsp_UnknownOpcode;
	RSP_RegImm[22] = (void*)CompileRsp_UnknownOpcode;
	RSP_RegImm[23] = (void*)CompileRsp_UnknownOpcode;
	RSP_RegImm[24] = (void*)CompileRsp_UnknownOpcode;
	RSP_RegImm[25] = (void*)CompileRsp_UnknownOpcode;
	RSP_RegImm[26] = (void*)CompileRsp_UnknownOpcode;
	RSP_RegImm[27] = (void*)CompileRsp_UnknownOpcode;
	RSP_RegImm[28] = (void*)CompileRsp_UnknownOpcode;
	RSP_RegImm[29] = (void*)CompileRsp_UnknownOpcode;
	RSP_RegImm[30] = (void*)CompileRsp_UnknownOpcode;
	RSP_RegImm[31] = (void*)CompileRsp_UnknownOpcode;

	RSP_Cop0[ 0] = /*Compile_Cop0_MF*/(void*)CompileRsp_UnknownOpcode;
	RSP_Cop0[ 1] = (void*)CompileRsp_UnknownOpcode;
	RSP_Cop0[ 2] = (void*)CompileRsp_UnknownOpcode;
	RSP_Cop0[ 3] = (void*)CompileRsp_UnknownOpcode;
	RSP_Cop0[ 4] = /*Compile_Cop0_MT*/(void*)CompileRsp_UnknownOpcode;
	RSP_Cop0[ 5] = (void*)CompileRsp_UnknownOpcode;
	RSP_Cop0[ 6] = (void*)CompileRsp_UnknownOpcode;
	RSP_Cop0[ 7] = (void*)CompileRsp_UnknownOpcode;
	RSP_Cop0[ 8] = (void*)CompileRsp_UnknownOpcode;
	RSP_Cop0[ 9] = (void*)CompileRsp_UnknownOpcode;
	RSP_Cop0[10] = (void*)CompileRsp_UnknownOpcode;
	RSP_Cop0[11] = (void*)CompileRsp_UnknownOpcode;
	RSP_Cop0[12] = (void*)CompileRsp_UnknownOpcode;
	RSP_Cop0[13] = (void*)CompileRsp_UnknownOpcode;
	RSP_Cop0[14] = (void*)CompileRsp_UnknownOpcode;
	RSP_Cop0[15] = (void*)CompileRsp_UnknownOpcode;
	RSP_Cop0[16] = (void*)CompileRsp_UnknownOpcode;
	RSP_Cop0[17] = (void*)CompileRsp_UnknownOpcode;
	RSP_Cop0[18] = (void*)CompileRsp_UnknownOpcode;
	RSP_Cop0[19] = (void*)CompileRsp_UnknownOpcode;
	RSP_Cop0[20] = (void*)CompileRsp_UnknownOpcode;
	RSP_Cop0[21] = (void*)CompileRsp_UnknownOpcode;
	RSP_Cop0[22] = (void*)CompileRsp_UnknownOpcode;
	RSP_Cop0[23] = (void*)CompileRsp_UnknownOpcode;
	RSP_Cop0[24] = (void*)CompileRsp_UnknownOpcode;
	RSP_Cop0[25] = (void*)CompileRsp_UnknownOpcode;
	RSP_Cop0[26] = (void*)CompileRsp_UnknownOpcode;
	RSP_Cop0[27] = (void*)CompileRsp_UnknownOpcode;
	RSP_Cop0[28] = (void*)CompileRsp_UnknownOpcode;
	RSP_Cop0[29] = (void*)CompileRsp_UnknownOpcode;
	RSP_Cop0[30] = (void*)CompileRsp_UnknownOpcode;
	RSP_Cop0[31] = (void*)CompileRsp_UnknownOpcode;
	
	RSP_Cop2[ 0] = /*Compile_Cop2_MF*/(void*)CompileRsp_UnknownOpcode;
	RSP_Cop2[ 1] = (void*)CompileRsp_UnknownOpcode;
	RSP_Cop2[ 2] = /*Compile_Cop2_CF*/(void*)CompileRsp_UnknownOpcode;
	RSP_Cop2[ 3] = (void*)CompileRsp_UnknownOpcode;
	RSP_Cop2[ 4] = /*Compile_Cop2_MT*/(void*)CompileRsp_UnknownOpcode;
	RSP_Cop2[ 5] = (void*)CompileRsp_UnknownOpcode;
	RSP_Cop2[ 6] = /*Compile_Cop2_CT*/(void*)CompileRsp_UnknownOpcode;
	RSP_Cop2[ 7] = (void*)CompileRsp_UnknownOpcode;
	RSP_Cop2[ 8] = (void*)CompileRsp_UnknownOpcode;
	RSP_Cop2[ 9] = (void*)CompileRsp_UnknownOpcode;
	RSP_Cop2[10] = (void*)CompileRsp_UnknownOpcode;
	RSP_Cop2[11] = (void*)CompileRsp_UnknownOpcode;
	RSP_Cop2[12] = (void*)CompileRsp_UnknownOpcode;
	RSP_Cop2[13] = (void*)CompileRsp_UnknownOpcode;
	RSP_Cop2[14] = (void*)CompileRsp_UnknownOpcode;
	RSP_Cop2[15] = (void*)CompileRsp_UnknownOpcode;
	RSP_Cop2[16] = /*Compile_COP2_VECTOR*/(void*)CompileRsp_UnknownOpcode;
	RSP_Cop2[17] = /*Compile_COP2_VECTOR*/(void*)CompileRsp_UnknownOpcode;
	RSP_Cop2[18] = /*Compile_COP2_VECTOR*/(void*)CompileRsp_UnknownOpcode;
	RSP_Cop2[19] = /*Compile_COP2_VECTOR*/(void*)CompileRsp_UnknownOpcode;
	RSP_Cop2[20] = /*Compile_COP2_VECTOR*/(void*)CompileRsp_UnknownOpcode;
	RSP_Cop2[21] = /*Compile_COP2_VECTOR*/(void*)CompileRsp_UnknownOpcode;
	RSP_Cop2[22] = /*Compile_COP2_VECTOR*/(void*)CompileRsp_UnknownOpcode;
	RSP_Cop2[23] = /*Compile_COP2_VECTOR*/(void*)CompileRsp_UnknownOpcode;
	RSP_Cop2[24] = /*Compile_COP2_VECTOR*/(void*)CompileRsp_UnknownOpcode;
	RSP_Cop2[25] = /*Compile_COP2_VECTOR*/(void*)CompileRsp_UnknownOpcode;
	RSP_Cop2[26] = /*Compile_COP2_VECTOR*/(void*)CompileRsp_UnknownOpcode;
	RSP_Cop2[27] = /*Compile_COP2_VECTOR*/(void*)CompileRsp_UnknownOpcode;
	RSP_Cop2[28] = /*Compile_COP2_VECTOR*/(void*)CompileRsp_UnknownOpcode;
	RSP_Cop2[29] = /*Compile_COP2_VECTOR*/(void*)CompileRsp_UnknownOpcode;
	RSP_Cop2[30] = /*Compile_COP2_VECTOR*/(void*)CompileRsp_UnknownOpcode;
	RSP_Cop2[31] = /*Compile_COP2_VECTOR*/(void*)CompileRsp_UnknownOpcode;

	RSP_Vector[ 0] = /*Compile_Vector_VMULF*/(void*)CompileRsp_UnknownOpcode;
	RSP_Vector[ 1] = /*Compile_Vector_VMULU*/(void*)CompileRsp_UnknownOpcode;
	RSP_Vector[ 2] = (void*)CompileRsp_UnknownOpcode;
	RSP_Vector[ 3] = (void*)CompileRsp_UnknownOpcode;
	RSP_Vector[ 4] = /*Compile_Vector_VMUDL*/(void*)CompileRsp_UnknownOpcode;
	RSP_Vector[ 5] = /*Compile_Vector_VMUDM*/(void*)CompileRsp_UnknownOpcode;
	RSP_Vector[ 6] = /*Compile_Vector_VMUDN*/(void*)CompileRsp_UnknownOpcode;
	RSP_Vector[ 7] = /*Compile_Vector_VMUDH*/(void*)CompileRsp_UnknownOpcode;
	RSP_Vector[ 8] = /*Compile_Vector_VMACF*/(void*)CompileRsp_UnknownOpcode;
	RSP_Vector[ 9] = /*Compile_Vector_VMACU*/(void*)CompileRsp_UnknownOpcode;
	RSP_Vector[10] = (void*)CompileRsp_UnknownOpcode;
	RSP_Vector[11] = /*Compile_Vector_VMACQ*/(void*)CompileRsp_UnknownOpcode;
	RSP_Vector[12] = /*Compile_Vector_VMADL*/(void*)CompileRsp_UnknownOpcode;
	RSP_Vector[13] = /*Compile_Vector_VMADM*/(void*)CompileRsp_UnknownOpcode;
	RSP_Vector[14] = /*Compile_Vector_VMADN*/(void*)CompileRsp_UnknownOpcode;
	RSP_Vector[15] = /*Compile_Vector_VMADH*/(void*)CompileRsp_UnknownOpcode;
	RSP_Vector[16] = /*Compile_Vector_VADD*/(void*)CompileRsp_UnknownOpcode;
	RSP_Vector[17] = /*Compile_Vector_VSUB*/(void*)CompileRsp_UnknownOpcode;
	RSP_Vector[18] = (void*)CompileRsp_UnknownOpcode;
	RSP_Vector[19] = /*Compile_Vector_VABS*/(void*)CompileRsp_UnknownOpcode;
	RSP_Vector[20] = /*Compile_Vector_VADDC*/(void*)CompileRsp_UnknownOpcode;
	RSP_Vector[21] = /*Compile_Vector_VSUBC*/(void*)CompileRsp_UnknownOpcode;
	RSP_Vector[22] = (void*)CompileRsp_UnknownOpcode;
	RSP_Vector[23] = (void*)CompileRsp_UnknownOpcode;
	RSP_Vector[24] = (void*)CompileRsp_UnknownOpcode;
	RSP_Vector[25] = (void*)CompileRsp_UnknownOpcode;
	RSP_Vector[26] = (void*)CompileRsp_UnknownOpcode;
	RSP_Vector[27] = (void*)CompileRsp_UnknownOpcode;
	RSP_Vector[28] = (void*)CompileRsp_UnknownOpcode;
	RSP_Vector[29] = /*Compile_Vector_VSAW*/(void*)CompileRsp_UnknownOpcode;
	RSP_Vector[30] = (void*)CompileRsp_UnknownOpcode;
	RSP_Vector[31] = (void*)CompileRsp_UnknownOpcode;
	RSP_Vector[32] = /*Compile_Vector_VLT*/(void*)CompileRsp_UnknownOpcode;
	RSP_Vector[33] = /*Compile_Vector_VEQ*/(void*)CompileRsp_UnknownOpcode;
	RSP_Vector[34] = /*Compile_Vector_VNE*/(void*)CompileRsp_UnknownOpcode;
	RSP_Vector[35] = /*Compile_Vector_VGE*/(void*)CompileRsp_UnknownOpcode;
	RSP_Vector[36] = /*Compile_Vector_VCL*/(void*)CompileRsp_UnknownOpcode;
	RSP_Vector[37] = /*Compile_Vector_VCH*/(void*)CompileRsp_UnknownOpcode;
	RSP_Vector[38] = /*Compile_Vector_VCR*/(void*)CompileRsp_UnknownOpcode;
	RSP_Vector[39] = /*Compile_Vector_VMRG*/(void*)CompileRsp_UnknownOpcode;
	RSP_Vector[40] = /*Compile_Vector_VAND*/(void*)CompileRsp_UnknownOpcode;
	RSP_Vector[41] = /*Compile_Vector_VNAND*/(void*)CompileRsp_UnknownOpcode;
	RSP_Vector[42] = /*Compile_Vector_VOR*/(void*)CompileRsp_UnknownOpcode;
	RSP_Vector[43] = /*Compile_Vector_VNOR*/(void*)CompileRsp_UnknownOpcode;
	RSP_Vector[44] = /*Compile_Vector_VXOR*/(void*)CompileRsp_UnknownOpcode;
	RSP_Vector[45] = /*Compile_Vector_VNXOR*/(void*)CompileRsp_UnknownOpcode;
	RSP_Vector[46] = (void*)CompileRsp_UnknownOpcode;
	RSP_Vector[47] = (void*)CompileRsp_UnknownOpcode;
	RSP_Vector[48] = /*Compile_Vector_VRCP*/(void*)CompileRsp_UnknownOpcode;
	RSP_Vector[49] = /*Compile_Vector_VRCPL*/(void*)CompileRsp_UnknownOpcode;
	RSP_Vector[50] = /*Compile_Vector_VRCPH*/(void*)CompileRsp_UnknownOpcode;
	RSP_Vector[51] = /*Compile_Vector_VMOV*/(void*)CompileRsp_UnknownOpcode;
	RSP_Vector[52] = /*Compile_Vector_VRSQ*/(void*)CompileRsp_UnknownOpcode;
	RSP_Vector[53] = /*Compile_Vector_VRSQL*/(void*)CompileRsp_UnknownOpcode;
	RSP_Vector[54] = /*Compile_Vector_VRSQH*/(void*)CompileRsp_UnknownOpcode;
	RSP_Vector[55] = /*Compile_Vector_VNOOP*/(void*)CompileRsp_UnknownOpcode;
	RSP_Vector[56] = (void*)CompileRsp_UnknownOpcode;
	RSP_Vector[57] = (void*)CompileRsp_UnknownOpcode;
	RSP_Vector[58] = (void*)CompileRsp_UnknownOpcode;
	RSP_Vector[59] = (void*)CompileRsp_UnknownOpcode;
	RSP_Vector[60] = (void*)CompileRsp_UnknownOpcode;
	RSP_Vector[61] = (void*)CompileRsp_UnknownOpcode;
	RSP_Vector[62] = (void*)CompileRsp_UnknownOpcode;
	RSP_Vector[63] = (void*)CompileRsp_UnknownOpcode;

	RSP_Lc2[ 0] = /*Compile_Opcode_LBV*/(void*)CompileRsp_UnknownOpcode;
	RSP_Lc2[ 1] = /*Compile_Opcode_LSV*/(void*)CompileRsp_UnknownOpcode;
	RSP_Lc2[ 2] = /*Compile_Opcode_LLV*/(void*)CompileRsp_UnknownOpcode;
	RSP_Lc2[ 3] = /*Compile_Opcode_LDV*/(void*)CompileRsp_UnknownOpcode;
	RSP_Lc2[ 4] = /*Compile_Opcode_LQV*/(void*)CompileRsp_UnknownOpcode;
	RSP_Lc2[ 5] = /*Compile_Opcode_LRV*/(void*)CompileRsp_UnknownOpcode;
	RSP_Lc2[ 6] = /*Compile_Opcode_LPV*/(void*)CompileRsp_UnknownOpcode;
	RSP_Lc2[ 7] = /*Compile_Opcode_LUV*/(void*)CompileRsp_UnknownOpcode;
	RSP_Lc2[ 8] = (void*)CompileRsp_UnknownOpcode;
	RSP_Lc2[ 9] = (void*)CompileRsp_UnknownOpcode;
	RSP_Lc2[10] = (void*)CompileRsp_UnknownOpcode;
	RSP_Lc2[11] = /*Compile_Opcode_LTV*/(void*)CompileRsp_UnknownOpcode;
	RSP_Lc2[12] = (void*)CompileRsp_UnknownOpcode;
	RSP_Lc2[13] = (void*)CompileRsp_UnknownOpcode;
	RSP_Lc2[14] = (void*)CompileRsp_UnknownOpcode;
	RSP_Lc2[15] = (void*)CompileRsp_UnknownOpcode;
	RSP_Lc2[16] = (void*)CompileRsp_UnknownOpcode;
	RSP_Lc2[17] = (void*)CompileRsp_UnknownOpcode;
	RSP_Lc2[18] = (void*)CompileRsp_UnknownOpcode;
	RSP_Lc2[19] = (void*)CompileRsp_UnknownOpcode;
	RSP_Lc2[20] = (void*)CompileRsp_UnknownOpcode;
	RSP_Lc2[21] = (void*)CompileRsp_UnknownOpcode;
	RSP_Lc2[22] = (void*)CompileRsp_UnknownOpcode;
	RSP_Lc2[23] = (void*)CompileRsp_UnknownOpcode;
	RSP_Lc2[24] = (void*)CompileRsp_UnknownOpcode;
	RSP_Lc2[25] = (void*)CompileRsp_UnknownOpcode;
	RSP_Lc2[26] = (void*)CompileRsp_UnknownOpcode;
	RSP_Lc2[27] = (void*)CompileRsp_UnknownOpcode;
	RSP_Lc2[28] = (void*)CompileRsp_UnknownOpcode;
	RSP_Lc2[29] = (void*)CompileRsp_UnknownOpcode;
	RSP_Lc2[30] = (void*)CompileRsp_UnknownOpcode;
	RSP_Lc2[31] = (void*)CompileRsp_UnknownOpcode;

	RSP_Sc2[ 0] = /*Compile_Opcode_SBV*/(void*)CompileRsp_UnknownOpcode;
	RSP_Sc2[ 1] = /*Compile_Opcode_SSV*/(void*)CompileRsp_UnknownOpcode;
	RSP_Sc2[ 2] = /*Compile_Opcode_SLV*/(void*)CompileRsp_UnknownOpcode;
	RSP_Sc2[ 3] = /*Compile_Opcode_SDV*/(void*)CompileRsp_UnknownOpcode;
	RSP_Sc2[ 4] = /*Compile_Opcode_SQV*/(void*)CompileRsp_UnknownOpcode;
	RSP_Sc2[ 5] = /*Compile_Opcode_SRV*/(void*)CompileRsp_UnknownOpcode;
	RSP_Sc2[ 6] = /*Compile_Opcode_SPV*/(void*)CompileRsp_UnknownOpcode;
	RSP_Sc2[ 7] = /*Compile_Opcode_SUV*/(void*)CompileRsp_UnknownOpcode;
	RSP_Sc2[ 8] = (void*)CompileRsp_UnknownOpcode;
	RSP_Sc2[ 9] = (void*)CompileRsp_UnknownOpcode;
	RSP_Sc2[10] = (void*)CompileRsp_UnknownOpcode;
	RSP_Sc2[11] = /*Compile_Opcode_STV*/(void*)CompileRsp_UnknownOpcode;
	RSP_Sc2[12] = (void*)CompileRsp_UnknownOpcode;
	RSP_Sc2[13] = (void*)CompileRsp_UnknownOpcode;
	RSP_Sc2[14] = (void*)CompileRsp_UnknownOpcode;
	RSP_Sc2[15] = (void*)CompileRsp_UnknownOpcode;
	RSP_Sc2[16] = (void*)CompileRsp_UnknownOpcode;
	RSP_Sc2[17] = (void*)CompileRsp_UnknownOpcode;
	RSP_Sc2[18] = (void*)CompileRsp_UnknownOpcode;
	RSP_Sc2[19] = (void*)CompileRsp_UnknownOpcode;
	RSP_Sc2[20] = (void*)CompileRsp_UnknownOpcode;
	RSP_Sc2[21] = (void*)CompileRsp_UnknownOpcode;
	RSP_Sc2[22] = (void*)CompileRsp_UnknownOpcode;
	RSP_Sc2[23] = (void*)CompileRsp_UnknownOpcode;
	RSP_Sc2[24] = (void*)CompileRsp_UnknownOpcode;
	RSP_Sc2[25] = (void*)CompileRsp_UnknownOpcode;
	RSP_Sc2[26] = (void*)CompileRsp_UnknownOpcode;
	RSP_Sc2[27] = (void*)CompileRsp_UnknownOpcode;
	RSP_Sc2[28] = (void*)CompileRsp_UnknownOpcode;
	RSP_Sc2[29] = (void*)CompileRsp_UnknownOpcode;
	RSP_Sc2[30] = (void*)CompileRsp_UnknownOpcode;
	RSP_Sc2[31] = (void*)CompileRsp_UnknownOpcode;
	
	/*BlockID = 0;
	#ifdef Log_x86Code
	Start_x86_Log();
	#endif*/
}

/******************************************************
** ReOrderSubBlock
**
** Desc:
** this can be done, but will be interesting to put
** between branches labels, and actual branches, whichever
** occurs first in code
**
********************************************************/

/*void ReOrderInstructions(DWORD StartPC, DWORD EndPC) {
	DWORD InstructionCount = EndPC - StartPC;
	DWORD Count, ReorderedOps, CurrentPC;
	OPCODE PreviousOp, CurrentOp, RspOp;

	PreviousOp.Hex = *(DWORD*)(RSPInfo.IMEM + StartPC);

	if (TRUE == IsOpcodeBranch(StartPC, PreviousOp)) {*/
		/* the sub block ends here anyway */
/*		return;
	} 
	
	if (IsOpcodeNop(StartPC) && IsOpcodeNop(StartPC + 4) && IsOpcodeNop(StartPC + 8)) {*/
		/* Dont even bother */
/*		return;
	}

	CPU_Message("***** Doing reorder (%X to %X) *****", StartPC, EndPC);

	if (InstructionCount < 0x0010) { return; }
	if (InstructionCount > 0x0A00) { return; }
	
	CPU_Message(" Before:");
	for (Count = StartPC; Count < EndPC; Count += 4) {
		RSP_LW_IMEM(Count, &RspOp.Hex);
		CPU_Message("  %X %s",Count,RSPOpcodeName(RspOp.Hex,Count));
	}

	for (Count = 0; Count < InstructionCount; Count += 4) {
		CurrentPC = StartPC;
		PreviousOp.Hex = *(DWORD*)(RSPInfo.IMEM + CurrentPC);
		ReorderedOps = 0;

		for (;;) {
			CurrentPC += 4;
			if (CurrentPC >= EndPC) { break; }
			CurrentOp.Hex = *(DWORD*)(RSPInfo.IMEM + CurrentPC);

			if (TRUE == CompareInstructions(CurrentPC, &PreviousOp, &CurrentOp)) {*/
				/* Move current opcode up */	
/*				*(DWORD*)(RSPInfo.IMEM + CurrentPC - 4) = CurrentOp.Hex;
			 	*(DWORD*)(RSPInfo.IMEM + CurrentPC) = PreviousOp.Hex;

				ReorderedOps++;
				#ifdef REORDER_BLOCK_VERBOSE
				CPU_Message("Swapped %X and %X", CurrentPC - 4, CurrentPC);
				#endif
			}
			PreviousOp.Hex = *(DWORD*)(RSPInfo.IMEM + CurrentPC);

			if (IsOpcodeNop(CurrentPC) && IsOpcodeNop(CurrentPC + 4) && IsOpcodeNop(CurrentPC + 8)) {
				CurrentPC = EndPC;
			}
		}

		if (ReorderedOps == 0) {
			Count = InstructionCount;
		}
	}

	CPU_Message(" After:");
	for (Count = StartPC; Count < EndPC; Count += 4) {
		RSP_LW_IMEM(Count, &RspOp.Hex);
		CPU_Message("  %X %s",Count,RSPOpcodeName(RspOp.Hex,Count));
	}
	CPU_Message("");
}

void ReOrderSubBlock(RSP_BLOCK * Block) {
	DWORD end = 0x0ffc;
	DWORD count;

	if (!Compiler.bReOrdering) { 
		return; 
	}
	if (Block->CurrPC > 0xFF0) { 
		return; 
	}*/

	/* find the label or jump closest to us */
/*	if (RspCode.LabelCount) {
		for (count = 0; count < RspCode.LabelCount; count++) {
			if (RspCode.BranchLabels[count] < end && 
				  RspCode.BranchLabels[count] > Block->CurrPC) { 
				end = RspCode.BranchLabels[count];
			}
		}
	}
	if (RspCode.BranchCount) {
		for (count = 0; count < RspCode.BranchCount; count++) {
			if (RspCode.BranchLocations[count] < end && 
				  RspCode.BranchLocations[count] > Block->CurrPC) { 
				end = RspCode.BranchLocations[count];
			}
		}
	}*/

	/* it wont actually re-order the op at the end */
/*	ReOrderInstructions(Block->CurrPC, end);
}*/

/******************************************************
** DetectGPRConstants
**
** Desc:
**  this needs to be called on a sub-block basis, like
**  after every time we hit a branch and delay slot
**
********************************************************/

/*void DetectGPRConstants(RSP_CODE * code) {
	DWORD Count, Constant = 0;

	memset(&code->bIsRegConst, 0, sizeof(BOOL) * 0x20);
	memset(&code->MipsRegConst, 0, sizeof(DWORD) * 0x20);
	
	if (!Compiler.bGPRConstants) { 
		return; 
	}
	CPU_Message("***** Detecting constants *****");*/

	/*** Setup R0 ***/
/*	code->bIsRegConst[0] = TRUE;
	code->MipsRegConst[0] = 0;*/

	/* Do your global search for them */
/*	for (Count = 1; Count < 32; Count++) {
		if (IsRegisterConstant(Count, &Constant) == TRUE) {
			CPU_Message("Global: %s is a constant of: %08X", GPR_Name(Count), Constant);
			code->bIsRegConst[Count] = TRUE;
			code->MipsRegConst[Count] = Constant;
		}
	}
	CPU_Message("");
}*/

/******************************************************
** CompilerToggleBuffer and ClearX86Code
**
** Desc:
**  1> toggles the compiler buffer, useful for poorly
**  taken branches like alignment
**
**  2> clears all the x86 code, jump tables etc
**
********************************************************/

/*void CompilerToggleBuffer(void) {
	if (dwBuffer == MainBuffer) {
		dwBuffer = SecondaryBuffer;
		pLastPrimary = RecompPos;

		if (pLastSecondary == NULL) {
			pLastSecondary = RecompCodeSecondary;
		}

		RecompPos = pLastSecondary;
		CPU_Message("   (Secondary Buffer Active 0x%08X)", pLastSecondary);
	} else {
		dwBuffer = MainBuffer;
		pLastSecondary = RecompPos;

		if (pLastPrimary == NULL) {
			pLastPrimary = RecompCode;
		}
	
		RecompPos = pLastPrimary;
		CPU_Message("   (Primary Buffer Active 0x%08X)", pLastPrimary);
	}
}*/

void ClearAllx86Code (void) {
	extern DWORD NoOfRspMaps, RspMapsCRC[32];
	extern BYTE *RspJumpTables;

	memset(&RspMapsCRC, 0, sizeof(DWORD) * 0x20);
	NoOfRspMaps = 0;
	memset(RspJumpTables,0,0x1000*32);

	/*RecompPos = RecompCode;

	pLastPrimary = NULL;
	pLastSecondary = NULL;*/
}

/******************************************************
** Link Branches
**
** Desc:
**  resolves all the collected branches, x86 style
**
********************************************************/

/*void LinkBranches(RSP_BLOCK * Block) {
	DWORD Count, Target;
	DWORD * JumpWord;
	BYTE * X86Code;
	RSP_BLOCK Save;

	if (!CurrentBlock.ResolveCount) { 
		return; 
	}
	CPU_Message("***** Linking branches (%i) *****", CurrentBlock.ResolveCount);

	for (Count = 0; Count < CurrentBlock.ResolveCount; Count++) {
		Target = CurrentBlock.BranchesToResolve[Count].TargetPC;
		X86Code = *(JumpTable + (Target >> 2));

		if (!X86Code) {
			*PrgCount = Target;
			CPU_Message("");
			CPU_Message("===== (Generate Code: %04X) =====", Target);
			Save = *Block;*/

			/* compile this block and link */
/*			CompilerRSPBlock();
			LinkBranches(Block);

			*Block = Save;
			CPU_Message("===== (End Generate Code: %04X) =====", Target);
			CPU_Message("");
			X86Code = *(JumpTable + (Target >> 2));
		}

		JumpWord = CurrentBlock.BranchesToResolve[Count].X86JumpLoc;
		x86_SetBranch32b(JumpWord, (DWORD*)X86Code);

		CPU_Message("Linked RSP branch from x86: %08X, to RSP: %X / x86: %08X",
			JumpWord, Target, X86Code);
	}
	CPU_Message("***** Done Linking Branches *****");
	CPU_Message("");
}*/

/******************************************************
** BuildBranchLabels
**
** Desc:
**   Branch labels are used to start and stop re-ordering 
**   sections as well as set the jump table to points 
**   within a block that are safe
**
********************************************************/

void BuildBranchLabels(void) {
	OPCODE RspOp;
	/*DWORD Dest;*/

	#ifdef BUILD_BRANCHLABELS_VERBOSE
	CPU_Message("***** Building branch labels *****");
	#endif

	for (DWORD i = 0; i < 0x1000; i += 4) {
		RspOp.OP.Hex = *(DWORD*)(IMEM + i);

		if (TRUE == IsRspOpcodeBranch(i, RspOp)) {
			/*if (RspCode.LabelCount >= 175) {
				CompilerWarning("Out of space for Branch Labels");
				return;
			}

			RspCode.BranchLocations[RspCode.BranchCount++] = i;
			if (RspOp.op == RSP_SPECIAL) {*/
				/* register jump not predictable */
/*			} else if (RspOp.op == RSP_J || RspOp.op == RSP_JAL) {*/
				/* for JAL its a sub-block for returns */
/*				Dest = (RspOp.target << 2) & 0xFFC;
				RspCode.BranchLabels[RspCode.LabelCount] = Dest;
				RspCode.LabelCount += 1;
				#ifdef BUILD_BRANCHLABELS_VERBOSE
				CPU_Message("[%02i] Added branch at %X to %X", RspCode.LabelCount, i, Dest);
				#endif
			} else {
				Dest = (i + ((short)RspOp.offset << 2) + 4) & 0xFFC;
				RspCode.BranchLabels[RspCode.LabelCount] = Dest;
				RspCode.LabelCount += 1;
				#ifdef BUILD_BRANCHLABELS_VERBOSE
				CPU_Message("[%02i] Added branch at %X to %X", RspCode.LabelCount, i, Dest);
				#endif
			}*/
			LogMessage("TODO: BuildBranchLabels loop, found branch");
		}
	}

	#ifdef BUILD_BRANCHLABELS_VERBOSE
	CPU_Message("***** End branch labels *****");
	CPU_Message("");
	#endif
}

/*BOOL IsJumpLabel(DWORD PC) {
	DWORD Count;
	
	if (!RspCode.LabelCount) {
		return FALSE; 
	}

	for (Count = 0; Count < RspCode.LabelCount; Count++) {
		if (PC == RspCode.BranchLabels[Count]) {
			return TRUE;
		}
	}
	return FALSE;
}

void CompilerLinkBlocks(void) {
	BYTE * KnownCode = *(JumpTable + (CompilePC >> 2));

	CPU_Message("***** Linking block to X86: %08X *****", KnownCode);
	NextInstruction = FINISH_BLOCK;*/

	/* block linking scenario */				
/*	JmpLabel32("Linked block", 0);
	x86_SetBranch32b(RecompPos - 4, KnownCode);
}

void CompilerRSPBlock ( void ) {
	DWORD Count, Padding, X86BaseAddress = (DWORD)RecompPos;

	NextInstruction = NORMAL;
	CompilePC = *PrgCount;
	
	memset(&CurrentBlock, 0, sizeof(CurrentBlock));
	CurrentBlock.StartPC = CompilePC;
	CurrentBlock.CurrPC = CompilePC;*/

	/* Align the block to a boundary */	
/*	if (X86BaseAddress & 7) {
		Padding = (8 - (X86BaseAddress & 7)) & 7;
		for (Count = 0; Count < Padding; Count++) {
			CPU_Message("%08X: nop", RecompPos);
			*(RecompPos++) = 0x90;
		}
	}

	CPU_Message("====== block %d ======", BlockID++);
	CPU_Message("x86 code at: %X",RecompPos);
	CPU_Message("Jumpt Table: %X",Table );
	CPU_Message("Start of Block: %X",CurrentBlock.StartPC );
	CPU_Message("====== recompiled code ======");

	if (Compiler.bReOrdering == TRUE) {
		memcpy(&CurrentBlock.IMEM[0], RSPInfo.IMEM, 0x1000);
		ReOrderSubBlock(&CurrentBlock);
	}*/

	/* this is for the block about to be compiled */
/*	*(JumpTable + (CompilePC >> 2)) = RecompPos;

	do {*/
		/*
		** Re-Ordering is setup to allow us to have loop labels
		** so here we see if this is one and put it in the jump table
		**/
	
/*		if (NextInstruction == NORMAL && IsJumpLabel(CompilePC)) {*/
			/* jumps come around twice */
/*			if (NULL == *(JumpTable + (CompilePC >> 2))) {
				CPU_Message("***** Adding Jump Table Entry for PC: %04X at X86: %08X *****", CompilePC, RecompPos);
				CPU_Message("");
				*(JumpTable + (CompilePC >> 2)) = RecompPos;*/

				/* reorder from here to next label or branch */
/*				CurrentBlock.CurrPC = CompilePC;
				ReOrderSubBlock(&CurrentBlock);
			} else if (NextInstruction != DELAY_SLOT_DONE) {*/
				/*
				 * we could link the blocks here, but performance
				 * wise it might be better to just let it run
				 */
/*			}
		}

		if (Compiler.bSections == TRUE) {
			if (TRUE == RSP_DoSections()) {
				continue;
			}
		}

		#ifdef X86_RECOMP_VERBOSE
		if (FALSE == IsOpcodeNop(CompilePC)) {
			CPU_Message("X86 Address: %08X", RecompPos);
		}
		#endif

		RSP_LW_IMEM(CompilePC, &RSPOpC.Hex);

		if (RSPOpC.Hex == 0xFFFFFFFF) {*/
			/* i think this pops up an unknown op dialog */
			/* NextInstruction = FINISH_BLOCK; */
/*		} else {
			((void (*)()) RSP_Opcode[ RSPOpC.op ])();
		}

		switch (NextInstruction) {
		case NORMAL: 
			CompilePC += 4; 
			break;
		case DO_DELAY_SLOT:
			NextInstruction = DELAY_SLOT;
			CompilePC += 4;
			break;
		case DELAY_SLOT:
			NextInstruction = DELAY_SLOT_DONE;
			CompilePC -= 4;
			break;
		case FINISH_SUB_BLOCK:
			NextInstruction = NORMAL;
			CompilePC += 8;
			if (CompilePC >= 0x1000) {
				NextInstruction = FINISH_BLOCK;				
			} else if (NULL == *(JumpTable + (CompilePC >> 2))) {*/
				/* this is for the new block being compiled now */
/*				CPU_Message("**** Continuing static SubBlock (jump table entry added for PC: %04X at X86: %08X) *****", CompilePC, RecompPos);
				*(JumpTable + (CompilePC >> 2)) = RecompPos;

				CurrentBlock.CurrPC = CompilePC;*/
				/* reorder from after delay to next label or branch */
/*				ReOrderSubBlock(&CurrentBlock);
			} else {
				CompilerLinkBlocks();
			}
			break;

		case FINISH_BLOCK: break;
		default:
			DisplayError("Rsp Main loop\n\nWTF NextInstruction = %d",NextInstruction);
			CompilePC += 4;
			break;
		}
	} while ( NextInstruction != FINISH_BLOCK && CompilePC < 0x1000);
	CPU_Message("==== end of recompiled code ====");

	if (Compiler.bReOrdering == TRUE) {
		memcpy(RSPInfo.IMEM, &CurrentBlock.IMEM[0], 0x1000);
	}
}*/

DWORD RunRecompilerRspCPU ( DWORD Cycles ) {
	BYTE * Block;

	RSP_Running = TRUE;
	if (IMEMIsUpdated) {
		SetRspJumpTable();
		IMEMIsUpdated = FALSE;
	}

	while (RSP_Running) {
		Block = *(RspJumpTable + (SP_PC_REG >> 2));

		if (Block == NULL) {
			__try {
				memset(&RspCode, 0, sizeof(RspCode));
				BuildBranchLabels();
				/*DetectGPRConstants(&RspCode);
				CompilerRSPBlock();*/
				LogMessage("TODO: RunRecompilerRspCPU(DWORD) loop should compile");
			} __except(EXCEPTION_EXECUTE_HANDLER) {
				DisplayError("Error CompilePC = %08X", RspCompilePC);
				ClearAllx86Code();
				continue;
			}

			LogMessage("TODO: RunRecompilerRspCPU(DWORD) loop should compile, first pass done");
			/*Block = *(JumpTable + (*PrgCount >> 2));*/

			/*
			** we are done compiling, but we may have references
			** to fill in still either from this block, or jumps
			** that go out of it, let's rock
			**/

/*			LinkBranches(&CurrentBlock);*/
		}
		LogMessage("TODO: RunRecompilerRspCPU(DWORD) loop compilation done");

/*	#if !defined(EXTERNAL_RELEASE)
		if (Profiling && IndvidualBlock) {
			char Label[100];
			sprintf(Label,"RSP PC: %03X",*PrgCount);
			StartTimer(Label);
		}
	#endif
		_asm {
			pushad
			call Block
			popad
		}		
	#if !defined(EXTERNAL_RELEASE)
		if (Profiling && IndvidualBlock) {
			StopTimer();
		}
	#endif*/

	}
	LogMessage("TODO: RunRecompilerRspCPU(DWORD)");

	/*if (IsMmxEnabled == TRUE) {
		_asm emms
	}*/
	return Cycles;
}
