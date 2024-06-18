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
#include "RSP.h"
#include "CPU.h"
#include "Recompiler CPU.h"
#include "RSP Command.h"
#include "RSP Registers.h"
#include "memory.h"
#include "dma.h"
#include "log.h"
#include "x86.h"

void RSP_Sections_VMUDH ( OPCODE RspOp, DWORD AccumStyle ) {
	char Reg[256];*/

	/*****************************************
	** VMUDH 
	**  - affects the upper 32-bits
	******************************************/

/*	if (AccumStyle == Low16BitAccum) {
		MmxXorRegToReg(x86_MM0, x86_MM0);
		MmxXorRegToReg(x86_MM1, x86_MM1);
		return;
	}

	RSPOpC = RspOp;*/

	/**** Load source registers ****/
/*	sprintf(Reg, "RSP_Vect[%i].HW[0]", RspOp.rd);
	MmxMoveQwordVariableToReg(x86_MM0, &RSP_Vect[RspOp.rd].HW[0], Reg);
	sprintf(Reg, "RSP_Vect[%i].HW[4]", RspOp.rd);
	MmxMoveQwordVariableToReg(x86_MM1, &RSP_Vect[RspOp.rd].HW[4], Reg);*/

	/******* VMUDH *******/
/*	if ((RspOp.rs & 0x0f) < 2) {
		sprintf(Reg, "RSP_Vect[%i].HW[0]", RspOp.rt);
		MmxMoveQwordVariableToReg(x86_MM2, &RSP_Vect[RspOp.rt].HW[0], Reg);
		sprintf(Reg, "RSP_Vect[%i].HW[4]", RspOp.rt);
		MmxMoveQwordVariableToReg(x86_MM3, &RSP_Vect[RspOp.rt].HW[4], Reg);

		if (AccumStyle == Middle16BitAccum) {
			MmxPmullwRegToReg(x86_MM0, x86_MM2);
			MmxPmullwRegToReg(x86_MM1, x86_MM3);
		} else {
			MmxPmulhwRegToReg(x86_MM0, x86_MM2);
			MmxPmulhwRegToReg(x86_MM1, x86_MM3);
		}
	} else if ((RspOp.rs & 0x0f) >= 8) {
		RSP_Element2Mmx(x86_MM2);
		if (AccumStyle == Middle16BitAccum) {
			MmxPmullwRegToReg(x86_MM0, x86_MM2);
			MmxPmullwRegToReg(x86_MM1, x86_MM2);
		} else {
			MmxPmulhwRegToReg(x86_MM0, x86_MM2);
			MmxPmulhwRegToReg(x86_MM1, x86_MM2);
		}
	} else {
		RSP_MultiElement2Mmx(x86_MM2, x86_MM3);
		if (AccumStyle == Middle16BitAccum) {
			MmxPmullwRegToReg(x86_MM0, x86_MM2);
			MmxPmullwRegToReg(x86_MM1, x86_MM3);
		} else {
			MmxPmulhwRegToReg(x86_MM0, x86_MM2);
			MmxPmulhwRegToReg(x86_MM1, x86_MM3);
		}
	}
}

void RSP_Sections_VMADH ( OPCODE RspOp, DWORD AccumStyle ) {
	char Reg[256];*/

	/*****************************************
	** VMADH 
	**  - affects the upper 32-bits
	******************************************/

/*	if (AccumStyle == Low16BitAccum) {
		return;
	}

	RSPOpC = RspOp;*/

	/**** Load source registers ****/
/*	sprintf(Reg, "RSP_Vect[%i].HW[0]", RspOp.rd);
	MmxMoveQwordVariableToReg(x86_MM0 + 2, &RSP_Vect[RspOp.rd].HW[0], Reg);
	sprintf(Reg, "RSP_Vect[%i].HW[4]", RspOp.rd);
	MmxMoveQwordVariableToReg(x86_MM1 + 2, &RSP_Vect[RspOp.rd].HW[4], Reg);*/

	/******* VMUDH *******/
/*	if ((RspOp.rs & 0x0f) < 2) {
		sprintf(Reg, "RSP_Vect[%i].HW[0]", RspOp.rt);
		MmxMoveQwordVariableToReg(x86_MM2 + 2, &RSP_Vect[RspOp.rt].HW[0], Reg);
		sprintf(Reg, "RSP_Vect[%i].HW[4]", RspOp.rt);
		MmxMoveQwordVariableToReg(x86_MM3 + 2, &RSP_Vect[RspOp.rt].HW[4], Reg);

		if (AccumStyle == Middle16BitAccum) {
			MmxPmullwRegToReg(x86_MM0 + 2, x86_MM2 + 2);
			MmxPmullwRegToReg(x86_MM1 + 2, x86_MM3 + 2);
		} else {
			MmxPmulhwRegToReg(x86_MM0 + 2, x86_MM2 + 2);
			MmxPmulhwRegToReg(x86_MM1 + 2, x86_MM3 + 2);
		}
	} else if ((RspOp.rs & 0x0f) >= 8) {
		RSP_Element2Mmx(x86_MM2 + 2);
		if (AccumStyle == Middle16BitAccum) {
			MmxPmullwRegToReg(x86_MM0 + 2, x86_MM2 + 2);
			MmxPmullwRegToReg(x86_MM1 + 2, x86_MM2 + 2);
		} else {
			MmxPmulhwRegToReg(x86_MM0 + 2, x86_MM2 + 2);
			MmxPmulhwRegToReg(x86_MM1 + 2, x86_MM2 + 2);
		}
	} else {
		RSP_MultiElement2Mmx(x86_MM2 + 2, x86_MM3 + 2);
		if (AccumStyle == Middle16BitAccum) {
			MmxPmullwRegToReg(x86_MM0 + 2, x86_MM2 + 2);
			MmxPmullwRegToReg(x86_MM1 + 2, x86_MM3 + 2);
		} else {
			MmxPmulhwRegToReg(x86_MM0 + 2, x86_MM2 + 2);
			MmxPmulhwRegToReg(x86_MM1 + 2, x86_MM3 + 2);
		}
	}

	MmxPaddswRegToReg(x86_MM0, x86_MM0 + 2);
	MmxPaddswRegToReg(x86_MM1, x86_MM1 + 2);
}

void RSP_Sections_VMUDL ( OPCODE RspOp, DWORD AccumStyle ) {
	char Reg[256];*/

	/*****************************************
	** VMUDL 
	**  - affects the lower 16-bits
	******************************************/

/*	if (AccumStyle != Low16BitAccum) {
		MmxXorRegToReg(x86_MM0, x86_MM0);
		MmxXorRegToReg(x86_MM1, x86_MM1);
		return;
	}

	RSPOpC = RspOp;*/

	/**** Load source registers ****/
/*	sprintf(Reg, "RSP_Vect[%i].HW[0]", RspOp.rd);
	MmxMoveQwordVariableToReg(x86_MM0, &RSP_Vect[RspOp.rd].HW[0], Reg);
	sprintf(Reg, "RSP_Vect[%i].HW[4]", RspOp.rd);
	MmxMoveQwordVariableToReg(x86_MM1, &RSP_Vect[RspOp.rd].HW[4], Reg);*/

	/******* VMUDL *******/
/*	if ((RspOp.rs & 0x0f) < 2) {
		sprintf(Reg, "RSP_Vect[%i].HW[0]", RspOp.rt);
		MmxMoveQwordVariableToReg(x86_MM2, &RSP_Vect[RspOp.rt].HW[0], Reg);
		sprintf(Reg, "RSP_Vect[%i].HW[4]", RspOp.rt);
		MmxMoveQwordVariableToReg(x86_MM3, &RSP_Vect[RspOp.rt].HW[4], Reg);

		MmxPmullwRegToReg(x86_MM0, x86_MM2);
		MmxPmullwRegToReg(x86_MM1, x86_MM3);
	} else if ((RspOp.rs & 0x0f) >= 8) {
		RSP_Element2Mmx(x86_MM2);
		MmxPmullwRegToReg(x86_MM0, x86_MM2);
		MmxPmullwRegToReg(x86_MM1, x86_MM2);
	} else {
		RSP_MultiElement2Mmx(x86_MM2, x86_MM3);
		MmxPmullwRegToReg(x86_MM0, x86_MM2);
		MmxPmullwRegToReg(x86_MM1, x86_MM3);
	}
}

void RSP_Sections_VMADL ( OPCODE RspOp, DWORD AccumStyle ) {
	char Reg[256];*/

	/*****************************************
	** VMADL
	**  - affects the lower 16-bits
	******************************************/

/*	if (AccumStyle != Low16BitAccum) {
		return;
	}

	RSPOpC = RspOp;*/

	/**** Load source registers ****/
/*	sprintf(Reg, "RSP_Vect[%i].HW[0]", RspOp.rd);
	MmxMoveQwordVariableToReg(x86_MM0 + 2, &RSP_Vect[RspOp.rd].HW[0], Reg);
	sprintf(Reg, "RSP_Vect[%i].HW[4]", RspOp.rd);
	MmxMoveQwordVariableToReg(x86_MM1 + 2, &RSP_Vect[RspOp.rd].HW[4], Reg);*/

	/******* VMADL *******/
/*	if ((RspOp.rs & 0x0f) < 2) {
		sprintf(Reg, "RSP_Vect[%i].HW[0]", RspOp.rt);
		MmxMoveQwordVariableToReg(x86_MM2 + 2, &RSP_Vect[RspOp.rt].HW[0], Reg);
		sprintf(Reg, "RSP_Vect[%i].HW[4]", RspOp.rt);
		MmxMoveQwordVariableToReg(x86_MM3 + 2, &RSP_Vect[RspOp.rt].HW[4], Reg);

		MmxPmullwRegToReg(x86_MM0 + 2, x86_MM2 + 2);
		MmxPmullwRegToReg(x86_MM1 + 2, x86_MM3 + 2);
	} else if ((RspOp.rs & 0x0f) >= 8) {
		RSP_Element2Mmx(x86_MM2);
		MmxPmullwRegToReg(x86_MM0 + 2, x86_MM2 + 2);
		MmxPmullwRegToReg(x86_MM1 + 2, x86_MM2 + 2);
	} else {
		RSP_MultiElement2Mmx(x86_MM2 + 2, x86_MM3 + 2);
		MmxPmullwRegToReg(x86_MM0 + 2, x86_MM2 + 2);
		MmxPmullwRegToReg(x86_MM1 + 2, x86_MM3 + 2);
	}

	MmxPaddswRegToReg(x86_MM0, x86_MM0 + 2);
	MmxPaddswRegToReg(x86_MM1, x86_MM1 + 2);
}

void RSP_Sections_VMUDM ( OPCODE RspOp, DWORD AccumStyle ) {
	char Reg[256];*/

	/*****************************************
	** VMUDM
	**  - affects the middle 32-bits, s16*u16
	******************************************/

/*	if (AccumStyle == High16BitAccum) {
		MmxXorRegToReg(x86_MM0, x86_MM0);
		MmxXorRegToReg(x86_MM1, x86_MM1);
		return;
	}

	RSPOpC = RspOp;*/

	/**** Load source registers ****/
/*	sprintf(Reg, "RSP_Vect[%i].HW[0]", RspOp.rd);
	MmxMoveQwordVariableToReg(x86_MM0, &RSP_Vect[RspOp.rd].HW[0], Reg);
	sprintf(Reg, "RSP_Vect[%i].HW[4]", RspOp.rd);
	MmxMoveQwordVariableToReg(x86_MM1, &RSP_Vect[RspOp.rd].HW[4], Reg);*/

	/******* VMUDM *******/
/*	if (AccumStyle != Middle16BitAccum) {
		if ((RspOp.rs & 0x0f) < 2) {
			sprintf(Reg, "RSP_Vect[%i].HW[0]", RspOp.rt);
			MmxMoveQwordVariableToReg(x86_MM2, &RSP_Vect[RspOp.rt].HW[0], Reg);
			sprintf(Reg, "RSP_Vect[%i].HW[4]", RspOp.rt);
			MmxMoveQwordVariableToReg(x86_MM3, &RSP_Vect[RspOp.rt].HW[4], Reg);

			MmxPmullwRegToReg(x86_MM0, x86_MM2);
			MmxPmullwRegToReg(x86_MM1, x86_MM3);
		} else if ((RspOp.rs & 0x0f) >= 8) {
			RSP_Element2Mmx(x86_MM2);
			MmxPmullwRegToReg(x86_MM0, x86_MM2);
			MmxPmullwRegToReg(x86_MM1, x86_MM2);
		} else {
			RSP_MultiElement2Mmx(x86_MM2, x86_MM3);
			MmxPmullwRegToReg(x86_MM0, x86_MM2);
			MmxPmullwRegToReg(x86_MM1, x86_MM3);
		}
	} else {
		if ((RSPOpC.rs & 0xF) < 2) {
			sprintf(Reg, "RSP_Vect[%i].UHW[0]", RSPOpC.rt);
			MmxMoveQwordVariableToReg(x86_MM4, &RSP_Vect[RSPOpC.rt].UHW[0], Reg);
			sprintf(Reg, "RSP_Vect[%i].UHW[4]", RSPOpC.rt);
			MmxMoveQwordVariableToReg(x86_MM5, &RSP_Vect[RSPOpC.rt].UHW[4], Reg);*/

			/* Copy the signed portion */
/*			MmxMoveRegToReg(x86_MM2, x86_MM0);
			MmxMoveRegToReg(x86_MM3, x86_MM1);*/

			/* high((u16)a * b) */
/*			MmxPmulhuwRegToReg(x86_MM0, x86_MM4);
			MmxPmulhuwRegToReg(x86_MM1, x86_MM5);*/

			/* low((a >> 15) * b) */
/*			MmxPsrawImmed(x86_MM2, 15);
			MmxPsrawImmed(x86_MM3, 15);
			MmxPmullwRegToReg(x86_MM2, x86_MM4);
			MmxPmullwRegToReg(x86_MM3, x86_MM5);
		} else if ((RSPOpC.rs & 0xF) >= 8) {
			RSP_Element2Mmx(x86_MM4);*/

			/* Copy the signed portion */
/*			MmxMoveRegToReg(x86_MM2, x86_MM0);
			MmxMoveRegToReg(x86_MM3, x86_MM1);*/

			/* high((u16)a * b) */
/*			MmxPmulhuwRegToReg(x86_MM0, x86_MM4);
			MmxPmulhuwRegToReg(x86_MM1, x86_MM4);*/

			/* low((a >> 15) * b) */
/*			MmxPsrawImmed(x86_MM2, 15);
			MmxPsrawImmed(x86_MM3, 15);
			MmxPmullwRegToReg(x86_MM2, x86_MM4);
			MmxPmullwRegToReg(x86_MM3, x86_MM4);
		} else {
			RSP_MultiElement2Mmx(x86_MM4, x86_MM5);*/

			/* Copy the signed portion */
/*			MmxMoveRegToReg(x86_MM2, x86_MM0);
			MmxMoveRegToReg(x86_MM3, x86_MM1);*/

			/* high((u16)a * b) */
/*			MmxPmulhuwRegToReg(x86_MM0, x86_MM4);
			MmxPmulhuwRegToReg(x86_MM1, x86_MM5);*/

			/* low((a >> 15) * b) */
/*			MmxPsrawImmed(x86_MM2, 15);
			MmxPsrawImmed(x86_MM3, 15);
			MmxPmullwRegToReg(x86_MM2, x86_MM4);
			MmxPmullwRegToReg(x86_MM3, x86_MM5);
		}*/

		/* Add them up */
/*		MmxPaddwRegToReg(x86_MM0, x86_MM2);
		MmxPaddwRegToReg(x86_MM1, x86_MM3);
	}
}

void RSP_Sections_VMADM ( OPCODE RspOp, DWORD AccumStyle ) {
	char Reg[256];*/

	/*****************************************
	** VMADM
	**  - affects the middle 32-bits, s16*u16
	******************************************/

/*	if (AccumStyle == High16BitAccum) {
		MmxXorRegToReg(x86_MM0, x86_MM0);
		MmxXorRegToReg(x86_MM1, x86_MM1);
		return;
	}

	RSPOpC = RspOp;*/

	/**** Load source registers ****/
/*	sprintf(Reg, "RSP_Vect[%i].HW[0]", RspOp.rd);
	MmxMoveQwordVariableToReg(x86_MM0 + 2, &RSP_Vect[RspOp.rd].HW[0], Reg);
	sprintf(Reg, "RSP_Vect[%i].HW[4]", RspOp.rd);
	MmxMoveQwordVariableToReg(x86_MM1 + 2, &RSP_Vect[RspOp.rd].HW[4], Reg);*/

	/******* VMADM *******/
/*	if (AccumStyle != Middle16BitAccum) {
		if ((RspOp.rs & 0x0f) < 2) {
			sprintf(Reg, "RSP_Vect[%i].HW[0]", RspOp.rt);
			MmxMoveQwordVariableToReg(x86_MM2 + 2, &RSP_Vect[RspOp.rt].HW[0], Reg);
			sprintf(Reg, "RSP_Vect[%i].HW[4]", RspOp.rt);
			MmxMoveQwordVariableToReg(x86_MM3 + 2, &RSP_Vect[RspOp.rt].HW[4], Reg);

			MmxPmullwRegToReg(x86_MM0 + 2, x86_MM2 + 2);
			MmxPmullwRegToReg(x86_MM1 + 2, x86_MM3 + 2);
		} else if ((RspOp.rs & 0x0f) >= 8) {
			RSP_Element2Mmx(x86_MM2 + 2);
			MmxPmullwRegToReg(x86_MM0 + 2, x86_MM2 + 2);
			MmxPmullwRegToReg(x86_MM1 + 2, x86_MM2 + 2);
		} else {
			RSP_MultiElement2Mmx(x86_MM2 + 2, x86_MM3 + 2);
			MmxPmullwRegToReg(x86_MM0 + 2, x86_MM2 + 2);
			MmxPmullwRegToReg(x86_MM1 + 2, x86_MM3 + 2);
		}
	} else {
		if ((RSPOpC.rs & 0xF) < 2) {
			sprintf(Reg, "RSP_Vect[%i].UHW[0]", RSPOpC.rt);
			MmxMoveQwordVariableToReg(x86_MM4 + 2, &RSP_Vect[RSPOpC.rt].UHW[0], Reg);
			sprintf(Reg, "RSP_Vect[%i].UHW[4]", RSPOpC.rt);
			MmxMoveQwordVariableToReg(x86_MM5 + 2, &RSP_Vect[RSPOpC.rt].UHW[4], Reg);*/

			/* Copy the signed portion */
/*			MmxMoveRegToReg(x86_MM2 + 2, x86_MM0 + 2);
			MmxMoveRegToReg(x86_MM3 + 2, x86_MM1 + 2);*/

			/* high((u16)a * b) */
/*			MmxPmulhuwRegToReg(x86_MM0 + 2, x86_MM4 + 2);
			MmxPmulhuwRegToReg(x86_MM1 + 2, x86_MM5 + 2);*/

			/* low((a >> 15) * b) */
/*			MmxPsrawImmed(x86_MM2 + 2, 15);
			MmxPsrawImmed(x86_MM3 + 2, 15);
			MmxPmullwRegToReg(x86_MM2 + 2, x86_MM4 + 2);
			MmxPmullwRegToReg(x86_MM3 + 2, x86_MM5 + 2);
		} else if ((RSPOpC.rs & 0xF) >= 8) {
			RSP_Element2Mmx(x86_MM4 + 2);*/

			/* Copy the signed portion */
/*			MmxMoveRegToReg(x86_MM2 + 2, x86_MM0 + 2);
			MmxMoveRegToReg(x86_MM3 + 2, x86_MM1 + 2);*/

			/* high((u16)a * b) */
/*			MmxPmulhuwRegToReg(x86_MM0 + 2, x86_MM4 + 2);
			MmxPmulhuwRegToReg(x86_MM1 + 2, x86_MM4 + 2);*/

			/* low((a >> 15) * b) */
/*			MmxPsrawImmed(x86_MM2 + 2, 15);
			MmxPsrawImmed(x86_MM3 + 2, 15);
			MmxPmullwRegToReg(x86_MM2 + 2, x86_MM4 + 2);
			MmxPmullwRegToReg(x86_MM3 + 2, x86_MM4 + 2);
		} else {
			RSP_MultiElement2Mmx(x86_MM4 + 2, x86_MM5 + 2);*/

			/* Copy the signed portion */
/*			MmxMoveRegToReg(x86_MM2 + 2, x86_MM0 + 2);
			MmxMoveRegToReg(x86_MM3 + 2, x86_MM1 + 2);*/

			/* high((u16)a * b) */
/*			MmxPmulhuwRegToReg(x86_MM0 + 2, x86_MM4 + 2);
			MmxPmulhuwRegToReg(x86_MM1 + 2, x86_MM5 + 2);*/

			/* low((a >> 15) * b) */
/*			MmxPsrawImmed(x86_MM2 + 2, 15);
			MmxPsrawImmed(x86_MM3 + 2, 15);
			MmxPmullwRegToReg(x86_MM2 + 2, x86_MM4 + 2);
			MmxPmullwRegToReg(x86_MM3 + 2, x86_MM5 + 2);
		}*/

		/* Add them up */
/*		MmxPaddwRegToReg(x86_MM0 + 2, x86_MM2 + 2);
		MmxPaddwRegToReg(x86_MM1 + 2, x86_MM3 + 2);
	}

	MmxPaddswRegToReg(x86_MM0, x86_MM0 + 2);
	MmxPaddswRegToReg(x86_MM1, x86_MM1 + 2);
}

void RSP_Sections_VMUDN ( OPCODE RspOp, DWORD AccumStyle ) {
	char Reg[256];*/

	/*****************************************
	** VMUDN
	**  - affects the middle 32-bits, u16*s16
	******************************************/

/*	if (AccumStyle == High16BitAccum) {
		MmxXorRegToReg(x86_MM0, x86_MM0);
		MmxXorRegToReg(x86_MM1, x86_MM1);
		return;
	}

	RSPOpC = RspOp;*/

	/******* VMUDN *******/
/*	if (AccumStyle != Middle16BitAccum) {*/

		/**** Load source registers ****/
/*		sprintf(Reg, "RSP_Vect[%i].HW[0]", RspOp.rd);
		MmxMoveQwordVariableToReg(x86_MM0, &RSP_Vect[RspOp.rd].HW[0], Reg);
		sprintf(Reg, "RSP_Vect[%i].HW[4]", RspOp.rd);
		MmxMoveQwordVariableToReg(x86_MM1, &RSP_Vect[RspOp.rd].HW[4], Reg);

		if ((RspOp.rs & 0x0f) < 2) {
			sprintf(Reg, "RSP_Vect[%i].HW[0]", RspOp.rt);
			MmxMoveQwordVariableToReg(x86_MM2, &RSP_Vect[RspOp.rt].HW[0], Reg);
			sprintf(Reg, "RSP_Vect[%i].HW[4]", RspOp.rt);
			MmxMoveQwordVariableToReg(x86_MM3, &RSP_Vect[RspOp.rt].HW[4], Reg);

			MmxPmullwRegToReg(x86_MM0, x86_MM2);
			MmxPmullwRegToReg(x86_MM1, x86_MM3);
		} else if ((RspOp.rs & 0x0f) >= 8) {
			RSP_Element2Mmx(x86_MM2);

			MmxPmullwRegToReg(x86_MM0, x86_MM2);
			MmxPmullwRegToReg(x86_MM1, x86_MM2);
		} else {
			RSP_MultiElement2Mmx(x86_MM2, x86_MM3);

			MmxPmullwRegToReg(x86_MM0, x86_MM2);
			MmxPmullwRegToReg(x86_MM1, x86_MM3);
		}
	} else {*/

		/***
		** NOTE: for code clarity, this is the same as VMUDM,
		** just the mmx registers are swapped, this is easier
		****/

		/**** Load source registers ****/
/*		sprintf(Reg, "RSP_Vect[%i].HW[0]", RspOp.rd);
		MmxMoveQwordVariableToReg(x86_MM4, &RSP_Vect[RspOp.rd].HW[0], Reg);
		sprintf(Reg, "RSP_Vect[%i].HW[4]", RspOp.rd);
		MmxMoveQwordVariableToReg(x86_MM5, &RSP_Vect[RspOp.rd].HW[4], Reg);

		if ((RSPOpC.rs & 0xF) < 2) {
			sprintf(Reg, "RSP_Vect[%i].UHW[0]", RSPOpC.rt);
			MmxMoveQwordVariableToReg(x86_MM0, &RSP_Vect[RSPOpC.rt].UHW[0], Reg);
			sprintf(Reg, "RSP_Vect[%i].UHW[4]", RSPOpC.rt);
			MmxMoveQwordVariableToReg(x86_MM1, &RSP_Vect[RSPOpC.rt].UHW[4], Reg);
		} else if ((RSPOpC.rs & 0xF) >= 8) {
			RSP_Element2Mmx(x86_MM0);
			MmxMoveRegToReg(x86_MM1, x86_MM0);
		} else {
			RSP_MultiElement2Mmx(x86_MM0, x86_MM1);
		}*/

		/* Copy the signed portion */
/*		MmxMoveRegToReg(x86_MM2, x86_MM0);
		MmxMoveRegToReg(x86_MM3, x86_MM1);*/

		/* high((u16)a * b) */
/*		MmxPmulhuwRegToReg(x86_MM0, x86_MM4);
		MmxPmulhuwRegToReg(x86_MM1, x86_MM5);*/

		/* low((a >> 15) * b) */
/*		MmxPsrawImmed(x86_MM2, 15);
		MmxPsrawImmed(x86_MM3, 15);
		MmxPmullwRegToReg(x86_MM2, x86_MM4);
		MmxPmullwRegToReg(x86_MM3, x86_MM5);*/

		/* Add them up */
/*		MmxPaddwRegToReg(x86_MM0, x86_MM2);
		MmxPaddwRegToReg(x86_MM1, x86_MM3);
	}
}

void RSP_Sections_VMADN ( OPCODE RspOp, DWORD AccumStyle ) {
	char Reg[256];*/

	/*****************************************
	** VMADN
	**  - affects the middle 32-bits, u16*s16
	******************************************/

/*	if (AccumStyle == High16BitAccum) {
		return;
	}

	RSPOpC = RspOp;*/

	/******* VMADN *******/
/*	if (AccumStyle != Middle16BitAccum) {*/
		/**** Load source registers ****/
/*		sprintf(Reg, "RSP_Vect[%i].HW[0]", RspOp.rd);
		MmxMoveQwordVariableToReg(x86_MM0 + 2, &RSP_Vect[RspOp.rd].HW[0], Reg);
		sprintf(Reg, "RSP_Vect[%i].HW[4]", RspOp.rd);
		MmxMoveQwordVariableToReg(x86_MM1 + 2, &RSP_Vect[RspOp.rd].HW[4], Reg);

		if ((RspOp.rs & 0x0f) < 2) {
			sprintf(Reg, "RSP_Vect[%i].HW[0]", RspOp.rt);
			MmxMoveQwordVariableToReg(x86_MM2 + 2, &RSP_Vect[RspOp.rt].HW[0], Reg);
			sprintf(Reg, "RSP_Vect[%i].HW[4]", RspOp.rt);
			MmxMoveQwordVariableToReg(x86_MM3 + 2, &RSP_Vect[RspOp.rt].HW[4], Reg);

			MmxPmullwRegToReg(x86_MM0 + 2, x86_MM2 + 2);
			MmxPmullwRegToReg(x86_MM1 + 2, x86_MM3 + 2);
		} else if ((RspOp.rs & 0x0f) >= 8) {
			RSP_Element2Mmx(x86_MM2 + 2);

			MmxPmullwRegToReg(x86_MM0 + 2, x86_MM2 + 2);
			MmxPmullwRegToReg(x86_MM1 + 2, x86_MM2 + 2);
		} else {
			RSP_MultiElement2Mmx(x86_MM2 + 2, x86_MM3 + 2);

			MmxPmullwRegToReg(x86_MM0 + 2, x86_MM2 + 2);
			MmxPmullwRegToReg(x86_MM1 + 2, x86_MM3 + 2);
		}
	} else {*/

		/*
		** NOTE: for code clarity, this is the same as VMADM,
		** just the mmx registers are swapped, this is easier
		*/

		/**** Load source registers ****/
/*		sprintf(Reg, "RSP_Vect[%i].HW[0]", RspOp.rd);
		MmxMoveQwordVariableToReg(x86_MM4 + 2, &RSP_Vect[RspOp.rd].HW[0], Reg);
		sprintf(Reg, "RSP_Vect[%i].HW[4]", RspOp.rd);
		MmxMoveQwordVariableToReg(x86_MM5 + 2, &RSP_Vect[RspOp.rd].HW[4], Reg);

		if ((RSPOpC.rs & 0xF) < 2) {
			sprintf(Reg, "RSP_Vect[%i].UHW[0]", RSPOpC.rt);
			MmxMoveQwordVariableToReg(x86_MM0 + 2, &RSP_Vect[RSPOpC.rt].UHW[0], Reg);
			sprintf(Reg, "RSP_Vect[%i].UHW[4]", RSPOpC.rt);
			MmxMoveQwordVariableToReg(x86_MM1 + 2, &RSP_Vect[RSPOpC.rt].UHW[4], Reg);
		} else if ((RSPOpC.rs & 0xF) >= 8) {
			RSP_Element2Mmx(x86_MM0 + 2);
			MmxMoveRegToReg(x86_MM1 + 2, x86_MM0 + 2);
		} else {
			RSP_MultiElement2Mmx(x86_MM0 + 2, x86_MM1 + 2);
		}*/

		/* Copy the signed portion */
/*		MmxMoveRegToReg(x86_MM2 + 2, x86_MM0 + 2);
		MmxMoveRegToReg(x86_MM3 + 2, x86_MM1 + 2);*/

		/* high((u16)a * b) */
/*		MmxPmulhuwRegToReg(x86_MM0 + 2, x86_MM4 + 2);
		MmxPmulhuwRegToReg(x86_MM1 + 2, x86_MM5 + 2);*/

		/* low((a >> 15) * b) */
/*		MmxPsrawImmed(x86_MM2 + 2, 15);
		MmxPsrawImmed(x86_MM3 + 2, 15);
		MmxPmullwRegToReg(x86_MM2 + 2, x86_MM4 + 2);
		MmxPmullwRegToReg(x86_MM3 + 2, x86_MM5 + 2);*/

		/* Add them up */
/*		MmxPaddwRegToReg(x86_MM0 + 2, x86_MM2 + 2);
		MmxPaddwRegToReg(x86_MM1 + 2, x86_MM3 + 2);
	}*/

	/* 
	** only thing is when we are responsible for clamping
	** so we adopt unsigned here?
	*/
/*	MmxPaddswRegToReg(x86_MM0, x86_MM0 + 2);
	MmxPaddswRegToReg(x86_MM1, x86_MM1 + 2);

}

void RSP_Sections_VMULF ( OPCODE RspOp, DWORD AccumStyle ) {
	char Reg[256];*/

	/*****************************************
	** VMULF 
	**  - affects the middle 32-bits, s16*s16*2
	******************************************/

/*	if (AccumStyle == High16BitAccum) {
		MmxXorRegToReg(x86_MM0, x86_MM0);
		MmxXorRegToReg(x86_MM1, x86_MM1);
		return;
	}

	RSPOpC = RspOp;*/

	/**** Load source registers ****/
/*	sprintf(Reg, "RSP_Vect[%i].HW[0]", RspOp.rd);
	MmxMoveQwordVariableToReg(x86_MM0, &RSP_Vect[RspOp.rd].HW[0], Reg);
	sprintf(Reg, "RSP_Vect[%i].HW[4]", RspOp.rd);
	MmxMoveQwordVariableToReg(x86_MM1, &RSP_Vect[RspOp.rd].HW[4], Reg);*/

	/******* VMULF *******/
/*	if ((RspOp.rs & 0x0f) < 2) {
		sprintf(Reg, "RSP_Vect[%i].HW[0]", RspOp.rt);
		MmxMoveQwordVariableToReg(x86_MM2, &RSP_Vect[RspOp.rt].HW[0], Reg);
		sprintf(Reg, "RSP_Vect[%i].HW[4]", RspOp.rt);
		MmxMoveQwordVariableToReg(x86_MM3, &RSP_Vect[RspOp.rt].HW[4], Reg);

		if (AccumStyle != Middle16BitAccum) {
			MmxPmullwRegToReg(x86_MM0, x86_MM2);
			MmxPmullwRegToReg(x86_MM1, x86_MM3);
		} else {
			MmxPmulhwRegToReg(x86_MM0, x86_MM2);
			MmxPmulhwRegToReg(x86_MM1, x86_MM3);
		}
	} else if ((RspOp.rs & 0x0f) >= 8) {
		RSP_Element2Mmx(x86_MM2);
		if (AccumStyle != Middle16BitAccum) {
			MmxPmullwRegToReg(x86_MM0, x86_MM2);
			MmxPmullwRegToReg(x86_MM1, x86_MM2);
		} else {
			MmxPmulhwRegToReg(x86_MM0, x86_MM2);
			MmxPmulhwRegToReg(x86_MM1, x86_MM2);
		}
	} else {
		RSP_MultiElement2Mmx(x86_MM2, x86_MM3);
		if (AccumStyle != Middle16BitAccum) {
			MmxPmullwRegToReg(x86_MM0, x86_MM2);
			MmxPmullwRegToReg(x86_MM1, x86_MM3);
		} else {
			MmxPmulhwRegToReg(x86_MM0, x86_MM2);
			MmxPmulhwRegToReg(x86_MM1, x86_MM3);
		}
	}

	MmxPsllwImmed(x86_MM0, 1);
	MmxPsllwImmed(x86_MM1, 1);
}

void RSP_Sections_VMACF ( OPCODE RspOp, DWORD AccumStyle ) {
	char Reg[256];*/

	/*****************************************
	** VMACF 
	**  - affects the upper 32-bits, s16*s16*2
	******************************************/

/*	if (AccumStyle == High16BitAccum) {
		return;
	}

	RSPOpC = RspOp;*/

	/**** Load source registers ****/
/*	sprintf(Reg, "RSP_Vect[%i].HW[0]", RspOp.rd);
	MmxMoveQwordVariableToReg(x86_MM0 + 2, &RSP_Vect[RspOp.rd].HW[0], Reg);
	sprintf(Reg, "RSP_Vect[%i].HW[4]", RspOp.rd);
	MmxMoveQwordVariableToReg(x86_MM1 + 2, &RSP_Vect[RspOp.rd].HW[4], Reg);*/

	/******* VMACF *******/
/*	if ((RspOp.rs & 0x0f) < 2) {
		sprintf(Reg, "RSP_Vect[%i].HW[0]", RspOp.rt);
		MmxMoveQwordVariableToReg(x86_MM2 + 2, &RSP_Vect[RspOp.rt].HW[0], Reg);
		sprintf(Reg, "RSP_Vect[%i].HW[4]", RspOp.rt);
		MmxMoveQwordVariableToReg(x86_MM3 + 2, &RSP_Vect[RspOp.rt].HW[4], Reg);

		if (AccumStyle != Middle16BitAccum) {
			MmxPmullwRegToReg(x86_MM0 + 2, x86_MM2 + 2);
			MmxPmullwRegToReg(x86_MM1 + 2, x86_MM3 + 2);
		} else {
			MmxPmulhwRegToReg(x86_MM0 + 2, x86_MM2 + 2);
			MmxPmulhwRegToReg(x86_MM1 + 2, x86_MM3 + 2);
		}
	} else if ((RspOp.rs & 0x0f) >= 8) {
		RSP_Element2Mmx(x86_MM2 + 2);
		if (AccumStyle != Middle16BitAccum) {
			MmxPmullwRegToReg(x86_MM0 + 2, x86_MM2 + 2);
			MmxPmullwRegToReg(x86_MM1 + 2, x86_MM2 + 2);
		} else {
			MmxPmulhwRegToReg(x86_MM0 + 2, x86_MM2 + 2);
			MmxPmulhwRegToReg(x86_MM1 + 2, x86_MM2 + 2);
		}
	} else {
		RSP_MultiElement2Mmx(x86_MM2 + 2, x86_MM3 + 2);
		if (AccumStyle != Middle16BitAccum) {
			MmxPmullwRegToReg(x86_MM0 + 2, x86_MM2 + 2);
			MmxPmullwRegToReg(x86_MM1 + 2, x86_MM3 + 2);
		} else {
			MmxPmulhwRegToReg(x86_MM0 + 2, x86_MM2 + 2);
			MmxPmulhwRegToReg(x86_MM1 + 2, x86_MM3 + 2);
		}
	}

	MmxPsllwImmed(x86_MM0 + 2, 1);
	MmxPsllwImmed(x86_MM1 + 2, 1);
	MmxPaddswRegToReg(x86_MM0, x86_MM0 + 2);
	MmxPaddswRegToReg(x86_MM1, x86_MM1 + 2);
}*/

/******************** Microcode Sections *********************/

/*static DWORD Section_000_VMADN;*/	/* Yah i know, but leave it */

/*BOOL Check_Section_000(void) {
	DWORD i;
	OPCODE op0, op1;

	RSP_LW_IMEM(CompilePC + 0x00, &op0.Hex);*/

	/************************************
	** Example: (mario audio microcode)
	** 
	** 0x574 VMUDN	$v30, $v3, $v23
	** 0x578 VMADN	$v30, $v4, $v23
	** 
	*************************************/

/*	if (!(op0.op == RSP_CP2 && (op0.rs & 0x10) != 0 && op0.funct == RSP_VECTOR_VMUDN)) {
		return FALSE;
	}
	Section_000_VMADN = 0;

	for (i = 0; i < 0x20; i++) {
		RSP_LW_IMEM(CompilePC + 0x04 + (i * 4), &op1.Hex);

		if (!(op1.op == RSP_CP2 && (op1.rs & 0x10) != 0 && op1.funct == RSP_VECTOR_VMADN)) {
			break;
		} else {			
			Section_000_VMADN++;
		}

		if ((op1.rs & 0xF) >= 2 && (op1.rs & 0xF) <= 7 && IsMmx2Enabled == FALSE) {
			return FALSE;
		}
	}*/

	/* We need at least 1 VMADN */
/*	if (Section_000_VMADN == 0) {
		return FALSE;
	}*/

	/* FIXME: check dest & flushes */
/*	if (TRUE == WriteToAccum(7, CompilePC + 0x4 + (Section_000_VMADN * 4) - 0x4)) {
		return FALSE;
	}
	if (IsMmxEnabled == FALSE) {
		return FALSE;
	}
	return TRUE;
}

void Compile_Section_000(void) {
	char Reg[256];
	OPCODE vmudn, vmadn;
	DWORD i;

	RSP_LW_IMEM(CompilePC + 0x00, &vmudn.Hex);

	CPU_Message("Compiling: %X to ..., RSP Optimization $000", CompilePC);
	CPU_Message("  %X %s",CompilePC+0x00,RSPOpcodeName(vmudn.Hex,CompilePC + 0x00));

	for (i = 0; i < Section_000_VMADN; i++) {
		RSP_LW_IMEM(CompilePC + 0x04 + (i * 4), &vmadn.Hex);
		CPU_Message("  %X %s",CompilePC+0x04+(i*4),RSPOpcodeName(vmadn.Hex,CompilePC+0x04+(i*4)));
	}

	RSP_Sections_VMUDN(vmudn, Low16BitAccum);
	CompilePC += 4;

	for (i = 0; i < Section_000_VMADN; i++) {
		RSP_LW_IMEM(CompilePC, &vmadn.Hex);
		CompilePC += 4;
		RSP_Sections_VMADN(vmadn, Low16BitAccum);
		if (WriteToVectorDest(vmadn.sa, CompilePC - 4) == TRUE) {
			sprintf(Reg, "RSP_Vect[%i].HW[0]", vmadn.sa);
			MmxMoveQwordRegToVariable(x86_MM0, &RSP_Vect[vmadn.sa].HW[0], Reg);
			sprintf(Reg, "RSP_Vect[%i].HW[4]", vmadn.sa);
			MmxMoveQwordRegToVariable(x86_MM1, &RSP_Vect[vmadn.sa].HW[4], Reg);
		}
	}
	
	sprintf(Reg, "RSP_Vect[%i].HW[0]", vmadn.sa);
	MmxMoveQwordRegToVariable(x86_MM0, &RSP_Vect[vmadn.sa].HW[0], Reg);
	sprintf(Reg, "RSP_Vect[%i].HW[4]", vmadn.sa);
	MmxMoveQwordRegToVariable(x86_MM1, &RSP_Vect[vmadn.sa].HW[4], Reg);

	MmxEmptyMultimediaState();
}

static DWORD Section_001_VMACF;

BOOL Check_Section_001(void) {
	DWORD i;
	OPCODE op0, op1;

	RSP_LW_IMEM(CompilePC + 0x00, &op0.Hex);*/

	/************************************
	** Example: (mario audio microcode)
	** 
	** 0xCC0	VMULF	$v28, $v28, $v10 [6]
	** 0xCC4	VMACF	$v28, $v17, $v16
	*************************************/

/*	if (!(op0.op == RSP_CP2 && (op0.rs & 0x10) != 0 && op0.funct == RSP_VECTOR_VMULF)) {
		return FALSE;
	}
	Section_001_VMACF = 0;

	for (i = 0; i < 0x20; i++) {
		RSP_LW_IMEM(CompilePC + 0x04 + (i * 4), &op1.Hex);

		if (!(op1.op == RSP_CP2 && (op1.rs & 0x10) != 0 && op1.funct == RSP_VECTOR_VMACF)) {
			break;
		} else {
			Section_001_VMACF++;
		}

		if ((op1.rs & 0xF) >= 2 && (op1.rs & 0xF) <= 7 && IsMmx2Enabled == FALSE) {
			return FALSE;
		}
	}*/

	/* We need at least 1 VMACF */
/*	if (Section_001_VMACF == 0) {
		return FALSE;
	}

	if (IsMmxEnabled == FALSE) {
		return FALSE;
	}*/

	/* dests are checked elsewhere, this is fine */
/*	if (TRUE == WriteToAccum(7, CompilePC + 0x4 + (Section_001_VMACF * 4) - 0x4)) {
		return FALSE;
	}
	
	return TRUE;
}

void Compile_Section_001(void) {
	DWORD i;
	char Reg[256];
	OPCODE vmulf, vmacf;

	RSP_LW_IMEM(CompilePC + 0x00, &vmulf.Hex);

	CPU_Message("Compiling: %X to ..., RSP Optimization $001", CompilePC);
	CPU_Message("  %X %s",CompilePC+0x00,RSPOpcodeName(vmulf.Hex,CompilePC + 0x00));
	
	for (i = 0; i < Section_001_VMACF; i++) {
		RSP_LW_IMEM(CompilePC + 0x04 + (i * 4), &vmacf.Hex);
		CPU_Message("  %X %s",CompilePC+0x04+(i*4),RSPOpcodeName(vmacf.Hex,CompilePC+0x04+(i*4)));
	}

	RSP_Sections_VMULF(vmulf, Middle16BitAccum);

	if (WriteToVectorDest(vmulf.sa, CompilePC) == TRUE) {
		sprintf(Reg, "RSP_Vect[%i].HW[0]", vmulf.sa);
		MmxMoveQwordRegToVariable(x86_MM0, &RSP_Vect[vmulf.sa].HW[0], Reg);
		sprintf(Reg, "RSP_Vect[%i].HW[4]", vmulf.sa);
		MmxMoveQwordRegToVariable(x86_MM1, &RSP_Vect[vmulf.sa].HW[4], Reg);
	}
	CompilePC += 4;

	for (i = 0; i < Section_001_VMACF; i++) {
		RSP_LW_IMEM(CompilePC, &vmacf.Hex);
		CompilePC += 4;

		RSP_Sections_VMACF(vmacf, Middle16BitAccum);
		if (WriteToVectorDest(vmacf.sa, CompilePC - 4) == TRUE) {
			sprintf(Reg, "RSP_Vect[%i].HW[0]", vmacf.sa);
			MmxMoveQwordRegToVariable(x86_MM0, &RSP_Vect[vmacf.sa].HW[0], Reg);
			sprintf(Reg, "RSP_Vect[%i].HW[4]", vmacf.sa);
			MmxMoveQwordRegToVariable(x86_MM1, &RSP_Vect[vmacf.sa].HW[4], Reg);
		}
	}

	MmxEmptyMultimediaState();
}

BOOL Check_Section_002 ( void ) {
	DWORD Count;
	OPCODE op[0x0C];

	for (Count = 0; Count < 0x0C; Count++) {
		RSP_LW_IMEM(CompilePC + (Count * 0x04), &op[Count].Hex);
	}*/

	/************************************
	** Example: (mario audio microcode)
	** 
	** 5F4 VMUDH $v2, $v21, $v27 [6]
	** 5F8 VMADH $v2, $v20, $v27 [7]
	** 5FC VMADH $v2, $v19, $v30 [0]
	** 600 VMADH $v2, $v18, $v30 [1]
	** 604 VMADH $v2, $v17, $v30 [2]
	** 608 VMADH $v2, $v16, $v30 [3]
	** 60C VMADH $v28, $v15, $v30 [4]
	** 610 VMADH $v2, $v14, $v30 [5]
	** 614 VMADH $v2, $v13, $v30 [6]
	** 618 VMADH $v2, $v30, $v31 [5]
	** 61C VSAW	$v26 [9], $v7, $v28 
	** 620 VSAW	$v28 [8], $v7, $v28 
	************************************/

/*	if (IsMmxEnabled == FALSE) {
		return FALSE;
	}

	if (!(op[0].op == RSP_CP2 && (op[0].rs & 0x10) != 0 && op[0].funct == RSP_VECTOR_VMUDH)) {
		return FALSE;
	}
	if ((op[0].rs & 0xF) < 8) {
		return FALSE; 
	}

	for (Count = 1; Count < 10; Count++) {
		if (!(op[Count].op == RSP_CP2 && (op[Count].rs & 0x10) != 0 && op[Count].funct == RSP_VECTOR_VMADH)) {
			return FALSE;
		}
		if ((op[Count].rs & 0xF) < 8) {
			return FALSE;
		}
	}

	if (!(op[10].op == RSP_CP2 && (op[10].rs & 0x10) != 0 && op[10].funct == RSP_VECTOR_VSAW)) return FALSE;
	if (!(op[11].op == RSP_CP2 && (op[11].rs & 0x10) != 0 && op[11].funct == RSP_VECTOR_VSAW)) return FALSE;

	if ((op[10].rs & 0xF) != 9) { return FALSE; }
	if ((op[11].rs & 0xF) != 8) { return FALSE; }

	if (TRUE == WriteToAccum(7, CompilePC + 0x2C))
		return FALSE;

	return TRUE;
}

void Compile_Section_002 ( void ) {
	char Reg[256];

	DWORD Count;
	OPCODE op[0x0C];

	OPCODE vmudh, vsaw;

	CPU_Message("Compiling: %X to ..., RSP Optimization $002", CompilePC);	
	for (Count = 0; Count < 0xC; Count++) {
		RSP_LW_IMEM(CompilePC + (Count * 0x04), &op[Count].Hex);
		CPU_Message("  %X %s",CompilePC+(Count*0x04),RSPOpcodeName(op[Count].Hex,CompilePC + (Count*0x04)));
	}

	vmudh = op[0];
	RSP_Sections_VMUDH(vmudh, High16BitAccum);*/
	
	/******* VMADHs *******/
/*	for (Count = 1; Count < 10; Count++) {
		RSP_Sections_VMADH(op[Count], High16BitAccum);
	}*/

	/***** VSAWs *****/
/*	vsaw = op[10];
	MmxXorRegToReg(x86_MM4, x86_MM4);
	sprintf(Reg, "RSP_Vect[%i].HW[0]", RSPOpC.sa);
	MmxMoveQwordRegToVariable(x86_MM4, &RSP_Vect[vsaw.sa].HW[0], Reg);
	sprintf(Reg, "RSP_Vect[%i].HW[4]", RSPOpC.sa);
	MmxMoveQwordRegToVariable(x86_MM4, &RSP_Vect[vsaw.sa].HW[4], Reg);

	vsaw = op[11];
	sprintf(Reg, "RSP_Vect[%i].HW[0]", RSPOpC.sa);
	MmxMoveQwordRegToVariable(x86_MM0, &RSP_Vect[vsaw.sa].HW[0], Reg);
	sprintf(Reg, "RSP_Vect[%i].HW[4]", RSPOpC.sa);
	MmxMoveQwordRegToVariable(x86_MM1, &RSP_Vect[vsaw.sa].HW[4], Reg);

	MmxEmptyMultimediaState();

	CompilePC += 12 * sizeof(OPCODE);
}*/

BOOL RSP_DoSections(void) {
	/*if (TRUE == Check_Section_000()) {
		Compile_Section_000();
		return TRUE;
	}
	if (TRUE == Check_Section_001()) {
		Compile_Section_001();
		return TRUE;
	}
	if (TRUE == Check_Section_002()) {
		Compile_Section_002();
		return TRUE;
	}*/
	return FALSE;
}
