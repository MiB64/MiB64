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
#include "X86.h"
#include "RSP/rsp_log.h"

#define PUTDST8(dest,value)  (*((BYTE *)(dest))=(BYTE)(value)); dest += 1;
#define PUTDST16(dest,value) (*((WORD *)(dest))=(WORD)(value)); dest += 2;
#define PUTDST32(dest,value) (*((DWORD *)(dest))=(DWORD)(value)); dest += 4;

char* avx_Strings[8] = {
	"ymm0", "ymm1", "ymm2", "ymm3",
	"ymm4", "ymm5", "ymm6", "ymm7"
};

extern char* sse_Strings[8];

#define avx_Name(Reg) (avx_Strings[(Reg)])
#define sse_Name(Reg) (sse_Strings[(Reg)])

void AvxCompareEqualDWordRegToReg256(BYTE** code, int Dest, int Src1, int Src2) {
	BYTE x86Command = 0;
	BYTE x86Src1 = 0;

	RSP_CPU_Message("      vpcmpeqd %s, %s, %s", avx_Name(Dest), avx_Name(Src1), avx_Name(Src2));

	switch (Src1) {
	case x86_YMM0: x86Src1 = 0xfd; break;
	case x86_YMM1: x86Src1 = 0xf5; break;
	case x86_YMM2: x86Src1 = 0xed; break;
	case x86_YMM3: x86Src1 = 0xe5; break;
	case x86_YMM4: x86Src1 = 0xdd; break;
	case x86_YMM5: x86Src1 = 0xd5; break;
	case x86_YMM6: x86Src1 = 0xcd; break;
	case x86_YMM7: x86Src1 = 0xc5; break;
	}
	switch (Dest) {
	case x86_YMM0: x86Command = 0 << 3; break;
	case x86_YMM1: x86Command = 1 << 3; break;
	case x86_YMM2: x86Command = 2 << 3; break;
	case x86_YMM3: x86Command = 3 << 3; break;
	case x86_YMM4: x86Command = 4 << 3; break;
	case x86_YMM5: x86Command = 5 << 3; break;
	case x86_YMM6: x86Command = 6 << 3; break;
	case x86_YMM7: x86Command = 7 << 3; break;
	}
	switch (Src2) {
	case x86_YMM0: x86Command |= 0; break;
	case x86_YMM1: x86Command |= 1; break;
	case x86_YMM2: x86Command |= 2; break;
	case x86_YMM3: x86Command |= 3; break;
	case x86_YMM4: x86Command |= 4; break;
	case x86_YMM5: x86Command |= 5; break;
	case x86_YMM6: x86Command |= 6; break;
	case x86_YMM7: x86Command |= 7; break;
	}
	PUTDST8(*code, 0xc5);
	PUTDST8(*code, x86Src1);
	PUTDST8(*code, 0x76);
	PUTDST8(*code, 0xc0 | x86Command);
}

void AvxCompareGreaterDWordRegToReg256(BYTE** code, int Dest, int Src1, int Src2) {
	BYTE x86Command = 0;
	BYTE x86Src1 = 0;

	RSP_CPU_Message("      vpcmpgtd %s, %s, %s", avx_Name(Dest), avx_Name(Src1), avx_Name(Src2));

	switch (Src1) {
	case x86_YMM0: x86Src1 = 0xfd; break;
	case x86_YMM1: x86Src1 = 0xf5; break;
	case x86_YMM2: x86Src1 = 0xed; break;
	case x86_YMM3: x86Src1 = 0xe5; break;
	case x86_YMM4: x86Src1 = 0xdd; break;
	case x86_YMM5: x86Src1 = 0xd5; break;
	case x86_YMM6: x86Src1 = 0xcd; break;
	case x86_YMM7: x86Src1 = 0xc5; break;
	}
	switch (Dest) {
	case x86_YMM0: x86Command = 0 << 3; break;
	case x86_YMM1: x86Command = 1 << 3; break;
	case x86_YMM2: x86Command = 2 << 3; break;
	case x86_YMM3: x86Command = 3 << 3; break;
	case x86_YMM4: x86Command = 4 << 3; break;
	case x86_YMM5: x86Command = 5 << 3; break;
	case x86_YMM6: x86Command = 6 << 3; break;
	case x86_YMM7: x86Command = 7 << 3; break;
	}
	switch (Src2) {
	case x86_YMM0: x86Command |= 0; break;
	case x86_YMM1: x86Command |= 1; break;
	case x86_YMM2: x86Command |= 2; break;
	case x86_YMM3: x86Command |= 3; break;
	case x86_YMM4: x86Command |= 4; break;
	case x86_YMM5: x86Command |= 5; break;
	case x86_YMM6: x86Command |= 6; break;
	case x86_YMM7: x86Command |= 7; break;
	}
	PUTDST8(*code, 0xc5);
	PUTDST8(*code, x86Src1);
	PUTDST8(*code, 0x66);
	PUTDST8(*code, 0xc0 | x86Command);
}

void AvxVPackSignedDWordRegToWordReg128(BYTE** code, int Dest, int Src1, int Src2) {
	BYTE x86Command = 0;
	BYTE x86Src1 = 0;

	RSP_CPU_Message("      vpackssdw %s, %s, %s", sse_Name(Dest), sse_Name(Src1), sse_Name(Src2));

	switch (Src1) {
	case x86_XMM0: x86Src1 = 0xf9; break;
	case x86_XMM1: x86Src1 = 0xf1; break;
	case x86_XMM2: x86Src1 = 0xe9; break;
	case x86_XMM3: x86Src1 = 0xe1; break;
	case x86_XMM4: x86Src1 = 0xd9; break;
	case x86_XMM5: x86Src1 = 0xd1; break;
	case x86_XMM6: x86Src1 = 0xc9; break;
	case x86_XMM7: x86Src1 = 0xc1; break;
	}
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
	switch (Src2) {
	case x86_XMM0: x86Command |= 0; break;
	case x86_XMM1: x86Command |= 1; break;
	case x86_XMM2: x86Command |= 2; break;
	case x86_XMM3: x86Command |= 3; break;
	case x86_XMM4: x86Command |= 4; break;
	case x86_XMM5: x86Command |= 5; break;
	case x86_XMM6: x86Command |= 6; break;
	case x86_XMM7: x86Command |= 7; break;
	}

	PUTDST8(*code, 0xc5);
	PUTDST8(*code, x86Src1);
	PUTDST8(*code, 0x6b);
	PUTDST8(*code, 0xC0 | x86Command);
}

void AvxVPackUnsignedDWordRegToWordReg128(BYTE** code, int Dest, int Src1, int Src2) {
	BYTE x86Command = 0;
	BYTE x86Src1 = 0;

	RSP_CPU_Message("      vpackusdw %s, %s, %s", sse_Name(Dest), sse_Name(Src1), sse_Name(Src2));

	switch (Src1) {
	case x86_XMM0: x86Src1 = 0x79; break;
	case x86_XMM1: x86Src1 = 0x71; break;
	case x86_XMM2: x86Src1 = 0x69; break;
	case x86_XMM3: x86Src1 = 0x61; break;
	case x86_XMM4: x86Src1 = 0x59; break;
	case x86_XMM5: x86Src1 = 0x51; break;
	case x86_XMM6: x86Src1 = 0x49; break;
	case x86_XMM7: x86Src1 = 0x41; break;
	}
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
	switch (Src2) {
	case x86_XMM0: x86Command |= 0; break;
	case x86_XMM1: x86Command |= 1; break;
	case x86_XMM2: x86Command |= 2; break;
	case x86_XMM3: x86Command |= 3; break;
	case x86_XMM4: x86Command |= 4; break;
	case x86_XMM5: x86Command |= 5; break;
	case x86_XMM6: x86Command |= 6; break;
	case x86_XMM7: x86Command |= 7; break;
	}

	PUTDST16(*code, 0xe2c4);
	PUTDST8(*code, x86Src1);
	PUTDST8(*code, 0x2b);
	PUTDST8(*code, 0xC0 | x86Command);
}

void AvxVPAdddRegToReg256(BYTE** code, int Dest, int Src1, int Src2) {
	BYTE x86Command = 0;
	BYTE x86Src1 = 0;

	RSP_CPU_Message("      vpaddd %s, %s, %s", avx_Name(Dest), avx_Name(Src1), avx_Name(Src2));

	switch (Src1) {
	case x86_YMM0: x86Src1 = 0xfd; break;
	case x86_YMM1: x86Src1 = 0xf5; break;
	case x86_YMM2: x86Src1 = 0xed; break;
	case x86_YMM3: x86Src1 = 0xe5; break;
	case x86_YMM4: x86Src1 = 0xdd; break;
	case x86_YMM5: x86Src1 = 0xd5; break;
	case x86_YMM6: x86Src1 = 0xcd; break;
	case x86_YMM7: x86Src1 = 0xc5; break;
	}
	switch (Dest) {
	case x86_YMM0: x86Command = 0 << 3; break;
	case x86_YMM1: x86Command = 1 << 3; break;
	case x86_YMM2: x86Command = 2 << 3; break;
	case x86_YMM3: x86Command = 3 << 3; break;
	case x86_YMM4: x86Command = 4 << 3; break;
	case x86_YMM5: x86Command = 5 << 3; break;
	case x86_YMM6: x86Command = 6 << 3; break;
	case x86_YMM7: x86Command = 7 << 3; break;
	}
	switch (Src2) {
	case x86_YMM0: x86Command |= 0; break;
	case x86_YMM1: x86Command |= 1; break;
	case x86_YMM2: x86Command |= 2; break;
	case x86_YMM3: x86Command |= 3; break;
	case x86_YMM4: x86Command |= 4; break;
	case x86_YMM5: x86Command |= 5; break;
	case x86_YMM6: x86Command |= 6; break;
	case x86_YMM7: x86Command |= 7; break;
	}
	PUTDST8(*code, 0xc5);
	PUTDST8(*code, x86Src1);
	PUTDST8(*code, 0xfe);
	PUTDST8(*code, 0xc0 | x86Command);
}

void AvxVPAddwRegToReg128(BYTE** code, int Dest, int Src1, int Src2) {
	BYTE x86Command = 0;
	BYTE x86Src1 = 0;

	RSP_CPU_Message("      vpaddw %s, %s, %s", sse_Name(Dest), sse_Name(Src1), sse_Name(Src2));

	switch (Src1) {
	case x86_XMM0: x86Src1 = 0xf9; break;
	case x86_XMM1: x86Src1 = 0xf1; break;
	case x86_XMM2: x86Src1 = 0xe9; break;
	case x86_XMM3: x86Src1 = 0xe1; break;
	case x86_XMM4: x86Src1 = 0xd9; break;
	case x86_XMM5: x86Src1 = 0xd1; break;
	case x86_XMM6: x86Src1 = 0xc9; break;
	case x86_XMM7: x86Src1 = 0xc1; break;
	}
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
	switch (Src2) {
	case x86_XMM0: x86Command |= 0; break;
	case x86_XMM1: x86Command |= 1; break;
	case x86_XMM2: x86Command |= 2; break;
	case x86_XMM3: x86Command |= 3; break;
	case x86_XMM4: x86Command |= 4; break;
	case x86_XMM5: x86Command |= 5; break;
	case x86_XMM6: x86Command |= 6; break;
	case x86_XMM7: x86Command |= 7; break;
	}
	PUTDST8(*code, 0xc5);
	PUTDST8(*code, x86Src1);
	PUTDST8(*code, 0xfd);
	PUTDST8(*code, 0xc0 | x86Command);
}

void AvxVPandnRegToReg128(BYTE** code, int Dest, int Src1, int Src2) {
	BYTE x86Command = 0;
	BYTE x86Src1 = 0;

	RSP_CPU_Message("      vpandn %s, %s, %s", sse_Name(Dest), sse_Name(Src1), sse_Name(Src2));

	switch (Src1) {
	case x86_XMM0: x86Src1 = 0xf9; break;
	case x86_XMM1: x86Src1 = 0xf1; break;
	case x86_XMM2: x86Src1 = 0xe9; break;
	case x86_XMM3: x86Src1 = 0xe1; break;
	case x86_XMM4: x86Src1 = 0xd9; break;
	case x86_XMM5: x86Src1 = 0xd1; break;
	case x86_XMM6: x86Src1 = 0xc9; break;
	case x86_XMM7: x86Src1 = 0xc1; break;
	}
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
	switch (Src2) {
	case x86_XMM0: x86Command |= 0; break;
	case x86_XMM1: x86Command |= 1; break;
	case x86_XMM2: x86Command |= 2; break;
	case x86_XMM3: x86Command |= 3; break;
	case x86_XMM4: x86Command |= 4; break;
	case x86_XMM5: x86Command |= 5; break;
	case x86_XMM6: x86Command |= 6; break;
	case x86_XMM7: x86Command |= 7; break;
	}
	PUTDST8(*code, 0xc5);
	PUTDST8(*code, x86Src1);
	PUTDST8(*code, 0xdf);
	PUTDST8(*code, 0xc0 | x86Command);
}

void AvxVPandnRegToReg256(BYTE** code, int Dest, int Src1, int Src2) {
	BYTE x86Command = 0;
	BYTE x86Src1 = 0;

	RSP_CPU_Message("      vpandn %s, %s, %s", avx_Name(Dest), avx_Name(Src1), avx_Name(Src2));

	switch (Src1) {
	case x86_YMM0: x86Src1 = 0xfd; break;
	case x86_YMM1: x86Src1 = 0xf5; break;
	case x86_YMM2: x86Src1 = 0xed; break;
	case x86_YMM3: x86Src1 = 0xe5; break;
	case x86_YMM4: x86Src1 = 0xdd; break;
	case x86_YMM5: x86Src1 = 0xd5; break;
	case x86_YMM6: x86Src1 = 0xcd; break;
	case x86_YMM7: x86Src1 = 0xc5; break;
	}
	switch (Dest) {
	case x86_YMM0: x86Command = 0 << 3; break;
	case x86_YMM1: x86Command = 1 << 3; break;
	case x86_YMM2: x86Command = 2 << 3; break;
	case x86_YMM3: x86Command = 3 << 3; break;
	case x86_YMM4: x86Command = 4 << 3; break;
	case x86_YMM5: x86Command = 5 << 3; break;
	case x86_YMM6: x86Command = 6 << 3; break;
	case x86_YMM7: x86Command = 7 << 3; break;
	}
	switch (Src2) {
	case x86_YMM0: x86Command |= 0; break;
	case x86_YMM1: x86Command |= 1; break;
	case x86_YMM2: x86Command |= 2; break;
	case x86_YMM3: x86Command |= 3; break;
	case x86_YMM4: x86Command |= 4; break;
	case x86_YMM5: x86Command |= 5; break;
	case x86_YMM6: x86Command |= 6; break;
	case x86_YMM7: x86Command |= 7; break;
	}
	PUTDST8(*code, 0xc5);
	PUTDST8(*code, x86Src1);
	PUTDST8(*code, 0xdf);
	PUTDST8(*code, 0xc0 | x86Command);
}

void AvxVPBlendvbRegToReg256(BYTE** code, int Dest, int Src1, int Src2, int Src3Mask) {
	BYTE x86Command = 0;
	BYTE x86Src1 = 0;

	RSP_CPU_Message("      vpblendvb %s, %s, %s, %s", avx_Name(Dest), avx_Name(Src1), avx_Name(Src2), avx_Name(Src3Mask));

	switch (Src1) {
	case x86_YMM0: x86Src1 = 0x7d; break;
	case x86_YMM1: x86Src1 = 0x75; break;
	case x86_YMM2: x86Src1 = 0x6d; break;
	case x86_YMM3: x86Src1 = 0x65; break;
	case x86_YMM4: x86Src1 = 0x5d; break;
	case x86_YMM5: x86Src1 = 0x55; break;
	case x86_YMM6: x86Src1 = 0x4d; break;
	case x86_YMM7: x86Src1 = 0x45; break;
	}
	switch (Dest) {
	case x86_YMM0: x86Command = 0 << 3; break;
	case x86_YMM1: x86Command = 1 << 3; break;
	case x86_YMM2: x86Command = 2 << 3; break;
	case x86_YMM3: x86Command = 3 << 3; break;
	case x86_YMM4: x86Command = 4 << 3; break;
	case x86_YMM5: x86Command = 5 << 3; break;
	case x86_YMM6: x86Command = 6 << 3; break;
	case x86_YMM7: x86Command = 7 << 3; break;
	}
	switch (Src2) {
	case x86_YMM0: x86Command |= 0; break;
	case x86_YMM1: x86Command |= 1; break;
	case x86_YMM2: x86Command |= 2; break;
	case x86_YMM3: x86Command |= 3; break;
	case x86_YMM4: x86Command |= 4; break;
	case x86_YMM5: x86Command |= 5; break;
	case x86_YMM6: x86Command |= 6; break;
	case x86_YMM7: x86Command |= 7; break;
	}
	PUTDST16(*code, 0xe3c4);
	PUTDST8(*code, x86Src1);
	PUTDST8(*code, 0x4c);
	PUTDST8(*code, 0xc0 | x86Command);
	PUTDST8(*code, Src3Mask << 4);
}

void AvxVPBroadcastdVariableToReg256(BYTE** code, int Dest, void* Variable, char* VariableName) {
	BYTE x86Command = 0;

	RSP_CPU_Message("      vpbroadcastd %s, dword ptr %s", avx_Name(Dest), VariableName);

	switch (Dest) {
	case x86_YMM0: x86Command = 0x05; break;
	case x86_YMM1: x86Command = 0x0D; break;
	case x86_YMM2: x86Command = 0x15; break;
	case x86_YMM3: x86Command = 0x1D; break;
	case x86_YMM4: x86Command = 0x25; break;
	case x86_YMM5: x86Command = 0x2D; break;
	case x86_YMM6: x86Command = 0x35; break;
	case x86_YMM7: x86Command = 0x3D; break;
	}

	PUTDST32(*code, 0x587de2c4);
	PUTDST8(*code, x86Command);
	PUTDST32(*code, Variable);
}

void AvxVPBroadcastwVariableToReg128(BYTE** code, int Dest, void* Variable, char* VariableName) {
	BYTE x86Command = 0;

	RSP_CPU_Message("      vpbroadcastw %s, dword ptr %s", sse_Name(Dest), VariableName);

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

	PUTDST32(*code, 0x7979e2c4);
	PUTDST8(*code, x86Command);
	PUTDST32(*code, Variable);
}

void AvxVPMovesxWordVariableToDWordReg256(BYTE** code, int Dest, void* Variable, char* VariableName) {
	BYTE x86Command = 0;

	RSP_CPU_Message("      vpmovsxwd %s, xmmword ptr [%s]", avx_Name(Dest), VariableName);

	switch (Dest) {
	case x86_YMM0: x86Command = 0x05; break;
	case x86_YMM1: x86Command = 0x0D; break;
	case x86_YMM2: x86Command = 0x15; break;
	case x86_YMM3: x86Command = 0x1D; break;
	case x86_YMM4: x86Command = 0x25; break;
	case x86_YMM5: x86Command = 0x2D; break;
	case x86_YMM6: x86Command = 0x35; break;
	case x86_YMM7: x86Command = 0x3D; break;
	}

	PUTDST32(*code, 0x237de2c4);
	PUTDST8(*code, x86Command);
	PUTDST32(*code, Variable);
}

void AvxVPMovesxWordReg128ToDwordReg256(BYTE** code, int Dest, int Source) {
	BYTE x86Command = 0;

	RSP_CPU_Message("      vpmovsxwd %s, %s", avx_Name(Dest), sse_Name(Source));

	switch (Dest) {
	case x86_YMM0: x86Command = 0 << 3; break;
	case x86_YMM1: x86Command = 1 << 3; break;
	case x86_YMM2: x86Command = 2 << 3; break;
	case x86_YMM3: x86Command = 3 << 3; break;
	case x86_YMM4: x86Command = 4 << 3; break;
	case x86_YMM5: x86Command = 5 << 3; break;
	case x86_YMM6: x86Command = 6 << 3; break;
	case x86_YMM7: x86Command = 7 << 3; break;
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

	PUTDST32(*code, 0x237de2c4);
	PUTDST8(*code, 0xc0 | x86Command);
}

void AvxVPMulldRegToReg256(BYTE** code, int Dest, int Src1, int Src2) {
	BYTE x86Command = 0;
	BYTE x86Src1 = 0;

	RSP_CPU_Message("      vpmulld %s, %s, %s", avx_Name(Dest), avx_Name(Src1), avx_Name(Src2));

	switch (Src1) {
	case x86_YMM0: x86Src1 = 0x7d; break;
	case x86_YMM1: x86Src1 = 0x75; break;
	case x86_YMM2: x86Src1 = 0x6d; break;
	case x86_YMM3: x86Src1 = 0x65; break;
	case x86_YMM4: x86Src1 = 0x5d; break;
	case x86_YMM5: x86Src1 = 0x55; break;
	case x86_YMM6: x86Src1 = 0x4d; break;
	case x86_YMM7: x86Src1 = 0x45; break;
	}
	switch (Dest) {
	case x86_YMM0: x86Command = 0 << 3; break;
	case x86_YMM1: x86Command = 1 << 3; break;
	case x86_YMM2: x86Command = 2 << 3; break;
	case x86_YMM3: x86Command = 3 << 3; break;
	case x86_YMM4: x86Command = 4 << 3; break;
	case x86_YMM5: x86Command = 5 << 3; break;
	case x86_YMM6: x86Command = 6 << 3; break;
	case x86_YMM7: x86Command = 7 << 3; break;
	}
	switch (Src2) {
	case x86_YMM0: x86Command |= 0; break;
	case x86_YMM1: x86Command |= 1; break;
	case x86_YMM2: x86Command |= 2; break;
	case x86_YMM3: x86Command |= 3; break;
	case x86_YMM4: x86Command |= 4; break;
	case x86_YMM5: x86Command |= 5; break;
	case x86_YMM6: x86Command |= 6; break;
	case x86_YMM7: x86Command |= 7; break;
	}
	PUTDST16(*code, 0xe2c4);
	PUTDST8(*code, x86Src1);
	PUTDST8(*code, 0x40);
	PUTDST8(*code, 0xC0 | x86Command);
}

void AvxVPMullwRegToReg128(BYTE** code, int Dest, int Src1, int Src2) {
	BYTE x86Command = 0;
	BYTE x86Src1 = 0;

	RSP_CPU_Message("      vpmullw %s, %s, %s", sse_Name(Dest), sse_Name(Src1), sse_Name(Src2));

	switch (Src1) {
	case x86_XMM0: x86Src1 = 0xf9; break;
	case x86_XMM1: x86Src1 = 0xf1; break;
	case x86_XMM2: x86Src1 = 0xe9; break;
	case x86_XMM3: x86Src1 = 0xe1; break;
	case x86_XMM4: x86Src1 = 0xd9; break;
	case x86_XMM5: x86Src1 = 0xd1; break;
	case x86_XMM6: x86Src1 = 0xc9; break;
	case x86_XMM7: x86Src1 = 0xc1; break;
	}
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
	switch (Src2) {
	case x86_XMM0: x86Command |= 0; break;
	case x86_XMM1: x86Command |= 1; break;
	case x86_XMM2: x86Command |= 2; break;
	case x86_XMM3: x86Command |= 3; break;
	case x86_XMM4: x86Command |= 4; break;
	case x86_XMM5: x86Command |= 5; break;
	case x86_XMM6: x86Command |= 6; break;
	case x86_XMM7: x86Command |= 7; break;
	}
	PUTDST8(*code, 0xc5);
	PUTDST8(*code, x86Src1);
	PUTDST8(*code, 0xd5);
	PUTDST8(*code, 0xC0 | x86Command);
}

void AvxVPorRegToReg256(BYTE** code, int Dest, int Src1, int Src2) {
	BYTE x86Command = 0;
	BYTE x86Src1 = 0;

	RSP_CPU_Message("      vpor %s, %s, %s", avx_Name(Dest), avx_Name(Src1), avx_Name(Src2));

	switch (Src1) {
	case x86_YMM0: x86Src1 = 0xfd; break;
	case x86_YMM1: x86Src1 = 0xf5; break;
	case x86_YMM2: x86Src1 = 0xed; break;
	case x86_YMM3: x86Src1 = 0xe5; break;
	case x86_YMM4: x86Src1 = 0xdd; break;
	case x86_YMM5: x86Src1 = 0xd5; break;
	case x86_YMM6: x86Src1 = 0xcd; break;
	case x86_YMM7: x86Src1 = 0xc5; break;
	}
	switch (Dest) {
	case x86_YMM0: x86Command = 0 << 3; break;
	case x86_YMM1: x86Command = 1 << 3; break;
	case x86_YMM2: x86Command = 2 << 3; break;
	case x86_YMM3: x86Command = 3 << 3; break;
	case x86_YMM4: x86Command = 4 << 3; break;
	case x86_YMM5: x86Command = 5 << 3; break;
	case x86_YMM6: x86Command = 6 << 3; break;
	case x86_YMM7: x86Command = 7 << 3; break;
	}
	switch (Src2) {
	case x86_YMM0: x86Command |= 0; break;
	case x86_YMM1: x86Command |= 1; break;
	case x86_YMM2: x86Command |= 2; break;
	case x86_YMM3: x86Command |= 3; break;
	case x86_YMM4: x86Command |= 4; break;
	case x86_YMM5: x86Command |= 5; break;
	case x86_YMM6: x86Command |= 6; break;
	case x86_YMM7: x86Command |= 7; break;
	}
	PUTDST8(*code, 0xc5);
	PUTDST8(*code, x86Src1);
	PUTDST8(*code, 0xeb);
	PUTDST8(*code, 0xc0 | x86Command);
}

void AvxVPSlldRegToReg256Immed(BYTE** code, int Dest, int Src, BYTE Immed) {
	BYTE x86Command = 0;
	BYTE x86Dest = 0;

	RSP_CPU_Message("      vpslld %s, %s, %i", avx_Name(Dest), avx_Name(Src), Immed);

	switch (Dest) {
	case x86_YMM0: x86Dest = 0xfd; break;
	case x86_YMM1: x86Dest = 0xf5; break;
	case x86_YMM2: x86Dest = 0xed; break;
	case x86_YMM3: x86Dest = 0xe5; break;
	case x86_YMM4: x86Dest = 0xdd; break;
	case x86_YMM5: x86Dest = 0xd5; break;
	case x86_YMM6: x86Dest = 0xcd; break;
	case x86_YMM7: x86Dest = 0xc5; break;
	}
	switch (Src) {
	case x86_YMM0: x86Command = 0xf0; break;
	case x86_YMM1: x86Command = 0xf1; break;
	case x86_YMM2: x86Command = 0xf2; break;
	case x86_YMM3: x86Command = 0xf3; break;
	case x86_YMM4: x86Command = 0xf4; break;
	case x86_YMM5: x86Command = 0xf5; break;
	case x86_YMM6: x86Command = 0xf6; break;
	case x86_YMM7: x86Command = 0xf7; break;
	}
	PUTDST8(*code, 0xc5);
	PUTDST8(*code, x86Dest);
	PUTDST8(*code, 0x72);
	PUTDST8(*code, x86Command);
	PUTDST8(*code, Immed);
}

void AvxVPSllwRegToReg128Immed(BYTE** code, int Dest, int Src, BYTE Immed) {
	BYTE x86Command = 0;
	BYTE x86Dest = 0;

	RSP_CPU_Message("      vpsllw %s, %s, %i", sse_Name(Dest), sse_Name(Src), Immed);

	switch (Dest) {
	case x86_XMM0: x86Dest = 0xf9; break;
	case x86_XMM1: x86Dest = 0xf1; break;
	case x86_XMM2: x86Dest = 0xe9; break;
	case x86_XMM3: x86Dest = 0xe1; break;
	case x86_XMM4: x86Dest = 0xd9; break;
	case x86_XMM5: x86Dest = 0xd1; break;
	case x86_XMM6: x86Dest = 0xc9; break;
	case x86_XMM7: x86Dest = 0xc1; break;
	}
	switch (Src) {
	case x86_XMM0: x86Command = 0xf0; break;
	case x86_XMM1: x86Command = 0xf1; break;
	case x86_XMM2: x86Command = 0xf2; break;
	case x86_XMM3: x86Command = 0xf3; break;
	case x86_XMM4: x86Command = 0xf4; break;
	case x86_XMM5: x86Command = 0xf5; break;
	case x86_XMM6: x86Command = 0xf6; break;
	case x86_XMM7: x86Command = 0xf7; break;
	}
	PUTDST8(*code, 0xc5);
	PUTDST8(*code, x86Dest);
	PUTDST8(*code, 0x71);
	PUTDST8(*code, x86Command);
	PUTDST8(*code, Immed);
}

void AvxVPSradRegToReg256Immed(BYTE** code, int Dest, int Src, BYTE Immed) {
	BYTE x86Command = 0;
	BYTE x86Dest = 0;

	RSP_CPU_Message("      vpsrad %s, %s, %i", avx_Name(Dest), avx_Name(Src), Immed);

	switch (Dest) {
	case x86_YMM0: x86Dest = 0xfd; break;
	case x86_YMM1: x86Dest = 0xf5; break;
	case x86_YMM2: x86Dest = 0xed; break;
	case x86_YMM3: x86Dest = 0xe5; break;
	case x86_YMM4: x86Dest = 0xdd; break;
	case x86_YMM5: x86Dest = 0xd5; break;
	case x86_YMM6: x86Dest = 0xcd; break;
	case x86_YMM7: x86Dest = 0xc5; break;
	}
	switch (Src) {
	case x86_YMM0: x86Command = 0xe0; break;
	case x86_YMM1: x86Command = 0xe1; break;
	case x86_YMM2: x86Command = 0xe2; break;
	case x86_YMM3: x86Command = 0xe3; break;
	case x86_YMM4: x86Command = 0xe4; break;
	case x86_YMM5: x86Command = 0xe5; break;
	case x86_YMM6: x86Command = 0xe6; break;
	case x86_YMM7: x86Command = 0xe7; break;
	}
	PUTDST8(*code, 0xc5);
	PUTDST8(*code, x86Dest);
	PUTDST8(*code, 0x72);
	PUTDST8(*code, x86Command);
	PUTDST8(*code, Immed);
}

void AvxVPSrawRegToReg128Immed(BYTE** code, int Dest, int Src, BYTE Immed) {
	BYTE x86Command = 0;
	BYTE x86Dest = 0;

	RSP_CPU_Message("      vpsraw %s, %s, %i", sse_Name(Dest), sse_Name(Src), Immed);

	switch (Dest) {
	case x86_XMM0: x86Dest = 0xf9; break;
	case x86_XMM1: x86Dest = 0xf1; break;
	case x86_XMM2: x86Dest = 0xe9; break;
	case x86_XMM3: x86Dest = 0xe1; break;
	case x86_XMM4: x86Dest = 0xd9; break;
	case x86_XMM5: x86Dest = 0xd1; break;
	case x86_XMM6: x86Dest = 0xc9; break;
	case x86_XMM7: x86Dest = 0xc1; break;
	}
	switch (Src) {
	case x86_XMM0: x86Command = 0xe0; break;
	case x86_XMM1: x86Command = 0xe1; break;
	case x86_XMM2: x86Command = 0xe2; break;
	case x86_XMM3: x86Command = 0xe3; break;
	case x86_XMM4: x86Command = 0xe4; break;
	case x86_XMM5: x86Command = 0xe5; break;
	case x86_XMM6: x86Command = 0xe6; break;
	case x86_XMM7: x86Command = 0xe7; break;
	}
	PUTDST8(*code, 0xc5);
	PUTDST8(*code, x86Dest);
	PUTDST8(*code, 0x71);
	PUTDST8(*code, x86Command);
	PUTDST8(*code, Immed);
}

void AvxVPSrldRegToReg256Immed(BYTE** code, int Dest, int Src, BYTE Immed) {
	BYTE x86Command = 0;
	BYTE x86Dest = 0;

	RSP_CPU_Message("      vpsrld %s, %s, %i", avx_Name(Dest), avx_Name(Src), Immed);

	switch (Dest) {
	case x86_YMM0: x86Dest = 0xfd; break;
	case x86_YMM1: x86Dest = 0xf5; break;
	case x86_YMM2: x86Dest = 0xed; break;
	case x86_YMM3: x86Dest = 0xe5; break;
	case x86_YMM4: x86Dest = 0xdd; break;
	case x86_YMM5: x86Dest = 0xd5; break;
	case x86_YMM6: x86Dest = 0xcd; break;
	case x86_YMM7: x86Dest = 0xc5; break;
	}
	switch (Src) {
	case x86_YMM0: x86Command = 0xd0; break;
	case x86_YMM1: x86Command = 0xd1; break;
	case x86_YMM2: x86Command = 0xd2; break;
	case x86_YMM3: x86Command = 0xd3; break;
	case x86_YMM4: x86Command = 0xd4; break;
	case x86_YMM5: x86Command = 0xd5; break;
	case x86_YMM6: x86Command = 0xd6; break;
	case x86_YMM7: x86Command = 0xd7; break;
	}
	PUTDST8(*code, 0xc5);
	PUTDST8(*code, x86Dest);
	PUTDST8(*code, 0x72);
	PUTDST8(*code, x86Command);
	PUTDST8(*code, Immed);
}

void AvxVPunpckHighWordsRegToReg128(BYTE** code, int Dest, int Src1, int Src2) {
	BYTE x86Command = 0;
	BYTE x86Src1 = 0;

	RSP_CPU_Message("      vpunpckhwd %s, %s, %s", sse_Name(Dest), sse_Name(Src1), sse_Name(Src2));

	switch (Src1) {
	case x86_XMM0: x86Src1 = 0xf9; break;
	case x86_XMM1: x86Src1 = 0xf1; break;
	case x86_XMM2: x86Src1 = 0xe9; break;
	case x86_XMM3: x86Src1 = 0xe1; break;
	case x86_XMM4: x86Src1 = 0xd9; break;
	case x86_XMM5: x86Src1 = 0xd1; break;
	case x86_XMM6: x86Src1 = 0xc9; break;
	case x86_XMM7: x86Src1 = 0xc1; break;
	}
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
	switch (Src2) {
	case x86_XMM0: x86Command |= 0; break;
	case x86_XMM1: x86Command |= 1; break;
	case x86_XMM2: x86Command |= 2; break;
	case x86_XMM3: x86Command |= 3; break;
	case x86_XMM4: x86Command |= 4; break;
	case x86_XMM5: x86Command |= 5; break;
	case x86_XMM6: x86Command |= 6; break;
	case x86_XMM7: x86Command |= 7; break;
	}
	PUTDST8(*code, 0xc5);
	PUTDST8(*code, x86Src1);
	PUTDST8(*code, 0x69);
	PUTDST8(*code, 0xc0 | x86Command);
}

void AvxVPunpckHighWordsRegToReg256(BYTE** code, int Dest, int Src1, int Src2) {
	BYTE x86Command = 0;
	BYTE x86Src1 = 0;

	RSP_CPU_Message("      vpunpckhwd %s, %s, %s", avx_Name(Dest), avx_Name(Src1), avx_Name(Src2));

	switch (Src1) {
	case x86_YMM0: x86Src1 = 0xfd; break;
	case x86_YMM1: x86Src1 = 0xf5; break;
	case x86_YMM2: x86Src1 = 0xed; break;
	case x86_YMM3: x86Src1 = 0xe5; break;
	case x86_YMM4: x86Src1 = 0xdd; break;
	case x86_YMM5: x86Src1 = 0xd5; break;
	case x86_YMM6: x86Src1 = 0xcd; break;
	case x86_YMM7: x86Src1 = 0xc5; break;
	}
	switch (Dest) {
	case x86_YMM0: x86Command = 0 << 3; break;
	case x86_YMM1: x86Command = 1 << 3; break;
	case x86_YMM2: x86Command = 2 << 3; break;
	case x86_YMM3: x86Command = 3 << 3; break;
	case x86_YMM4: x86Command = 4 << 3; break;
	case x86_YMM5: x86Command = 5 << 3; break;
	case x86_YMM6: x86Command = 6 << 3; break;
	case x86_YMM7: x86Command = 7 << 3; break;
	}
	switch (Src2) {
	case x86_YMM0: x86Command |= 0; break;
	case x86_YMM1: x86Command |= 1; break;
	case x86_YMM2: x86Command |= 2; break;
	case x86_YMM3: x86Command |= 3; break;
	case x86_YMM4: x86Command |= 4; break;
	case x86_YMM5: x86Command |= 5; break;
	case x86_YMM6: x86Command |= 6; break;
	case x86_YMM7: x86Command |= 7; break;
	}
	PUTDST8(*code, 0xc5);
	PUTDST8(*code, x86Src1);
	PUTDST8(*code, 0x69);
	PUTDST8(*code, 0xc0 | x86Command);
}

void AvxVPunpckLowWordsRegToReg128(BYTE** code, int Dest, int Src1, int Src2) {
	BYTE x86Command = 0;
	BYTE x86Src1 = 0;

	RSP_CPU_Message("      vpunpcklwd %s, %s, %s", sse_Name(Dest), sse_Name(Src1), sse_Name(Src2));

	switch (Src1) {
	case x86_XMM0: x86Src1 = 0xf9; break;
	case x86_XMM1: x86Src1 = 0xf1; break;
	case x86_XMM2: x86Src1 = 0xe9; break;
	case x86_XMM3: x86Src1 = 0xe1; break;
	case x86_XMM4: x86Src1 = 0xd9; break;
	case x86_XMM5: x86Src1 = 0xd1; break;
	case x86_XMM6: x86Src1 = 0xc9; break;
	case x86_XMM7: x86Src1 = 0xc1; break;
	}
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
	switch (Src2) {
	case x86_XMM0: x86Command |= 0; break;
	case x86_XMM1: x86Command |= 1; break;
	case x86_XMM2: x86Command |= 2; break;
	case x86_XMM3: x86Command |= 3; break;
	case x86_XMM4: x86Command |= 4; break;
	case x86_XMM5: x86Command |= 5; break;
	case x86_XMM6: x86Command |= 6; break;
	case x86_XMM7: x86Command |= 7; break;
	}
	PUTDST8(*code, 0xc5);
	PUTDST8(*code, x86Src1);
	PUTDST8(*code, 0x61);
	PUTDST8(*code, 0xc0 | x86Command);
}

void AvxVPunpckLowWordsRegToReg256(BYTE** code, int Dest, int Src1, int Src2) {
	BYTE x86Command = 0;
	BYTE x86Src1 = 0;

	RSP_CPU_Message("      vpunpcklwd %s, %s, %s", avx_Name(Dest), avx_Name(Src1), avx_Name(Src2));

	switch (Src1) {
	case x86_YMM0: x86Src1 = 0xfd; break;
	case x86_YMM1: x86Src1 = 0xf5; break;
	case x86_YMM2: x86Src1 = 0xed; break;
	case x86_YMM3: x86Src1 = 0xe5; break;
	case x86_YMM4: x86Src1 = 0xdd; break;
	case x86_YMM5: x86Src1 = 0xd5; break;
	case x86_YMM6: x86Src1 = 0xcd; break;
	case x86_YMM7: x86Src1 = 0xc5; break;
	}
	switch (Dest) {
	case x86_YMM0: x86Command = 0 << 3; break;
	case x86_YMM1: x86Command = 1 << 3; break;
	case x86_YMM2: x86Command = 2 << 3; break;
	case x86_YMM3: x86Command = 3 << 3; break;
	case x86_YMM4: x86Command = 4 << 3; break;
	case x86_YMM5: x86Command = 5 << 3; break;
	case x86_YMM6: x86Command = 6 << 3; break;
	case x86_YMM7: x86Command = 7 << 3; break;
	}
	switch (Src2) {
	case x86_YMM0: x86Command |= 0; break;
	case x86_YMM1: x86Command |= 1; break;
	case x86_YMM2: x86Command |= 2; break;
	case x86_YMM3: x86Command |= 3; break;
	case x86_YMM4: x86Command |= 4; break;
	case x86_YMM5: x86Command |= 5; break;
	case x86_YMM6: x86Command |= 6; break;
	case x86_YMM7: x86Command |= 7; break;
	}
	PUTDST8(*code, 0xc5);
	PUTDST8(*code, x86Src1);
	PUTDST8(*code, 0x61);
	PUTDST8(*code, 0xc0 | x86Command);
}

void AvxVPxorRegToReg256(BYTE** code, int Dest, int Src1, int Src2) {
	BYTE x86Command = 0;
	BYTE x86Src1 = 0;

	RSP_CPU_Message("      vpxor %s, %s, %s", avx_Name(Dest), avx_Name(Src1), avx_Name(Src2));

	switch (Src1) {
	case x86_YMM0: x86Src1 = 0xfd; break;
	case x86_YMM1: x86Src1 = 0xf5; break;
	case x86_YMM2: x86Src1 = 0xed; break;
	case x86_YMM3: x86Src1 = 0xe5; break;
	case x86_YMM4: x86Src1 = 0xdd; break;
	case x86_YMM5: x86Src1 = 0xd5; break;
	case x86_YMM6: x86Src1 = 0xcd; break;
	case x86_YMM7: x86Src1 = 0xc5; break;
	}
	switch (Dest) {
	case x86_YMM0: x86Command = 0 << 3; break;
	case x86_YMM1: x86Command = 1 << 3; break;
	case x86_YMM2: x86Command = 2 << 3; break;
	case x86_YMM3: x86Command = 3 << 3; break;
	case x86_YMM4: x86Command = 4 << 3; break;
	case x86_YMM5: x86Command = 5 << 3; break;
	case x86_YMM6: x86Command = 6 << 3; break;
	case x86_YMM7: x86Command = 7 << 3; break;
	}
	switch (Src2) {
	case x86_YMM0: x86Command |= 0; break;
	case x86_YMM1: x86Command |= 1; break;
	case x86_YMM2: x86Command |= 2; break;
	case x86_YMM3: x86Command |= 3; break;
	case x86_YMM4: x86Command |= 4; break;
	case x86_YMM5: x86Command |= 5; break;
	case x86_YMM6: x86Command |= 6; break;
	case x86_YMM7: x86Command |= 7; break;
	}
	PUTDST8(*code, 0xc5);
	PUTDST8(*code, x86Src1);
	PUTDST8(*code, 0xef);
	PUTDST8(*code, 0xc0 | x86Command);
}

void AvxVExtracti128RegToReg(BYTE** code, int Dest, int Src, BOOL msb) {
	BYTE x86Command = 0;

	RSP_CPU_Message("      vextracti128 %s, %s, %d", sse_Name(Dest), avx_Name(Src), msb ? 1 : 0);

	switch (Src) {
	case x86_YMM0: x86Command = 0 << 3; break;
	case x86_YMM1: x86Command = 1 << 3; break;
	case x86_YMM2: x86Command = 2 << 3; break;
	case x86_YMM3: x86Command = 3 << 3; break;
	case x86_YMM4: x86Command = 4 << 3; break;
	case x86_YMM5: x86Command = 5 << 3; break;
	case x86_YMM6: x86Command = 6 << 3; break;
	case x86_YMM7: x86Command = 7 << 3; break;
	}
	switch (Dest) {
	case x86_XMM0: x86Command |= 0; break;
	case x86_XMM1: x86Command |= 1; break;
	case x86_XMM2: x86Command |= 2; break;
	case x86_XMM3: x86Command |= 3; break;
	case x86_XMM4: x86Command |= 4; break;
	case x86_XMM5: x86Command |= 5; break;
	case x86_XMM6: x86Command |= 6; break;
	case x86_XMM7: x86Command |= 7; break;
	}
	PUTDST32(*code, 0x397de3c4);
	PUTDST8(*code, 0xc0 | x86Command);
	PUTDST8(*code, msb ? 1 : 0);
}

void AvxVInserti128RegToReg(BYTE** code, int Dest, int Src1, int Src2, BOOL msb) {
	BYTE x86Command = 0;
	BYTE x86Src1 = 0;

	RSP_CPU_Message("      vinserti128 %s, %s, %d", avx_Name(Dest), avx_Name(Src1), sse_Name(Src2), msb ? 1 : 0);

	switch (Src1) {
	case x86_YMM0: x86Src1 = 0x7d; break;
	case x86_YMM1: x86Src1 = 0x75; break;
	case x86_YMM2: x86Src1 = 0x6d; break;
	case x86_YMM3: x86Src1 = 0x65; break;
	case x86_YMM4: x86Src1 = 0x5d; break;
	case x86_YMM5: x86Src1 = 0x55; break;
	case x86_YMM6: x86Src1 = 0x4d; break;
	case x86_YMM7: x86Src1 = 0x45; break;
	}
	switch (Dest) {
	case x86_YMM0: x86Command = 0 << 3; break;
	case x86_YMM1: x86Command = 1 << 3; break;
	case x86_YMM2: x86Command = 2 << 3; break;
	case x86_YMM3: x86Command = 3 << 3; break;
	case x86_YMM4: x86Command = 4 << 3; break;
	case x86_YMM5: x86Command = 5 << 3; break;
	case x86_YMM6: x86Command = 6 << 3; break;
	case x86_YMM7: x86Command = 7 << 3; break;
	}
	switch (Src2) {
	case x86_XMM0: x86Command |= 0; break;
	case x86_XMM1: x86Command |= 1; break;
	case x86_XMM2: x86Command |= 2; break;
	case x86_XMM3: x86Command |= 3; break;
	case x86_XMM4: x86Command |= 4; break;
	case x86_XMM5: x86Command |= 5; break;
	case x86_XMM6: x86Command |= 6; break;
	case x86_XMM7: x86Command |= 7; break;
	}
	PUTDST16(*code, 0xe3c4);
	PUTDST8(*code, x86Src1);
	PUTDST8(*code, 0x38);
	PUTDST8(*code, 0xc0 | x86Command);
	PUTDST8(*code, msb ? 1 : 0);
}
