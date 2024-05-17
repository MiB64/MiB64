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

#include "../Types.h"

#define SP_MEM_ADDR_REGW		RegSPW[0]
#define SP_DRAM_ADDR_REGW		RegSPW[1]
#define SP_MEM_ADDR_REG			RegSP[0]
#define SP_DRAM_ADDR_REG		RegSP[1]
#define SP_RD_LEN_REG			RegSP[2]
#define SP_WR_LEN_REG			RegSP[3]
#define SP_STATUS_REG			RegSP[4]
#define SP_DMA_FULL_REG			RegSP[5]
#define SP_DMA_BUSY_REG			RegSP[6]
#define SP_SEMAPHORE_REG		RegSP[7]
#define SP_PC_REG				RegSP[8]
#define SP_IBIST_REG			RegSP[9]

#define SP_CLR_HALT				0x00001	    /* Bit  0: clear halt */
#define SP_SET_HALT				0x00002	    /* Bit  1: set halt */
#define SP_CLR_BROKE			0x00004	    /* Bit  2: clear broke */
#define SP_CLR_INTR				0x00008	    /* Bit  3: clear intr */
#define SP_SET_INTR				0x00010	    /* Bit  4: set intr */
#define SP_CLR_SSTEP			0x00020	    /* Bit  5: clear sstep */
#define SP_SET_SSTEP			0x00040	    /* Bit  6: set sstep */
#define SP_CLR_INTR_BREAK		0x00080	    /* Bit  7: clear intr on break */
#define SP_SET_INTR_BREAK		0x00100	    /* Bit  8: set intr on break */
#define SP_CLR_SIG0				0x00200	    /* Bit  9: clear signal 0 */
#define SP_SET_SIG0				0x00400	    /* Bit 10: set signal 0 */
#define SP_CLR_SIG1				0x00800	    /* Bit 11: clear signal 1 */
#define SP_SET_SIG1				0x01000	    /* Bit 12: set signal 1 */
#define SP_CLR_SIG2				0x02000	    /* Bit 13: clear signal 2 */
#define SP_SET_SIG2				0x04000	    /* Bit 14: set signal 2 */
#define SP_CLR_SIG3				0x08000	    /* Bit 15: clear signal 3 */
#define SP_SET_SIG3				0x10000	    /* Bit 16: set signal 3 */
#define SP_CLR_SIG4				0x20000	    /* Bit 17: clear signal 4 */
#define SP_SET_SIG4				0x40000	    /* Bit 18: set signal 4 */
#define SP_CLR_SIG5				0x80000	    /* Bit 19: clear signal 5 */
#define SP_SET_SIG5				0x100000	/* Bit 20: set signal 5 */
#define SP_CLR_SIG6				0x200000	/* Bit 21: clear signal 6 */
#define SP_SET_SIG6				0x400000	/* Bit 22: set signal 6 */
#define SP_CLR_SIG7				0x800000	/* Bit 23: clear signal 7 */
#define SP_SET_SIG7				0x1000000   /* Bit 24: set signal 7 */

#define SP_STATUS_HALT			0x001		/* Bit  0: halt */
#define SP_STATUS_BROKE			0x002		/* Bit  1: broke */
#define SP_STATUS_DMA_BUSY		0x004		/* Bit  2: dma busy */
#define SP_STATUS_DMA_FULL		0x008		/* Bit  3: dma full */
#define SP_STATUS_IO_FULL		0x010		/* Bit  4: io full */
#define SP_STATUS_SSTEP			0x020		/* Bit  5: single step */
#define SP_STATUS_INTR_BREAK	0x040		/* Bit  6: interrupt on break */
#define SP_STATUS_SIG0			0x080		/* Bit  7: signal 0 set */
#define SP_STATUS_SIG1			0x100		/* Bit  8: signal 1 set */
#define SP_STATUS_SIG2			0x200		/* Bit  9: signal 2 set */
#define SP_STATUS_SIG3			0x400		/* Bit 10: signal 3 set */
#define SP_STATUS_SIG4			0x800		/* Bit 11: signal 4 set */
#define SP_STATUS_SIG5	       0x1000		/* Bit 12: signal 5 set */
#define SP_STATUS_SIG6	       0x2000		/* Bit 13: signal 6 set */
#define SP_STATUS_SIG7	       0x4000		/* Bit 14: signal 7 set */

extern DWORD* RegSP;
extern DWORD RegSPW[2];

/*extern char* x86_Strings[8];*/
extern char* RspGPR_Strings[32];

/*#define x86_Name(Reg) (x86_Strings[(Reg)])*/
#define RspGPR_Name(Reg) (RspGPR_Strings[(Reg)])

#define RspCOP0_Name(Reg)\
	(Reg) == 0  ? "SP memory address" :\
	(Reg) == 1  ? "SP DRAM DMA address" :\
	(Reg) == 2  ? "SP read DMA length" :\
	(Reg) == 3  ? "SP write DMA length" :\
	(Reg) == 4  ? "SP status" :\
	(Reg) == 5  ? "SP DMA full" :\
	(Reg) == 6  ? "SP DMA busy" :\
	(Reg) == 7  ? "SP semaphore" :\
	(Reg) == 8  ? "DP CMD DMA start" :\
	(Reg) == 9  ? "DP CMD DMA end" :\
	(Reg) == 10 ? "DP CMD DMA current" :\
	(Reg) == 11 ? "DP CMD status" :\
	(Reg) == 12 ? "DP clock counter" :\
	(Reg) == 13 ? "DP buffer busy counter" :\
	(Reg) == 14 ? "DP pipe busy counter" :\
	(Reg) == 15 ? "DP TMEM load counter" :\
	"Unknown Register"

#define RspElementSpecifier(Elem)\
	(Elem) == 0  ? "" : (Elem) == 1  ? "" : (Elem) == 2  ? " [0q]" :\
	(Elem) == 3  ? " [1q]" : (Elem) == 4  ? " [0h]" : (Elem) == 5  ? " [1h]" :\
    (Elem) == 6  ? " [2h]" : (Elem) == 7  ? " [3h]" : (Elem) == 8  ? " [0]" :\
	(Elem) == 9  ? " [1]" : (Elem) == 10 ? " [2]" : (Elem) == 11 ? " [3]" :\
	(Elem) == 12 ? " [4]" : (Elem) == 13 ? " [5]" : (Elem) == 14 ? " [6]" :\
	(Elem) == 15 ? " [7]" : "Unknown Element"

void Enter_RSP_Register_Window(void);
void InitilizeRSPRegisters(void);
void UpdateRSPRegistersScreen(void);

/*** RSP Registers ***/
extern MIPS_WORD   RSP_GPR[32]/*, RSP_Flags[4]*/;
/*extern UDWORD  RSP_ACCUM[8];
extern VECTOR  RSP_Vect[32];*/

void WriteRspStatusRegister(DWORD Value);
