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
#include "rsp.h"*/
#include "x86.h"
/*#include "memory.h"
#include "RSP registers.h"*/
#include "RSP/rsp_log.h"

#define PUTDST8(dest,value)  (*((BYTE *)(dest))=(BYTE)(value)); dest += 1;
#define PUTDST16(dest,value) (*((WORD *)(dest))=(WORD)(value)); dest += 2;
#define PUTDST32(dest,value) (*((DWORD *)(dest))=(DWORD)(value)); dest += 4;

static char * mmx_Strings[8] = {
	"mm0", "mm1", "mm2", "mm3", 
	"mm4", "mm5", "mm6", "mm7"
};

#define mmx_Name(Reg) (mmx_Strings[(Reg)])


void MmxEmptyMultimediaState(BYTE**code) {
	RSP_CPU_Message("      emms");
	PUTDST16(*code,0x770f);
}

void MmxMoveRegToReg(BYTE** code, int Dest, int Source) {
	BYTE x86Command = 0;

	RSP_CPU_Message("      movq %s, %s", mmx_Name(Dest), mmx_Name(Source));

	switch (Dest) {
	case x86_MM0: x86Command = 0; break;
	case x86_MM1: x86Command = 1; break;
	case x86_MM2: x86Command = 2; break;
	case x86_MM3: x86Command = 3; break;
	case x86_MM4: x86Command = 4; break;
	case x86_MM5: x86Command = 5; break;
	case x86_MM6: x86Command = 6; break;
	case x86_MM7: x86Command = 7; break;
	}
	switch (Source) {
	case x86_MM0: x86Command |= 0 << 3; break;
	case x86_MM1: x86Command |= 1 << 3; break;
	case x86_MM2: x86Command |= 2 << 3; break;
	case x86_MM3: x86Command |= 3 << 3; break;
	case x86_MM4: x86Command |= 4 << 3; break;
	case x86_MM5: x86Command |= 5 << 3; break;
	case x86_MM6: x86Command |= 6 << 3; break;
	case x86_MM7: x86Command |= 7 << 3; break;
	}
	PUTDST16(*code,0x7f0f);
	PUTDST8(*code, 0xC0 | x86Command);
}

void MmxMoveQwordVariableToReg(BYTE** code, int Dest, void *Variable, char *VariableName) {
	BYTE x86Command = 0;

	RSP_CPU_Message("      movq %s, qword ptr [%s]",mmx_Name(Dest), VariableName);

	switch (Dest) {
	case x86_MM0: x86Command = 0x05; break;
	case x86_MM1: x86Command = 0x0D; break;
	case x86_MM2: x86Command = 0x15; break;
	case x86_MM3: x86Command = 0x1D; break;
	case x86_MM4: x86Command = 0x25; break;
	case x86_MM5: x86Command = 0x2D; break;
	case x86_MM6: x86Command = 0x35; break;
	case x86_MM7: x86Command = 0x3D; break;
	}	

	PUTDST16(*code,0x6f0f);
	PUTDST8(*code, x86Command);
	PUTDST32(*code,Variable);
}

void MmxMoveQwordRegToVariable(BYTE** code, int Dest, void *Variable, char *VariableName) {
	BYTE x86Command = 0;

	RSP_CPU_Message("      movq qword ptr [%s], %s", VariableName, mmx_Name(Dest));

	switch (Dest) {
	case x86_MM0: x86Command = 0x05; break;
	case x86_MM1: x86Command = 0x0D; break;
	case x86_MM2: x86Command = 0x15; break;
	case x86_MM3: x86Command = 0x1D; break;
	case x86_MM4: x86Command = 0x25; break;
	case x86_MM5: x86Command = 0x2D; break;
	case x86_MM6: x86Command = 0x35; break;
	case x86_MM7: x86Command = 0x3D; break;
	}	

	PUTDST16(*code,0x7f0f);
	PUTDST8(*code, x86Command);
	PUTDST32(*code,Variable);
}

void MmxPorRegToReg(BYTE** code, int Dest, int Source) {
	BYTE x86Command = 0;

	RSP_CPU_Message("      por %s, %s", mmx_Name(Dest), mmx_Name(Source));

	switch (Dest) {
	case x86_MM0: x86Command = 0 << 3; break;
	case x86_MM1: x86Command = 1 << 3; break;
	case x86_MM2: x86Command = 2 << 3; break;
	case x86_MM3: x86Command = 3 << 3; break;
	case x86_MM4: x86Command = 4 << 3; break;
	case x86_MM5: x86Command = 5 << 3; break;
	case x86_MM6: x86Command = 6 << 3; break;
	case x86_MM7: x86Command = 7 << 3; break;
	}
	switch (Source) {
	case x86_MM0: x86Command |= 0; break;
	case x86_MM1: x86Command |= 1; break;
	case x86_MM2: x86Command |= 2; break;
	case x86_MM3: x86Command |= 3; break;
	case x86_MM4: x86Command |= 4; break;
	case x86_MM5: x86Command |= 5; break;
	case x86_MM6: x86Command |= 6; break;
	case x86_MM7: x86Command |= 7; break;
	}
	PUTDST16(*code,0xeb0f);
	PUTDST8(*code, 0xC0 | x86Command);
}

/*void MmxPorVariableToReg(void * Variable, char * VariableName, int Dest) {
	BYTE x86Command;

	CPU_Message("      por %s, qword ptr [%s]",mmx_Name(Dest), VariableName);

	switch (Dest) {
	case x86_MM0: x86Command = 0x05; break;
	case x86_MM1: x86Command = 0x0D; break;
	case x86_MM2: x86Command = 0x15; break;
	case x86_MM3: x86Command = 0x1D; break;
	case x86_MM4: x86Command = 0x25; break;
	case x86_MM5: x86Command = 0x2D; break;
	case x86_MM6: x86Command = 0x35; break;
	case x86_MM7: x86Command = 0x3D; break;
	}	

	PUTDST16(RecompPos,0xeb0f);
	PUTDST8(RecompPos, x86Command);
	PUTDST32(RecompPos,Variable);
}*/

void MmxPandRegToReg(BYTE** code, int Dest, int Source) {
	BYTE x86Command = 0;

	RSP_CPU_Message("      pand %s, %s", mmx_Name(Dest), mmx_Name(Source));

	switch (Dest) {
	case x86_MM0: x86Command = 0 << 3; break;
	case x86_MM1: x86Command = 1 << 3; break;
	case x86_MM2: x86Command = 2 << 3; break;
	case x86_MM3: x86Command = 3 << 3; break;
	case x86_MM4: x86Command = 4 << 3; break;
	case x86_MM5: x86Command = 5 << 3; break;
	case x86_MM6: x86Command = 6 << 3; break;
	case x86_MM7: x86Command = 7 << 3; break;
	}
	switch (Source) {
	case x86_MM0: x86Command |= 0; break;
	case x86_MM1: x86Command |= 1; break;
	case x86_MM2: x86Command |= 2; break;
	case x86_MM3: x86Command |= 3; break;
	case x86_MM4: x86Command |= 4; break;
	case x86_MM5: x86Command |= 5; break;
	case x86_MM6: x86Command |= 6; break;
	case x86_MM7: x86Command |= 7; break;
	}
	PUTDST16(*code,0xdb0f);
	PUTDST8(*code, 0xC0 | x86Command);
}

/*void MmxPandVariableToReg(void * Variable, char * VariableName, int Dest) {
	BYTE x86Command;

	CPU_Message("      pand %s, qword ptr [%s]",mmx_Name(Dest), VariableName);

	switch (Dest) {
	case x86_MM0: x86Command = 0x05; break;
	case x86_MM1: x86Command = 0x0D; break;
	case x86_MM2: x86Command = 0x15; break;
	case x86_MM3: x86Command = 0x1D; break;
	case x86_MM4: x86Command = 0x25; break;
	case x86_MM5: x86Command = 0x2D; break;
	case x86_MM6: x86Command = 0x35; break;
	case x86_MM7: x86Command = 0x3D; break;
	}	

	PUTDST16(RecompPos,0xdb0f);
	PUTDST8(RecompPos, x86Command);
	PUTDST32(RecompPos,Variable);
}*/

void MmxPandnRegToReg(BYTE** code, int Dest, int Source) {
	BYTE x86Command = 0;

	RSP_CPU_Message("      pandn %s, %s", mmx_Name(Dest), mmx_Name(Source));

	switch (Dest) {
	case x86_MM0: x86Command = 0 << 3; break;
	case x86_MM1: x86Command = 1 << 3; break;
	case x86_MM2: x86Command = 2 << 3; break;
	case x86_MM3: x86Command = 3 << 3; break;
	case x86_MM4: x86Command = 4 << 3; break;
	case x86_MM5: x86Command = 5 << 3; break;
	case x86_MM6: x86Command = 6 << 3; break;
	case x86_MM7: x86Command = 7 << 3; break;
	}
	switch (Source) {
	case x86_MM0: x86Command |= 0; break;
	case x86_MM1: x86Command |= 1; break;
	case x86_MM2: x86Command |= 2; break;
	case x86_MM3: x86Command |= 3; break;
	case x86_MM4: x86Command |= 4; break;
	case x86_MM5: x86Command |= 5; break;
	case x86_MM6: x86Command |= 6; break;
	case x86_MM7: x86Command |= 7; break;
	}
	PUTDST16(*code,0xdf0f);
	PUTDST8(*code, 0xC0 | x86Command);
}

/*void MmxXorRegToReg(int Dest, int Source) {
	BYTE x86Command;

	CPU_Message("      pxor %s, %s", mmx_Name(Dest), mmx_Name(Source));

	switch (Dest) {
	case x86_MM0: x86Command = 0x00; break;
	case x86_MM1: x86Command = 0x08; break;
	case x86_MM2: x86Command = 0x10; break;
	case x86_MM3: x86Command = 0x18; break;
	case x86_MM4: x86Command = 0x20; break;
	case x86_MM5: x86Command = 0x28; break;
	case x86_MM6: x86Command = 0x30; break;
	case x86_MM7: x86Command = 0x38; break;
	}
	switch (Source) {
	case x86_MM0: x86Command += 0x00; break;
	case x86_MM1: x86Command += 0x01; break;
	case x86_MM2: x86Command += 0x02; break;
	case x86_MM3: x86Command += 0x03; break;
	case x86_MM4: x86Command += 0x04; break;
	case x86_MM5: x86Command += 0x05; break;
	case x86_MM6: x86Command += 0x06; break;
	case x86_MM7: x86Command += 0x07; break;
	}	
	PUTDST16(RecompPos,0xef0f);
	PUTDST8(RecompPos, 0xC0 | x86Command);
}*/

void MmxShuffleMemoryToReg(BYTE** code, int Dest, void * Variable, char * VariableName, BYTE Immed) {
	BYTE x86Command = 0;

	RSP_CPU_Message("      pshufw %s, [%s], %02X", mmx_Name(Dest), VariableName, Immed);

	switch (Dest) {
	case x86_MM0: x86Command = 0x05; break;
	case x86_MM1: x86Command = 0x0D; break;
	case x86_MM2: x86Command = 0x15; break;
	case x86_MM3: x86Command = 0x1D; break;
	case x86_MM4: x86Command = 0x25; break;
	case x86_MM5: x86Command = 0x2D; break;
	case x86_MM6: x86Command = 0x35; break;
	case x86_MM7: x86Command = 0x3D; break;
	}

	PUTDST16(*code,0x700f);
	PUTDST8(*code, x86Command);
	PUTDST32(*code,Variable);
	PUTDST8(*code, Immed);	
}

void MmxPmullwRegToReg(BYTE** code, int Dest, int Source) {
	BYTE x86Command = 0;

	RSP_CPU_Message("      pmullw %s, %s", mmx_Name(Dest), mmx_Name(Source));

	switch (Dest) {
	case x86_MM0: x86Command = 0 << 3; break;
	case x86_MM1: x86Command = 1 << 3; break;
	case x86_MM2: x86Command = 2 << 3; break;
	case x86_MM3: x86Command = 3 << 3; break;
	case x86_MM4: x86Command = 4 << 3; break;
	case x86_MM5: x86Command = 5 << 3; break;
	case x86_MM6: x86Command = 6 << 3; break;
	case x86_MM7: x86Command = 7 << 3; break;
	}
	switch (Source) {
	case x86_MM0: x86Command |= 0; break;
	case x86_MM1: x86Command |= 1; break;
	case x86_MM2: x86Command |= 2; break;
	case x86_MM3: x86Command |= 3; break;
	case x86_MM4: x86Command |= 4; break;
	case x86_MM5: x86Command |= 5; break;
	case x86_MM6: x86Command |= 6; break;
	case x86_MM7: x86Command |= 7; break;
	}
	PUTDST16(*code,0xd50f);
	PUTDST8(*code, 0xC0 | x86Command);
}

/*void MmxPmullwVariableToReg(int Dest, void * Variable, char * VariableName) {
	BYTE x86Command;

	CPU_Message("      pmullw %s, [%s]", mmx_Name(Dest), VariableName);

	switch (Dest) {
	case x86_MM0: x86Command = 0x05; break;
	case x86_MM1: x86Command = 0x0D; break;
	case x86_MM2: x86Command = 0x15; break;
	case x86_MM3: x86Command = 0x1D; break;
	case x86_MM4: x86Command = 0x25; break;
	case x86_MM5: x86Command = 0x2D; break;
	case x86_MM6: x86Command = 0x35; break;
	case x86_MM7: x86Command = 0x3D; break;
	}
	PUTDST16(RecompPos,0xd50f);
	PUTDST8(RecompPos, x86Command);
	PUTDST32(RecompPos, Variable);
}

void MmxPmulhuwRegToReg(int Dest, int Source) {
	BYTE x86Command;

	CPU_Message("      pmulhuw %s, %s", mmx_Name(Dest), mmx_Name(Source));

	switch (Dest) {
	case x86_MM0: x86Command = 0 << 3; break;
	case x86_MM1: x86Command = 1 << 3; break;
	case x86_MM2: x86Command = 2 << 3; break;
	case x86_MM3: x86Command = 3 << 3; break;
	case x86_MM4: x86Command = 4 << 3; break;
	case x86_MM5: x86Command = 5 << 3; break;
	case x86_MM6: x86Command = 6 << 3; break;
	case x86_MM7: x86Command = 7 << 3; break;
	}
	switch (Source) {
	case x86_MM0: x86Command |= 0; break;
	case x86_MM1: x86Command |= 1; break;
	case x86_MM2: x86Command |= 2; break;
	case x86_MM3: x86Command |= 3; break;
	case x86_MM4: x86Command |= 4; break;
	case x86_MM5: x86Command |= 5; break;
	case x86_MM6: x86Command |= 6; break;
	case x86_MM7: x86Command |= 7; break;
	}
	PUTDST16(RecompPos,0xe40f);
	PUTDST8(RecompPos, 0xC0 | x86Command);
}*/

void MmxPmulhwRegToReg(BYTE** code, int Dest, int Source) {
	BYTE x86Command = 0;

	RSP_CPU_Message("      pmulhw %s, %s", mmx_Name(Dest), mmx_Name(Source));

	switch (Dest) {
	case x86_MM0: x86Command = 0 << 3; break;
	case x86_MM1: x86Command = 1 << 3; break;
	case x86_MM2: x86Command = 2 << 3; break;
	case x86_MM3: x86Command = 3 << 3; break;
	case x86_MM4: x86Command = 4 << 3; break;
	case x86_MM5: x86Command = 5 << 3; break;
	case x86_MM6: x86Command = 6 << 3; break;
	case x86_MM7: x86Command = 7 << 3; break;
	}
	switch (Source) {
	case x86_MM0: x86Command |= 0; break;
	case x86_MM1: x86Command |= 1; break;
	case x86_MM2: x86Command |= 2; break;
	case x86_MM3: x86Command |= 3; break;
	case x86_MM4: x86Command |= 4; break;
	case x86_MM5: x86Command |= 5; break;
	case x86_MM6: x86Command |= 6; break;
	case x86_MM7: x86Command |= 7; break;
	}
	PUTDST16(*code,0xe50f);
	PUTDST8(*code, 0xC0 | x86Command);
}

void MmxPmulhwRegToVariable(BYTE** code, int Dest, void * Variable, char * VariableName) {
	BYTE x86Command = 0;

	RSP_CPU_Message("      pmulhw %s, [%s]", mmx_Name(Dest), VariableName);

	switch (Dest) {
	case x86_MM0: x86Command = 0x05; break;
	case x86_MM1: x86Command = 0x0D; break;
	case x86_MM2: x86Command = 0x15; break;
	case x86_MM3: x86Command = 0x1D; break;
	case x86_MM4: x86Command = 0x25; break;
	case x86_MM5: x86Command = 0x2D; break;
	case x86_MM6: x86Command = 0x35; break;
	case x86_MM7: x86Command = 0x3D; break;
	}
	PUTDST16(*code,0xe50f);
	PUTDST8(*code, x86Command);
	PUTDST32(*code, Variable);
}


void MmxPsrlwImmed(BYTE** code, int Dest, BYTE Immed) {
	BYTE x86Command = 0;

	RSP_CPU_Message("      psrlw %s, %i", mmx_Name(Dest), Immed);

	switch (Dest) {
	case x86_MM0: x86Command = 0xD0; break;
	case x86_MM1: x86Command = 0xD1; break;
	case x86_MM2: x86Command = 0xD2; break;
	case x86_MM3: x86Command = 0xD3; break;
	case x86_MM4: x86Command = 0xD4; break;
	case x86_MM5: x86Command = 0xD5; break;
	case x86_MM6: x86Command = 0xD6; break;
	case x86_MM7: x86Command = 0xD7; break;
	}	

	PUTDST16(*code,0x710f);
	PUTDST8(*code, x86Command);
	PUTDST8(*code, Immed);
}

/*void MmxPsrawImmed(int Dest, BYTE Immed) {
	BYTE x86Command;

	CPU_Message("      psraw %s, %i", mmx_Name(Dest), Immed);

	switch (Dest) {
	case x86_MM0: x86Command = 0xE0; break;
	case x86_MM1: x86Command = 0xE1; break;
	case x86_MM2: x86Command = 0xE2; break;
	case x86_MM3: x86Command = 0xE3; break;
	case x86_MM4: x86Command = 0xE4; break;
	case x86_MM5: x86Command = 0xE5; break;
	case x86_MM6: x86Command = 0xE6; break;
	case x86_MM7: x86Command = 0xE7; break;
	}	

	PUTDST16(RecompPos,0x710f);
	PUTDST8(RecompPos, x86Command);
	PUTDST8(RecompPos, Immed);
}*/

void MmxPsllwImmed(BYTE** code, int Dest, BYTE Immed) {
	BYTE x86Command = 0;

	RSP_CPU_Message("      psllw %s, %i", mmx_Name(Dest), Immed);

	switch (Dest) {
	case x86_MM0: x86Command = 0xF0; break;
	case x86_MM1: x86Command = 0xF1; break;
	case x86_MM2: x86Command = 0xF2; break;
	case x86_MM3: x86Command = 0xF3; break;
	case x86_MM4: x86Command = 0xF4; break;
	case x86_MM5: x86Command = 0xF5; break;
	case x86_MM6: x86Command = 0xF6; break;
	case x86_MM7: x86Command = 0xF7; break;
	}	

	PUTDST16(*code,0x710f);
	PUTDST8(*code, x86Command);
	PUTDST8(*code, Immed);
}

/*void MmxPaddswRegToReg(int Dest, int Source) {
	BYTE x86Command;

	CPU_Message("      paddsw %s, %s", mmx_Name(Dest), mmx_Name(Source));

	switch (Dest) {
	case x86_MM0: x86Command = 0 << 3; break;
	case x86_MM1: x86Command = 1 << 3; break;
	case x86_MM2: x86Command = 2 << 3; break;
	case x86_MM3: x86Command = 3 << 3; break;
	case x86_MM4: x86Command = 4 << 3; break;
	case x86_MM5: x86Command = 5 << 3; break;
	case x86_MM6: x86Command = 6 << 3; break;
	case x86_MM7: x86Command = 7 << 3; break;
	}
	switch (Source) {
	case x86_MM0: x86Command |= 0; break;
	case x86_MM1: x86Command |= 1; break;
	case x86_MM2: x86Command |= 2; break;
	case x86_MM3: x86Command |= 3; break;
	case x86_MM4: x86Command |= 4; break;
	case x86_MM5: x86Command |= 5; break;
	case x86_MM6: x86Command |= 6; break;
	case x86_MM7: x86Command |= 7; break;
	}
	PUTDST16(RecompPos,0xed0f);
	PUTDST8(RecompPos, 0xC0 | x86Command);
}

void MmxPaddswVariableToReg(int Dest, void * Variable, char * VariableName) {
	BYTE x86Command;

	CPU_Message("      paddsw %s, [%s]", mmx_Name(Dest), VariableName);

	switch (Dest) {
	case x86_MM0: x86Command = 0x05; break;
	case x86_MM1: x86Command = 0x0D; break;
	case x86_MM2: x86Command = 0x15; break;
	case x86_MM3: x86Command = 0x1D; break;
	case x86_MM4: x86Command = 0x25; break;
	case x86_MM5: x86Command = 0x2D; break;
	case x86_MM6: x86Command = 0x35; break;
	case x86_MM7: x86Command = 0x3D; break;
	}

	PUTDST16(RecompPos,0xed0f);
	PUTDST8(RecompPos, x86Command);
	PUTDST32(RecompPos, Variable);
}*/

void MmxPaddwRegToReg(BYTE** code, int Dest, int Source) {
	BYTE x86Command = 0;

	RSP_CPU_Message("      paddw %s, %s", mmx_Name(Dest), mmx_Name(Source));

	switch (Dest) {
	case x86_MM0: x86Command = 0 << 3; break;
	case x86_MM1: x86Command = 1 << 3; break;
	case x86_MM2: x86Command = 2 << 3; break;
	case x86_MM3: x86Command = 3 << 3; break;
	case x86_MM4: x86Command = 4 << 3; break;
	case x86_MM5: x86Command = 5 << 3; break;
	case x86_MM6: x86Command = 6 << 3; break;
	case x86_MM7: x86Command = 7 << 3; break;
	}
	switch (Source) {
	case x86_MM0: x86Command |= 0; break;
	case x86_MM1: x86Command |= 1; break;
	case x86_MM2: x86Command |= 2; break;
	case x86_MM3: x86Command |= 3; break;
	case x86_MM4: x86Command |= 4; break;
	case x86_MM5: x86Command |= 5; break;
	case x86_MM6: x86Command |= 6; break;
	case x86_MM7: x86Command |= 7; break;
	}
	PUTDST16(*code,0xfd0f);
	PUTDST8(*code, 0xC0 | x86Command);
}

/*void MmxPackSignedDwords(int Dest, int Source) {
	BYTE x86Command;

	CPU_Message("      packssdw %s, %s", mmx_Name(Dest), mmx_Name(Source));

	switch (Dest) {
	case x86_MM0: x86Command = 0 << 3; break;
	case x86_MM1: x86Command = 1 << 3; break;
	case x86_MM2: x86Command = 2 << 3; break;
	case x86_MM3: x86Command = 3 << 3; break;
	case x86_MM4: x86Command = 4 << 3; break;
	case x86_MM5: x86Command = 5 << 3; break;
	case x86_MM6: x86Command = 6 << 3; break;
	case x86_MM7: x86Command = 7 << 3; break;
	}
	switch (Source) {
	case x86_MM0: x86Command |= 0; break;
	case x86_MM1: x86Command |= 1; break;
	case x86_MM2: x86Command |= 2; break;
	case x86_MM3: x86Command |= 3; break;
	case x86_MM4: x86Command |= 4; break;
	case x86_MM5: x86Command |= 5; break;
	case x86_MM6: x86Command |= 6; break;
	case x86_MM7: x86Command |= 7; break;
	}
	PUTDST16(RecompPos,0x6b0f);
	PUTDST8(RecompPos, 0xC0 | x86Command);
}

void MmxUnpackLowWord(int Dest, int Source) {
	BYTE x86Command;

	CPU_Message("      punpcklwd %s, %s", mmx_Name(Dest), mmx_Name(Source));

	switch (Dest) {
	case x86_MM0: x86Command = 0 << 3; break;
	case x86_MM1: x86Command = 1 << 3; break;
	case x86_MM2: x86Command = 2 << 3; break;
	case x86_MM3: x86Command = 3 << 3; break;
	case x86_MM4: x86Command = 4 << 3; break;
	case x86_MM5: x86Command = 5 << 3; break;
	case x86_MM6: x86Command = 6 << 3; break;
	case x86_MM7: x86Command = 7 << 3; break;
	}
	switch (Source) {
	case x86_MM0: x86Command |= 0; break;
	case x86_MM1: x86Command |= 1; break;
	case x86_MM2: x86Command |= 2; break;
	case x86_MM3: x86Command |= 3; break;
	case x86_MM4: x86Command |= 4; break;
	case x86_MM5: x86Command |= 5; break;
	case x86_MM6: x86Command |= 6; break;
	case x86_MM7: x86Command |= 7; break;
	}
	PUTDST16(RecompPos,0x610f);
	PUTDST8(RecompPos, 0xC0 | x86Command);
}

void MmxUnpackHighWord(int Dest, int Source) {
	BYTE x86Command;

	CPU_Message("      punpckhwd %s, %s", mmx_Name(Dest), mmx_Name(Source));

	switch (Dest) {
	case x86_MM0: x86Command = 0 << 3; break;
	case x86_MM1: x86Command = 1 << 3; break;
	case x86_MM2: x86Command = 2 << 3; break;
	case x86_MM3: x86Command = 3 << 3; break;
	case x86_MM4: x86Command = 4 << 3; break;
	case x86_MM5: x86Command = 5 << 3; break;
	case x86_MM6: x86Command = 6 << 3; break;
	case x86_MM7: x86Command = 7 << 3; break;
	}
	switch (Source) {
	case x86_MM0: x86Command |= 0; break;
	case x86_MM1: x86Command |= 1; break;
	case x86_MM2: x86Command |= 2; break;
	case x86_MM3: x86Command |= 3; break;
	case x86_MM4: x86Command |= 4; break;
	case x86_MM5: x86Command |= 5; break;
	case x86_MM6: x86Command |= 6; break;
	case x86_MM7: x86Command |= 7; break;
	}
	PUTDST16(RecompPos,0x690f);
	PUTDST8(RecompPos, 0xC0 | x86Command);
}

void MmxCompareGreaterWordRegToReg(int Dest, int Source) {
	BYTE x86Command;

	CPU_Message("      pcmpgtw %s, %s", mmx_Name(Dest), mmx_Name(Source));

	switch (Dest) {
	case x86_MM0: x86Command = 0 << 3; break;
	case x86_MM1: x86Command = 1 << 3; break;
	case x86_MM2: x86Command = 2 << 3; break;
	case x86_MM3: x86Command = 3 << 3; break;
	case x86_MM4: x86Command = 4 << 3; break;
	case x86_MM5: x86Command = 5 << 3; break;
	case x86_MM6: x86Command = 6 << 3; break;
	case x86_MM7: x86Command = 7 << 3; break;
	}
	switch (Source) {
	case x86_MM0: x86Command |= 0; break;
	case x86_MM1: x86Command |= 1; break;
	case x86_MM2: x86Command |= 2; break;
	case x86_MM3: x86Command |= 3; break;
	case x86_MM4: x86Command |= 4; break;
	case x86_MM5: x86Command |= 5; break;
	case x86_MM6: x86Command |= 6; break;
	case x86_MM7: x86Command |= 7; break;
	}
	PUTDST16(RecompPos,0x650f);
	PUTDST8(RecompPos, 0xC0 | x86Command);
}*/

void MmxCompareEqualWordRegToReg(BYTE** code, int Dest, int Source) {
	BYTE x86Command = 0;

	RSP_CPU_Message("      pcmpeqw %s, %s", mmx_Name(Dest), mmx_Name(Source));

	switch (Dest) {
	case x86_MM0: x86Command = 0 << 3; break;
	case x86_MM1: x86Command = 1 << 3; break;
	case x86_MM2: x86Command = 2 << 3; break;
	case x86_MM3: x86Command = 3 << 3; break;
	case x86_MM4: x86Command = 4 << 3; break;
	case x86_MM5: x86Command = 5 << 3; break;
	case x86_MM6: x86Command = 6 << 3; break;
	case x86_MM7: x86Command = 7 << 3; break;
	}
	switch (Source) {
	case x86_MM0: x86Command |= 0; break;
	case x86_MM1: x86Command |= 1; break;
	case x86_MM2: x86Command |= 2; break;
	case x86_MM3: x86Command |= 3; break;
	case x86_MM4: x86Command |= 4; break;
	case x86_MM5: x86Command |= 5; break;
	case x86_MM6: x86Command |= 6; break;
	case x86_MM7: x86Command |= 7; break;
	}
	PUTDST16(*code, 0x750f);
	PUTDST8(*code, 0xC0 | x86Command);
}
