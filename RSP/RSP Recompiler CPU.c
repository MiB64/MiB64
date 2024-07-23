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
#include "RSP Command.h"
#include "rsp_config.h"
#include "rsp_memory.h"
/*#include "opcode.h"*/
#include "rsp_log.h"
#include "../Profiling.h"
#include "../x86.h"
#include "../Main.h"

#define REORDER_BLOCK_VERBOSE
#define LINK_BRANCHES_VERBOSE /* no choice really */
#define X86_RECOMP_VERBOSE
#define BUILD_BRANCHLABELS_VERBOSE

DWORD RspCompilePC;
static DWORD BlockID = 0;
/*DWORD dwBuffer = MainBuffer;*/

RSP_BLOCK RspCurrentBlock;
static RSP_CODE RspCode;
static DWORD maxRecompiledOpcode;

/*BYTE * pLastSecondary = NULL, * pLastPrimary = NULL;*/

BOOL IMEMIsUpdated = TRUE;
DWORD RemainingRspCycles = 0;

static void CompilerRSPBlock(void);

void BuildRecompilerRspCPU ( void ) {
	RSP_Opcode[ 0] = (void*)CompileRsp_SPECIAL;
	RSP_Opcode[ 1] = (void*)CompileRsp_REGIMM;
	RSP_Opcode[ 2] = (void*)CompileRsp_J;
	RSP_Opcode[ 3] = (void*)CompileRsp_JAL;
	RSP_Opcode[ 4] = (void*)CompileRsp_BEQ;
	RSP_Opcode[ 5] = (void*)CompileRsp_BNE;
	RSP_Opcode[ 6] = (void*)CompileRsp_BLEZ;
	RSP_Opcode[ 7] = (void*)CompileRsp_BGTZ;
	RSP_Opcode[ 8] = (void*)CompileRsp_ADDI;
	RSP_Opcode[ 9] = (void*)CompileRsp_ADDIU;
	RSP_Opcode[10] = (void*)CompileRsp_SLTI;
	RSP_Opcode[11] = (void*)CompileRsp_SLTIU;
	RSP_Opcode[12] = (void*)CompileRsp_ANDI;
	RSP_Opcode[13] = (void*)CompileRsp_ORI;
	RSP_Opcode[14] = (void*)CompileRsp_XORI;
	RSP_Opcode[15] = (void*)CompileRsp_LUI;
	RSP_Opcode[16] = (void*)CompileRsp_COP0;
	RSP_Opcode[17] = (void*)CompileRsp_UnknownOpcode;
	RSP_Opcode[18] = (void*)CompileRsp_COP2;
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
	RSP_Opcode[32] = (void*)CompileRsp_LB;
	RSP_Opcode[33] = (void*)CompileRsp_LH;
	RSP_Opcode[34] = (void*)CompileRsp_UnknownOpcode;
	RSP_Opcode[35] = (void*)CompileRsp_LW;
	RSP_Opcode[36] = (void*)CompileRsp_LBU;
	RSP_Opcode[37] = (void*)CompileRsp_LHU;
	RSP_Opcode[38] = (void*)CompileRsp_UnknownOpcode;
	RSP_Opcode[39] = (void*)CompileRsp_LWU;
	RSP_Opcode[40] = (void*)CompileRsp_SB;
	RSP_Opcode[41] = (void*)CompileRsp_SH;
	RSP_Opcode[42] = (void*)CompileRsp_UnknownOpcode;
	RSP_Opcode[43] = (void*)CompileRsp_SW;
	RSP_Opcode[44] = (void*)CompileRsp_UnknownOpcode;
	RSP_Opcode[45] = (void*)CompileRsp_UnknownOpcode;
	RSP_Opcode[46] = (void*)CompileRsp_UnknownOpcode;
	RSP_Opcode[47] = (void*)CompileRsp_UnknownOpcode;
	RSP_Opcode[48] = (void*)CompileRsp_UnknownOpcode;
	RSP_Opcode[49] = (void*)CompileRsp_UnknownOpcode;
	RSP_Opcode[50] = (void*)CompileRsp_LC2;
	RSP_Opcode[51] = (void*)CompileRsp_UnknownOpcode;
	RSP_Opcode[52] = (void*)CompileRsp_UnknownOpcode;
	RSP_Opcode[53] = (void*)CompileRsp_UnknownOpcode;
	RSP_Opcode[54] = (void*)CompileRsp_UnknownOpcode;
	RSP_Opcode[55] = (void*)CompileRsp_UnknownOpcode;
	RSP_Opcode[56] = (void*)CompileRsp_UnknownOpcode;
	RSP_Opcode[57] = (void*)CompileRsp_UnknownOpcode;
	RSP_Opcode[58] = (void*)CompileRsp_SC2;
	RSP_Opcode[59] = (void*)CompileRsp_UnknownOpcode;
	RSP_Opcode[60] = (void*)CompileRsp_UnknownOpcode;
	RSP_Opcode[61] = (void*)CompileRsp_UnknownOpcode;
	RSP_Opcode[62] = (void*)CompileRsp_UnknownOpcode;
	RSP_Opcode[63] = (void*)CompileRsp_UnknownOpcode;

	RSP_Special[ 0] = (void*)CompileRsp_Special_SLL;
	RSP_Special[ 1] = (void*)CompileRsp_UnknownOpcode;
	RSP_Special[ 2] = (void*)CompileRsp_Special_SRL;
	RSP_Special[ 3] = (void*)CompileRsp_Special_SRA;
	RSP_Special[ 4] = (void*)CompileRsp_Special_SLLV;
	RSP_Special[ 5] = (void*)CompileRsp_UnknownOpcode;
	RSP_Special[ 6] = (void*)CompileRsp_Special_SRLV;
	RSP_Special[ 7] = (void*)CompileRsp_Special_SRAV;
	RSP_Special[ 8] = (void*)CompileRsp_Special_JR;
	RSP_Special[ 9] = (void*)CompileRsp_Special_JALR;
	RSP_Special[10] = (void*)CompileRsp_UnknownOpcode;
	RSP_Special[11] = (void*)CompileRsp_UnknownOpcode;
	RSP_Special[12] = (void*)CompileRsp_UnknownOpcode;
	RSP_Special[13] = (void*)CompileRsp_Special_BREAK;
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
	RSP_Special[32] = (void*)CompileRsp_Special_ADD;
	RSP_Special[33] = (void*)CompileRsp_Special_ADDU;
	RSP_Special[34] = (void*)CompileRsp_Special_SUB;
	RSP_Special[35] = (void*)CompileRsp_Special_SUBU;
	RSP_Special[36] = (void*)CompileRsp_Special_AND;
	RSP_Special[37] = (void*)CompileRsp_Special_OR;
	RSP_Special[38] = (void*)CompileRsp_Special_XOR;
	RSP_Special[39] = (void*)CompileRsp_Special_NOR;
	RSP_Special[40] = (void*)CompileRsp_UnknownOpcode;
	RSP_Special[41] = (void*)CompileRsp_UnknownOpcode;
	RSP_Special[42] = (void*)CompileRsp_Special_SLT;
	RSP_Special[43] = (void*)CompileRsp_Special_SLTU;
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

	RSP_RegImm[ 0] = (void*)CompileRsp_RegImm_BLTZ;
	RSP_RegImm[ 1] = (void*)CompileRsp_RegImm_BGEZ;
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
	RSP_RegImm[16] = (void*)CompileRsp_RegImm_BLTZAL;
	RSP_RegImm[17] = (void*)CompileRsp_RegImm_BGEZAL;
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

	RSP_Cop0[ 0] = (void*)CompileRsp_Cop0_MF;
	RSP_Cop0[ 1] = (void*)CompileRsp_UnknownOpcode;
	RSP_Cop0[ 2] = (void*)CompileRsp_UnknownOpcode;
	RSP_Cop0[ 3] = (void*)CompileRsp_UnknownOpcode;
	RSP_Cop0[ 4] = (void*)CompileRsp_Cop0_MT;
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
	
	RSP_Cop2[ 0] = (void*)CompileRsp_Cop2_MF;
	RSP_Cop2[ 1] = (void*)CompileRsp_UnknownOpcode;
	RSP_Cop2[ 2] = (void*)CompileRsp_Cop2_CF;
	RSP_Cop2[ 3] = (void*)CompileRsp_UnknownOpcode;
	RSP_Cop2[ 4] = (void*)CompileRsp_Cop2_MT;
	RSP_Cop2[ 5] = (void*)CompileRsp_UnknownOpcode;
	RSP_Cop2[ 6] = (void*)CompileRsp_Cop2_CT;
	RSP_Cop2[ 7] = (void*)CompileRsp_UnknownOpcode;
	RSP_Cop2[ 8] = (void*)CompileRsp_UnknownOpcode;
	RSP_Cop2[ 9] = (void*)CompileRsp_UnknownOpcode;
	RSP_Cop2[10] = (void*)CompileRsp_UnknownOpcode;
	RSP_Cop2[11] = (void*)CompileRsp_UnknownOpcode;
	RSP_Cop2[12] = (void*)CompileRsp_UnknownOpcode;
	RSP_Cop2[13] = (void*)CompileRsp_UnknownOpcode;
	RSP_Cop2[14] = (void*)CompileRsp_UnknownOpcode;
	RSP_Cop2[15] = (void*)CompileRsp_UnknownOpcode;
	RSP_Cop2[16] = (void*)CompileRsp_COP2_VECTOR;
	RSP_Cop2[17] = (void*)CompileRsp_COP2_VECTOR;
	RSP_Cop2[18] = (void*)CompileRsp_COP2_VECTOR;
	RSP_Cop2[19] = (void*)CompileRsp_COP2_VECTOR;
	RSP_Cop2[20] = (void*)CompileRsp_COP2_VECTOR;
	RSP_Cop2[21] = (void*)CompileRsp_COP2_VECTOR;
	RSP_Cop2[22] = (void*)CompileRsp_COP2_VECTOR;
	RSP_Cop2[23] = (void*)CompileRsp_COP2_VECTOR;
	RSP_Cop2[24] = (void*)CompileRsp_COP2_VECTOR;
	RSP_Cop2[25] = (void*)CompileRsp_COP2_VECTOR;
	RSP_Cop2[26] = (void*)CompileRsp_COP2_VECTOR;
	RSP_Cop2[27] = (void*)CompileRsp_COP2_VECTOR;
	RSP_Cop2[28] = (void*)CompileRsp_COP2_VECTOR;
	RSP_Cop2[29] = (void*)CompileRsp_COP2_VECTOR;
	RSP_Cop2[30] = (void*)CompileRsp_COP2_VECTOR;
	RSP_Cop2[31] = (void*)CompileRsp_COP2_VECTOR;

	RSP_Vector[ 0] = (void*)CompileRsp_Vector_VMULF;
	RSP_Vector[ 1] = (void*)CompileRsp_Vector_VMULU;
	RSP_Vector[ 2] = (void*)CompileRsp_Vector_VRNDP;
	RSP_Vector[ 3] = (void*)CompileRsp_Vector_VMULQ;
	RSP_Vector[ 4] = (void*)CompileRsp_Vector_VMUDL;
	RSP_Vector[ 5] = (void*)CompileRsp_Vector_VMUDM;
	RSP_Vector[ 6] = (void*)CompileRsp_Vector_VMUDN;
	RSP_Vector[ 7] = (void*)CompileRsp_Vector_VMUDH;
	RSP_Vector[ 8] = (void*)CompileRsp_Vector_VMACF;
	RSP_Vector[ 9] = (void*)CompileRsp_Vector_VMACU;
	RSP_Vector[10] = (void*)CompileRsp_Vector_VRNDN;
	RSP_Vector[11] = (void*)CompileRsp_Vector_VMACQ;
	RSP_Vector[12] = (void*)CompileRsp_Vector_VMADL;
	RSP_Vector[13] = (void*)CompileRsp_Vector_VMADM;
	RSP_Vector[14] = (void*)CompileRsp_Vector_VMADN;
	RSP_Vector[15] = (void*)CompileRsp_Vector_VMADH;
	RSP_Vector[16] = (void*)CompileRsp_Vector_VADD;
	RSP_Vector[17] = (void*)CompileRsp_Vector_VSUB;
	RSP_Vector[18] = (void*)CompileRsp_Vector_VSUT;
	RSP_Vector[19] = (void*)CompileRsp_Vector_VABS;
	RSP_Vector[20] = (void*)CompileRsp_Vector_VADDC;
	RSP_Vector[21] = (void*)CompileRsp_Vector_VSUBC;
	RSP_Vector[22] = (void*)CompileRsp_Vector_VADDB;
	RSP_Vector[23] = (void*)CompileRsp_Vector_VSUBB;
	RSP_Vector[24] = (void*)CompileRsp_Vector_VACCB;
	RSP_Vector[25] = (void*)CompileRsp_Vector_VSUCB;
	RSP_Vector[26] = (void*)CompileRsp_Vector_VSAD;
	RSP_Vector[27] = (void*)CompileRsp_Vector_VSAC;
	RSP_Vector[28] = (void*)CompileRsp_Vector_VSUM;
	RSP_Vector[29] = (void*)CompileRsp_Vector_VSAR;
	RSP_Vector[30] = (void*)CompileRsp_Vector_V30;
	RSP_Vector[31] = (void*)CompileRsp_Vector_V31;
	RSP_Vector[32] = (void*)CompileRsp_Vector_VLT;
	RSP_Vector[33] = (void*)CompileRsp_Vector_VEQ;
	RSP_Vector[34] = (void*)CompileRsp_Vector_VNE;
	RSP_Vector[35] = (void*)CompileRsp_Vector_VGE;
	RSP_Vector[36] = (void*)CompileRsp_Vector_VCL;
	RSP_Vector[37] = (void*)CompileRsp_Vector_VCH;
	RSP_Vector[38] = (void*)CompileRsp_Vector_VCR;
	RSP_Vector[39] = (void*)CompileRsp_Vector_VMRG;
	RSP_Vector[40] = (void*)CompileRsp_Vector_VAND;
	RSP_Vector[41] = (void*)CompileRsp_Vector_VNAND;
	RSP_Vector[42] = (void*)CompileRsp_Vector_VOR;
	RSP_Vector[43] = (void*)CompileRsp_Vector_VNOR;
	RSP_Vector[44] = (void*)CompileRsp_Vector_VXOR;
	RSP_Vector[45] = (void*)CompileRsp_Vector_VNXOR;
	RSP_Vector[46] = (void*)CompileRsp_Vector_V46;
	RSP_Vector[47] = (void*)CompileRsp_Vector_V47;
	RSP_Vector[48] = (void*)CompileRsp_Vector_VRCP;
	RSP_Vector[49] = (void*)CompileRsp_Vector_VRCPL;
	RSP_Vector[50] = (void*)CompileRsp_Vector_VRCPH;
	RSP_Vector[51] = (void*)CompileRsp_Vector_VMOV;
	RSP_Vector[52] = (void*)CompileRsp_Vector_VRSQ;
	RSP_Vector[53] = (void*)CompileRsp_Vector_VRSQL;
	RSP_Vector[54] = (void*)CompileRsp_Vector_VRSQH;
	RSP_Vector[55] = (void*)CompileRsp_Vector_VNOOP;
	RSP_Vector[56] = (void*)CompileRsp_Vector_VEXTT;
	RSP_Vector[57] = (void*)CompileRsp_Vector_VEXTQ;
	RSP_Vector[58] = (void*)CompileRsp_Vector_VEXTN;
	RSP_Vector[59] = (void*)CompileRsp_Vector_V59;
	RSP_Vector[60] = (void*)CompileRsp_Vector_VINST;
	RSP_Vector[61] = (void*)CompileRsp_Vector_VINSQ;
	RSP_Vector[62] = (void*)CompileRsp_Vector_VINSN;
	RSP_Vector[63] = (void*)CompileRsp_Vector_VNULL;

	RSP_Lc2[ 0] = (void*)CompileRsp_Opcode_LBV;
	RSP_Lc2[ 1] = (void*)CompileRsp_Opcode_LSV;
	RSP_Lc2[ 2] = (void*)CompileRsp_Opcode_LLV;
	RSP_Lc2[ 3] = (void*)CompileRsp_Opcode_LDV;
	RSP_Lc2[ 4] = (void*)CompileRsp_Opcode_LQV;
	RSP_Lc2[ 5] = (void*)CompileRsp_Opcode_LRV;
	RSP_Lc2[ 6] = (void*)CompileRsp_Opcode_LPV;
	RSP_Lc2[ 7] = (void*)CompileRsp_Opcode_LUV;
	RSP_Lc2[ 8] = (void*)CompileRsp_Opcode_LHV;
	RSP_Lc2[ 9] = (void*)CompileRsp_Opcode_LFV;
	RSP_Lc2[10] = (void*)CompileRsp_Opcode_LWV;
	RSP_Lc2[11] = (void*)CompileRsp_Opcode_LTV;
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

	RSP_Sc2[ 0] = (void*)CompileRsp_Opcode_SBV;
	RSP_Sc2[ 1] = (void*)CompileRsp_Opcode_SSV;
	RSP_Sc2[ 2] = (void*)CompileRsp_Opcode_SLV;
	RSP_Sc2[ 3] = (void*)CompileRsp_Opcode_SDV;
	RSP_Sc2[ 4] = (void*)CompileRsp_Opcode_SQV;
	RSP_Sc2[ 5] = (void*)CompileRsp_Opcode_SRV;
	RSP_Sc2[ 6] = (void*)CompileRsp_Opcode_SPV;
	RSP_Sc2[ 7] = (void*)CompileRsp_Opcode_SUV;
	RSP_Sc2[ 8] = (void*)CompileRsp_Opcode_SHV;
	RSP_Sc2[ 9] = (void*)CompileRsp_Opcode_SFV;
	RSP_Sc2[10] = (void*)CompileRsp_Opcode_SWV;
	RSP_Sc2[11] = (void*)CompileRsp_Opcode_STV;
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
	
	BlockID = 0;
#ifdef RspLog_x86Code
	RSP_Start_x86_Log();
#endif
	RSP_NextInstruction = NORMAL;
	IMEMIsUpdated = TRUE;
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

static void ReOrderInstructions(DWORD StartPC, DWORD EndPC) {
	DWORD InstructionCount = EndPC - StartPC;
	DWORD Count, ReorderedOps, CurrentPC;
	OPCODE PreviousOp, CurrentOp;
#ifdef RspLog_x86Code
	OPCODE RspOp;
#endif

	PreviousOp.OP.Hex = *(DWORD*)(IMEM + StartPC);

	if (TRUE == IsRspOpcodeBranch(StartPC, PreviousOp)) {
		/* the sub block ends here anyway */
		return;
	} 
	
	if (IsRspOpcodeNop(StartPC) && IsRspOpcodeNop(StartPC + 4) && IsRspOpcodeNop(StartPC + 8)) {
		/* Dont even bother */
		return;
	}

	RSP_CPU_Message("***** Doing reorder (%X to %X) *****", StartPC, EndPC);

	if (InstructionCount < 0x0010) { return; }
	if (InstructionCount > 0x0A00) { return; }

#ifdef RspLog_x86Code
	RSP_CPU_Message(" Before:");
	for (Count = StartPC; Count < EndPC; Count += 4) {
		RSP_LW_IMEM(Count, &RspOp.OP.Hex);
		RSP_CPU_Message("  %X %s",Count,RSPOpcodeName(RspOp.OP.Hex,Count));
	}
#endif

	for (Count = 0; Count < InstructionCount; Count += 4) {
		CurrentPC = StartPC;
		PreviousOp.OP.Hex = *(DWORD*)(IMEM + CurrentPC);
		ReorderedOps = 0;

		for (;;) {
			CurrentPC += 4;
			if (CurrentPC >= EndPC) { break; }
			CurrentOp.OP.Hex = *(DWORD*)(IMEM + CurrentPC);

			if (TRUE == RspCompareInstructions(CurrentPC, &PreviousOp, &CurrentOp)) {
				/* Move current opcode up */
				*(DWORD*)(IMEM + CurrentPC - 4) = CurrentOp.OP.Hex;
			 	*(DWORD*)(IMEM + CurrentPC) = PreviousOp.OP.Hex;

				ReorderedOps++;
				#ifdef REORDER_BLOCK_VERBOSE
				RSP_CPU_Message("Swapped %X and %X", CurrentPC - 4, CurrentPC);
				#endif
			}
			PreviousOp.OP.Hex = *(DWORD*)(IMEM + CurrentPC);

			if (IsRspOpcodeNop(CurrentPC) && IsRspOpcodeNop(CurrentPC + 4) && IsRspOpcodeNop(CurrentPC + 8)) {
				CurrentPC = EndPC;
			}
		}

		if (ReorderedOps == 0) {
			Count = InstructionCount;
		}
	}

#ifdef RspLog_x86Code
	RSP_CPU_Message(" After:");
	for (Count = StartPC; Count < EndPC; Count += 4) {
		RSP_LW_IMEM(Count, &RspOp.OP.Hex);
		RSP_CPU_Message("  %X %s",Count,RSPOpcodeName(RspOp.OP.Hex,Count));
	}
	RSP_CPU_Message("");
#endif
}

static void ReOrderSubBlock(RSP_BLOCK * Block) {
	DWORD end = 0x0ffc;
	DWORD count;

	if (!RspCompiler.bReOrdering) { 
		return; 
	}
	if (Block->CurrPC > 0xFF0) { 
		return; 
	}

	/* find the label or jump closest to us */
	if (RspCode.LabelCount) {
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
	}

	/* it wont actually re-order the op at the end */
	ReOrderInstructions(Block->CurrPC, end);
}

/******************************************************
** DetectGPRConstants
**
** Desc:
**  this needs to be called on a sub-block basis, like
**  after every time we hit a branch and delay slot
**
********************************************************/

static void DetectGPRConstants(RSP_CODE * code) {
	DWORD Count, Constant = 0;

	memset(&code->bIsRegConst, 0, sizeof(BOOL) * 0x20);
	memset(&code->MipsRegConst, 0, sizeof(DWORD) * 0x20);
	
	if (!RspCompiler.bGPRConstants) { 
		return; 
	}
	RSP_CPU_Message("***** Detecting constants *****");

	/*** Setup R0 ***/
	code->bIsRegConst[0] = TRUE;
	code->MipsRegConst[0] = 0;

	/* Do your global search for them */
	for (Count = 1; Count < 32; Count++) {
		if (IsRspRegisterConstant(Count, &Constant) == TRUE) {
			RSP_CPU_Message("Global: %s is a constant of: %08X", RspGPR_Name(Count), Constant);
			code->bIsRegConst[Count] = TRUE;
			code->MipsRegConst[Count] = Constant;
		}
	}
	RSP_CPU_Message("");
}

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

static void ClearAllx86Code (void) {
	ClearJumpTables();

	RspRecompPos = RspRecompCode;

	/*pLastPrimary = NULL;
	pLastSecondary = NULL;*/
}

/******************************************************
** Link Branches
**
** Desc:
**  resolves all the collected branches, x86 style
**
********************************************************/

static void LinkBranches(RSP_BLOCK * Block) {
	DWORD Count, Target;
	DWORD * JumpWord;
	BYTE * X86Code;
	RSP_BLOCK Save;

	if (!RspCurrentBlock.ResolveCount) {
		return; 
	}
	RSP_CPU_Message("***** Linking branches (%i) *****", RspCurrentBlock.ResolveCount);

	for (Count = 0; Count < RspCurrentBlock.ResolveCount; Count++) {
		Target = RspCurrentBlock.BranchesToResolve[Count].TargetPC;
		X86Code = *(RspJumpTable + (Target >> 2));

		if (!X86Code) {
			SP_PC_REG = Target;
			RSP_CPU_Message("");
			RSP_CPU_Message("===== (Generate Code: %04X) =====", Target);
			Save = *Block;

			/* compile this block and link */
			CompilerRSPBlock();
			LinkBranches(Block);

			*Block = Save;
			RSP_CPU_Message("===== (End Generate Code: %04X) =====", Target);
			RSP_CPU_Message("");
			X86Code = *(RspJumpTable + (Target >> 2));
		}

		JumpWord = RspCurrentBlock.BranchesToResolve[Count].X86JumpLoc;
		x86_SetBranch32b(JumpWord, (DWORD*)X86Code);

		RSP_CPU_Message("Linked RSP branch from x86: %08X, to RSP: %X / x86: %08X",
			JumpWord, Target, X86Code);
	}
	RSP_CPU_Message("***** Done Linking Branches *****");
	RSP_CPU_Message("");
}

/******************************************************
** BuildBranchLabels
**
** Desc:
**   Branch labels are used to start and stop re-ordering 
**   sections as well as set the jump table to points 
**   within a block that are safe
**
********************************************************/

static void BuildBranchLabels(void) {
	OPCODE RspOp;
	DWORD Dest;

	#ifdef BUILD_BRANCHLABELS_VERBOSE
	RSP_CPU_Message("***** Building branch labels *****");
	#endif

	for (DWORD i = 0; i < 0x1000; i += 4) {
		RspOp.OP.Hex = *(DWORD*)(IMEM + i);

		if (TRUE == IsRspOpcodeBranch(i, RspOp)) {
			if (RspCode.LabelCount >= 175) {
				RspCompilerWarning("Out of space for Branch Labels");
				return;
			}

			RspCode.BranchLocations[RspCode.BranchCount++] = i;
			if (RspOp.OP.I.op == RSP_SPECIAL) {
				/* register jump not predictable */
			} else if (RspOp.OP.I.op == RSP_J || RspOp.OP.I.op == RSP_JAL) {
				/* for JAL its a sub-block for returns */
				Dest = (RspOp.OP.J.target << 2) & 0xFFC;
				RspCode.BranchLabels[RspCode.LabelCount] = Dest;
				RspCode.LabelCount += 1;
				#ifdef BUILD_BRANCHLABELS_VERBOSE
				RSP_CPU_Message("[%02i] Added branch at %X to %X", RspCode.LabelCount, i, Dest);
				#endif
			} else {
				Dest = (i + ((short)RspOp.OP.B.offset << 2) + 4) & 0xFFC;
				RspCode.BranchLabels[RspCode.LabelCount] = Dest;
				RspCode.LabelCount += 1;
				#ifdef BUILD_BRANCHLABELS_VERBOSE
				RSP_CPU_Message("[%02i] Added branch at %X to %X", RspCode.LabelCount, i, Dest);
				#endif
			}
		}
	}

	#ifdef BUILD_BRANCHLABELS_VERBOSE
	RSP_CPU_Message("***** End branch labels *****");
	RSP_CPU_Message("");
	#endif
}

static BOOL IsJumpLabel(DWORD PC) {
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

static void CompilerLinkBlocks(void) {
	BYTE * KnownCode = *(RspJumpTable + (RspCompilePC >> 2));

	RSP_CPU_Message("***** Linking block to X86: %08X *****", KnownCode);
	RSP_NextInstruction = FINISH_BLOCK;

	/* block linking scenario */
	JmpLabel32(&RspRecompPos, "Linked block", 0);
	x86_SetBranch32b(RspRecompPos - 4, KnownCode);
}

static void CompilerRSPBlock ( void ) {
	DWORD Count, Padding, X86BaseAddress = (DWORD)RspRecompPos;

	RSP_NextInstruction = NORMAL;
	RspCompilePC = SP_PC_REG;
	maxRecompiledOpcode = GetMaxRecompiledOpcode();
	
	memset(&RspCurrentBlock, 0, sizeof(RspCurrentBlock));
	RspCurrentBlock.StartPC = RspCompilePC;
	RspCurrentBlock.CurrPC = RspCompilePC;

	/* Align the block to a boundary */	
	if (X86BaseAddress & 7) {
		Padding = (8 - (X86BaseAddress & 7)) & 7;
		for (Count = 0; Count < Padding; Count++) {
			RSP_CPU_Message("%08X: nop", RspRecompPos);
			*(RspRecompPos++) = 0x90;
		}
	}

	RSP_CPU_Message("====== block %d ======", BlockID++);
	RSP_CPU_Message("x86 code at: %X",RspRecompPos);
	RSP_CPU_Message("Jump Table: %X",RspTable );
	RSP_CPU_Message("Start of Block: %X",RspCurrentBlock.StartPC );
	RSP_CPU_Message("====== recompiled code ======");

	if (RspCompiler.bReOrdering == TRUE) {
		memcpy(&RspCurrentBlock.IMEM[0], IMEM, 0x1000);
		ReOrderSubBlock(&RspCurrentBlock);
	}

	/* this is for the block about to be compiled */
	*(RspJumpTable + (RspCompilePC >> 2)) = RspRecompPos;

	CompileRsp_CheckRspIsRunning();
	CompileRsp_SaveBeginOfSubBlock();

	do {
		/*
		** Re-Ordering is setup to allow us to have loop labels
		** so here we see if this is one and put it in the jump table
		**/
	
		if (RSP_NextInstruction == NORMAL && IsJumpLabel(RspCompilePC)) {
			/* jumps come around twice */
			if (NULL == *(RspJumpTable + (RspCompilePC >> 2))) {
				RSP_CPU_Message("***** Adding Jump Table Entry for PC: %04X at X86: %08X *****", RspCompilePC, RspRecompPos);
				RSP_CPU_Message("");
				*(RspJumpTable + (RspCompilePC >> 2)) = RspRecompPos;

				CompileRsp_CheckRspIsRunning();
				CompileRsp_SaveBeginOfSubBlock();

				/* reorder from here to next label or branch */
				RspCurrentBlock.CurrPC = RspCompilePC;
				ReOrderSubBlock(&RspCurrentBlock);
			} else if (RSP_NextInstruction != DELAY_SLOT_DONE) {
				/*
				 * we could link the blocks here, but performance
				 * wise it might be better to just let it run
				 */
			}
		}

		if (RspCompiler.bSections == TRUE) {
			if (TRUE == RSP_DoSections()) {
				LogMessage("TODO: CompilerRSPBlock, compilation loop, RSP_DoSections");
				continue;
			}
		}

		#ifdef X86_RECOMP_VERBOSE
		if (FALSE == IsRspOpcodeNop(RspCompilePC)) {
			RSP_CPU_Message("X86 Address: %08X", RspRecompPos);
		}
		#endif

		RSP_LW_IMEM(RspCompilePC, &RSPOpC.OP.Hex);

		((void (*)()) RSP_Opcode[ RSPOpC.OP.I.op ])();

		if (RspCompilePC > maxRecompiledOpcode) {
			maxRecompiledOpcode = RspCompilePC;
		}

		switch (RSP_NextInstruction) {
		case NORMAL: 
			RspCompilePC += 4;
			break;
		case DO_DELAY_SLOT:
			RSP_NextInstruction = DELAY_SLOT;
			RspCompilePC += 4;
			RspCompilePC &= 0xFFF;
			break;
		case DELAY_SLOT:
			CompileRsp_UpdateCycleCounts();
			RSP_NextInstruction = DELAY_SLOT_DONE;
			RspCompilePC -= 4;
			RspCompilePC &= 0xFFF;
			break;
		case FINISH_SUB_BLOCK:
			RSP_NextInstruction = NORMAL;
			RspCompilePC += 8;
			if (RspCompilePC >= 0x1000) {
				RSP_NextInstruction = FINISH_BLOCK;				
			} else if (NULL == *(RspJumpTable + (RspCompilePC >> 2))) {
				/* this is for the new block being compiled now */
				RSP_CPU_Message("**** Continuing static SubBlock (jump table entry added for PC: %04X at X86: %08X) *****", RspCompilePC, RspRecompPos);
				*(RspJumpTable + (RspCompilePC >> 2)) = RspRecompPos;

				CompileRsp_CheckRspIsRunning();
				CompileRsp_SaveBeginOfSubBlock();

				RspCurrentBlock.CurrPC = RspCompilePC;
				/* reorder from after delay to next label or branch */
				ReOrderSubBlock(&RspCurrentBlock);
			} else {
				CompilerLinkBlocks();
			}
			break;

		case FINISH_BLOCK: break;
		default:
			DisplayError("Rsp Main loop\n\nWTF NextInstruction = %d",RSP_NextInstruction);
			RspCompilePC += 4;
			break;
		}
	} while ( RSP_NextInstruction != FINISH_BLOCK && RspCompilePC < 0x1000);

	if (RspCompilePC >= 0x1000) {
		CompileRsp_UpdateCycleCounts();
		CompileRsp_WrapToBeginOfImem();
	}

	RSP_CPU_Message("==== end of recompiled code ====");

	if (RspCompiler.bReOrdering == TRUE) {
		memcpy(IMEM, &RspCurrentBlock.IMEM[0], 0x1000);
	}
	UpdateJumpTableHash(maxRecompiledOpcode);
}

DWORD RunRecompilerRspCPU ( DWORD Cycles ) {
	BYTE * Block;

	RemainingRspCycles = Cycles;

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
				DetectGPRConstants(&RspCode);
				CompilerRSPBlock();
			} __except(EXCEPTION_EXECUTE_HANDLER) {
				DisplayError("Error CompilePC = %08X", RspCompilePC);
				ClearAllx86Code();
				continue;
			}

			Block = *(RspJumpTable + (SP_PC_REG >> 2));

			/*
			** we are done compiling, but we may have references
			** to fill in still either from this block, or jumps
			** that go out of it, let's rock
			**/

			LinkBranches(&RspCurrentBlock);
		}

	#if !defined(EXTERNAL_RELEASE)
		if (RspProfiling && IndividualRspBlock) {
			char Label[100];
			sprintf(Label,"RSP PC: %03X",SP_PC_REG);
			StartTimer(Label);
		}
	#endif
		_asm {
			pushad
			call Block
			popad
		}		
	#if !defined(EXTERNAL_RELEASE)
		if (RspProfiling && IndividualRspBlock) {
			StopTimer();
		}
	#endif

	}

	/*if (IsMmxEnabled == TRUE) {
		_asm emms
	}*/

	return Cycles - RemainingRspCycles;
}
