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
void CompileRsp_REGIMM        ( void );
void CompileRsp_J             ( void );
void CompileRsp_JAL           ( void );
void CompileRsp_BEQ           ( void );
void CompileRsp_BNE           ( void );
void CompileRsp_BLEZ          ( void );
void CompileRsp_BGTZ          ( void );
void CompileRsp_ADDI          ( void );
void CompileRsp_ADDIU         ( void );
void CompileRsp_SLTI          ( void );
void CompileRsp_SLTIU         ( void );
void CompileRsp_ANDI          ( void );
void CompileRsp_ORI           ( void );
void CompileRsp_XORI          ( void );
void CompileRsp_LUI           ( void );
void CompileRsp_COP0          ( void );
void CompileRsp_COP2          ( void );
void CompileRsp_LB            ( void );
void CompileRsp_LH            ( void );
void CompileRsp_LW            ( void );
void CompileRsp_LBU           ( void );
void CompileRsp_LHU           ( void );
void CompileRsp_LWU			  ( void );
void CompileRsp_SB            ( void );
void CompileRsp_SH            ( void );
void CompileRsp_SW            ( void );
void CompileRsp_LC2           ( void );
void CompileRsp_SC2           ( void );
/********************** R4300i OpCodes: Special **********************/
void CompileRsp_Special_SLL   ( void );
void CompileRsp_Special_SRL   ( void );
void CompileRsp_Special_SRA   ( void );
void CompileRsp_Special_SLLV  ( void );
void CompileRsp_Special_SRLV  ( void );
void CompileRsp_Special_SRAV  ( void );
void CompileRsp_Special_JR    ( void );
void CompileRsp_Special_JALR  ( void );
void CompileRsp_Special_BREAK ( void );
void CompileRsp_Special_ADD   ( void );
void CompileRsp_Special_ADDU  ( void );
void CompileRsp_Special_SUB   ( void );
void CompileRsp_Special_SUBU  ( void );
void CompileRsp_Special_AND   ( void );
void CompileRsp_Special_OR    ( void );
void CompileRsp_Special_XOR   ( void );
void CompileRsp_Special_NOR   ( void );
void CompileRsp_Special_SLT   ( void );
void CompileRsp_Special_SLTU  ( void );
/********************** R4300i OpCodes: RegImm **********************/
void CompileRsp_RegImm_BLTZ   ( void );
void CompileRsp_RegImm_BGEZ   ( void );
void CompileRsp_RegImm_BLTZAL ( void );
void CompileRsp_RegImm_BGEZAL ( void );
/************************** Cop0 functions *************************/
void CompileRsp_Cop0_MF       ( void );
void CompileRsp_Cop0_MT       ( void );
/************************** Cop2 functions *************************/
void CompileRsp_Cop2_MF       ( void );
void CompileRsp_Cop2_CF       ( void );
void CompileRsp_Cop2_MT       ( void );
void CompileRsp_Cop2_CT       ( void );
void CompileRsp_COP2_VECTOR   ( void );
/************************** Vect functions **************************/
void CompileRsp_Vector_VMULF  ( void );
void CompileRsp_Vector_VMULU  ( void );
void CompileRsp_Vector_VRNDP  ( void );
void CompileRsp_Vector_VMULQ  ( void );
void CompileRsp_Vector_VMUDL  ( void );
void CompileRsp_Vector_VMUDM  ( void );
void CompileRsp_Vector_VMUDN  ( void );
void CompileRsp_Vector_VMUDH  ( void );
void CompileRsp_Vector_VMACF  ( void );
void CompileRsp_Vector_VMACU  ( void );
void CompileRsp_Vector_VRNDN  ( void );
void CompileRsp_Vector_VMACQ  ( void );
void CompileRsp_Vector_VMADL  ( void );
void CompileRsp_Vector_VMADM  ( void );
void CompileRsp_Vector_VMADN  ( void );
void CompileRsp_Vector_VMADH  ( void );
void CompileRsp_Vector_VADD   ( void );
void CompileRsp_Vector_VSUB   ( void );
void CompileRsp_Vector_VSUT	  ( void );
void CompileRsp_Vector_VABS   ( void );
void CompileRsp_Vector_VADDC  ( void );
void CompileRsp_Vector_VSUBC  ( void );
void CompileRsp_Vector_VADDB  ( void );
void CompileRsp_Vector_VSUBB  ( void );
void CompileRsp_Vector_VACCB  ( void );
void CompileRsp_Vector_VSUCB  ( void );
void CompileRsp_Vector_VSAD   ( void );
void CompileRsp_Vector_VSAC   ( void );
void CompileRsp_Vector_VSUM   ( void );
void CompileRsp_Vector_VSAR   ( void );
void CompileRsp_Vector_V30    ( void );
void CompileRsp_Vector_V31    ( void );
void CompileRsp_Vector_VLT    ( void );
void CompileRsp_Vector_VEQ    ( void );
void CompileRsp_Vector_VNE    ( void );
void CompileRsp_Vector_VGE    ( void );
void CompileRsp_Vector_VCL    ( void );
void CompileRsp_Vector_VCH    ( void );
void CompileRsp_Vector_VCR    ( void );
void CompileRsp_Vector_VMRG   ( void );
void CompileRsp_Vector_VAND   ( void );
void CompileRsp_Vector_VNAND  ( void );
void CompileRsp_Vector_VOR    ( void );
void CompileRsp_Vector_VNOR   ( void );
void CompileRsp_Vector_VXOR   ( void );
void CompileRsp_Vector_VNXOR  ( void );
void CompileRsp_Vector_V46    ( void );
void CompileRsp_Vector_V47    ( void );
void CompileRsp_Vector_VRCP   ( void );
void CompileRsp_Vector_VRCPL  ( void );
void CompileRsp_Vector_VRCPH  ( void );
void CompileRsp_Vector_VMOV   ( void );
void CompileRsp_Vector_VRSQ   ( void );
void CompileRsp_Vector_VRSQL  ( void );
void CompileRsp_Vector_VRSQH  ( void );
void CompileRsp_Vector_VNOOP  ( void );
void CompileRsp_Vector_VEXTT  ( void );
void CompileRsp_Vector_VEXTQ  ( void );
void CompileRsp_Vector_VEXTN  ( void );
void CompileRsp_Vector_V59    ( void );
void CompileRsp_Vector_VINST  ( void );
void CompileRsp_Vector_VINSQ  ( void );
void CompileRsp_Vector_VINSN  ( void );
void CompileRsp_Vector_VNULL  ( void );
/************************** lc2 functions **************************/
void CompileRsp_Opcode_LBV    ( void );
void CompileRsp_Opcode_LSV    ( void );
void CompileRsp_Opcode_LLV    ( void );
void CompileRsp_Opcode_LDV    ( void );
void CompileRsp_Opcode_LQV    ( void );
void CompileRsp_Opcode_LRV    ( void );
void CompileRsp_Opcode_LPV    ( void );
void CompileRsp_Opcode_LUV    ( void );
void CompileRsp_Opcode_LHV    ( void );
void CompileRsp_Opcode_LFV    ( void );
void CompileRsp_Opcode_LWV	  ( void );
void CompileRsp_Opcode_LTV    ( void );
/************************** sc2 functions **************************/
void CompileRsp_Opcode_SBV    ( void );
void CompileRsp_Opcode_SSV    ( void );
void CompileRsp_Opcode_SLV    ( void );
void CompileRsp_Opcode_SDV    ( void );
void CompileRsp_Opcode_SQV    ( void );
void CompileRsp_Opcode_SRV    ( void );
void CompileRsp_Opcode_SPV    ( void );
void CompileRsp_Opcode_SUV    ( void );
void CompileRsp_Opcode_SHV    ( void );
void CompileRsp_Opcode_SFV    ( void );
void CompileRsp_Opcode_SWV    ( void );
void CompileRsp_Opcode_STV    ( void );
/************************** Other functions **************************/
void CompileRsp_UnknownOpcode (void);

void CompileRsp_WrapToBeginOfImem(void);
void CompileRsp_CheckRspIsRunning(void);
void CompileRsp_SaveBeginOfSubBlock(void);
void CompileRsp_UpdateCycleCounts(void);
