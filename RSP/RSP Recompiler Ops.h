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

/************************* OpCode functions *************************/
void CompileRsp_SPECIAL       ( void );
/*void Compile_REGIMM        ( void );
void Compile_J             ( void );
void Compile_JAL           ( void );*/
void CompileRsp_BEQ           ( void );
void CompileRsp_BNE           ( void );
/*void Compile_BLEZ          ( void );*/
void CompileRsp_BGTZ          ( void );
/*void Compile_ADDI          ( void );*/
void CompileRsp_ADDIU         ( void );
/*void Compile_SLTI          ( void );
void Compile_SLTIU         ( void );*/
void CompileRsp_ANDI          ( void );
void CompileRsp_ORI           ( void );
/*void Compile_XORI          ( void );*/
void CompileRsp_LUI           ( void );
void CompileRsp_COP0          ( void );
/*void Compile_COP2          ( void );
void Compile_LB            ( void );
void Compile_LH            ( void );
void Compile_LW            ( void );
void Compile_LBU           ( void );
void Compile_LHU           ( void );
void Compile_SB            ( void );
void Compile_SH            ( void );*/
void CompileRsp_SW            ( void );
/*void Compile_LC2           ( void );
void Compile_SC2           ( void );*/
/********************** R4300i OpCodes: Special **********************/
void CompileRsp_Special_SLL   ( void );
/*void Compile_Special_SRL   ( void );
void Compile_Special_SRA   ( void );
void Compile_Special_SLLV  ( void );
void Compile_Special_SRLV  ( void );
void Compile_Special_SRAV  ( void );
void Compile_Special_JR    ( void );
void Compile_Special_JALR  ( void );*/
void CompileRsp_Special_BREAK ( void );
/*void Compile_Special_ADD   ( void );
void Compile_Special_ADDU  ( void );
void Compile_Special_SUB   ( void );
void Compile_Special_SUBU  ( void );
void Compile_Special_AND   ( void );
void Compile_Special_OR    ( void );
void Compile_Special_XOR   ( void );
void Compile_Special_NOR   ( void );
void Compile_Special_SLT   ( void );
void Compile_Special_SLTU  ( void );*/
/********************** R4300i OpCodes: RegImm **********************/
/*void Compile_RegImm_BLTZ   ( void );
void Compile_RegImm_BGEZ   ( void );
void Compile_RegImm_BLTZAL ( void );
void Compile_RegImm_BGEZAL ( void );*/
/************************** Cop0 functions *************************/
void CompileRsp_Cop0_MF       ( void );
void CompileRsp_Cop0_MT       ( void );
/************************** Cop2 functions *************************/
/*void Compile_Cop2_MF       ( void );
void Compile_Cop2_CF       ( void );
void Compile_Cop2_MT       ( void );
void Compile_Cop2_CT       ( void );
void Compile_COP2_VECTOR  ( void );*/
/************************** Vect functions **************************/
/*void Compile_Vector_VMULF  ( void );
void Compile_Vector_VMULU  ( void );
void Compile_Vector_VMUDL  ( void );
void Compile_Vector_VMUDM  ( void );
void Compile_Vector_VMUDN  ( void );
void Compile_Vector_VMUDH  ( void );
void Compile_Vector_VMACF  ( void );
void Compile_Vector_VMACU  ( void );
void Compile_Vector_VMACQ  ( void );
void Compile_Vector_VMADL  ( void );
void Compile_Vector_VMADM  ( void );
void Compile_Vector_VMADN  ( void );
void Compile_Vector_VMADH  ( void );
void Compile_Vector_VADD   ( void );
void Compile_Vector_VSUB   ( void );
void Compile_Vector_VABS   ( void );
void Compile_Vector_VADDC  ( void );
void Compile_Vector_VSUBC  ( void );
void Compile_Vector_VSAW   ( void );
void Compile_Vector_VLT    ( void );
void Compile_Vector_VEQ    ( void );
void Compile_Vector_VNE    ( void );
void Compile_Vector_VGE    ( void );
void Compile_Vector_VCL    ( void );
void Compile_Vector_VCH    ( void );
void Compile_Vector_VCR    ( void );
void Compile_Vector_VMRG   ( void );
void Compile_Vector_VAND   ( void );
void Compile_Vector_VNAND  ( void );
void Compile_Vector_VOR    ( void );
void Compile_Vector_VNOR   ( void );
void Compile_Vector_VXOR   ( void );
void Compile_Vector_VNXOR  ( void );
void Compile_Vector_VRCP   ( void );
void Compile_Vector_VRCPL  ( void );
void Compile_Vector_VRCPH  ( void );
void Compile_Vector_VMOV   ( void );
void Compile_Vector_VRSQ   ( void );
void Compile_Vector_VRSQL  ( void );
void Compile_Vector_VRSQH  ( void );
void Compile_Vector_VNOOP  ( void );*/
/************************** lc2 functions **************************/
/*void Compile_Opcode_LBV    ( void );
void Compile_Opcode_LSV    ( void );
void Compile_Opcode_LLV    ( void );
void Compile_Opcode_LDV    ( void );
void Compile_Opcode_LQV    ( void );
void Compile_Opcode_LRV    ( void );
void Compile_Opcode_LPV    ( void );
void Compile_Opcode_LUV    ( void );
void Compile_Opcode_LHV    ( void );
void Compile_Opcode_LFV    ( void );
void Compile_Opcode_LTV    ( void );*/
/************************** sc2 functions **************************/
/*void Compile_Opcode_SBV    ( void );
void Compile_Opcode_SSV    ( void );
void Compile_Opcode_SLV    ( void );
void Compile_Opcode_SDV    ( void );
void Compile_Opcode_SQV    ( void );
void Compile_Opcode_SRV    ( void );
void Compile_Opcode_SPV    ( void );
void Compile_Opcode_SUV    ( void );
void Compile_Opcode_SHV    ( void );
void Compile_Opcode_SFV    ( void );
void Compile_Opcode_SWV    ( void );
void Compile_Opcode_STV    ( void );*/
/************************** Other functions **************************/
void CompileRsp_UnknownOpcode (void);

void CompileRsp_WrapToBeginOfImem(void);
void CompileRsp_CheckRspIsRunning(void);
void CompileRsp_SaveBeginOfSubBlock(void);
void CompileRsp_UpdateCycleCounts(void);
