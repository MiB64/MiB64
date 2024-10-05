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

char * sse_Strings[8] = {
	"xmm0", "xmm1", "xmm2", "xmm3", 
	"xmm4", "xmm5", "xmm6", "xmm7"
};

#define sse_Name(Reg) (sse_Strings[(Reg)])

void SseMoveAlignedVariableToReg(BYTE** code, void *Variable, char *VariableName, int sseReg, int sseDataType, BOOL SS2Supported) {
	if (sseDataType == SseType_QuadWord && SS2Supported) {
		BYTE x86Command = 0;

		RSP_CPU_Message("      movdqa %s, xmmword ptr [%s]", sse_Name(sseReg), VariableName);

		switch (sseReg) {
		case x86_XMM0: x86Command = 0x05; break;
		case x86_XMM1: x86Command = 0x0D; break;
		case x86_XMM2: x86Command = 0x15; break;
		case x86_XMM3: x86Command = 0x1D; break;
		case x86_XMM4: x86Command = 0x25; break;
		case x86_XMM5: x86Command = 0x2D; break;
		case x86_XMM6: x86Command = 0x35; break;
		case x86_XMM7: x86Command = 0x3D; break;
		}

		PUTDST16(*code, 0x0F66);
		PUTDST8(*code, 0x6F);
		PUTDST8(*code, x86Command);
		PUTDST32(*code, Variable);
	} else {
		BYTE x86Command = 0;

		RSP_CPU_Message("      movaps %s, xmmword ptr [%s]", sse_Name(sseReg), VariableName);

		switch (sseReg) {
		case x86_XMM0: x86Command = 0x05; break;
		case x86_XMM1: x86Command = 0x0D; break;
		case x86_XMM2: x86Command = 0x15; break;
		case x86_XMM3: x86Command = 0x1D; break;
		case x86_XMM4: x86Command = 0x25; break;
		case x86_XMM5: x86Command = 0x2D; break;
		case x86_XMM6: x86Command = 0x35; break;
		case x86_XMM7: x86Command = 0x3D; break;
		}

		PUTDST16(*code, 0x280f);
		PUTDST8(*code, x86Command);
		PUTDST32(*code, Variable);
	}
}

/*void SseMoveAlignedN64MemToReg(int sseReg, int AddrReg) {
	BYTE x86Command;

	CPU_Message("      movaps %s, xmmword ptr [Dmem+%s]",sse_Name(sseReg), x86_Name(AddrReg));

	switch (sseReg) {
	case x86_XMM0: x86Command = 0x80; break;
	case x86_XMM1: x86Command = 0x88; break;
	case x86_XMM2: x86Command = 0x90; break;
	case x86_XMM3: x86Command = 0x98; break;
	case x86_XMM4: x86Command = 0xA0; break;
	case x86_XMM5: x86Command = 0xA8; break;
	case x86_XMM6: x86Command = 0xB0; break;
	case x86_XMM7: x86Command = 0xB8; break;
	}	
	switch (AddrReg) {
	case x86_EAX: x86Command += 0x00; break;
	case x86_EBX: x86Command += 0x03; break;
	case x86_ECX: x86Command += 0x01; break;
	case x86_EDX: x86Command += 0x02; break;
	case x86_ESI: x86Command += 0x06; break;
	case x86_EDI: x86Command += 0x07; break;
	case x86_ESP: x86Command += 0x04; break;
	case x86_EBP: x86Command += 0x05; break;
	}

	PUTDST16(RecompPos,0x280f);
	PUTDST8(RecompPos, x86Command);
	PUTDST32(RecompPos,RSPInfo.DMEM);
}*/

void SseMoveAlignedRegToVariable(BYTE** code, int sseReg, void *Variable, char *VariableName, int sseDataType, BOOL SS2Supported) {
	if (sseDataType == SseType_QuadWord && SS2Supported) {
		BYTE x86Command = 0;

		RSP_CPU_Message("      movdqa xmmword ptr [%s], %s", VariableName, sse_Name(sseReg));

		switch (sseReg) {
		case x86_XMM0: x86Command = 0x05; break;
		case x86_XMM1: x86Command = 0x0D; break;
		case x86_XMM2: x86Command = 0x15; break;
		case x86_XMM3: x86Command = 0x1D; break;
		case x86_XMM4: x86Command = 0x25; break;
		case x86_XMM5: x86Command = 0x2D; break;
		case x86_XMM6: x86Command = 0x35; break;
		case x86_XMM7: x86Command = 0x3D; break;
		}

		PUTDST8(*code, 0x66);
		PUTDST16(*code, 0x7f0f);
		PUTDST8(*code, x86Command);
		PUTDST32(*code, Variable);
	} else {
		BYTE x86Command = 0;

		RSP_CPU_Message("      movaps xmmword ptr [%s], %s", VariableName, sse_Name(sseReg));

		switch (sseReg) {
		case x86_XMM0: x86Command = 0x05; break;
		case x86_XMM1: x86Command = 0x0D; break;
		case x86_XMM2: x86Command = 0x15; break;
		case x86_XMM3: x86Command = 0x1D; break;
		case x86_XMM4: x86Command = 0x25; break;
		case x86_XMM5: x86Command = 0x2D; break;
		case x86_XMM6: x86Command = 0x35; break;
		case x86_XMM7: x86Command = 0x3D; break;
		}

		PUTDST16(*code, 0x290f);
		PUTDST8(*code, x86Command);
		PUTDST32(*code, Variable);
	}
}

/*void SseMoveAlignedRegToN64Mem(int sseReg, int AddrReg) {
	BYTE x86Command;

	CPU_Message("      movaps xmmword ptr [Dmem+%s], %s",x86_Name(AddrReg),sse_Name(sseReg));

	switch (sseReg) {
	case x86_XMM0: x86Command = 0x80; break;
	case x86_XMM1: x86Command = 0x88; break;
	case x86_XMM2: x86Command = 0x90; break;
	case x86_XMM3: x86Command = 0x98; break;
	case x86_XMM4: x86Command = 0xA0; break;
	case x86_XMM5: x86Command = 0xA8; break;
	case x86_XMM6: x86Command = 0xB0; break;
	case x86_XMM7: x86Command = 0xB8; break;
	}	
	switch (AddrReg) {
	case x86_EAX: x86Command += 0x00; break;
	case x86_EBX: x86Command += 0x03; break;
	case x86_ECX: x86Command += 0x01; break;
	case x86_EDX: x86Command += 0x02; break;
	case x86_ESI: x86Command += 0x06; break;
	case x86_EDI: x86Command += 0x07; break;
	case x86_ESP: x86Command += 0x04; break;
	case x86_EBP: x86Command += 0x05; break;
	}

	PUTDST16(RecompPos,0x290f);
	PUTDST8(RecompPos, x86Command);
	PUTDST32(RecompPos,RSPInfo.DMEM);
}*/

void SseMoveLowRegToHighReg(BYTE** code, int Dest, int Source) {
	BYTE x86Command = 0;

	RSP_CPU_Message("      movlhps %s, %s", sse_Name(Dest), sse_Name(Source));

	switch (Dest) {
	case x86_XMM0: x86Command = 0 << 3; break;
	case x86_XMM1: x86Command = 1 << 3; break;
	case x86_XMM2: x86Command = 2 << 3; break;
	case x86_XMM3: x86Command = 3 << 3; break;
	case x86_XMM4: x86Command = 4 << 3; break;
	case x86_XMM5: x86Command = 5 << 3; break;
	case x86_XMM6: x86Command = 6 << 3; break;
	case x86_XMM7: x86Command = 7 << 3; break;
	}
	switch (Source) {
	case x86_XMM0: x86Command |= 0; break;
	case x86_XMM1: x86Command |= 1; break;
	case x86_XMM2: x86Command |= 2; break;
	case x86_XMM3: x86Command |= 3; break;
	case x86_XMM4: x86Command |= 4; break;
	case x86_XMM5: x86Command |= 5; break;
	case x86_XMM6: x86Command |= 6; break;
	case x86_XMM7: x86Command |= 7; break;
	}
	PUTDST16(*code, 0x160f);
	PUTDST8(*code, 0xC0 | x86Command);
}

void SseMoveHighRegToLowReg(BYTE** code, int Dest, int Source) {
	BYTE x86Command = 0;

	RSP_CPU_Message("      movhlps %s, %s", sse_Name(Dest), sse_Name(Source));

	switch (Dest) {
	case x86_XMM0: x86Command = 0 << 3; break;
	case x86_XMM1: x86Command = 1 << 3; break;
	case x86_XMM2: x86Command = 2 << 3; break;
	case x86_XMM3: x86Command = 3 << 3; break;
	case x86_XMM4: x86Command = 4 << 3; break;
	case x86_XMM5: x86Command = 5 << 3; break;
	case x86_XMM6: x86Command = 6 << 3; break;
	case x86_XMM7: x86Command = 7 << 3; break;
	}
	switch (Source) {
	case x86_XMM0: x86Command |= 0; break;
	case x86_XMM1: x86Command |= 1; break;
	case x86_XMM2: x86Command |= 2; break;
	case x86_XMM3: x86Command |= 3; break;
	case x86_XMM4: x86Command |= 4; break;
	case x86_XMM5: x86Command |= 5; break;
	case x86_XMM6: x86Command |= 6; break;
	case x86_XMM7: x86Command |= 7; break;
	}
	PUTDST16(*code, 0x120f);
	PUTDST8(*code, 0xC0 | x86Command);
}

/*void SseMoveUnalignedVariableToReg(void *Variable, char *VariableName, int sseReg) {
	BYTE x86Command;

	CPU_Message("      movups %s, xmmword ptr [%s]",sse_Name(sseReg), VariableName);

	switch (sseReg) {
	case x86_XMM0: x86Command = 0x05; break;
	case x86_XMM1: x86Command = 0x0D; break;
	case x86_XMM2: x86Command = 0x15; break;
	case x86_XMM3: x86Command = 0x1D; break;
	case x86_XMM4: x86Command = 0x25; break;
	case x86_XMM5: x86Command = 0x2D; break;
	case x86_XMM6: x86Command = 0x35; break;
	case x86_XMM7: x86Command = 0x3D; break;
	}	

	PUTDST16(RecompPos,0x100f);
	PUTDST8(RecompPos, x86Command);
	PUTDST32(RecompPos,Variable);
}

void SseMoveUnalignedN64MemToReg(int sseReg, int AddrReg) {
	BYTE x86Command;

	CPU_Message("      movups %s, xmmword ptr [Dmem+%s]",sse_Name(sseReg), x86_Name(AddrReg));

	switch (sseReg) {
	case x86_XMM0: x86Command = 0x80; break;
	case x86_XMM1: x86Command = 0x88; break;
	case x86_XMM2: x86Command = 0x90; break;
	case x86_XMM3: x86Command = 0x98; break;
	case x86_XMM4: x86Command = 0xA0; break;
	case x86_XMM5: x86Command = 0xA8; break;
	case x86_XMM6: x86Command = 0xB0; break;
	case x86_XMM7: x86Command = 0xB8; break;
	}	
	switch (AddrReg) {
	case x86_EAX: x86Command += 0x00; break;
	case x86_EBX: x86Command += 0x03; break;
	case x86_ECX: x86Command += 0x01; break;
	case x86_EDX: x86Command += 0x02; break;
	case x86_ESI: x86Command += 0x06; break;
	case x86_EDI: x86Command += 0x07; break;
	case x86_ESP: x86Command += 0x04; break;
	case x86_EBP: x86Command += 0x05; break;
	}

	PUTDST16(RecompPos,0x100f);
	PUTDST8(RecompPos, x86Command);
	PUTDST32(RecompPos,RSPInfo.DMEM);
}

void SseMoveUnalignedRegToVariable(int sseReg, void *Variable, char *VariableName) {
	BYTE x86Command;

	CPU_Message("      movups xmmword ptr [%s], %s",VariableName, sse_Name(sseReg));

	switch (sseReg) {
	case x86_XMM0: x86Command = 0x05; break;
	case x86_XMM1: x86Command = 0x0D; break;
	case x86_XMM2: x86Command = 0x15; break;
	case x86_XMM3: x86Command = 0x1D; break;
	case x86_XMM4: x86Command = 0x25; break;
	case x86_XMM5: x86Command = 0x2D; break;
	case x86_XMM6: x86Command = 0x35; break;
	case x86_XMM7: x86Command = 0x3D; break;
	}	

	PUTDST16(RecompPos,0x110f);
	PUTDST8(RecompPos, x86Command);
	PUTDST32(RecompPos,Variable);
}

void SseMoveUnalignedRegToN64Mem(int sseReg, int AddrReg) {
	BYTE x86Command;

	CPU_Message("      movups xmmword ptr [Dmem+%s], %s",x86_Name(AddrReg),sse_Name(sseReg));

	switch (sseReg) {
	case x86_XMM0: x86Command = 0x80; break;
	case x86_XMM1: x86Command = 0x88; break;
	case x86_XMM2: x86Command = 0x90; break;
	case x86_XMM3: x86Command = 0x98; break;
	case x86_XMM4: x86Command = 0xA0; break;
	case x86_XMM5: x86Command = 0xA8; break;
	case x86_XMM6: x86Command = 0xB0; break;
	case x86_XMM7: x86Command = 0xB8; break;
	}	
	switch (AddrReg) {
	case x86_EAX: x86Command += 0x00; break;
	case x86_EBX: x86Command += 0x03; break;
	case x86_ECX: x86Command += 0x01; break;
	case x86_EDX: x86Command += 0x02; break;
	case x86_ESI: x86Command += 0x06; break;
	case x86_EDI: x86Command += 0x07; break;
	case x86_ESP: x86Command += 0x04; break;
	case x86_EBP: x86Command += 0x05; break;
	}

	PUTDST16(RecompPos,0x110f);
	PUTDST8(RecompPos, x86Command);
	PUTDST32(RecompPos,RSPInfo.DMEM);
}*/

void SseMoveRegToReg(BYTE** code, int Dest, int Source, int sseDataType, BOOL SSE2Supported) {
	if (sseDataType == SseType_QuadWord && SSE2Supported) {
		BYTE x86Command = 0;

		RSP_CPU_Message("      movdqa %s, %s", sse_Name(Dest), sse_Name(Source));

		switch (Dest) {
		case x86_XMM0: x86Command = 0x00; break;
		case x86_XMM1: x86Command = 0x08; break;
		case x86_XMM2: x86Command = 0x10; break;
		case x86_XMM3: x86Command = 0x18; break;
		case x86_XMM4: x86Command = 0x20; break;
		case x86_XMM5: x86Command = 0x28; break;
		case x86_XMM6: x86Command = 0x30; break;
		case x86_XMM7: x86Command = 0x38; break;
		}
		switch (Source) {
		case x86_XMM0: x86Command += 0x00; break;
		case x86_XMM1: x86Command += 0x01; break;
		case x86_XMM2: x86Command += 0x02; break;
		case x86_XMM3: x86Command += 0x03; break;
		case x86_XMM4: x86Command += 0x04; break;
		case x86_XMM5: x86Command += 0x05; break;
		case x86_XMM6: x86Command += 0x06; break;
		case x86_XMM7: x86Command += 0x07; break;
		}

		PUTDST16(*code, 0x0F66);
		PUTDST8(*code, 0x6F);
		PUTDST8(*code, 0xC0 | x86Command);
	} else {
		BYTE x86Command = 0;

		RSP_CPU_Message("      movaps %s, %s", sse_Name(Dest), sse_Name(Source));

		switch (Dest) {
		case x86_XMM0: x86Command = 0x00; break;
		case x86_XMM1: x86Command = 0x08; break;
		case x86_XMM2: x86Command = 0x10; break;
		case x86_XMM3: x86Command = 0x18; break;
		case x86_XMM4: x86Command = 0x20; break;
		case x86_XMM5: x86Command = 0x28; break;
		case x86_XMM6: x86Command = 0x30; break;
		case x86_XMM7: x86Command = 0x38; break;
		}
		switch (Source) {
		case x86_XMM0: x86Command += 0x00; break;
		case x86_XMM1: x86Command += 0x01; break;
		case x86_XMM2: x86Command += 0x02; break;
		case x86_XMM3: x86Command += 0x03; break;
		case x86_XMM4: x86Command += 0x04; break;
		case x86_XMM5: x86Command += 0x05; break;
		case x86_XMM6: x86Command += 0x06; break;
		case x86_XMM7: x86Command += 0x07; break;
		}

		PUTDST16(*code, 0x280f);
		PUTDST8(*code, 0xC0 | x86Command);
	}
}

/*void SseXorRegToReg(int Dest, int Source) {
	BYTE x86Command;

	CPU_Message("      xorps %s, %s", sse_Name(Dest), sse_Name(Source));

	switch (Dest) {
	case x86_XMM0: x86Command = 0x00; break;
	case x86_XMM1: x86Command = 0x08; break;
	case x86_XMM2: x86Command = 0x10; break;
	case x86_XMM3: x86Command = 0x18; break;
	case x86_XMM4: x86Command = 0x20; break;
	case x86_XMM5: x86Command = 0x28; break;
	case x86_XMM6: x86Command = 0x30; break;
	case x86_XMM7: x86Command = 0x28; break;
	}
	switch (Source) {
	case x86_XMM0: x86Command += 0x00; break;
	case x86_XMM1: x86Command += 0x01; break;
	case x86_XMM2: x86Command += 0x02; break;
	case x86_XMM3: x86Command += 0x03; break;
	case x86_XMM4: x86Command += 0x04; break;
	case x86_XMM5: x86Command += 0x05; break;
	case x86_XMM6: x86Command += 0x06; break;
	case x86_XMM7: x86Command += 0x07; break;
	}
	PUTDST16(RecompPos,0x570f);
	PUTDST8(RecompPos, 0xC0 | x86Command);
}

void SseShuffleReg(int Dest, int Source, BYTE Immed) {
	BYTE x86Command;

	CPU_Message("      shufps %s, %s, %02X", sse_Name(Dest), sse_Name(Source), Immed);

	switch (Dest) {
	case x86_XMM0: x86Command = 0x00; break;
	case x86_XMM1: x86Command = 0x08; break;
	case x86_XMM2: x86Command = 0x10; break;
	case x86_XMM3: x86Command = 0x18; break;
	case x86_XMM4: x86Command = 0x20; break;
	case x86_XMM5: x86Command = 0x28; break;
	case x86_XMM6: x86Command = 0x30; break;
	case x86_XMM7: x86Command = 0x38; break;
	}
	switch (Source) {
	case x86_XMM0: x86Command += 0x00; break;
	case x86_XMM1: x86Command += 0x01; break;
	case x86_XMM2: x86Command += 0x02; break;
	case x86_XMM3: x86Command += 0x03; break;
	case x86_XMM4: x86Command += 0x04; break;
	case x86_XMM5: x86Command += 0x05; break;
	case x86_XMM6: x86Command += 0x06; break;
	case x86_XMM7: x86Command += 0x07; break;
	}
	PUTDST16(RecompPos,0xC60f);
	PUTDST8(RecompPos, 0xC0 | x86Command);
	PUTDST8(RecompPos, Immed);
}*/

void Sse2CompareEqualDWordRegToReg(BYTE** code, int Dest, int Source) {
	BYTE x86Command = 0;

	RSP_CPU_Message("      pcmpeqd %s, %s", sse_Name(Dest), sse_Name(Source));

	switch (Dest) {
	case x86_XMM0: x86Command = 0 << 3; break;
	case x86_XMM1: x86Command = 1 << 3; break;
	case x86_XMM2: x86Command = 2 << 3; break;
	case x86_XMM3: x86Command = 3 << 3; break;
	case x86_XMM4: x86Command = 4 << 3; break;
	case x86_XMM5: x86Command = 5 << 3; break;
	case x86_XMM6: x86Command = 6 << 3; break;
	case x86_XMM7: x86Command = 7 << 3; break;
	}
	switch (Source) {
	case x86_XMM0: x86Command |= 0; break;
	case x86_XMM1: x86Command |= 1; break;
	case x86_XMM2: x86Command |= 2; break;
	case x86_XMM3: x86Command |= 3; break;
	case x86_XMM4: x86Command |= 4; break;
	case x86_XMM5: x86Command |= 5; break;
	case x86_XMM6: x86Command |= 6; break;
	case x86_XMM7: x86Command |= 7; break;
	}
	PUTDST8(*code, 0x66);
	PUTDST16(*code, 0x760f);
	PUTDST8(*code, 0xC0 | x86Command);
}

void Sse2CompareEqualDWordVariableToReg(BYTE** code, int Dest, void* Variable, char* VariableName) {
	BYTE x86Command = 0;

	RSP_CPU_Message("      pcmpeqd %s, xmmword ptr %s", sse_Name(Dest), VariableName);

	switch (Dest) {
	case x86_XMM0: x86Command = 0x05; break;
	case x86_XMM1: x86Command = 0x0D; break;
	case x86_XMM2: x86Command = 0x15; break;
	case x86_XMM3: x86Command = 0x1D; break;
	case x86_XMM4: x86Command = 0x25; break;
	case x86_XMM5: x86Command = 0x2D; break;
	case x86_XMM6: x86Command = 0x35; break;
	case x86_XMM7: x86Command = 0x3D; break;
	}
	PUTDST8(*code, 0x66);
	PUTDST16(*code, 0x760f);
	PUTDST8(*code, x86Command);
	PUTDST32(*code, Variable);
}

void Sse2CompareEqualWordRegToReg(BYTE** code, int Dest, int Source) {
	BYTE x86Command = 0;

	RSP_CPU_Message("      pcmpeqw %s, %s", sse_Name(Dest), sse_Name(Source));

	switch (Dest) {
	case x86_XMM0: x86Command = 0 << 3; break;
	case x86_XMM1: x86Command = 1 << 3; break;
	case x86_XMM2: x86Command = 2 << 3; break;
	case x86_XMM3: x86Command = 3 << 3; break;
	case x86_XMM4: x86Command = 4 << 3; break;
	case x86_XMM5: x86Command = 5 << 3; break;
	case x86_XMM6: x86Command = 6 << 3; break;
	case x86_XMM7: x86Command = 7 << 3; break;
	}
	switch (Source) {
	case x86_XMM0: x86Command |= 0; break;
	case x86_XMM1: x86Command |= 1; break;
	case x86_XMM2: x86Command |= 2; break;
	case x86_XMM3: x86Command |= 3; break;
	case x86_XMM4: x86Command |= 4; break;
	case x86_XMM5: x86Command |= 5; break;
	case x86_XMM6: x86Command |= 6; break;
	case x86_XMM7: x86Command |= 7; break;
	}
	PUTDST8(*code, 0x66);
	PUTDST16(*code, 0x750f);
	PUTDST8(*code, 0xC0 | x86Command);
}

void Sse2CompareGreaterWordRegToReg(BYTE** code, int Dest, int Source) {
	BYTE x86Command = 0;

	RSP_CPU_Message("      pcmpgtw %s, %s", sse_Name(Dest), sse_Name(Source));

	switch (Dest) {
	case x86_XMM0: x86Command = 0 << 3; break;
	case x86_XMM1: x86Command = 1 << 3; break;
	case x86_XMM2: x86Command = 2 << 3; break;
	case x86_XMM3: x86Command = 3 << 3; break;
	case x86_XMM4: x86Command = 4 << 3; break;
	case x86_XMM5: x86Command = 5 << 3; break;
	case x86_XMM6: x86Command = 6 << 3; break;
	case x86_XMM7: x86Command = 7 << 3; break;
	}
	switch (Source) {
	case x86_XMM0: x86Command |= 0; break;
	case x86_XMM1: x86Command |= 1; break;
	case x86_XMM2: x86Command |= 2; break;
	case x86_XMM3: x86Command |= 3; break;
	case x86_XMM4: x86Command |= 4; break;
	case x86_XMM5: x86Command |= 5; break;
	case x86_XMM6: x86Command |= 6; break;
	case x86_XMM7: x86Command |= 7; break;
	}
	PUTDST8(*code, 0x66);
	PUTDST16(*code, 0x650f);
	PUTDST8(*code, 0xC0 | x86Command);
}

void Sse2MoveQWordRegToReg(BYTE** code, int Dest, int Source) {
	BYTE x86Command = 0;

	RSP_CPU_Message("      movq %s, %s", sse_Name(Dest), sse_Name(Source));

	switch (Dest) {
	case x86_XMM0: x86Command = 0 << 3; break;
	case x86_XMM1: x86Command = 1 << 3; break;
	case x86_XMM2: x86Command = 2 << 3; break;
	case x86_XMM3: x86Command = 3 << 3; break;
	case x86_XMM4: x86Command = 4 << 3; break;
	case x86_XMM5: x86Command = 5 << 3; break;
	case x86_XMM6: x86Command = 6 << 3; break;
	case x86_XMM7: x86Command = 7 << 3; break;
	}
	switch (Source) {
	case x86_XMM0: x86Command |= 0; break;
	case x86_XMM1: x86Command |= 1; break;
	case x86_XMM2: x86Command |= 2; break;
	case x86_XMM3: x86Command |= 3; break;
	case x86_XMM4: x86Command |= 4; break;
	case x86_XMM5: x86Command |= 5; break;
	case x86_XMM6: x86Command |= 6; break;
	case x86_XMM7: x86Command |= 7; break;
	}
	PUTDST8(*code, 0xf3);
	PUTDST16(*code, 0x7e0f);
	PUTDST8(*code, 0xC0 | x86Command);
}

void Sse2MoveSxWordRegToDWordReg(BYTE** code, int Dest, int Source, BOOL SSE41Supported) {
	if (SSE41Supported) {
		BYTE x86Command = 0;

		RSP_CPU_Message("      pmovsxwd %s, %s", sse_Name(Dest), sse_Name(Source));

		switch (Dest) {
		case x86_XMM0: x86Command = 0 << 3; break;
		case x86_XMM1: x86Command = 1 << 3; break;
		case x86_XMM2: x86Command = 2 << 3; break;
		case x86_XMM3: x86Command = 3 << 3; break;
		case x86_XMM4: x86Command = 4 << 3; break;
		case x86_XMM5: x86Command = 5 << 3; break;
		case x86_XMM6: x86Command = 6 << 3; break;
		case x86_XMM7: x86Command = 7 << 3; break;
		}
		switch (Source) {
		case x86_XMM0: x86Command |= 0; break;
		case x86_XMM1: x86Command |= 1; break;
		case x86_XMM2: x86Command |= 2; break;
		case x86_XMM3: x86Command |= 3; break;
		case x86_XMM4: x86Command |= 4; break;
		case x86_XMM5: x86Command |= 5; break;
		case x86_XMM6: x86Command |= 6; break;
		case x86_XMM7: x86Command |= 7; break;
		}
		PUTDST32(*code, 0x23380f66);
		PUTDST8(*code, 0xC0 | x86Command);
	} else {
		Sse2PunpckLowWordsRegToReg(code, Dest, Source);
		Sse2PsradImmed(code, Dest, 16);
	}
}

void Sse2PackSignedDWordRegToWordReg(BYTE** code, int Dest, int Source) {
	BYTE x86Command = 0;

	RSP_CPU_Message("      packssdw %s, %s", sse_Name(Dest), sse_Name(Source));

	switch (Dest) {
	case x86_XMM0: x86Command = 0 << 3; break;
	case x86_XMM1: x86Command = 1 << 3; break;
	case x86_XMM2: x86Command = 2 << 3; break;
	case x86_XMM3: x86Command = 3 << 3; break;
	case x86_XMM4: x86Command = 4 << 3; break;
	case x86_XMM5: x86Command = 5 << 3; break;
	case x86_XMM6: x86Command = 6 << 3; break;
	case x86_XMM7: x86Command = 7 << 3; break;
	}
	switch (Source) {
	case x86_XMM0: x86Command |= 0; break;
	case x86_XMM1: x86Command |= 1; break;
	case x86_XMM2: x86Command |= 2; break;
	case x86_XMM3: x86Command |= 3; break;
	case x86_XMM4: x86Command |= 4; break;
	case x86_XMM5: x86Command |= 5; break;
	case x86_XMM6: x86Command |= 6; break;
	case x86_XMM7: x86Command |= 7; break;
	}

	PUTDST8(*code, 0x66);
	PUTDST16(*code, 0x6b0f);
	PUTDST8(*code, 0xC0 | x86Command);
}

void Sse2PadddRegToReg(BYTE** code, int Dest, int Source) {
	BYTE x86Command = 0;

	RSP_CPU_Message("      paddd %s, %s", sse_Name(Dest), sse_Name(Source));

	switch (Dest) {
	case x86_XMM0: x86Command = 0 << 3; break;
	case x86_XMM1: x86Command = 1 << 3; break;
	case x86_XMM2: x86Command = 2 << 3; break;
	case x86_XMM3: x86Command = 3 << 3; break;
	case x86_XMM4: x86Command = 4 << 3; break;
	case x86_XMM5: x86Command = 5 << 3; break;
	case x86_XMM6: x86Command = 6 << 3; break;
	case x86_XMM7: x86Command = 7 << 3; break;
	}
	switch (Source) {
	case x86_XMM0: x86Command |= 0; break;
	case x86_XMM1: x86Command |= 1; break;
	case x86_XMM2: x86Command |= 2; break;
	case x86_XMM3: x86Command |= 3; break;
	case x86_XMM4: x86Command |= 4; break;
	case x86_XMM5: x86Command |= 5; break;
	case x86_XMM6: x86Command |= 6; break;
	case x86_XMM7: x86Command |= 7; break;
	}
	PUTDST8(*code, 0x66);
	PUTDST16(*code, 0xfe0f);
	PUTDST8(*code, 0xC0 | x86Command);
}

void Sse2PadddVariableToReg(BYTE** code, int Dest, void* Variable, char* VariableName) {
	BYTE x86Command = 0;

	RSP_CPU_Message("      paddd %s, xmmword ptr %s", sse_Name(Dest), VariableName);

	switch (Dest) {
	case x86_XMM0: x86Command = 0x05; break;
	case x86_XMM1: x86Command = 0x0D; break;
	case x86_XMM2: x86Command = 0x15; break;
	case x86_XMM3: x86Command = 0x1D; break;
	case x86_XMM4: x86Command = 0x25; break;
	case x86_XMM5: x86Command = 0x2D; break;
	case x86_XMM6: x86Command = 0x35; break;
	case x86_XMM7: x86Command = 0x3D; break;
	}

	PUTDST8(*code, 0x66);
	PUTDST16(*code, 0xfe0f);
	PUTDST8(*code, x86Command);
	PUTDST32(*code, Variable);
}

void Sse2PadduswRegToReg(BYTE** code, int Dest, int Source) {
	BYTE x86Command = 0;

	RSP_CPU_Message("      paddusw %s, %s", sse_Name(Dest), sse_Name(Source));

	switch (Dest) {
	case x86_XMM0: x86Command = 0 << 3; break;
	case x86_XMM1: x86Command = 1 << 3; break;
	case x86_XMM2: x86Command = 2 << 3; break;
	case x86_XMM3: x86Command = 3 << 3; break;
	case x86_XMM4: x86Command = 4 << 3; break;
	case x86_XMM5: x86Command = 5 << 3; break;
	case x86_XMM6: x86Command = 6 << 3; break;
	case x86_XMM7: x86Command = 7 << 3; break;
	}
	switch (Source) {
	case x86_XMM0: x86Command |= 0; break;
	case x86_XMM1: x86Command |= 1; break;
	case x86_XMM2: x86Command |= 2; break;
	case x86_XMM3: x86Command |= 3; break;
	case x86_XMM4: x86Command |= 4; break;
	case x86_XMM5: x86Command |= 5; break;
	case x86_XMM6: x86Command |= 6; break;
	case x86_XMM7: x86Command |= 7; break;
	}
	PUTDST8(*code, 0x66);
	PUTDST16(*code, 0xdd0f);
	PUTDST8(*code, 0xC0 | x86Command);
}

void Sse2PaddwRegToReg(BYTE** code, int Dest, int Source) {
	BYTE x86Command = 0;

	RSP_CPU_Message("      paddw %s, %s", sse_Name(Dest), sse_Name(Source));

	switch (Dest) {
	case x86_XMM0: x86Command = 0 << 3; break;
	case x86_XMM1: x86Command = 1 << 3; break;
	case x86_XMM2: x86Command = 2 << 3; break;
	case x86_XMM3: x86Command = 3 << 3; break;
	case x86_XMM4: x86Command = 4 << 3; break;
	case x86_XMM5: x86Command = 5 << 3; break;
	case x86_XMM6: x86Command = 6 << 3; break;
	case x86_XMM7: x86Command = 7 << 3; break;
	}
	switch (Source) {
	case x86_XMM0: x86Command |= 0; break;
	case x86_XMM1: x86Command |= 1; break;
	case x86_XMM2: x86Command |= 2; break;
	case x86_XMM3: x86Command |= 3; break;
	case x86_XMM4: x86Command |= 4; break;
	case x86_XMM5: x86Command |= 5; break;
	case x86_XMM6: x86Command |= 6; break;
	case x86_XMM7: x86Command |= 7; break;
	}
	PUTDST8(*code, 0x66);
	PUTDST16(*code, 0xfd0f);
	PUTDST8(*code, 0xC0 | x86Command);
}

void Sse2PandRegToReg(BYTE** code, int Dest, int Source) {
	BYTE x86Command = 0;

	RSP_CPU_Message("      pand %s, %s", sse_Name(Dest), sse_Name(Source));

	switch (Dest) {
	case x86_XMM0: x86Command = 0 << 3; break;
	case x86_XMM1: x86Command = 1 << 3; break;
	case x86_XMM2: x86Command = 2 << 3; break;
	case x86_XMM3: x86Command = 3 << 3; break;
	case x86_XMM4: x86Command = 4 << 3; break;
	case x86_XMM5: x86Command = 5 << 3; break;
	case x86_XMM6: x86Command = 6 << 3; break;
	case x86_XMM7: x86Command = 7 << 3; break;
	}
	switch (Source) {
	case x86_XMM0: x86Command |= 0; break;
	case x86_XMM1: x86Command |= 1; break;
	case x86_XMM2: x86Command |= 2; break;
	case x86_XMM3: x86Command |= 3; break;
	case x86_XMM4: x86Command |= 4; break;
	case x86_XMM5: x86Command |= 5; break;
	case x86_XMM6: x86Command |= 6; break;
	case x86_XMM7: x86Command |= 7; break;
	}
	PUTDST8(*code, 0x66);
	PUTDST16(*code, 0xdb0f);
	PUTDST8(*code, 0xC0 | x86Command);
}

void Sse2PandVariableToReg(BYTE** code, int Dest, void* Variable, char* VariableName) {
	BYTE x86Command = 0;

	RSP_CPU_Message("      pand %s, xmmword ptr %s", sse_Name(Dest), VariableName);

	switch (Dest) {
	case x86_XMM0: x86Command = 0x05; break;
	case x86_XMM1: x86Command = 0x0D; break;
	case x86_XMM2: x86Command = 0x15; break;
	case x86_XMM3: x86Command = 0x1D; break;
	case x86_XMM4: x86Command = 0x25; break;
	case x86_XMM5: x86Command = 0x2D; break;
	case x86_XMM6: x86Command = 0x35; break;
	case x86_XMM7: x86Command = 0x3D; break;
	}
	PUTDST8(*code, 0x66);
	PUTDST16(*code, 0xdb0f);
	PUTDST8(*code, x86Command);
	PUTDST32(*code, Variable);
}

void Sse2PandnRegToReg( BYTE** code, int Dest, int Source ) {
	BYTE x86Command = 0;

	RSP_CPU_Message("      pandn %s, %s", sse_Name(Dest), sse_Name(Source));

	switch (Dest) {
	case x86_XMM0: x86Command = 0 << 3; break;
	case x86_XMM1: x86Command = 1 << 3; break;
	case x86_XMM2: x86Command = 2 << 3; break;
	case x86_XMM3: x86Command = 3 << 3; break;
	case x86_XMM4: x86Command = 4 << 3; break;
	case x86_XMM5: x86Command = 5 << 3; break;
	case x86_XMM6: x86Command = 6 << 3; break;
	case x86_XMM7: x86Command = 7 << 3; break;
	}
	switch (Source) {
	case x86_XMM0: x86Command |= 0; break;
	case x86_XMM1: x86Command |= 1; break;
	case x86_XMM2: x86Command |= 2; break;
	case x86_XMM3: x86Command |= 3; break;
	case x86_XMM4: x86Command |= 4; break;
	case x86_XMM5: x86Command |= 5; break;
	case x86_XMM6: x86Command |= 6; break;
	case x86_XMM7: x86Command |= 7; break;
	}
	PUTDST8(*code, 0x66);
	PUTDST16(*code, 0xdf0f);
	PUTDST8(*code, 0xC0 | x86Command);
}

void Sse2PmulldRegToReg(BYTE** code, int Dest, int Source) {
	BYTE x86Command = 0;

	RSP_CPU_Message("      pmulld %s, %s", sse_Name(Dest), sse_Name(Source));

	switch (Dest) {
	case x86_XMM0: x86Command = 0 << 3; break;
	case x86_XMM1: x86Command = 1 << 3; break;
	case x86_XMM2: x86Command = 2 << 3; break;
	case x86_XMM3: x86Command = 3 << 3; break;
	case x86_XMM4: x86Command = 4 << 3; break;
	case x86_XMM5: x86Command = 5 << 3; break;
	case x86_XMM6: x86Command = 6 << 3; break;
	case x86_XMM7: x86Command = 7 << 3; break;
	}
	switch (Source) {
	case x86_XMM0: x86Command |= 0; break;
	case x86_XMM1: x86Command |= 1; break;
	case x86_XMM2: x86Command |= 2; break;
	case x86_XMM3: x86Command |= 3; break;
	case x86_XMM4: x86Command |= 4; break;
	case x86_XMM5: x86Command |= 5; break;
	case x86_XMM6: x86Command |= 6; break;
	case x86_XMM7: x86Command |= 7; break;
	}
	PUTDST32(*code, 0x40380f66);
	PUTDST8(*code, 0xC0 | x86Command);
}

void Sse2PmullwRegToReg(BYTE** code, int Dest, int Source) {
	BYTE x86Command = 0;

	RSP_CPU_Message("      pmullw %s, %s", sse_Name(Dest), sse_Name(Source));

	switch (Dest) {
	case x86_XMM0: x86Command = 0 << 3; break;
	case x86_XMM1: x86Command = 1 << 3; break;
	case x86_XMM2: x86Command = 2 << 3; break;
	case x86_XMM3: x86Command = 3 << 3; break;
	case x86_XMM4: x86Command = 4 << 3; break;
	case x86_XMM5: x86Command = 5 << 3; break;
	case x86_XMM6: x86Command = 6 << 3; break;
	case x86_XMM7: x86Command = 7 << 3; break;
	}
	switch (Source) {
	case x86_XMM0: x86Command |= 0; break;
	case x86_XMM1: x86Command |= 1; break;
	case x86_XMM2: x86Command |= 2; break;
	case x86_XMM3: x86Command |= 3; break;
	case x86_XMM4: x86Command |= 4; break;
	case x86_XMM5: x86Command |= 5; break;
	case x86_XMM6: x86Command |= 6; break;
	case x86_XMM7: x86Command |= 7; break;
	}
	PUTDST8(*code, 0x66);
	PUTDST16(*code, 0xd50f);
	PUTDST8(*code, 0xC0 | x86Command);
}

void Sse2PmulhwRegToReg(BYTE** code, int Dest, int Source) {
	BYTE x86Command = 0;

	RSP_CPU_Message("      pmulhw %s, %s", sse_Name(Dest), sse_Name(Source));

	switch (Dest) {
	case x86_XMM0: x86Command = 0 << 3; break;
	case x86_XMM1: x86Command = 1 << 3; break;
	case x86_XMM2: x86Command = 2 << 3; break;
	case x86_XMM3: x86Command = 3 << 3; break;
	case x86_XMM4: x86Command = 4 << 3; break;
	case x86_XMM5: x86Command = 5 << 3; break;
	case x86_XMM6: x86Command = 6 << 3; break;
	case x86_XMM7: x86Command = 7 << 3; break;
	}
	switch (Source) {
	case x86_XMM0: x86Command |= 0; break;
	case x86_XMM1: x86Command |= 1; break;
	case x86_XMM2: x86Command |= 2; break;
	case x86_XMM3: x86Command |= 3; break;
	case x86_XMM4: x86Command |= 4; break;
	case x86_XMM5: x86Command |= 5; break;
	case x86_XMM6: x86Command |= 6; break;
	case x86_XMM7: x86Command |= 7; break;
	}
	PUTDST8(*code, 0x66);
	PUTDST16(*code, 0xe50f);
	PUTDST8(*code, 0xC0 | x86Command);
}

void Sse2PorRegToReg(BYTE** code, int Dest, int Source) {
	BYTE x86Command = 0;

	RSP_CPU_Message("      por %s, %s", sse_Name(Dest), sse_Name(Source));

	switch (Dest) {
	case x86_XMM0: x86Command = 0 << 3; break;
	case x86_XMM1: x86Command = 1 << 3; break;
	case x86_XMM2: x86Command = 2 << 3; break;
	case x86_XMM3: x86Command = 3 << 3; break;
	case x86_XMM4: x86Command = 4 << 3; break;
	case x86_XMM5: x86Command = 5 << 3; break;
	case x86_XMM6: x86Command = 6 << 3; break;
	case x86_XMM7: x86Command = 7 << 3; break;
	}
	switch (Source) {
	case x86_XMM0: x86Command |= 0; break;
	case x86_XMM1: x86Command |= 1; break;
	case x86_XMM2: x86Command |= 2; break;
	case x86_XMM3: x86Command |= 3; break;
	case x86_XMM4: x86Command |= 4; break;
	case x86_XMM5: x86Command |= 5; break;
	case x86_XMM6: x86Command |= 6; break;
	case x86_XMM7: x86Command |= 7; break;
	}
	PUTDST8(*code, 0x66);
	PUTDST16(*code, 0xeb0f);
	PUTDST8(*code, 0xC0 | x86Command);
}

void Sse2PorVariableToReg(BYTE** code, int Dest, void* Variable, char* VariableName) {
	BYTE x86Command = 0;

	RSP_CPU_Message("      por %s, xmmword ptr %s", sse_Name(Dest), VariableName);

	switch (Dest) {
	case x86_XMM0: x86Command = 0x05; break;
	case x86_XMM1: x86Command = 0x0D; break;
	case x86_XMM2: x86Command = 0x15; break;
	case x86_XMM3: x86Command = 0x1D; break;
	case x86_XMM4: x86Command = 0x25; break;
	case x86_XMM5: x86Command = 0x2D; break;
	case x86_XMM6: x86Command = 0x35; break;
	case x86_XMM7: x86Command = 0x3D; break;
	}
	PUTDST8(*code, 0x66);
	PUTDST16(*code, 0xeb0f);
	PUTDST8(*code, x86Command);
	PUTDST32(*code, Variable);
}

void Sse2PslldImmed(BYTE** code, int Dest, BYTE Immed) {
	BYTE x86Command = 0;

	RSP_CPU_Message("      psllw %d, %i", sse_Name(Dest), Immed);

	switch (Dest) {
	case x86_XMM0: x86Command = 0xF0; break;
	case x86_XMM1: x86Command = 0xF1; break;
	case x86_XMM2: x86Command = 0xF2; break;
	case x86_XMM3: x86Command = 0xF3; break;
	case x86_XMM4: x86Command = 0xF4; break;
	case x86_XMM5: x86Command = 0xF5; break;
	case x86_XMM6: x86Command = 0xF6; break;
	case x86_XMM7: x86Command = 0xF7; break;
	}

	PUTDST8(*code, 0x66);
	PUTDST16(*code, 0x720f);
	PUTDST8(*code, x86Command);
	PUTDST8(*code, Immed);
}

void Sse2PsllwImmed(BYTE** code, int Dest, BYTE Immed) {
	BYTE x86Command = 0;

	RSP_CPU_Message("      psllw %s, %i", sse_Name(Dest), Immed);

	switch (Dest) {
	case x86_XMM0: x86Command = 0xF0; break;
	case x86_XMM1: x86Command = 0xF1; break;
	case x86_XMM2: x86Command = 0xF2; break;
	case x86_XMM3: x86Command = 0xF3; break;
	case x86_XMM4: x86Command = 0xF4; break;
	case x86_XMM5: x86Command = 0xF5; break;
	case x86_XMM6: x86Command = 0xF6; break;
	case x86_XMM7: x86Command = 0xF7; break;
	}

	PUTDST8(*code, 0x66);
	PUTDST16(*code, 0x710f);
	PUTDST8(*code, x86Command);
	PUTDST8(*code, Immed);
}

void Sse2PsradImmed(BYTE** code, int Dest, BYTE Immed) {
	BYTE x86Command = 0;

	RSP_CPU_Message("      psrad %s, %i", sse_Name(Dest), Immed);

	switch (Dest) {
	case x86_XMM0: x86Command = 0xE0; break;
	case x86_XMM1: x86Command = 0xE1; break;
	case x86_XMM2: x86Command = 0xE2; break;
	case x86_XMM3: x86Command = 0xE3; break;
	case x86_XMM4: x86Command = 0xE4; break;
	case x86_XMM5: x86Command = 0xE5; break;
	case x86_XMM6: x86Command = 0xE6; break;
	case x86_XMM7: x86Command = 0xE7; break;
	}

	PUTDST8(*code, 0x66);
	PUTDST16(*code, 0x720f);
	PUTDST8(*code, x86Command);
	PUTDST8(*code, Immed);
}

void Sse2PsrawImmed(BYTE** code, int Dest, BYTE Immed) {
	BYTE x86Command = 0;

	RSP_CPU_Message("      psraw %s, %i", sse_Name(Dest), Immed);

	switch (Dest) {
	case x86_XMM0: x86Command = 0xE0; break;
	case x86_XMM1: x86Command = 0xE1; break;
	case x86_XMM2: x86Command = 0xE2; break;
	case x86_XMM3: x86Command = 0xE3; break;
	case x86_XMM4: x86Command = 0xE4; break;
	case x86_XMM5: x86Command = 0xE5; break;
	case x86_XMM6: x86Command = 0xE6; break;
	case x86_XMM7: x86Command = 0xE7; break;
	}

	PUTDST8(*code, 0x66);
	PUTDST16(*code, 0x710f);
	PUTDST8(*code, x86Command);
	PUTDST8(*code, Immed);
}

void Sse2PsrldImmed(BYTE** code, int Dest, BYTE Immed) {
	BYTE x86Command = 0;

	RSP_CPU_Message("      psrld %s, %i", sse_Name(Dest), Immed);

	switch (Dest) {
	case x86_XMM0: x86Command = 0xD0; break;
	case x86_XMM1: x86Command = 0xD1; break;
	case x86_XMM2: x86Command = 0xD2; break;
	case x86_XMM3: x86Command = 0xD3; break;
	case x86_XMM4: x86Command = 0xD4; break;
	case x86_XMM5: x86Command = 0xD5; break;
	case x86_XMM6: x86Command = 0xD6; break;
	case x86_XMM7: x86Command = 0xD7; break;
	}

	PUTDST8(*code, 0x66);
	PUTDST16(*code, 0x720f);
	PUTDST8(*code, x86Command);
	PUTDST8(*code, Immed);
}

void Sse2PsrlwImmed(BYTE** code, int Dest, BYTE Immed) {
	BYTE x86Command = 0;

	RSP_CPU_Message("      psrlw %s, %i", sse_Name(Dest), Immed);

	switch (Dest) {
	case x86_XMM0: x86Command = 0xD0; break;
	case x86_XMM1: x86Command = 0xD1; break;
	case x86_XMM2: x86Command = 0xD2; break;
	case x86_XMM3: x86Command = 0xD3; break;
	case x86_XMM4: x86Command = 0xD4; break;
	case x86_XMM5: x86Command = 0xD5; break;
	case x86_XMM6: x86Command = 0xD6; break;
	case x86_XMM7: x86Command = 0xD7; break;
	}

	PUTDST8(*code, 0x66);
	PUTDST16(*code, 0x710f);
	PUTDST8(*code, x86Command);
	PUTDST8(*code, Immed);
}

void Sse2PunpckHighWordsRegToReg(BYTE** code, int Dest, int Source) {
	BYTE x86Command = 0;

	RSP_CPU_Message("      punpckhwd %s, %s", sse_Name(Dest), sse_Name(Source));

	switch (Dest) {
	case x86_XMM0: x86Command = 0 << 3; break;
	case x86_XMM1: x86Command = 1 << 3; break;
	case x86_XMM2: x86Command = 2 << 3; break;
	case x86_XMM3: x86Command = 3 << 3; break;
	case x86_XMM4: x86Command = 4 << 3; break;
	case x86_XMM5: x86Command = 5 << 3; break;
	case x86_XMM6: x86Command = 6 << 3; break;
	case x86_XMM7: x86Command = 7 << 3; break;
	}
	switch (Source) {
	case x86_XMM0: x86Command |= 0; break;
	case x86_XMM1: x86Command |= 1; break;
	case x86_XMM2: x86Command |= 2; break;
	case x86_XMM3: x86Command |= 3; break;
	case x86_XMM4: x86Command |= 4; break;
	case x86_XMM5: x86Command |= 5; break;
	case x86_XMM6: x86Command |= 6; break;
	case x86_XMM7: x86Command |= 7; break;
	}

	PUTDST8(*code, 0x66);
	PUTDST16(*code, 0x690f);
	PUTDST8(*code, 0xC0 | x86Command);
}

void Sse2PunpckLowWordsRegToReg(BYTE** code, int Dest, int Source) {
	BYTE x86Command = 0;

	RSP_CPU_Message("      punpcklwd %s, %s", sse_Name(Dest), sse_Name(Source));

	switch (Dest) {
	case x86_XMM0: x86Command = 0 << 3; break;
	case x86_XMM1: x86Command = 1 << 3; break;
	case x86_XMM2: x86Command = 2 << 3; break;
	case x86_XMM3: x86Command = 3 << 3; break;
	case x86_XMM4: x86Command = 4 << 3; break;
	case x86_XMM5: x86Command = 5 << 3; break;
	case x86_XMM6: x86Command = 6 << 3; break;
	case x86_XMM7: x86Command = 7 << 3; break;
	}
	switch (Source) {
	case x86_XMM0: x86Command |= 0; break;
	case x86_XMM1: x86Command |= 1; break;
	case x86_XMM2: x86Command |= 2; break;
	case x86_XMM3: x86Command |= 3; break;
	case x86_XMM4: x86Command |= 4; break;
	case x86_XMM5: x86Command |= 5; break;
	case x86_XMM6: x86Command |= 6; break;
	case x86_XMM7: x86Command |= 7; break;
	}

	PUTDST8(*code, 0x66);
	PUTDST16(*code, 0x610f);
	PUTDST8(*code, 0xC0 | x86Command);
}

void Sse2PxorRegToReg(BYTE** code, int Dest, int Source) {
	BYTE x86Command = 0;

	RSP_CPU_Message("      pxor %s, %s", sse_Name(Dest), sse_Name(Source));

	switch (Dest) {
	case x86_XMM0: x86Command = 0 << 3; break;
	case x86_XMM1: x86Command = 1 << 3; break;
	case x86_XMM2: x86Command = 2 << 3; break;
	case x86_XMM3: x86Command = 3 << 3; break;
	case x86_XMM4: x86Command = 4 << 3; break;
	case x86_XMM5: x86Command = 5 << 3; break;
	case x86_XMM6: x86Command = 6 << 3; break;
	case x86_XMM7: x86Command = 7 << 3; break;
	}
	switch (Source) {
	case x86_XMM0: x86Command |= 0; break;
	case x86_XMM1: x86Command |= 1; break;
	case x86_XMM2: x86Command |= 2; break;
	case x86_XMM3: x86Command |= 3; break;
	case x86_XMM4: x86Command |= 4; break;
	case x86_XMM5: x86Command |= 5; break;
	case x86_XMM6: x86Command |= 6; break;
	case x86_XMM7: x86Command |= 7; break;
	}
	PUTDST8(*code, 0x66);
	PUTDST16(*code, 0xef0f);
	PUTDST8(*code, 0xC0 | x86Command);
}

void Sse2ShuffleDWordsRegToReg(BYTE** code, int Dest, int Source, BYTE Immed) {
	BYTE x86Command = 0;

	RSP_CPU_Message("      pshufd %s, %s, %02X", sse_Name(Dest), sse_Name(Source), Immed);

	switch (Dest) {
	case x86_XMM0: x86Command = 0 << 3; break;
	case x86_XMM1: x86Command = 1 << 3; break;
	case x86_XMM2: x86Command = 2 << 3; break;
	case x86_XMM3: x86Command = 3 << 3; break;
	case x86_XMM4: x86Command = 4 << 3; break;
	case x86_XMM5: x86Command = 5 << 3; break;
	case x86_XMM6: x86Command = 6 << 3; break;
	case x86_XMM7: x86Command = 7 << 3; break;
	}
	switch (Source) {
	case x86_XMM0: x86Command |= 0; break;
	case x86_XMM1: x86Command |= 1; break;
	case x86_XMM2: x86Command |= 2; break;
	case x86_XMM3: x86Command |= 3; break;
	case x86_XMM4: x86Command |= 4; break;
	case x86_XMM5: x86Command |= 5; break;
	case x86_XMM6: x86Command |= 6; break;
	case x86_XMM7: x86Command |= 7; break;
	}

	PUTDST8(*code, 0x66);
	PUTDST16(*code, 0x700f);
	PUTDST8(*code, 0xC0 | x86Command);
	PUTDST8(*code, Immed);
}

void Sse2ShuffleLowWordsMemoryToReg(BYTE** code, int Dest, void* Variable, char* VariableName, BYTE Immed) {
	BYTE x86Command = 0;

	RSP_CPU_Message("      pshuflw %s, xmmword ptr [%s], %02X", sse_Name(Dest), VariableName, Immed);

	switch (Dest) {
	case x86_XMM0: x86Command = 0x05; break;
	case x86_XMM1: x86Command = 0x0D; break;
	case x86_XMM2: x86Command = 0x15; break;
	case x86_XMM3: x86Command = 0x1D; break;
	case x86_XMM4: x86Command = 0x25; break;
	case x86_XMM5: x86Command = 0x2D; break;
	case x86_XMM6: x86Command = 0x35; break;
	case x86_XMM7: x86Command = 0x3D; break;
	}

	PUTDST8(*code, 0xf2);
	PUTDST16(*code, 0x700f);
	PUTDST8(*code, x86Command);
	PUTDST32(*code, Variable);
	PUTDST8(*code, Immed);
}

void Sse2ShuffleLowWordsRegToReg(BYTE** code, int Dest, int Source, BYTE Immed) {
	BYTE x86Command = 0;

	RSP_CPU_Message("      pshuflw %s, %s, %02X", sse_Name(Dest), sse_Name(Source), Immed);

	switch (Dest) {
	case x86_XMM0: x86Command = 0 << 3; break;
	case x86_XMM1: x86Command = 1 << 3; break;
	case x86_XMM2: x86Command = 2 << 3; break;
	case x86_XMM3: x86Command = 3 << 3; break;
	case x86_XMM4: x86Command = 4 << 3; break;
	case x86_XMM5: x86Command = 5 << 3; break;
	case x86_XMM6: x86Command = 6 << 3; break;
	case x86_XMM7: x86Command = 7 << 3; break;
	}
	switch (Source) {
	case x86_XMM0: x86Command |= 0; break;
	case x86_XMM1: x86Command |= 1; break;
	case x86_XMM2: x86Command |= 2; break;
	case x86_XMM3: x86Command |= 3; break;
	case x86_XMM4: x86Command |= 4; break;
	case x86_XMM5: x86Command |= 5; break;
	case x86_XMM6: x86Command |= 6; break;
	case x86_XMM7: x86Command |= 7; break;
	}

	PUTDST8(*code, 0xf2);
	PUTDST16(*code, 0x700f);
	PUTDST8(*code, 0xC0 | x86Command);
	PUTDST8(*code, Immed);
}

void Sse2ShuffleHighWordsMemoryToReg(BYTE** code, int Dest, void* Variable, char* VariableName, BYTE Immed) {
	BYTE x86Command = 0;

	RSP_CPU_Message("      pshufhw %s, xmmword ptr [%s], %02X", sse_Name(Dest), VariableName, Immed);

	switch (Dest) {
	case x86_XMM0: x86Command = 0x05; break;
	case x86_XMM1: x86Command = 0x0D; break;
	case x86_XMM2: x86Command = 0x15; break;
	case x86_XMM3: x86Command = 0x1D; break;
	case x86_XMM4: x86Command = 0x25; break;
	case x86_XMM5: x86Command = 0x2D; break;
	case x86_XMM6: x86Command = 0x35; break;
	case x86_XMM7: x86Command = 0x3D; break;
	}

	PUTDST8(*code, 0xf3);
	PUTDST16(*code, 0x700f);
	PUTDST8(*code, x86Command);
	PUTDST32(*code, Variable);
	PUTDST8(*code, Immed);
}

void Sse2ShuffleHighWordsRegToReg(BYTE** code, int Dest, int Source, BYTE Immed) {
	BYTE x86Command = 0;

	RSP_CPU_Message("      pshufhw %s, %s, %02X", sse_Name(Dest), sse_Name(Source), Immed);

	switch (Dest) {
	case x86_XMM0: x86Command = 0 << 3; break;
	case x86_XMM1: x86Command = 1 << 3; break;
	case x86_XMM2: x86Command = 2 << 3; break;
	case x86_XMM3: x86Command = 3 << 3; break;
	case x86_XMM4: x86Command = 4 << 3; break;
	case x86_XMM5: x86Command = 5 << 3; break;
	case x86_XMM6: x86Command = 6 << 3; break;
	case x86_XMM7: x86Command = 7 << 3; break;
	}
	switch (Source) {
	case x86_XMM0: x86Command |= 0; break;
	case x86_XMM1: x86Command |= 1; break;
	case x86_XMM2: x86Command |= 2; break;
	case x86_XMM3: x86Command |= 3; break;
	case x86_XMM4: x86Command |= 4; break;
	case x86_XMM5: x86Command |= 5; break;
	case x86_XMM6: x86Command |= 6; break;
	case x86_XMM7: x86Command |= 7; break;
	}

	PUTDST8(*code, 0xf3);
	PUTDST16(*code, 0x700f);
	PUTDST8(*code, 0xC0 | x86Command);
	PUTDST8(*code, Immed);
}

void Sse41PackUnsignedDWordRegToWordReg(BYTE** code, int Dest, int Source) {
	BYTE x86Command = 0;

	RSP_CPU_Message("      packusdw %s, %s", sse_Name(Dest), sse_Name(Source));

	switch (Dest) {
	case x86_XMM0: x86Command = 0 << 3; break;
	case x86_XMM1: x86Command = 1 << 3; break;
	case x86_XMM2: x86Command = 2 << 3; break;
	case x86_XMM3: x86Command = 3 << 3; break;
	case x86_XMM4: x86Command = 4 << 3; break;
	case x86_XMM5: x86Command = 5 << 3; break;
	case x86_XMM6: x86Command = 6 << 3; break;
	case x86_XMM7: x86Command = 7 << 3; break;
	}
	switch (Source) {
	case x86_XMM0: x86Command |= 0; break;
	case x86_XMM1: x86Command |= 1; break;
	case x86_XMM2: x86Command |= 2; break;
	case x86_XMM3: x86Command |= 3; break;
	case x86_XMM4: x86Command |= 4; break;
	case x86_XMM5: x86Command |= 5; break;
	case x86_XMM6: x86Command |= 6; break;
	case x86_XMM7: x86Command |= 7; break;
	}

	PUTDST32(*code, 0x2b380f66);
	PUTDST8(*code, 0xC0 | x86Command);
}

void Sse41PBlendVariableToRegWithXMM0Mask(BYTE** code, int Dest, void* Variable, char* VariableName) {
	BYTE x86Command = 0;

	RSP_CPU_Message("      pblendvb %s, xmmword ptr [%s], xmm0", sse_Name(Dest), VariableName);

	switch (Dest) {
	case x86_XMM0: x86Command = 0x05; break;
	case x86_XMM1: x86Command = 0x0D; break;
	case x86_XMM2: x86Command = 0x15; break;
	case x86_XMM3: x86Command = 0x1D; break;
	case x86_XMM4: x86Command = 0x25; break;
	case x86_XMM5: x86Command = 0x2D; break;
	case x86_XMM6: x86Command = 0x35; break;
	case x86_XMM7: x86Command = 0x3D; break;
	}

	PUTDST32(*code, 0x10380f66);
	PUTDST8(*code, x86Command);
	PUTDST32(*code, Variable);
}
