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

#include "rsp/rsp_registers.h"
#include "mi_registers.h"
#include "rdp_registers.h"
#include "Pif.h"

#define INDEX_REGISTER			CP0[0].UW[0]
#define RANDOM_REGISTER			CP0[1].UW[0]
#define ENTRYLO0_REGISTER		CP0[2].UW[0]
#define ENTRYLO1_REGISTER		CP0[3].UW[0]
#define CONTEXT_REGISTER		CP0[4].UDW
#define PAGE_MASK_REGISTER		CP0[5].UW[0]
#define WIRED_REGISTER			CP0[6].UW[0]
#define BAD_VADDR_REGISTER		CP0[8].UDW
#define COUNT_REGISTER			CP0[9].UW[0]
#define ENTRYHI_REGISTER		CP0[10].UDW
#define COMPARE_REGISTER		CP0[11].UW[0]
#define STATUS_REGISTER			CP0[12].UW[0]
#define CAUSE_REGISTER			CP0[13].UW[0]
#define EPC_REGISTER			CP0[14].UDW
#define PRID_REGISTER           CP0[15].UW[0]
#define CONFIG_REGISTER			CP0[16].UW[0]
#define LLADDR_REGISTER			CP0[17].UW[0]
#define XCONTEXT_REGISTER       CP0[20].UDW
#define TAGLO_REGISTER			CP0[28].UW[0]
#define TAGHI_REGISTER			CP0[29].UW[0]
#define ERROREPC_REGISTER		CP0[30].UDW

#define COMPARE_REGISTER_NO		11
#define STATUS_REGISTER_NO		12
#define CAUSE_REGISTER_NO		13

#define REVISION_REGISTER		FPCR[0]
#define FSTATUS_REGISTER		FPCR[31]

#define GPR_S0					GPR[16]
#define GPR_S1					GPR[17]
#define GPR_S2					GPR[18]
#define GPR_S3					GPR[19]
#define GPR_S4					GPR[20]
#define GPR_S5					GPR[21]
#define GPR_S6					GPR[22]
#define GPR_S7					GPR[23]
#define GPR_SP					GPR[29]
#define GPR_RA					GPR[31]

#define RDRAM_DEVICE_TYPE_REG(deviceId)		(*RegRDRAM)[deviceId][0]
#define RDRAM_DEVICE_ID_REG(deviceId)		(*RegRDRAM)[deviceId][1]
#define RDRAM_DELAY_REG(deviceId)			(*RegRDRAM)[deviceId][2]
#define RDRAM_MODE_REG(deviceId)			(*RegRDRAM)[deviceId][3]
#define RDRAM_REF_INTERVAL_REG(deviceId)	(*RegRDRAM)[deviceId][4]
#define RDRAM_REF_ROW_REG(deviceId)			(*RegRDRAM)[deviceId][5]
#define RDRAM_RAS_INTERVAL_REG(deviceId)	(*RegRDRAM)[deviceId][6]
#define RDRAM_MIN_INTERVAL_REG(deviceId)	(*RegRDRAM)[deviceId][7]
#define RDRAM_ADDR_SELECT_REG(deviceId)		(*RegRDRAM)[deviceId][8]
#define RDRAM_DEVICE_MANUF_REG(deviceId)	(*RegRDRAM)[deviceId][9]

#define NUMBER_OF_RDRAM_MODULES (RdramSize == 0x400000 ? 2U : 4U)

#define VI_STATUS_REG			RegVI[0]
#define VI_CONTROL_REG			RegVI[0]
#define VI_ORIGIN_REG 			RegVI[1]
#define VI_DRAM_ADDR_REG		RegVI[1]
#define VI_WIDTH_REG 			RegVI[2]
#define VI_H_WIDTH_REG 			RegVI[2]
#define VI_INTR_REG  			RegVI[3]
#define VI_V_INTR_REG 			RegVI[3]
#define VI_CURRENT_REG 			RegVI[4]
#define VI_V_CURRENT_LINE_REG	RegVI[4]
#define VI_BURST_REG  			RegVI[5]
#define VI_TIMING_REG 			RegVI[5]
#define VI_V_SYNC_REG 			RegVI[6]
#define VI_H_SYNC_REG 			RegVI[7]
#define VI_LEAP_REG  			RegVI[8]
#define VI_H_SYNC_LEAP_REG		RegVI[8]
#define VI_H_START_REG 			RegVI[9]
#define VI_H_VIDEO_REG			RegVI[9]
#define VI_V_START_REG 			RegVI[10]
#define VI_V_VIDEO_REG			RegVI[10]
#define VI_V_BURST_REG			RegVI[11]
#define VI_X_SCALE_REG			RegVI[12]
#define VI_Y_SCALE_REG			RegVI[13]

#define AI_DRAM_ADDR_REG		RegAI[0]
#define AI_LEN_REG				RegAI[1]
#define AI_CONTROL_REG			RegAI[2]
#define AI_STATUS_REG			RegAI[3]
#define AI_DACRATE_REG			RegAI[4]
#define AI_BITRATE_REG			RegAI[5]

#define PI_DRAM_ADDR_REG		RegPI[0]
#define PI_CART_ADDR_REG		RegPI[1]
#define PI_RD_LEN_REG			RegPI[2]
#define PI_WR_LEN_REG			RegPI[3]
#define PI_STATUS_REG			RegPI[4]
#define PI_BSD_DOM1_LAT_REG 	RegPI[5]
#define PI_DOMAIN1_REG		 	RegPI[5]
#define PI_BSD_DOM1_PWD_REG	 	RegPI[6]
#define PI_BSD_DOM1_PGS_REG	 	RegPI[7]
#define PI_BSD_DOM1_RLS_REG	 	RegPI[8]
#define PI_BSD_DOM2_LAT_REG	 	RegPI[9]
#define PI_DOMAIN2_REG		 	RegPI[9]
#define PI_BSD_DOM2_PWD_REG	 	RegPI[10]
#define PI_BSD_DOM2_PGS_REG	 	RegPI[11]
#define PI_BSD_DOM2_RLS_REG	 	RegPI[12]

#define RI_MODE_REG				RegRI[0]
#define RI_CONFIG_REG			RegRI[1]
#define RI_SELECT_REG			RegRI[3]
#define RI_REFRESH_REG			RegRI[4]
#define RI_LATENCY_REG			RegRI[5]
#define RI_RERROR_REG			RegRI[6]
#define RI_BANK_STATUS_REG		RegRI[7]

#define SI_DRAM_ADDR_REG		RegSI[0]
#define SI_PIF_ADDR_RD64B_REG	RegSI[1]
#define SI_PIF_ADDR_WR64B_REG	RegSI[2]
#define SI_STATUS_REG			RegSI[3]

#define RDRAM_DEVICE_TYPE_COLUMN_BITS	0xBU
#define RDRAM_DEVICE_TYPE_BN			1U
#define RDRAM_DEVICE_TYPE_EN			0U
#define RDRAM_DEVICE_TYPE_BANK_BITS		1U
#define RDRAM_DEVICE_TYPE_ROW_BITS		9U
#define RDRAM_DEVICE_TYPE_VERSION		1U
#define RDRAM_DEVICE_TYPE_TYPE			0U

#define RDRAM_DELAY_ACKWINBITS	(3 << 24)
#define RDRAM_DELAY_READBITS	(3 << 16)
#define RDRAM_DELAY_ACKBITS		(2 << 8)
#define RDRAM_DELAY_WRITEBITS	(3)
#define RDRAM_DELAY_FIXED_VALUE (RDRAM_DELAY_ACKWINBITS | RDRAM_DELAY_READBITS | RDRAM_DELAY_ACKBITS | RDRAM_DELAY_WRITEBITS)
#define RDRAM_DELAY_FIXED_VALUE_MASK 0x07070707

#define RDRAM_MODE_CC			0x80000000
#define RDRAM_MODE_X2			0x40000000
#define RDRAM_MODE_DE			0x02000000
#define RDRAM_MODE_C_MASK		0x00C0C0C0

#define RDRAM_DEVICE_MANUFACTURER_NEC 0x500

#define STATUS_IE				0x00000001
#define STATUS_EXL				0x00000002
#define STATUS_ERL				0x00000004
#define STATUS_KSU              0x00000018
#define STATUS_KERNEL           0x00000000
#define STATUS_SUPERVISOR       0x00000008
#define STATUS_USER             0x00000010
#define STATUS_UX               0x00000020
#define STATUS_SX               0x00000040
#define STATUS_KX               0x00000080
#define STATUS_SR				0x00100000	// Soft Reset Signal caused exception
#define STATUS_BEV				0x00400000
#define STATUS_FR				0x04000000
#define STATUS_CU0				0x10000000
#define STATUS_CU1				0x20000000
#define STATUS_CU2				0x40000000

#define CAUSE_EXC_CODE			0xFF
#define CAUSE_IP0				0x100
#define CAUSE_IP1				0x200
#define CAUSE_IP2				0x400
#define CAUSE_IP3				0x800
#define CAUSE_IP4				0x1000
#define CAUSE_IP5				0x2000
#define CAUSE_IP6				0x4000
#define CAUSE_IP7				0x8000
#define CAUSE_BD				0x80000000

#define	PI_STATUS_DMA_BUSY		0x01
#define	PI_STATUS_IO_BUSY		0x02
#define	PI_STATUS_ERROR			0x04
#define PI_STATUS_INTR			0x08

#define	PI_SET_RESET			0x01
#define	PI_CLR_INTR				0x02

#define	SI_STATUS_DMA_BUSY		0x0001
#define	SI_STATUS_RD_BUSY		0x0002
#define	SI_STATUS_DMA_ERROR		0x0008
#define	SI_STATUS_INTERRUPT		0x1000

#define FPCSR_FS				0x01000000	/* flush denorm to zero */
#define	FPCSR_C					0x00800000	/* condition bit */	
#define	FPCSR_CE				0x00020000	/* cause: unimplemented operation */
#define	FPCSR_CV				0x00010000	/* cause: invalid operation */
#define	FPCSR_CZ				0x00008000	/* cause: division by zero */
#define	FPCSR_CO				0x00004000	/* cause: overflow */
#define	FPCSR_CU				0x00002000	/* cause: underflow */
#define	FPCSR_CI				0x00001000	/* cause: inexact operation */
#define	FPCSR_EV				0x00000800	/* enable: invalid operation */
#define	FPCSR_EZ				0x00000400	/* enable: division by zero */
#define	FPCSR_EO				0x00000200	/* enable: overflow */
#define	FPCSR_EU				0x00000100	/* enable: underflow */
#define	FPCSR_EI				0x00000080	/* enable: inexact operation */
#define	FPCSR_FV				0x00000040	/* flag: invalid operation */
#define	FPCSR_FZ				0x00000020	/* flag: division by zero */
#define	FPCSR_FO				0x00000010	/* flag: overflow */
#define	FPCSR_FU				0x00000008	/* flag: underflow */
#define	FPCSR_FI				0x00000004	/* flag: inexact operation */
#define	FPCSR_RM_MASK			0x00000003	/* rounding mode mask */
#define	FPCSR_RM_RN				0x00000000	/* round to nearest */
#define	FPCSR_RM_RZ				0x00000001	/* round to zero */
#define	FPCSR_RM_RP				0x00000002	/* round to positive infinity */
#define	FPCSR_RM_RM				0x00000003	/* round to negative infinity */

#define FPR_Type(Reg)	(Reg) == R4300i_COP1_S ? "s" : (Reg) == R4300i_COP1_D ? "d" :\
						(Reg) == R4300i_COP1_W ? "w" : "l"

typedef struct {
	MIPS_DWORD PROGRAM_COUNTER;
    MIPS_DWORD GPR[32];
	MIPS_DWORD FPR[32];
	MIPS_DWORD CP0[32];
	DWORD      FPCR[32];
	MIPS_DWORD HI;
	MIPS_DWORD LO;
	DWORD      RDRAM[4][10];
	DWORD      SP[10];
	DWORD      DPC[10];
	DWORD      MI[4];
	DWORD      VI[14];
	DWORD      AI[6];
	DWORD      PI[13];
	DWORD      RI[8];
	DWORD      SI[4];
	BYTE       PIF_Ram[0x40];
	int        DMAUsed;
} N64_REGISTERS;

extern char *GPR_Name[32], *GPR_NameHi[32], *GPR_NameLo[32], *FPR_Name[32], *FPR_NameHi[32],
	*FPR_NameLo[32],*FPR_Ctrl_Name[32],*Cop0_Name[32];
extern DWORD *FPCR,*RegVI,*RegAI,*RegPI,
	*RegRI,*RegSI, HalfLine, RegModValue, ViFieldSerration, LLBit;
extern DWORD (*RegRDRAM)[4][10];
extern void* FPRDoubleLocation[32], * FPRFloatLoadStoreLocation[32], *FPRFloatUpperHalfLocation[32], *FPRFloatFSLocation[32];
extern void* FPRFloatOtherLocation[32], *FPRDoubleFTFDLocation[32];
extern MIPS_DWORD PROGRAM_COUNTER, *GPR, *FPR, HI, LO, *CP0;
extern N64_REGISTERS Registers;
extern int lastUnusedCOP0Register;
extern MIPS_DWORD cop2LatchedValue;

enum FPU_Format {
	FPU_Unkown,FPU_Dword, FPU_Qword, FPU_Float, FPU_Double
};

enum FPU_RoundingModel {
	RoundUnknown, RoundDefault, RoundTruncate, RoundNearest, RoundDown, RoundUp
};

void ChangeFPURegFormat       ( BLOCK_SECTION * Section, int Reg, int OldFormat, int NewFormat, int RoundingModel );
void ChangeMiIntrMask         ( void );
void ChangeMiModeReg          ( void );
void ChangeSpStatus           ( void );
void ChangeDpcStatus          ( void );
void InitalizeR4300iRegisters ( int UsePif, enum CIC_CHIP CIC_Chip );
BOOL Is8BitReg                ( int x86Reg);
void Load_FPR_ToTop           ( BLOCK_SECTION * Section, int Reg, int RegToLoad, int Format);
void Map_GPR_32bit            ( BLOCK_SECTION * Section, int Reg, BOOL SignValue, int MipsRegToLoad );
void Map_GPR_64bit            ( BLOCK_SECTION * Section, int Reg, int MipsRegToLoad );
int  Map_MemoryStack          ( BLOCK_SECTION * Section, BOOL AutoMap );
int  Map_TempReg              ( BLOCK_SECTION * Section, int x86Reg, int MipsReg, BOOL LoadHiWord );
BOOL RegInStack               ( BLOCK_SECTION * Section, int Reg, int Format );
void ProtectGPR               ( BLOCK_SECTION * Section, DWORD Reg );
void SetFpuLocations          ( void );
void SetupRegisters           ( N64_REGISTERS * n64_Registers );
int  StackPosition            ( BLOCK_SECTION * Section, int Reg );
void UnMap_AllFPRs            ( BLOCK_SECTION * Section );
void UnMap_FPR                ( BLOCK_SECTION * Section, int Reg, int WriteBackValue );
void UnMap_GPR                ( BLOCK_SECTION * Section, DWORD Reg, int WriteBackValue );
BOOL UnMap_X86reg             ( BLOCK_SECTION * Section, DWORD x86Reg );
void UnProtectGPR             ( BLOCK_SECTION * Section, DWORD Reg );
void UpdateCurrentHalfLine    ( void );
void UpdateFieldSerration     ( int interlaced );
void WriteBackRegisters       ( BLOCK_SECTION * Section );

BOOL IsSignExtended(MIPS_DWORD v);

#define NAN_S 0x7FBFFFFF
#define NAN_D 0x7FF7FFFFFFFFFFFF

#define EXPONENT_MASK_S 0x7F800000
#define MANTISSA_MASK_S 0x007FFFFF
#define QNAN_MASK_S     0x7FC00000

#define IsSubNormal_S(v) ((v & EXPONENT_MASK_S) == 0 && (v & MANTISSA_MASK_S) != 0)
#define IsNAN_S(v) ((v & EXPONENT_MASK_S) == EXPONENT_MASK_S && (v & MANTISSA_MASK_S) != 0)
#define IsQNAN_S(v) ((v & QNAN_MASK_S) == QNAN_MASK_S)

#define EXPONENT_MASK_D 0x7FF0000000000000LL
#define MANTISSA_MASK_D 0x000FFFFFFFFFFFFFLL
#define QNAN_MASK_D     0x7FF8000000000000LL

#define IsSubNormal_D(v) ((v & EXPONENT_MASK_D) == 0LL && (v & MANTISSA_MASK_D) != 0LL)
#define IsNAN_D(v) ((v & EXPONENT_MASK_D) == EXPONENT_MASK_D && (v & MANTISSA_MASK_D) != 0LL)
#define IsQNAN_D(v) ((v & QNAN_MASK_D) == QNAN_MASK_D)
