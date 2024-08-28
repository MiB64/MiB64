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
#include <stdio.h>
#include "main.h"
#include "cpu.h"
#include "x86.h"
#include "debugger.h"
#include "RSP/rsp_log.h"


static BOOL ConditionalMove;

void DetectCpuSpecs(void) {
	DWORD Intel_Features = 0;
	/*DWORD AMD_Features = 0;*/

	__try {
		_asm {
			/* Intel features */
			mov eax, 1
			cpuid
			mov[Intel_Features], edx

			/* AMD features */
/*			mov eax, 80000001h
			cpuid
			or [AMD_Features], edx*/
		}
	}
	__except (EXCEPTION_EXECUTE_HANDLER) {
		/*AMD_Features =*/ Intel_Features = 0;
	}

	/*if (Intel_Features & 0x02000000) {
		Compiler.mmx2 = TRUE;
		Compiler.sse = TRUE;
	}
	if (Intel_Features & 0x00800000) {
		Compiler.mmx = TRUE;
	}
	if (AMD_Features & 0x40000000) {
		Compiler.mmx2 = TRUE;
	}*/
	if (Intel_Features & 0x00008000) {
		ConditionalMove = TRUE;
	} else {
		ConditionalMove = FALSE;
	}
}

#if defined(Log_x86Code) || defined(RspLog_x86Code)
#define CPU_OR_RSP_Message(code, Message, ...) { \
	if(code == RecompPos) { \
		CPU_Message(Message, __VA_ARGS__); \
	} \
	else { \
		RSP_CPU_Message(Message, __VA_ARGS__); \
	} \
}
#else
#define CPU_OR_RSP_Message
#endif

#define PUTDST8(dest,value)  (*((BYTE *)(dest))=(BYTE)(value)); dest += 1;
#define PUTDST16(dest,value) (*((WORD *)(dest))=(WORD)(value)); dest += 2;
#define PUTDST32(dest,value) (*((DWORD *)(dest))=(DWORD)(value)); dest += 4;

void AdcX86regToVariable(BYTE** code, int x86reg, void * Variable, char * VariableName) {
	CPU_OR_RSP_Message(*code, "      adc dword ptr [%s], %s",VariableName, x86_Name(x86reg));
	switch (x86reg) {
	case x86_EAX: PUTDST16(*code,0x0511); break;
	case x86_EBX: PUTDST16(*code,0x1D11); break;
	case x86_ECX: PUTDST16(*code,0x0D11); break;
	case x86_EDX: PUTDST16(*code,0x1511); break;
	case x86_ESI: PUTDST16(*code,0x3511); break;
	case x86_EDI: PUTDST16(*code,0x3D11); break;
	case x86_ESP: PUTDST16(*code,0x2511); break;
	case x86_EBP: PUTDST16(*code,0x2D11); break;
	default:
		DisplayError("AddVariableToX86reg\nUnknown x86 Register");
	}
    PUTDST32(*code,Variable);
}

void AdcConstToVariable(BYTE** code,void *Variable, char *VariableName, BYTE Constant) {
	CPU_OR_RSP_Message(*code, "      adc dword ptr [%s], %Xh", VariableName, Constant);
	PUTDST16(*code,0x1583);
    PUTDST32(*code,Variable);
	PUTDST8(*code,Constant);
}

void AdcConstToX86Reg (BYTE** code, int x86Reg, DWORD Const) {
	CPU_OR_RSP_Message(*code, "      adc %s, %Xh",x86_Name(x86Reg),Const);
	if ((Const & 0xFFFFFF80) != 0 && (Const & 0xFFFFFF80) != 0xFFFFFF80) {
		switch (x86Reg) {
		case x86_EAX: PUTDST16(*code,0xD081); break;
		case x86_EBX: PUTDST16(*code,0xD381); break;
		case x86_ECX: PUTDST16(*code,0xD181); break;
		case x86_EDX: PUTDST16(*code,0xD281); break;
		case x86_ESI: PUTDST16(*code,0xD681); break;
		case x86_EDI: PUTDST16(*code,0xD781); break;
		case x86_ESP: PUTDST16(*code,0xD481); break;
		case x86_EBP: PUTDST16(*code,0xD581); break;
		}
		PUTDST32(*code, Const);
	} else {
		switch (x86Reg) {
		case x86_EAX: PUTDST16(*code,0xD083); break;
		case x86_EBX: PUTDST16(*code,0xD383); break;
		case x86_ECX: PUTDST16(*code,0xD183); break;
		case x86_EDX: PUTDST16(*code,0xD283); break;
		case x86_ESI: PUTDST16(*code,0xD683); break;
		case x86_EDI: PUTDST16(*code,0xD783); break;
		case x86_ESP: PUTDST16(*code,0xD483); break;
		case x86_EBP: PUTDST16(*code,0xD583); break;
		}
		PUTDST8(*code, Const);
	}
}

void AdcVariableToX86reg(BYTE** code, int x86reg, void * Variable, char * VariableName) {
	CPU_OR_RSP_Message(*code, "      adc %s, dword ptr [%s]",x86_Name(x86reg),VariableName);
	switch (x86reg) {
	case x86_EAX: PUTDST16(*code,0x0513); break;
	case x86_EBX: PUTDST16(*code,0x1D13); break;
	case x86_ECX: PUTDST16(*code,0x0D13); break;
	case x86_EDX: PUTDST16(*code,0x1513); break;
	case x86_ESI: PUTDST16(*code,0x3513); break;
	case x86_EDI: PUTDST16(*code,0x3D13); break;
	case x86_ESP: PUTDST16(*code,0x2513); break;
	case x86_EBP: PUTDST16(*code,0x2D13); break;
	default:
		DisplayError("AdcVariableToX86reg\nUnknown x86 Register");
	}
    PUTDST32(*code,Variable);
}

void AdcX86RegToX86Reg(BYTE** code, int Destination, int Source) {
	WORD x86Command = 0x0;

	CPU_OR_RSP_Message(*code, "      adc %s, %s",x86_Name(Destination),x86_Name(Source));
	switch (Source) {
	case x86_EAX: x86Command = 0x0013; break;
	case x86_EBX: x86Command = 0x0313; break;
	case x86_ECX: x86Command = 0x0113; break;
	case x86_EDX: x86Command = 0x0213; break;
	case x86_ESI: x86Command = 0x0613; break;
	case x86_EDI: x86Command = 0x0713; break;
	case x86_ESP: x86Command = 0x0413; break;
	case x86_EBP: x86Command = 0x0513; break;
	}
	switch (Destination) {
	case x86_EAX: x86Command += 0xC000; break;
	case x86_EBX: x86Command += 0xD800; break;
	case x86_ECX: x86Command += 0xC800; break;
	case x86_EDX: x86Command += 0xD000; break;
	case x86_ESI: x86Command += 0xF000; break;
	case x86_EDI: x86Command += 0xF800; break;
	case x86_ESP: x86Command += 0xE000; break;
	case x86_EBP: x86Command += 0xE800; break;
	}
	PUTDST16(*code,x86Command);
}

void AddConstToVariable (BYTE** code, DWORD Const, void *Variable, char *VariableName) {
	CPU_OR_RSP_Message(*code, "      add dword ptr [%s], 0x%X",VariableName, Const);
	if ((Const & 0xFFFFFF80) != 0 && (Const & 0xFFFFFF80) != 0xFFFFFF80) {
		PUTDST16(*code, 0x0581);
		PUTDST32(*code, Variable);
		PUTDST32(*code, Const);
	} else {
		PUTDST16(*code, 0x0583);
		PUTDST32(*code, Variable);
		PUTDST8(*code, Const);
	}
}

void AddConstToX86Reg (BYTE** code, int x86Reg, DWORD Const) {
	CPU_OR_RSP_Message(*code, "      add %s, %Xh",x86_Name(x86Reg),Const);
	if ((Const & 0xFFFFFF80) != 0 && (Const & 0xFFFFFF80) != 0xFFFFFF80) {
		switch (x86Reg) {
		case x86_EAX: PUTDST8(*code,0x05); break;
		case x86_EBX: PUTDST16(*code,0xC381); break;
		case x86_ECX: PUTDST16(*code,0xC181); break;
		case x86_EDX: PUTDST16(*code,0xC281); break;
		case x86_ESI: PUTDST16(*code,0xC681); break;
		case x86_EDI: PUTDST16(*code,0xC781); break;
		case x86_ESP: PUTDST16(*code,0xC481); break;
		case x86_EBP: PUTDST16(*code,0xC581); break;
		}
		PUTDST32(*code, Const);
	} else {
		switch (x86Reg) {
		case x86_EAX: PUTDST16(*code,0xC083); break;
		case x86_EBX: PUTDST16(*code,0xC383); break;
		case x86_ECX: PUTDST16(*code,0xC183); break;
		case x86_EDX: PUTDST16(*code,0xC283); break;
		case x86_ESI: PUTDST16(*code,0xC683); break;
		case x86_EDI: PUTDST16(*code,0xC783); break;
		case x86_ESP: PUTDST16(*code,0xC483); break;
		case x86_EBP: PUTDST16(*code,0xC583); break;
		}
		PUTDST8(*code, Const);
	}
}

void AddVariableToX86reg(BYTE** code, int x86reg, void * Variable, char * VariableName) {
	CPU_OR_RSP_Message(*code, "      add %s, dword ptr [%s]",x86_Name(x86reg),VariableName);
	switch (x86reg) {
	case x86_EAX: PUTDST16(*code,0x0503); break;
	case x86_EBX: PUTDST16(*code,0x1D03); break;
	case x86_ECX: PUTDST16(*code,0x0D03); break;
	case x86_EDX: PUTDST16(*code,0x1503); break;
	case x86_ESI: PUTDST16(*code,0x3503); break;
	case x86_EDI: PUTDST16(*code,0x3D03); break;
	case x86_ESP: PUTDST16(*code,0x2503); break;
	case x86_EBP: PUTDST16(*code,0x2D03); break;
	default:
		DisplayError("AddVariableToX86reg\nUnknown x86 Register");
	}
    PUTDST32(*code,Variable);
}

void AddX86regToVariable(BYTE** code, int x86reg, void * Variable, char * VariableName) {
	CPU_OR_RSP_Message(*code, "      add dword ptr [%s], %s",VariableName, x86_Name(x86reg));
	switch (x86reg) {
	case x86_EAX: PUTDST16(*code,0x0501); break;
	case x86_EBX: PUTDST16(*code,0x1D01); break;
	case x86_ECX: PUTDST16(*code,0x0D01); break;
	case x86_EDX: PUTDST16(*code,0x1501); break;
	case x86_ESI: PUTDST16(*code,0x3501); break;
	case x86_EDI: PUTDST16(*code,0x3D01); break;
	case x86_ESP: PUTDST16(*code,0x2501); break;
	case x86_EBP: PUTDST16(*code,0x2D01); break;
	default:
		DisplayError("AddVariableToX86reg\nUnknown x86 Register");
	}
    PUTDST32(*code,Variable);
}

void AddX86RegToX86Reg(BYTE** code, int Destination, int Source) {
	WORD x86Command = 0x0;

	CPU_OR_RSP_Message(*code, "      add %s, %s",x86_Name(Destination),x86_Name(Source));
	switch (Source) {
	case x86_EAX: x86Command = 0x0003; break;
	case x86_EBX: x86Command = 0x0303; break;
	case x86_ECX: x86Command = 0x0103; break;
	case x86_EDX: x86Command = 0x0203; break;
	case x86_ESI: x86Command = 0x0603; break;
	case x86_EDI: x86Command = 0x0703; break;
	case x86_ESP: x86Command = 0x0403; break;
	case x86_EBP: x86Command = 0x0503; break;
	}
	switch (Destination) {
	case x86_EAX: x86Command += 0xC000; break;
	case x86_EBX: x86Command += 0xD800; break;
	case x86_ECX: x86Command += 0xC800; break;
	case x86_EDX: x86Command += 0xD000; break;
	case x86_ESI: x86Command += 0xF000; break;
	case x86_EDI: x86Command += 0xF800; break;
	case x86_ESP: x86Command += 0xE000; break;
	case x86_EBP: x86Command += 0xE800; break;
	}
	PUTDST16(*code,x86Command);
}

void AndConstToVariable (BYTE** code, DWORD Const, void *Variable, char *VariableName) {
	CPU_OR_RSP_Message(*code, "      and dword ptr [%s], 0x%X",VariableName, Const);
	if ((Const & 0xFFFFFF80) != 0 && (Const & 0xFFFFFF80) != 0xFFFFFF80) {
		PUTDST16(*code, 0x2581);
		PUTDST32(*code, Variable);
		PUTDST32(*code, Const);
	} else {
		PUTDST16(*code, 0x2583);
		PUTDST32(*code, Variable);
		PUTDST8(*code, Const);
	}
}

void AndConstToX86Reg(BYTE** code, int x86Reg, DWORD Const) {
	CPU_OR_RSP_Message(*code, "      and %s, %Xh",x86_Name(x86Reg),Const);
	if ((Const & 0xFFFFFF80) != 0 && (Const & 0xFFFFFF80) != 0xFFFFFF80) {
		switch (x86Reg) {
		case x86_EAX: PUTDST8(*code,0x25); break;
		case x86_EBX: PUTDST16(*code,0xE381); break;
		case x86_ECX: PUTDST16(*code,0xE181); break;
		case x86_EDX: PUTDST16(*code,0xE281); break;
		case x86_ESI: PUTDST16(*code,0xE681); break;
		case x86_EDI: PUTDST16(*code,0xE781); break;
		case x86_ESP: PUTDST16(*code,0xE481); break;
		case x86_EBP: PUTDST16(*code,0xE581); break;
		}
		PUTDST32(*code, Const);
	} else {
		switch (x86Reg) {
		case x86_EAX: PUTDST16(*code,0xE083); break;
		case x86_EBX: PUTDST16(*code,0xE383); break;
		case x86_ECX: PUTDST16(*code,0xE183); break;
		case x86_EDX: PUTDST16(*code,0xE283); break;
		case x86_ESI: PUTDST16(*code,0xE683); break;
		case x86_EDI: PUTDST16(*code,0xE783); break;
		case x86_ESP: PUTDST16(*code,0xE483); break;
		case x86_EBP: PUTDST16(*code,0xE583); break;
		}
		PUTDST8(*code, Const);
	}
}

void AndConstToX86RegHalf(BYTE** code, int x86Reg, WORD Const) {
	CPU_OR_RSP_Message(*code, "      and %s, %Xh", x86Half_Name(x86Reg), Const);
	PUTDST8(*code, 0x66);
	if ((Const & 0xFF80) != 0 && (Const & 0xFF80) != 0xFF80) {
		switch (x86Reg) {
		case x86_EAX: PUTDST16(*code, 0xE081); break;
		case x86_EBX: PUTDST16(*code, 0xE381); break;
		case x86_ECX: PUTDST16(*code, 0xE181); break;
		case x86_EDX: PUTDST16(*code, 0xE281); break;
		case x86_ESI: PUTDST16(*code, 0xE681); break;
		case x86_EDI: PUTDST16(*code, 0xE781); break;
		case x86_ESP: PUTDST16(*code, 0xE481); break;
		case x86_EBP: PUTDST16(*code, 0xE581); break;
		}
		PUTDST16(*code, Const);
	}
	else {
		switch (x86Reg) {
		case x86_EAX: PUTDST16(*code, 0xE083); break;
		case x86_EBX: PUTDST16(*code, 0xE383); break;
		case x86_ECX: PUTDST16(*code, 0xE183); break;
		case x86_EDX: PUTDST16(*code, 0xE283); break;
		case x86_ESI: PUTDST16(*code, 0xE683); break;
		case x86_EDI: PUTDST16(*code, 0xE783); break;
		case x86_ESP: PUTDST16(*code, 0xE483); break;
		case x86_EBP: PUTDST16(*code, 0xE583); break;
		}
		PUTDST8(*code, Const);
	}
}

void AndVariableDispToX86Reg(BYTE** code, void *Variable, char *VariableName, int x86Reg, int AddrReg, int Multiplier) {
	int x;
	CPU_OR_RSP_Message(*code, "      and %s, dword ptr [%s+%s*%i]",x86_Name(x86Reg),VariableName, x86_Name(AddrReg), Multiplier);
	
	PUTDST8(*code,0x23);

	switch (x86Reg) {
	case x86_EAX: PUTDST8(*code,0x04); break;
	case x86_EBX: PUTDST8(*code,0x1C); break;
	case x86_ECX: PUTDST8(*code,0x0C); break;
	case x86_EDX: PUTDST8(*code,0x14); break;
	case x86_ESI: PUTDST8(*code,0x34); break;
	case x86_EDI: PUTDST8(*code,0x3C); break;
	case x86_ESP: PUTDST8(*code,0x24); break;
	case x86_EBP: PUTDST8(*code,0x2C); break;
	}

	/* put in shifter 2(01), 4(10), 8(11) */
	switch (Multiplier) {
	case 2: x = 0x40; break;
	case 4: x = 0x80; break;
	case 8: x = 0xC0; break;
	default: 
		x = 0x40;
		DisplayError("And\nInvalid x86 multiplier");
	}

	/* format xx|000000 */
	switch (AddrReg) {
	case x86_EAX: PUTDST8(*code,0x05|x); break;
	case x86_EBX: PUTDST8(*code,0x1D|x); break;
	case x86_ECX: PUTDST8(*code,0x0D|x); break;
	case x86_EDX: PUTDST8(*code,0x15|x); break;
	case x86_ESI: PUTDST8(*code,0x35|x); break;
	case x86_EDI: PUTDST8(*code,0x3D|x); break;
	case x86_ESP: PUTDST8(*code,0x25|x); break;
	case x86_EBP: PUTDST8(*code,0x2D|x); break;
	}

	PUTDST32(*code,Variable);
}

void AndVariableToX86Reg(BYTE** code, void * Variable, char * VariableName, int x86Reg) {
	CPU_OR_RSP_Message(*code, "      and %s, dword ptr [%s]",x86_Name(x86Reg),VariableName);
	switch (x86Reg) {
	case x86_EAX: PUTDST16(*code,0x0523); break;
	case x86_EBX: PUTDST16(*code,0x1D23); break;
	case x86_ECX: PUTDST16(*code,0x0D23); break;
	case x86_EDX: PUTDST16(*code,0x1523); break;
	case x86_ESI: PUTDST16(*code,0x3523); break;
	case x86_EDI: PUTDST16(*code,0x3D23); break;
	case x86_ESP: PUTDST16(*code,0x2523); break;
	case x86_EBP: PUTDST16(*code,0x2D23); break;
	}
	PUTDST32(*code,Variable);
}

void AndX86RegToVariable(BYTE** code, void* Variable, char* VariableName, int x86Reg) {
	CPU_OR_RSP_Message(*code, "      and dword ptr [%s], %s", VariableName, x86_Name(x86Reg));
	switch (x86Reg) {
	case x86_EAX: PUTDST16(*code, 0x0521); break;
	case x86_EBX: PUTDST16(*code, 0x1D21); break;
	case x86_ECX: PUTDST16(*code, 0x0D21); break;
	case x86_EDX: PUTDST16(*code, 0x1521); break;
	case x86_ESI: PUTDST16(*code, 0x3521); break;
	case x86_EDI: PUTDST16(*code, 0x3D21); break;
	case x86_ESP: PUTDST16(*code, 0x2521); break;
	case x86_EBP: PUTDST16(*code, 0x2D21); break;
	}
	PUTDST32(*code, Variable);
}

void AndX86RegToX86Reg(BYTE** code, int Destination, int Source) {
	WORD x86Command = 0x0;

	CPU_OR_RSP_Message(*code, "      and %s, %s",x86_Name(Destination),x86_Name(Source));
	switch (Destination) {
	case x86_EAX: x86Command = 0x0021; break;
	case x86_EBX: x86Command = 0x0321; break;
	case x86_ECX: x86Command = 0x0121; break;
	case x86_EDX: x86Command = 0x0221; break;
	case x86_ESI: x86Command = 0x0621; break;
	case x86_EDI: x86Command = 0x0721; break;
	case x86_ESP: x86Command = 0x0421; break;
	case x86_EBP: x86Command = 0x0521; break;
	}
	switch (Source) {
	case x86_EAX: x86Command += 0xC000; break;
	case x86_EBX: x86Command += 0xD800; break;
	case x86_ECX: x86Command += 0xC800; break;
	case x86_EDX: x86Command += 0xD000; break;
	case x86_ESI: x86Command += 0xF000; break;
	case x86_EDI: x86Command += 0xF800; break;
	case x86_ESP: x86Command += 0xE000; break;
	case x86_EBP: x86Command += 0xE800; break;
	}
	PUTDST16(*code,x86Command);
}

void BreakPoint (BYTE** code) {
	CPU_OR_RSP_Message(*code, "      int 3");
	PUTDST8(*code,0xCC);
}

void Call_Direct(BYTE** code, void * FunctAddress, char * FunctName) {
	CPU_OR_RSP_Message(*code, "      call offset %s",FunctName);
	PUTDST8(*code,0xE8);
	PUTDST32(*code,(DWORD)FunctAddress-(DWORD)*code - 4);
}

void Call_Indirect(BYTE** code, void * FunctAddress, char * FunctName) {
	CPU_OR_RSP_Message(*code, "      call [%s]",FunctName);
	PUTDST16(*code,0x15FF);
	PUTDST32(*code,FunctAddress);
}

void Cdq(BYTE** code) {
	CPU_OR_RSP_Message(*code, "      cdq");
	PUTDST8(*code, 0x99);
}

void CompConstToVariable(BYTE** code, DWORD Const, void * Variable, char * VariableName) {
	CPU_OR_RSP_Message(*code, "      cmp dword ptr [%s], 0x%X",VariableName, Const);
	if ((Const & 0xFFFFFF80) != 0 && (Const & 0xFFFFFF80) != 0xFFFFFF80) {
		PUTDST16(*code, 0x3D81);
		PUTDST32(*code, Variable);
		PUTDST32(*code, Const);
	}
	else {
		PUTDST16(*code, 0x3D83);
		PUTDST32(*code, Variable);
		PUTDST8(*code, Const);
	}
}

void CompConstToX86reg(BYTE** code, int x86Reg, DWORD Const) {
	CPU_OR_RSP_Message(*code, "      cmp %s, %Xh",x86_Name(x86Reg),Const);
	if ((Const & 0xFFFFFF80) != 0 && (Const & 0xFFFFFF80) != 0xFFFFFF80) {
		switch (x86Reg) {
		case x86_EAX: PUTDST16(*code,0xF881); break;
		case x86_EBX: PUTDST16(*code,0xFB81); break;
		case x86_ECX: PUTDST16(*code,0xF981); break;
		case x86_EDX: PUTDST16(*code,0xFA81); break;
		case x86_ESI: PUTDST16(*code,0xFE81); break;
		case x86_EDI: PUTDST16(*code,0xFF81); break;
		case x86_ESP: PUTDST16(*code,0xFC81); break;
		case x86_EBP: PUTDST16(*code,0xFD81); break;
		default:
			DisplayError("CompConstToX86reg\nUnknown x86 Register");
		}
		PUTDST32(*code,Const);
	} else {
	switch (x86Reg) {
		case x86_EAX: PUTDST16(*code,0xF883); break;
		case x86_EBX: PUTDST16(*code,0xFB83); break;
		case x86_ECX: PUTDST16(*code,0xF983); break;
		case x86_EDX: PUTDST16(*code,0xFA83); break;
		case x86_ESI: PUTDST16(*code,0xFE83); break;
		case x86_EDI: PUTDST16(*code,0xFF83); break;
		case x86_ESP: PUTDST16(*code,0xFC83); break;
		case x86_EBP: PUTDST16(*code,0xFD83); break;
		}
		PUTDST8(*code, Const);
	}
}

void CompX86regToVariable(BYTE** code, int x86Reg, void * Variable, char * VariableName) {
	CPU_OR_RSP_Message(*code, "      cmp %s, dword ptr [%s]",x86_Name(x86Reg),VariableName);
	switch (x86Reg) {
	case x86_EAX: PUTDST16(*code,0x053B); break;
	case x86_EBX: PUTDST16(*code,0x1D3B); break;
	case x86_ECX: PUTDST16(*code,0x0D3B); break;
	case x86_EDX: PUTDST16(*code,0x153B); break;
	case x86_ESI: PUTDST16(*code,0x353B); break;
	case x86_EDI: PUTDST16(*code,0x3D3B); break;
	case x86_ESP: PUTDST16(*code,0x253B); break;
	case x86_EBP: PUTDST16(*code,0x2D3B); break;
	default:
		DisplayError("Unknown x86 Register");
	}
	PUTDST32(*code,Variable);
}

void CompVariableToX86reg(BYTE** code, int x86Reg, void * Variable, char * VariableName) {
	CPU_OR_RSP_Message(*code, "      cmp dword ptr [%s], %s",VariableName, x86_Name(x86Reg));
	switch (x86Reg) {
	case x86_EAX: PUTDST16(*code,0x0539); break;
	case x86_EBX: PUTDST16(*code,0x1D39); break;
	case x86_ECX: PUTDST16(*code,0x0D39); break;
	case x86_EDX: PUTDST16(*code,0x1539); break;
	case x86_ESI: PUTDST16(*code,0x3539); break;
	case x86_EDI: PUTDST16(*code,0x3D39); break;
	case x86_ESP: PUTDST16(*code,0x2539); break;
	case x86_EBP: PUTDST16(*code,0x2D39); break;
	default:
		DisplayError("Unknown x86 Register");
	}
	PUTDST32(*code,Variable);
}

void CompX86RegToX86Reg(BYTE** code, int Destination, int Source) {
	WORD x86Command = 0x0;

	CPU_OR_RSP_Message(*code, "      cmp %s, %s",x86_Name(Destination),x86_Name(Source));
	
	switch (Source) {
	case x86_EAX: x86Command = 0x003B; break;
	case x86_EBX: x86Command = 0x033B; break;
	case x86_ECX: x86Command = 0x013B; break;
	case x86_EDX: x86Command = 0x023B; break;
	case x86_ESI: x86Command = 0x063B; break;
	case x86_EDI: x86Command = 0x073B; break;
	case x86_ESP: x86Command = 0x043B; break;
	case x86_EBP: x86Command = 0x053B; break;
	}
	switch (Destination) {
	case x86_EAX: x86Command += 0xC000; break;
	case x86_EBX: x86Command += 0xD800; break;
	case x86_ECX: x86Command += 0xC800; break;
	case x86_EDX: x86Command += 0xD000; break;
	case x86_ESI: x86Command += 0xF000; break;
	case x86_EDI: x86Command += 0xF800; break;
	case x86_ESP: x86Command += 0xE000; break;
	case x86_EBP: x86Command += 0xE800; break;
	}
	PUTDST16(*code,x86Command);
}

void CondMoveEqual(BYTE** code, int Destination, int Source) {
	if (ConditionalMove == FALSE) {
		BYTE* Jump;
		CPU_OR_RSP_Message(*code, "   [*]cmove %s, %s", x86_Name(Destination), x86_Name(Source));

		JneLabel8(code, "label", 0);
		Jump = *code - 1;
		MoveX86RegToX86Reg(code, Source, Destination);
		CPU_OR_RSP_Message(*code, "     label:");
		x86_SetBranch8b(Jump, *code);
	}
	else {
		BYTE x86Command = 0;
		CPU_OR_RSP_Message(*code, "      cmove %s, %s", x86_Name(Destination), x86_Name(Source));

		PUTDST16(*code, 0x440F);

		switch (Source) {
		case x86_EAX: x86Command = 0x00; break;
		case x86_EBX: x86Command = 0x03; break;
		case x86_ECX: x86Command = 0x01; break;
		case x86_EDX: x86Command = 0x02; break;
		case x86_ESI: x86Command = 0x06; break;
		case x86_EDI: x86Command = 0x07; break;
		case x86_ESP: x86Command = 0x04; break;
		case x86_EBP: x86Command = 0x05; break;
		}

		switch (Destination) {
		case x86_EAX: x86Command += 0xC0; break;
		case x86_EBX: x86Command += 0xD8; break;
		case x86_ECX: x86Command += 0xC8; break;
		case x86_EDX: x86Command += 0xD0; break;
		case x86_ESI: x86Command += 0xF0; break;
		case x86_EDI: x86Command += 0xF8; break;
		case x86_ESP: x86Command += 0xE0; break;
		case x86_EBP: x86Command += 0xE8; break;
		}

		PUTDST8(*code, x86Command);
	}
}

void Cwde(BYTE** code) {
	CPU_OR_RSP_Message(*code, "      cwde");
	PUTDST8(*code, 0x98);
}

void DecX86reg(BYTE** code, int x86Reg) {
	CPU_OR_RSP_Message(*code, "      dec %s",x86_Name(x86Reg));
	switch (x86Reg) {
	case x86_EAX: PUTDST16(*code,0xC8FF); break;
	case x86_EBX: PUTDST16(*code,0xCBFF); break;
	case x86_ECX: PUTDST16(*code,0xC9FF); break;
	case x86_EDX: PUTDST16(*code,0xCAFF); break;
	case x86_ESI: PUTDST16(*code,0xCEFF); break;
	case x86_EDI: PUTDST16(*code,0xCFFF); break;
	case x86_ESP: PUTDST8 (*code,0x4C);   break;
	case x86_EBP: PUTDST8 (*code,0x4D);   break;
	default:
		DisplayError("DecX86reg\nUnknown x86 Register");
	}
}

void DivX86reg(BYTE** code, int x86reg) {
	CPU_OR_RSP_Message(*code, "      div %s",x86_Name(x86reg));
	switch (x86reg) {
	case x86_EBX: PUTDST16(*code,0xf3F7); break;
	case x86_ECX: PUTDST16(*code,0xf1F7); break;
	case x86_EDX: PUTDST16(*code,0xf2F7); break;
	case x86_ESI: PUTDST16(*code,0xf6F7); break;
	case x86_EDI: PUTDST16(*code,0xf7F7); break;
	case x86_ESP: PUTDST16(*code,0xf4F7); break;
	case x86_EBP: PUTDST16(*code,0xf5F7); break;
	default:
		DisplayError("divX86reg\nUnknown x86 Register");
	}
}

void idivX86reg(BYTE** code, int x86reg) {
	CPU_OR_RSP_Message(*code, "      idiv %s",x86_Name(x86reg));\
	switch (x86reg) {
	case x86_EBX: PUTDST16(*code,0xfbF7); break;
	case x86_ECX: PUTDST16(*code,0xf9F7); break;
	case x86_EDX: PUTDST16(*code,0xfaF7); break;
	case x86_ESI: PUTDST16(*code,0xfeF7); break;
	case x86_EDI: PUTDST16(*code,0xffF7); break;
	case x86_ESP: PUTDST16(*code,0xfcF7); break;
	case x86_EBP: PUTDST16(*code,0xfdF7); break;
	default:
		DisplayError("idivX86reg\nUnknown x86 Register");
	}
}

void imulX86reg(BYTE** code, int x86reg) {
	CPU_OR_RSP_Message(*code, "      imul %s",x86_Name(x86reg));
	switch (x86reg) {
	case x86_EAX: PUTDST16(*code,0xE8F7); break;
	case x86_EBX: PUTDST16(*code,0xEBF7); break;
	case x86_ECX: PUTDST16(*code,0xE9F7); break;
	case x86_EDX: PUTDST16(*code,0xEAF7); break;
	case x86_ESI: PUTDST16(*code,0xEEF7); break;
	case x86_EDI: PUTDST16(*code,0xEFF7); break;
	case x86_ESP: PUTDST16(*code,0xECF7); break;
	case x86_EBP: PUTDST16(*code,0xEDF7); break;
	default:
		DisplayError("imulX86reg\nUnknown x86 Register");
	}
}

void IncX86reg(BYTE** code, int x86Reg) {
	CPU_OR_RSP_Message(*code, "      inc %s",x86_Name(x86Reg));
	switch (x86Reg) {
	case x86_EAX: PUTDST16(*code,0xC0FF); break;
	case x86_EBX: PUTDST16(*code,0xC3FF); break;
	case x86_ECX: PUTDST16(*code,0xC1FF); break;
	case x86_EDX: PUTDST16(*code,0xC2FF); break;
	case x86_ESI: PUTDST16(*code,0xC6FF); break;
	case x86_EDI: PUTDST16(*code,0xC7FF); break;
	case x86_ESP: PUTDST8 (*code,0x44);   break;
	case x86_EBP: PUTDST8 (*code,0x45);   break;
	default:
		DisplayError("IncX86reg\nUnknown x86 Register");
	}
}

void JaeLabel8(BYTE** code, char * Label, BYTE Value) {
	CPU_OR_RSP_Message(*code, "      jae $%s",Label);
	PUTDST8(*code,0x73);
	PUTDST8(*code,Value);
}

void JaeLabel32(BYTE** code, char * Label,DWORD Value) {
	CPU_OR_RSP_Message(*code, "      jae $%s",Label);
	PUTDST16(*code,0x830F);
	PUTDST32(*code,Value);
}

void JaLabel8(BYTE** code, char * Label, BYTE Value) {
	CPU_OR_RSP_Message(*code, "      ja $%s",Label);
	PUTDST8(*code,0x77);
	PUTDST8(*code,Value);
}

void JaLabel32(BYTE** code, char * Label,DWORD Value) {
	CPU_OR_RSP_Message(*code, "      ja $%s",Label);
	PUTDST16(*code,0x870F);
	PUTDST32(*code,Value);
}

void JbLabel8(BYTE** code, char * Label, BYTE Value) {
	CPU_OR_RSP_Message(*code, "      jb $%s",Label);
	PUTDST8(*code,0x72);
	PUTDST8(*code,Value);
}

void JbLabel32(BYTE** code, char * Label,DWORD Value) {
	CPU_OR_RSP_Message(*code, "      jb $%s",Label);
	PUTDST16(*code,0x820F);
	PUTDST32(*code,Value);
}

void JecxzLabel8(BYTE** code, char * Label, BYTE Value) {
	CPU_OR_RSP_Message(*code, "      jecxz $%s",Label);
	PUTDST8(*code,0xE3);
	PUTDST8(*code,Value);
}

void JeLabel8(BYTE** code, char * Label, BYTE Value) {
	CPU_OR_RSP_Message(*code, "      je $%s",Label);
	PUTDST8(*code,0x74);
	PUTDST8(*code,Value);
}

void JeLabel32(BYTE** code, char * Label,DWORD Value) {
	CPU_OR_RSP_Message(*code, "      je $%s",Label);
	PUTDST16(*code,0x840F);
	PUTDST32(*code,Value);
}

void JgeLabel32(BYTE** code, char * Label,DWORD Value) {
	CPU_OR_RSP_Message(*code, "      jge $%s",Label);
	PUTDST16(*code,0x8D0F);
	PUTDST32(*code,Value);
}

void JgLabel8(BYTE** code, char * Label, BYTE Value) {
	CPU_OR_RSP_Message(*code, "      jg $%s",Label);
	PUTDST8(*code,0x7F);
	PUTDST8(*code,Value);
}

void JgLabel32(BYTE** code, char * Label,DWORD Value) {
	CPU_OR_RSP_Message(*code, "      jg $%s",Label);
	PUTDST16(*code,0x8F0F);
	PUTDST32(*code,Value);
}

void JleLabel8(BYTE** code, char * Label, BYTE Value) {
	CPU_OR_RSP_Message(*code, "      jle $%s",Label);
	PUTDST8(*code,0x7E);
	PUTDST8(*code,Value);
}

void JleLabel32(BYTE** code, char * Label,DWORD Value) {
	CPU_OR_RSP_Message(*code, "      jle $%s",Label);
	PUTDST16(*code,0x8E0F);
	PUTDST32(*code,Value);
}

void JlLabel8(BYTE** code, char * Label, BYTE Value) {
	CPU_OR_RSP_Message(*code, "      jl $%s",Label);
	PUTDST8(*code,0x7C);
	PUTDST8(*code,Value);
}

void JlLabel32(BYTE** code, char * Label,DWORD Value) {
	CPU_OR_RSP_Message(*code, "      jl $%s",Label);
	PUTDST16(*code,0x8C0F);
	PUTDST32(*code,Value);
}

void JmpDirectReg( BYTE** code, int x86reg ) {
	CPU_OR_RSP_Message(*code, "      jmp %s",x86_Name(x86reg));

	switch (x86reg) {
	case x86_EAX: PUTDST16(*code,0xE0ff); break;
	case x86_EBX: PUTDST16(*code,0xE3ff); break;
	case x86_ECX: PUTDST16(*code,0xE1ff); break;
	case x86_EDX: PUTDST16(*code,0xE2ff); break;
	case x86_ESI: PUTDST16(*code,0xE6ff); break;
	case x86_EDI: PUTDST16(*code,0xE7ff); break;
	default:
		DisplayError("JmpDirectReg\nUnknown x86 Register");		
		break;
	}
}

void JmpIndirectLabel32(BYTE** code, char * Label,DWORD location) {
	CPU_OR_RSP_Message(*code, "      jmp dword ptr [%s]", Label);
	PUTDST16(*code, 0x25ff);
	PUTDST32(*code, location);
}

void JmpIndirectReg(BYTE** code, int x86reg ) {
	CPU_OR_RSP_Message(*code, "      jmp dword ptr [%s]",x86_Name(x86reg));

	switch (x86reg) {
	case x86_EAX: PUTDST16(*code,0x20ff); break;
	case x86_EBX: PUTDST16(*code,0x23ff); break;
	case x86_ECX: PUTDST16(*code,0x21ff); break;
	case x86_EDX: PUTDST16(*code,0x22ff); break;
	case x86_ESI: PUTDST16(*code,0x26ff); break;
	case x86_EDI: PUTDST16(*code,0x27ff); break;
	case x86_ESP: 
		PUTDST8(*code,0xff);
		PUTDST16(*code,0x2434);
	/*	_asm int 3 */
		break;		
	case x86_EBP: 
		PUTDST8(*code,0xff);
		PUTDST16(*code,0x0065);
	/*	_asm int 3 */
		break;
	}
}

void JmpLabel8(BYTE** code, char * Label, BYTE Value) {
	CPU_OR_RSP_Message(*code, "      jmp $%s",Label);
	PUTDST8(*code,0xEB);
	PUTDST8(*code,Value);
}

void JmpLabel32(BYTE** code, char * Label, DWORD Value) {
	CPU_OR_RSP_Message(*code, "      jmp $%s",Label);
	PUTDST8(*code,0xE9);
	PUTDST32(*code,Value);
}

void JneLabel8(BYTE** code, char * Label, BYTE Value) {
	CPU_OR_RSP_Message(*code, "      jne $%s",Label);
	PUTDST8(*code,0x75);
	PUTDST8(*code,Value);
}

void JneLabel32(BYTE** code, char *Label, DWORD Value) {
	CPU_OR_RSP_Message(*code, "      jne $%s",Label);
	PUTDST16(*code,0x850F);
	PUTDST32(*code,Value);
}

void JnsLabel8(BYTE** code, char * Label, BYTE Value) {
	CPU_OR_RSP_Message(*code, "      jns $%s",Label);
	PUTDST8(*code,0x79);
	PUTDST8(*code,Value);
}

void JnsLabel32(BYTE** code, char *Label, DWORD Value) {
	CPU_OR_RSP_Message(*code, "      jns $%s",Label);
	PUTDST16(*code,0x890F);
	PUTDST32(*code,Value);
}

void JsLabel32(BYTE** code, char *Label, DWORD Value) {
	CPU_OR_RSP_Message(*code, "      js $%s",Label);
	PUTDST16(*code,0x880F);
	PUTDST32(*code,Value);
}

void LeaRegReg(BYTE** code, int x86RegDest, int x86RegSrc, int multiplier) {
	int s = 0x0;
	CPU_OR_RSP_Message(*code, "      lea %s, [%s*%i]", x86_Name(x86RegDest), x86_Name(x86RegSrc), multiplier);

	PUTDST8(*code,0x8D);
	switch (x86RegDest) {
	case x86_EAX: PUTDST8(*code,0x04); break;
	case x86_EBX: PUTDST8(*code,0x1C); break;
	case x86_ECX: PUTDST8(*code,0x0C); break;
	case x86_EDX: PUTDST8(*code,0x14); break;
	case x86_ESI: PUTDST8(*code,0x34); break;
	case x86_EDI: PUTDST8(*code,0x3C); break;
	case x86_ESP: PUTDST8(*code,0x24); break;
	case x86_EBP: PUTDST8(*code,0x2C); break;
	default:
		DisplayError("LEA\nUnknown x86 Register");
	}

	/* put in shifter 2(01), 4(10), 8(11) */
	switch(multiplier) {
	case 2: s = 0x40; break;
	case 4: s = 0x80; break;
	case 8: s = 0xC0; break;
	default:
		DisplayError("LEA\nInvalid x86 multiplier");
	}

	/* format ss|000000 */
	switch (x86RegSrc) {
	case x86_EAX: PUTDST8(*code,0x05|s); break;
	case x86_EBX: PUTDST8(*code,0x1D|s); break;
	case x86_ECX: PUTDST8(*code,0x0D|s); break;
	case x86_EDX: PUTDST8(*code,0x15|s); break;
	case x86_ESI: PUTDST8(*code,0x35|s); break;
	case x86_EDI: PUTDST8(*code,0x3D|s); break;
	case x86_EBP: PUTDST8(*code,0x2D|s); break;
	case x86_ESP: 
		DisplayError("LEA\nESP is invalid"); break;
	default:
		DisplayError("LEA\nUnknown x86 Register");
	}

	PUTDST32(*code,0x00000000);
}

void LeaSourceAndOffset(BYTE** code, int x86DestReg, int x86SourceReg, int offset) {
	WORD x86Command = 0x0;

	CPU_OR_RSP_Message(*code, "      lea %s, [%s + %0Xh]",x86_Name(x86DestReg),x86_Name(x86SourceReg),offset);

	if ((offset & 0xFFFFFF80) != 0 && (offset & 0xFFFFFF80) != 0xFFFFFF80) {
		switch (x86DestReg) {
		case x86_EAX: x86Command = 0x808D; break;
		case x86_EBX: x86Command = 0x988D; break;
		case x86_ECX: x86Command = 0x888D; break;
		case x86_EDX: x86Command = 0x908D; break;
		case x86_ESI: x86Command = 0xB08D; break;
		case x86_EDI: x86Command = 0xB88D; break;
		case x86_ESP: x86Command = 0xA08D; break;
		case x86_EBP: x86Command = 0xA88D; break;
		default:
			DisplayError("LeaSourceAndOffset\nUnknown x86 Register");
		}
		switch (x86SourceReg) {
		case x86_EAX: x86Command += 0x0000; break;
		case x86_EBX: x86Command += 0x0300; break;
		case x86_ECX: x86Command += 0x0100; break;
		case x86_EDX: x86Command += 0x0200; break;
		case x86_ESI: x86Command += 0x0600; break;
		case x86_EDI: x86Command += 0x0700; break;
		case x86_ESP: x86Command += 0x0400; break;
		case x86_EBP: x86Command += 0x0500; break;
		default:
			DisplayError("LeaSourceAndOffset\nUnknown x86 Register");
		}
		PUTDST16(*code, x86Command);
		PUTDST32(*code, offset);
	}
	else {
		switch (x86DestReg) {
		case x86_EAX: x86Command = 0x408D; break;
		case x86_EBX: x86Command = 0x588D; break;
		case x86_ECX: x86Command = 0x488D; break;
		case x86_EDX: x86Command = 0x508D; break;
		case x86_ESI: x86Command = 0x708D; break;
		case x86_EDI: x86Command = 0x788D; break;
		case x86_ESP: x86Command = 0x608D; break;
		case x86_EBP: x86Command = 0x688D; break;
		default:
			DisplayError("LeaSourceAndOffset\nUnknown x86 Register");
		}
		switch (x86SourceReg) {
		case x86_EAX: x86Command += 0x0000; break;
		case x86_EBX: x86Command += 0x0300; break;
		case x86_ECX: x86Command += 0x0100; break;
		case x86_EDX: x86Command += 0x0200; break;
		case x86_ESI: x86Command += 0x0600; break;
		case x86_EDI: x86Command += 0x0700; break;
		case x86_ESP: x86Command += 0x0400; break;
		case x86_EBP: x86Command += 0x0500; break;
		default:
			DisplayError("LeaSourceAndOffset\nUnknown x86 Register");
		}
		PUTDST16(*code, x86Command);
		PUTDST8(*code, offset);
	}
}

static void MoveConstByteToBaseMem(BYTE** code, BYTE Const, int AddrReg, BYTE* base, char* baseName) {
	CPU_OR_RSP_Message(*code, "      mov byte ptr [%s+%s], %Xh", x86_Name(AddrReg), baseName, Const);
	switch (AddrReg) {
	case x86_EAX: PUTDST16(*code, 0x80C6); break;
	case x86_EBX: PUTDST16(*code, 0x83C6); break;
	case x86_ECX: PUTDST16(*code, 0x81C6); break;
	case x86_EDX: PUTDST16(*code, 0x82C6); break;
	case x86_ESI: PUTDST16(*code, 0x86C6); break;
	case x86_EDI: PUTDST16(*code, 0x87C6); break;
	case x86_ESP: PUTDST16(*code, 0x84C6); break;
	case x86_EBP: PUTDST16(*code, 0x85C6); break;
	default:
		DisplayError("MoveConstByteToN64Mem\nUnknown x86 Register");
	}
	PUTDST32(*code, base);
	PUTDST8(*code, Const);
}

void MoveConstByteToN64Mem(BYTE** code, BYTE Const, int AddrReg) {
	MoveConstByteToBaseMem(code, Const, AddrReg, N64MEM, "N64MEM");
}

void MoveConstByteToDMem(BYTE** code, BYTE Const, int AddrReg) {
	MoveConstByteToBaseMem(code, Const, AddrReg, DMEM, "DMEM");
}

void MoveConstByteToVariable (BYTE** code, BYTE Const,void *Variable, char *VariableName) {
	CPU_OR_RSP_Message(*code, "      mov byte ptr [%s], %Xh",VariableName,Const);
	PUTDST16(*code,0x05C6);
    PUTDST32(*code,Variable);
    PUTDST8(*code,Const);
}

static void MoveConstHalfToBaseMem(BYTE** code, WORD Const, int AddrReg, BYTE* base, char* baseName) {
	CPU_OR_RSP_Message(*code, "      mov word ptr [%s+%s], %Xh", x86_Name(AddrReg), baseName, Const);
	PUTDST8(*code, 0x66);
	switch (AddrReg) {
	case x86_EAX: PUTDST16(*code, 0x80C7); break;
	case x86_EBX: PUTDST16(*code, 0x83C7); break;
	case x86_ECX: PUTDST16(*code, 0x81C7); break;
	case x86_EDX: PUTDST16(*code, 0x82C7); break;
	case x86_ESI: PUTDST16(*code, 0x86C7); break;
	case x86_EDI: PUTDST16(*code, 0x87C7); break;
	case x86_ESP: PUTDST16(*code, 0x84C7); break;
	case x86_EBP: PUTDST16(*code, 0x85C7); break;
	default:
		DisplayError("MoveConstToN64Mem\nUnknown x86 Register");
	}
	PUTDST32(*code, base);
	PUTDST16(*code, Const);
}

void MoveConstHalfToN64Mem(BYTE** code, WORD Const, int AddrReg) {
	MoveConstHalfToBaseMem(code, Const, AddrReg, N64MEM, "N64MEM");
}

void MoveConstHalfToDMem(BYTE** code, WORD Const, int AddrReg) {
	MoveConstHalfToBaseMem(code, Const, AddrReg, DMEM, "DMEM");
}

void MoveConstHalfToVariable (BYTE** code, WORD Const,void *Variable, char *VariableName) {
	CPU_OR_RSP_Message(*code,"      mov word ptr [%s], %Xh",VariableName,Const);
	PUTDST8(*code,0x66);
	PUTDST16(*code,0x05C7);
    PUTDST32(*code,Variable);
    PUTDST16(*code,Const);
}

void MoveConstHalfToX86regPointer(BYTE** code, WORD Const, int AddrReg1, int AddrReg2) {
	BYTE Param = 0x0;

	CPU_OR_RSP_Message(*code, "      mov word ptr [%s+%s],%Xh",x86_Name(AddrReg1), x86_Name(AddrReg2), Const);

	PUTDST8(*code,0x66);
	PUTDST16(*code,0x04C7);

	switch (AddrReg1) {
	case x86_EAX: Param = 0x00; break;
	case x86_EBX: Param = 0x03; break;
	case x86_ECX: Param = 0x01; break;
	case x86_EDX: Param = 0x02; break;
	case x86_ESI: Param = 0x06; break;
	case x86_EDI: Param = 0x07; break;
	default:
		DisplayError("MoveConstToX86regPointer\nUnhandled x86 Register");
	}

	switch (AddrReg2) {
	case x86_EAX: Param += 0x00; break;
	case x86_EBX: Param += 0x18; break;
	case x86_ECX: Param += 0x08; break;
	case x86_EDX: Param += 0x10; break;
	case x86_ESI: Param += 0x30; break;
	case x86_EDI: Param += 0x38; break;
	case x86_ESP: Param += 0x20; break;
	case x86_EBP: Param += 0x28; break;
	default:
		DisplayError("MoveConstToX86regPointer\nUnhandled x86 Register");
	}
	PUTDST8(*code,Param);
    PUTDST16(*code,Const);
}

void MoveConstToMemoryDisp (BYTE** code, DWORD Const, int AddrReg, DWORD Disp) {
	CPU_OR_RSP_Message(*code, "      mov dword ptr [%s+%Xh], %Xh",x86_Name(AddrReg),Disp,Const);
	switch (AddrReg) {
	case x86_EAX: PUTDST16(*code,0x80C7); break;
	case x86_EBX: PUTDST16(*code,0x83C7); break;
	case x86_ECX: PUTDST16(*code,0x81C7); break;
	case x86_EDX: PUTDST16(*code,0x82C7); break;
	case x86_ESI: PUTDST16(*code,0x86C7); break;
	case x86_EDI: PUTDST16(*code,0x87C7); break;
	case x86_ESP: PUTDST16(*code,0x84C7); break;
	case x86_EBP: PUTDST16(*code,0x85C7); break;
	default:
		DisplayError("MoveConstToN64Mem\nUnknown x86 Register");
	}
	PUTDST32(*code,Disp);
	PUTDST32(*code,Const);
}

static void MoveConstToBaseMem(BYTE** code, DWORD Const, int AddrReg, BYTE* base, char* baseName) {
	CPU_OR_RSP_Message(*code, "      mov dword ptr [%s+%s], %Xh", x86_Name(AddrReg), baseName, Const);
	switch (AddrReg) {
	case x86_EAX: PUTDST16(*code, 0x80C7); break;
	case x86_EBX: PUTDST16(*code, 0x83C7); break;
	case x86_ECX: PUTDST16(*code, 0x81C7); break;
	case x86_EDX: PUTDST16(*code, 0x82C7); break;
	case x86_ESI: PUTDST16(*code, 0x86C7); break;
	case x86_EDI: PUTDST16(*code, 0x87C7); break;
	case x86_ESP: PUTDST16(*code, 0x84C7); break;
	case x86_EBP: PUTDST16(*code, 0x85C7); break;
	default:
		DisplayError("MoveConstToN64Mem\nUnknown x86 Register");
	}
	PUTDST32(*code, base);
	PUTDST32(*code, Const);
}

void MoveConstToN64Mem(BYTE** code, DWORD Const, int AddrReg) {
	MoveConstToBaseMem(code, Const, AddrReg, N64MEM, "N64MEM");
}

void MoveConstToDMem(BYTE** code, DWORD Const, int AddrReg) {
	MoveConstToBaseMem(code, Const, AddrReg, DMEM, "DMEM");
}

void MoveConstToN64MemDisp (BYTE** code, DWORD Const, int AddrReg, BYTE Disp) {
	CPU_OR_RSP_Message(*code, "      mov dword ptr [%s+N64mem+%Xh], %Xh",x86_Name(AddrReg),Const,Disp);
	switch (AddrReg) {
	case x86_EAX: PUTDST16(*code,0x80C7); break;
	case x86_EBX: PUTDST16(*code,0x83C7); break;
	case x86_ECX: PUTDST16(*code,0x81C7); break;
	case x86_EDX: PUTDST16(*code,0x82C7); break;
	case x86_ESI: PUTDST16(*code,0x86C7); break;
	case x86_EDI: PUTDST16(*code,0x87C7); break;
	case x86_ESP: PUTDST16(*code,0x84C7); break;
	case x86_EBP: PUTDST16(*code,0x85C7); break;
	default:
		DisplayError("MoveConstToN64Mem\nUnknown x86 Register");
	}
	PUTDST32(*code,N64MEM + Disp);
	PUTDST32(*code,Const);
}

void MoveConstToVariable (BYTE** code, DWORD Const,void *Variable, char *VariableName) {
	CPU_OR_RSP_Message(*code, "      mov dword ptr [%s], %Xh",VariableName,Const);
	PUTDST16(*code,0x05C7);
    PUTDST32(*code,Variable);
    PUTDST32(*code,Const);
}

void MoveConstToX86Pointer(BYTE** code, DWORD Const, int X86Pointer) {
	WORD x86Command = 0x0;

	CPU_OR_RSP_Message(*code, "      mov dword ptr [%s], %Xh",x86_Name(X86Pointer),Const);

	switch (X86Pointer) {
	case x86_EAX: x86Command = 0x00C7; break;
	case x86_EBX: x86Command = 0x03C7; break;
	case x86_ECX: x86Command = 0x01C7; break;
	case x86_EDX: x86Command = 0x02C7; break;
	case x86_ESI: x86Command = 0x06C7; break;
	case x86_EDI: x86Command = 0x07C7; break;
	}
	PUTDST16(*code,x86Command);
    PUTDST32(*code,Const);
}


void MoveConstToX86reg(BYTE** code, DWORD Const, int x86reg) {
	CPU_OR_RSP_Message(*code, "      mov %s, %Xh",x86_Name(x86reg),Const);
	switch (x86reg) {
	case x86_EAX: PUTDST8(*code,0xB8); break;
	case x86_EBX: PUTDST8(*code,0xBB); break;
	case x86_ECX: PUTDST8(*code,0xB9); break;
	case x86_EDX: PUTDST8(*code,0xBA); break;
	case x86_ESI: PUTDST8(*code,0xBE); break;
	case x86_EDI: PUTDST8(*code,0xBF); break;
	case x86_ESP: PUTDST8(*code,0xBC); break;
	case x86_EBP: PUTDST8(*code,0xBD); break;
	default:
		DisplayError("MoveConstToX86reg\nUnknown x86 Register");
		_asm int 3
	}
    PUTDST32(*code,Const);
}

void MoveConstByteToX86regPointer(BYTE** code, BYTE Const, int AddrReg1, int AddrReg2) {
	BYTE Param = 0x0;

	CPU_OR_RSP_Message(*code, "      mov byte ptr [%s+%s],%Xh",x86_Name(AddrReg1), x86_Name(AddrReg2), Const);

	PUTDST16(*code,0x04C6);

	switch (AddrReg1) {
	case x86_EAX: Param = 0x00; break;
	case x86_EBX: Param = 0x03; break;
	case x86_ECX: Param = 0x01; break;
	case x86_EDX: Param = 0x02; break;
	case x86_ESI: Param = 0x06; break;
	case x86_EDI: Param = 0x07; break;
	default:
		DisplayError("MoveConstByteToX86regPointer\nUnhandled x86 Register");
	}

	switch (AddrReg2) {
	case x86_EAX: Param += 0x00; break;
	case x86_EBX: Param += 0x18; break;
	case x86_ECX: Param += 0x08; break;
	case x86_EDX: Param += 0x10; break;
	case x86_ESI: Param += 0x30; break;
	case x86_EDI: Param += 0x38; break;
	case x86_ESP: Param += 0x20; break;
	case x86_EBP: Param += 0x28; break;
	default:
		DisplayError("MoveConstByteToX86regPointer\nUnhandled x86 Register");
	}
	PUTDST8(*code,Param);
    PUTDST8(*code,Const);
}

void MoveConstToX86regPointer(BYTE** code, DWORD Const, int AddrReg1, int AddrReg2) {
	BYTE Param = 0x0;

	CPU_OR_RSP_Message(*code, "      mov dword ptr [%s+%s],%Xh",x86_Name(AddrReg1), x86_Name(AddrReg2), Const);

	PUTDST16(*code,0x04C7);

	switch (AddrReg1) {
	case x86_EAX: Param = 0x00; break;
	case x86_EBX: Param = 0x03; break;
	case x86_ECX: Param = 0x01; break;
	case x86_EDX: Param = 0x02; break;
	case x86_ESI: Param = 0x06; break;
	case x86_EDI: Param = 0x07; break;
	default:
		DisplayError("MoveConstToX86regPointer\nUnhandled x86 Register");
	}

	switch (AddrReg2) {
	case x86_EAX: Param += 0x00; break;
	case x86_EBX: Param += 0x18; break;
	case x86_ECX: Param += 0x08; break;
	case x86_EDX: Param += 0x10; break;
	case x86_ESI: Param += 0x30; break;
	case x86_EDI: Param += 0x38; break;
	case x86_ESP: Param += 0x20; break;
	case x86_EBP: Param += 0x28; break;
	default:
		DisplayError("MoveConstToX86regPointer\nUnhandled x86 Register");
	}
	PUTDST8(*code,Param);
    PUTDST32(*code,Const);
}

void MoveN64MemDispToX86reg(BYTE** code, int x86reg, int AddrReg, BYTE Disp) {
	WORD x86Command = 0x0;

	CPU_OR_RSP_Message(*code, "      mov %s, dword ptr [%s+N64mem+%Xh]",x86_Name(x86reg),x86_Name(AddrReg),Disp);
	switch (AddrReg) {
	case x86_EAX: x86Command = 0x008B; break;
	case x86_EBX: x86Command = 0x038B; break;
	case x86_ECX: x86Command = 0x018B; break;
	case x86_EDX: x86Command = 0x028B; break;
	case x86_ESI: x86Command = 0x068B; break;
	case x86_EDI: x86Command = 0x078B; break;
	case x86_ESP: x86Command = 0x048B; break;
	case x86_EBP: x86Command = 0x058B; break;
	}
	switch (x86reg) {
	case x86_EAX: x86Command += 0x8000; break;
	case x86_EBX: x86Command += 0x9800; break;
	case x86_ECX: x86Command += 0x8800; break;
	case x86_EDX: x86Command += 0x9000; break;
	case x86_ESI: x86Command += 0xB000; break;
	case x86_EDI: x86Command += 0xB800; break;
	case x86_ESP: x86Command += 0xA000; break;
	case x86_EBP: x86Command += 0xA800; break;
	}
	PUTDST16(*code,x86Command);
	PUTDST32(*code,N64MEM + Disp);
}

static void MoveBaseMemToX86reg(BYTE** code, int x86reg, int AddrReg, BYTE* base, char* baseName) {
	WORD x86Command = 0x0;

	CPU_OR_RSP_Message(*code, "      mov %s, dword ptr [%s+%s]", x86_Name(x86reg), x86_Name(AddrReg), baseName);

	switch (AddrReg) {
	case x86_EAX: x86Command = 0x008B; break;
	case x86_EBX: x86Command = 0x038B; break;
	case x86_ECX: x86Command = 0x018B; break;
	case x86_EDX: x86Command = 0x028B; break;
	case x86_ESI: x86Command = 0x068B; break;
	case x86_EDI: x86Command = 0x078B; break;
	case x86_ESP: x86Command = 0x048B; break;
	case x86_EBP: x86Command = 0x058B; break;
	}
	switch (x86reg) {
	case x86_EAX: x86Command += 0x8000; break;
	case x86_EBX: x86Command += 0x9800; break;
	case x86_ECX: x86Command += 0x8800; break;
	case x86_EDX: x86Command += 0x9000; break;
	case x86_ESI: x86Command += 0xB000; break;
	case x86_EDI: x86Command += 0xB800; break;
	case x86_ESP: x86Command += 0xA000; break;
	case x86_EBP: x86Command += 0xA800; break;
	}
	PUTDST16(*code, x86Command);
	PUTDST32(*code, base);
}

void MoveN64MemToX86reg(BYTE** code, int x86reg, int AddrReg) {
	MoveBaseMemToX86reg(code, x86reg, AddrReg, N64MEM, "N64mem");
}

void MoveDMemToX86reg(BYTE** code, int x86reg, int AddrReg) {
	MoveBaseMemToX86reg(code, x86reg, AddrReg, DMEM, "DMEM");
}

static void MoveBaseMemToX86regByte(BYTE** code, int x86reg, int AddrReg, BYTE* base, char* baseName) {
	WORD x86Command = 0x0;

	CPU_OR_RSP_Message(*code, "      mov %s, byte ptr [%s+%s]", x86Byte_Name(x86reg), x86_Name(AddrReg), baseName);
	switch (AddrReg) {
	case x86_EAX: x86Command = 0x008A; break;
	case x86_EBX: x86Command = 0x038A; break;
	case x86_ECX: x86Command = 0x018A; break;
	case x86_EDX: x86Command = 0x028A; break;
	case x86_ESI: x86Command = 0x068A; break;
	case x86_EDI: x86Command = 0x078A; break;
	case x86_ESP: x86Command = 0x048A; break;
	case x86_EBP: x86Command = 0x058A; break;
	}
	switch (x86reg) {
	case x86_EAX: x86Command += 0x8000; break;
	case x86_EBX: x86Command += 0x9800; break;
	case x86_ECX: x86Command += 0x8800; break;
	case x86_EDX: x86Command += 0x9000; break;
		/*	case x86_ESI: x86Command += 0xB000; break; */
		/*	case x86_EDI: x86Command += 0xB800; break; */
		/*	case x86_ESP: case x86_EBP: */
	default:
		DisplayError("MoveN64MemToX86regByte\nInvalid x86 Register");
		break;
	}
	PUTDST16(*code, x86Command);
	PUTDST32(*code, base);
}

void MoveN64MemToX86regByte(BYTE** code, int x86reg, int AddrReg) {
	MoveBaseMemToX86regByte(code, x86reg, AddrReg, N64MEM, "N64mem");
}

void MoveDMemToX86regByte(BYTE** code, int x86reg, int AddrReg) {
	MoveBaseMemToX86regByte(code, x86reg, AddrReg, DMEM, "DMEM");
}

void MoveDMemToX86regHighByte(BYTE** code, int x86reg, int AddrReg) {
	WORD x86Command = 0x0;

	CPU_OR_RSP_Message(*code, "      mov %s, byte ptr [%s+DMEM]", x86HighByte_Name(x86reg), x86_Name(AddrReg));
	switch (AddrReg) {
	case x86_EAX: x86Command = 0x008A; break;
	case x86_EBX: x86Command = 0x038A; break;
	case x86_ECX: x86Command = 0x018A; break;
	case x86_EDX: x86Command = 0x028A; break;
	case x86_ESI: x86Command = 0x068A; break;
	case x86_EDI: x86Command = 0x078A; break;
	case x86_ESP: x86Command = 0x048A; break;
	case x86_EBP: x86Command = 0x058A; break;
	}
	switch (x86reg) {
	case x86_EAX: x86Command += 0xa000; break;
	case x86_EBX: x86Command += 0xb800; break;
	case x86_ECX: x86Command += 0xa800; break;
	case x86_EDX: x86Command += 0xb000; break;
	default:
		DisplayError("MoveN64MemToX86regByte\nInvalid x86 Register");
		break;
	}
	PUTDST16(*code, x86Command);
	PUTDST32(*code, DMEM);
}

void MoveN64MemToX86regHalf(BYTE** code, int x86reg, int AddrReg) {
	WORD x86Command = 0x0;

	CPU_OR_RSP_Message(*code, "      mov %s, word ptr [%s+N64mem]",x86Half_Name(x86reg),x86_Name(AddrReg));
	
	PUTDST8(*code,0x66);
	switch (AddrReg) {
	case x86_EAX: x86Command = 0x008B; break;
	case x86_EBX: x86Command = 0x038B; break;
	case x86_ECX: x86Command = 0x018B; break;
	case x86_EDX: x86Command = 0x028B; break;
	case x86_ESI: x86Command = 0x068B; break;
	case x86_EDI: x86Command = 0x078B; break;
	case x86_ESP: x86Command = 0x048B; break;
	case x86_EBP: x86Command = 0x058B; break;
	}
	switch (x86reg) {
	case x86_EAX: x86Command += 0x8000; break;
	case x86_EBX: x86Command += 0x9800; break;
	case x86_ECX: x86Command += 0x8800; break;
	case x86_EDX: x86Command += 0x9000; break;
	case x86_ESI: x86Command += 0xB000; break;
	case x86_EDI: x86Command += 0xB800; break;
	case x86_ESP: x86Command += 0xA000; break;
	case x86_EBP: x86Command += 0xA800; break;
	}
	PUTDST16(*code,x86Command);
	PUTDST32(*code,N64MEM);
}

void MoveSxByteX86regPointerToX86reg(BYTE** code, int AddrReg1, int AddrReg2, int x86reg) {
	BYTE Param = 0x0;

	CPU_OR_RSP_Message(*code, "      movsx %s, byte ptr [%s+%s]",x86_Name(x86reg),x86_Name(AddrReg1), x86_Name(AddrReg2));

	PUTDST16(*code,0xBE0F);
	switch (x86reg) {
	case x86_EAX: PUTDST8(*code,0x04); break;
	case x86_EBX: PUTDST8(*code,0x1C); break;
	case x86_ECX: PUTDST8(*code,0x0C); break;
	case x86_EDX: PUTDST8(*code,0x14); break;
	case x86_ESI: PUTDST8(*code,0x34); break;
	case x86_EDI: PUTDST8(*code,0x3C); break;
	case x86_ESP: PUTDST8(*code,0x24); break;
	case x86_EBP: PUTDST8(*code,0x2C); break;
	default:
		DisplayError("MoveZxByteX86regPointerToX86reg\nUnhandled x86 Register");
	}

	switch (AddrReg1) {
	case x86_EAX: Param = 0x00; break;
	case x86_EBX: Param = 0x03; break;
	case x86_ECX: Param = 0x01; break;
	case x86_EDX: Param = 0x02; break;
	case x86_ESI: Param = 0x06; break;
	case x86_EDI: Param = 0x07; break;
	default:
		DisplayError("MoveZxByteX86regPointerToX86reg\nUnhandled x86 Register");
	}

	switch (AddrReg2) {
	case x86_EAX: Param += 0x00; break;
	case x86_EBX: Param += 0x18; break;
	case x86_ECX: Param += 0x08; break;
	case x86_EDX: Param += 0x10; break;
	case x86_ESI: Param += 0x30; break;
	case x86_EDI: Param += 0x38; break;
	case x86_ESP: Param += 0x20; break;
	case x86_EBP: Param += 0x28; break;
	default:
		DisplayError("MoveZxByteX86regPointerToX86reg\nUnhandled x86 Register");
	}
	PUTDST8(*code,Param);
}

void MoveSxHalfX86regPointerToX86reg(BYTE** code, int AddrReg1, int AddrReg2, int x86reg) {
	BYTE Param = 0x0;

	CPU_OR_RSP_Message(*code, "      movsx %s, word ptr [%s+%s]",x86_Name(x86reg),x86_Name(AddrReg1), x86_Name(AddrReg2));

	PUTDST16(*code,0xBF0F);
	switch (x86reg) {
	case x86_EAX: PUTDST8(*code,0x04); break;
	case x86_EBX: PUTDST8(*code,0x1C); break;
	case x86_ECX: PUTDST8(*code,0x0C); break;
	case x86_EDX: PUTDST8(*code,0x14); break;
	case x86_ESI: PUTDST8(*code,0x34); break;
	case x86_EDI: PUTDST8(*code,0x3C); break;
	case x86_ESP: PUTDST8(*code,0x24); break;
	case x86_EBP: PUTDST8(*code,0x2C); break;
	default:
		DisplayError("MoveZxHalfX86regPointerToX86reg\nUnhandled x86 Register");
	}

	switch (AddrReg1) {
	case x86_EAX: Param = 0x00; break;
	case x86_EBX: Param = 0x03; break;
	case x86_ECX: Param = 0x01; break;
	case x86_EDX: Param = 0x02; break;
	case x86_ESI: Param = 0x06; break;
	case x86_EDI: Param = 0x07; break;
	default:
		DisplayError("MoveZxHalfX86regPointerToX86reg\nUnhandled x86 Register");
	}

	switch (AddrReg2) {
	case x86_EAX: Param += 0x00; break;
	case x86_EBX: Param += 0x18; break;
	case x86_ECX: Param += 0x08; break;
	case x86_EDX: Param += 0x10; break;
	case x86_ESI: Param += 0x30; break;
	case x86_EDI: Param += 0x38; break;
	case x86_ESP: Param += 0x20; break;
	case x86_EBP: Param += 0x28; break;
	default:
		DisplayError("MoveZxHalfX86regPointerToX86reg\nUnhandled x86 Register");
	}
	PUTDST8(*code,Param);
}

void MoveSxHalfX86regToX86reg(BYTE** code, int Source, int Destination) {
	if (Source == x86_EAX && Destination == x86_EAX) {
		CPU_OR_RSP_Message(*code, "      cwde");

		PUTDST8(*code, 0x98);
	} else {
		BYTE Param = 0x0;

		CPU_OR_RSP_Message(*code, "      movsx %s, %s", x86_Name(Destination), x86Half_Name(Source));

		PUTDST16(*code, 0xBF0F);
		switch (Destination) {
		case x86_EAX: Param = 0xC0; break;
		case x86_EBX: Param = 0xD8; break;
		case x86_ECX: Param = 0xC8; break;
		case x86_EDX: Param = 0xD0; break;
		case x86_ESI: Param = 0xF0; break;
		case x86_EDI: Param = 0xF8; break;
		case x86_ESP: Param = 0xE0; break;
		case x86_EBP: Param = 0xE8; break;
		default:
			DisplayError("MoveZxHalfX86regToX86reg\nUnhandled x86 Register");
		}

		switch (Source) {
		case x86_EAX: Param += 0x00; break;
		case x86_EBX: Param += 0x03; break;
		case x86_ECX: Param += 0x01; break;
		case x86_EDX: Param += 0x02; break;
		case x86_ESI: Param += 0x06; break;
		case x86_EDI: Param += 0x07; break;
		case x86_ESP: Param += 0x04; break;
		case x86_EBP: Param += 0x05; break;
		default:
			DisplayError("MoveZxHalfX86regToX86reg\nUnhandled x86 Register");
		}
		PUTDST8(*code, Param);
	}
}

static void MoveSxBaseMemToX86regByte(BYTE** code, int x86reg, int AddrReg, BYTE* base, char* baseName) {
	WORD x86Command = 0x0;

	CPU_OR_RSP_Message(*code, "      movsx %s, byte ptr [%s+%s]", x86_Name(x86reg), x86_Name(AddrReg), baseName);
	switch (AddrReg) {
	case x86_EAX: x86Command = 0x00BE; break;
	case x86_EBX: x86Command = 0x03BE; break;
	case x86_ECX: x86Command = 0x01BE; break;
	case x86_EDX: x86Command = 0x02BE; break;
	case x86_ESI: x86Command = 0x06BE; break;
	case x86_EDI: x86Command = 0x07BE; break;
	case x86_ESP: x86Command = 0x04BE; break;
	case x86_EBP: x86Command = 0x05BE; break;
	}
	switch (x86reg) {
	case x86_EAX: x86Command += 0x8000; break;
	case x86_EBX: x86Command += 0x9800; break;
	case x86_ECX: x86Command += 0x8800; break;
	case x86_EDX: x86Command += 0x9000; break;
	case x86_ESI: x86Command += 0xB000; break;
	case x86_EDI: x86Command += 0xB800; break;
	case x86_ESP: x86Command += 0xA000; break;
	case x86_EBP: x86Command += 0xA800; break;
	default:
		DisplayError("MoveSxN64MemToX86regByte\nInvalid x86 Register");
		break;
	}
	PUTDST8(*code, 0x0f);
	PUTDST16(*code, x86Command);
	PUTDST32(*code, base);
}

void MoveSxN64MemToX86regByte(BYTE** code, int x86reg, int AddrReg) {
	MoveSxBaseMemToX86regByte(code, x86reg, AddrReg, N64MEM, "N64mem");
}

void MoveSxDMemToX86regByte(BYTE** code, int x86reg, int AddrReg) {
	MoveSxBaseMemToX86regByte(code, x86reg, AddrReg, DMEM, "DMEM");
}

static void MoveSxBaseMemToX86regHalf(BYTE** code, int x86reg, int AddrReg, BYTE* base, char* baseName) {
	WORD x86Command = 0x0;

	CPU_OR_RSP_Message(*code, "      movsx %s, word ptr [%s+%s]", x86_Name(x86reg), x86_Name(AddrReg), baseName);

	switch (AddrReg) {
	case x86_EAX: x86Command = 0x00BF; break;
	case x86_EBX: x86Command = 0x03BF; break;
	case x86_ECX: x86Command = 0x01BF; break;
	case x86_EDX: x86Command = 0x02BF; break;
	case x86_ESI: x86Command = 0x06BF; break;
	case x86_EDI: x86Command = 0x07BF; break;
	case x86_ESP: x86Command = 0x04BF; break;
	case x86_EBP: x86Command = 0x05BF; break;
	}
	switch (x86reg) {
	case x86_EAX: x86Command += 0x8000; break;
	case x86_EBX: x86Command += 0x9800; break;
	case x86_ECX: x86Command += 0x8800; break;
	case x86_EDX: x86Command += 0x9000; break;
	case x86_ESI: x86Command += 0xB000; break;
	case x86_EDI: x86Command += 0xB800; break;
	case x86_ESP: x86Command += 0xA000; break;
	case x86_EBP: x86Command += 0xA800; break;
	}

	PUTDST8(*code, 0x0f);
	PUTDST16(*code, x86Command);
	PUTDST32(*code, base);
}

void MoveSxN64MemToX86regHalf(BYTE** code, int x86reg, int AddrReg) {
	MoveSxBaseMemToX86regHalf(code, x86reg, AddrReg, N64MEM, "N64mem");
}

void MoveSxDMemToX86regHalf(BYTE** code, int x86reg, int AddrReg) {
	MoveSxBaseMemToX86regHalf(code, x86reg, AddrReg, DMEM, "DMEM");
}

void MoveSxVariableToX86regByte(BYTE** code, void *Variable, char *VariableName, int x86reg) {
	CPU_OR_RSP_Message(*code, "      movsx %s, byte ptr [%s]",x86_Name(x86reg),VariableName);

	PUTDST16(*code, 0xbe0f);

	switch (x86reg) {
	case x86_EAX: PUTDST8(*code,0x05); break;
	case x86_EBX: PUTDST8(*code,0x1D); break;
	case x86_ECX: PUTDST8(*code,0x0D); break;
	case x86_EDX: PUTDST8(*code,0x15); break;
	case x86_ESI: PUTDST8(*code,0x35); break;
	case x86_EDI: PUTDST8(*code,0x3D); break;
	case x86_ESP: PUTDST8(*code,0x25); break;
	case x86_EBP: PUTDST8(*code,0x2D); break;
	default: DisplayError("MoveSxVariableToX86regHalf\nUnknown x86 Register");
	}
    PUTDST32(*code,Variable);
}

void MoveSxVariableToX86regHalf(BYTE** code, void *Variable, char *VariableName, int x86reg) {
	CPU_OR_RSP_Message(*code, "      movsx %s, word ptr [%s]",x86_Name(x86reg),VariableName);

	PUTDST16(*code, 0xbf0f);

	switch (x86reg) {
	case x86_EAX: PUTDST8(*code,0x05); break;
	case x86_EBX: PUTDST8(*code,0x1D); break;
	case x86_ECX: PUTDST8(*code,0x0D); break;
	case x86_EDX: PUTDST8(*code,0x15); break;
	case x86_ESI: PUTDST8(*code,0x35); break;
	case x86_EDI: PUTDST8(*code,0x3D); break;
	case x86_ESP: PUTDST8(*code,0x25); break;
	case x86_EBP: PUTDST8(*code,0x2D); break;
	default: DisplayError("MoveSxVariableToX86regHalf\nUnknown x86 Register");
	}
    PUTDST32(*code,Variable);
}

void MoveVariableToX86reg(BYTE** code, void *Variable, char *VariableName, int x86reg) {
	CPU_OR_RSP_Message(*code, "      mov %s, dword ptr [%s]",x86_Name(x86reg),VariableName);
	switch (x86reg) {
	case x86_EAX: PUTDST8(*code,0xA1); break;
	case x86_EBX: PUTDST16(*code,0x1D8B); break;
	case x86_ECX: PUTDST16(*code,0x0D8B); break;
	case x86_EDX: PUTDST16(*code,0x158B); break;
	case x86_ESI: PUTDST16(*code,0x358B); break;
	case x86_EDI: PUTDST16(*code,0x3D8B); break;
	case x86_ESP: PUTDST16(*code,0x258B); break;
	case x86_EBP: PUTDST16(*code,0x2D8B); break;
	default: DisplayError("MoveVariableToX86reg\nUnknown x86 Register");
	}
    PUTDST32(*code,Variable);
}

void MoveVariableDispToX86Reg(BYTE** code, void *Variable, char *VariableName, int x86Reg, int AddrReg, int Multiplier) {
	int x = 0x0;
	CPU_OR_RSP_Message(*code, "      mov %s, dword ptr [%s+%s*%i]",x86_Name(x86Reg),VariableName, x86_Name(AddrReg), Multiplier);
	
	PUTDST8(*code,0x8B);

	switch (x86Reg) {
	case x86_EAX: PUTDST8(*code,0x04); break;
	case x86_EBX: PUTDST8(*code,0x1C); break;
	case x86_ECX: PUTDST8(*code,0x0C); break;
	case x86_EDX: PUTDST8(*code,0x14); break;
	case x86_ESI: PUTDST8(*code,0x34); break;
	case x86_EDI: PUTDST8(*code,0x3C); break;
	case x86_ESP: PUTDST8(*code,0x24); break;
	case x86_EBP: PUTDST8(*code,0x2C); break;
	}

	/* put in shifter 2(01), 4(10), 8(11) */
	switch (Multiplier) {
	case 1: x = 0; break;
	case 2: x = 0x40; break;
	case 4: x = 0x80; break;
	case 8: x = 0xC0; break;
	default: DisplayError("Move\nInvalid x86 multiplier");
	}

	/* format xx|000000 */
	switch (AddrReg) {
	case x86_EAX: PUTDST8(*code,0x05|x); break;
	case x86_EBX: PUTDST8(*code,0x1D|x); break;
	case x86_ECX: PUTDST8(*code,0x0D|x); break;
	case x86_EDX: PUTDST8(*code,0x15|x); break;
	case x86_ESI: PUTDST8(*code,0x35|x); break;
	case x86_EDI: PUTDST8(*code,0x3D|x); break;
	case x86_ESP: PUTDST8(*code,0x25|x); break;
	case x86_EBP: PUTDST8(*code,0x2D|x); break;
	}

	PUTDST32(*code,Variable);
}

void MoveVariableToX86regByte(BYTE** code, void *Variable, char *VariableName, int x86reg) {
	CPU_OR_RSP_Message(*code, "      mov %s, byte ptr [%s]",x86Byte_Name(x86reg),VariableName);
	switch (x86reg) {
	case x86_EAX: PUTDST8(*code,0xA0); break;
	case x86_EBX: PUTDST16(*code,0x1D8A); break;
	case x86_ECX: PUTDST16(*code,0x0D8A); break;
	case x86_EDX: PUTDST16(*code,0x158A); break;
	default: DisplayError("MoveVariableToX86regByte\nUnknown x86 Register");
	}
    PUTDST32(*code,Variable);
}

void MoveVariableToX86regHighByte(BYTE** code, void* Variable, char* VariableName, int x86reg) {
	CPU_OR_RSP_Message(*code, "      mov %s, byte ptr [%s]", x86HighByte_Name(x86reg), VariableName);
	switch (x86reg) {
	case x86_EAX: PUTDST16(*code, 0x258A); break;
	case x86_EBX: PUTDST16(*code, 0x3D8A); break;
	case x86_ECX: PUTDST16(*code, 0x2D8A); break;
	case x86_EDX: PUTDST16(*code, 0x358A); break;
	default: DisplayError("MoveVariableToX86regByte\nUnknown x86 Register");
	}
	PUTDST32(*code, Variable);
}

void MoveVariableToX86regHalf(BYTE** code, void *Variable, char *VariableName, int x86reg) {
	CPU_OR_RSP_Message(*code, "      mov %s, word ptr [%s]",x86Half_Name(x86reg),VariableName);
	PUTDST8(*code,0x66);
	switch (x86reg) {
	case x86_EAX: PUTDST16(*code,0x058B); break;
	case x86_EBX: PUTDST16(*code,0x1D8B); break;
	case x86_ECX: PUTDST16(*code,0x0D8B); break;
	case x86_EDX: PUTDST16(*code,0x158B); break;
	case x86_ESI: PUTDST16(*code,0x358B); break;
	case x86_EDI: PUTDST16(*code,0x3D8B); break;
	case x86_ESP: PUTDST16(*code,0x258B); break;
	case x86_EBP: PUTDST16(*code,0x2D8B); break;
	default: DisplayError("MoveVariableToX86reg\nUnknown x86 Register");
	}
    PUTDST32(*code,Variable);
}

static void MoveX86regByteToBaseMem(BYTE** code, int x86reg, int AddrReg, BYTE* base, char* baseName) {
	WORD x86Command = 0x0;

	CPU_OR_RSP_Message(*code, "      mov byte ptr [%s+%s], %s", x86_Name(AddrReg), baseName, x86Byte_Name(x86reg));

	switch (AddrReg) {
	case x86_EAX: x86Command = 0x0088; break;
	case x86_EBX: x86Command = 0x0388; break;
	case x86_ECX: x86Command = 0x0188; break;
	case x86_EDX: x86Command = 0x0288; break;
	case x86_ESI: x86Command = 0x0688; break;
	case x86_EDI: x86Command = 0x0788; break;
	}
	switch (x86reg) {
	case x86_EAX: x86Command += 0x8000; break;
	case x86_EBX: x86Command += 0x9800; break;
	case x86_ECX: x86Command += 0x8800; break;
	case x86_EDX: x86Command += 0x9000; break;
	}
	PUTDST16(*code, x86Command);
	PUTDST32(*code, base);
}

void MoveX86regByteToN64Mem(BYTE** code, int x86reg, int AddrReg) {
	MoveX86regByteToBaseMem(code, x86reg, AddrReg, N64MEM, "N64MEM");
}

void MoveX86regByteToDMem(BYTE** code, int x86reg, int AddrReg) {
	MoveX86regByteToBaseMem(code, x86reg, AddrReg, DMEM, "DMEM");
}

void MoveX86regHighByteToDMem(BYTE** code, int x86reg, int AddrReg) {
	WORD x86Command = 0x0;

	CPU_OR_RSP_Message(*code, "      mov byte ptr [%s+DMEM], %s", x86_Name(AddrReg), x86HighByte_Name(x86reg));

	switch (AddrReg) {
	case x86_EAX: x86Command = 0x0088; break;
	case x86_EBX: x86Command = 0x0388; break;
	case x86_ECX: x86Command = 0x0188; break;
	case x86_EDX: x86Command = 0x0288; break;
	case x86_ESI: x86Command = 0x0688; break;
	case x86_EDI: x86Command = 0x0788; break;
	}
	switch (x86reg) {
	case x86_EAX: x86Command += 0xa000; break;
	case x86_EBX: x86Command += 0xb800; break;
	case x86_ECX: x86Command += 0xa800; break;
	case x86_EDX: x86Command += 0xb000; break;
	}
	PUTDST16(*code, x86Command);
	PUTDST32(*code, DMEM);
}


void MoveX86regByteToVariable(BYTE** code, int x86reg, void * Variable, char * VariableName) {
	CPU_OR_RSP_Message(*code, "      mov byte ptr [%s], %s",VariableName,x86Byte_Name(x86reg));
	switch (x86reg) {
	case x86_EAX: PUTDST8(*code,0xA2); break;
	case x86_EBX: PUTDST16(*code,0x1D88); break;
	case x86_ECX: PUTDST16(*code,0x0D88); break;
	case x86_EDX: PUTDST16(*code,0x1588); break;
	default:
		DisplayError("MoveX86regByteToVariable\nUnknown x86 Register");
	}
    PUTDST32(*code,Variable);
}

void MoveX86regHighByteToVariable(BYTE** code, int x86reg, void* Variable, char* VariableName) {
	CPU_OR_RSP_Message(*code, "      mov byte ptr [%s], %s", VariableName, x86HighByte_Name(x86reg));
	switch (x86reg) {
	case x86_EAX: PUTDST16(*code, 0x2588); break;
	case x86_EBX: PUTDST16(*code, 0x3D88); break;
	case x86_ECX: PUTDST16(*code, 0x2D88); break;
	case x86_EDX: PUTDST16(*code, 0x3588); break;
	default:
		DisplayError("MoveX86regByteToVariable\nUnknown x86 Register");
	}
	PUTDST32(*code, Variable);
}

void MoveX86regByteToX86regPointer(BYTE** code, int x86reg, int AddrReg1, int AddrReg2) {
	BYTE Param = 0x0;

	CPU_OR_RSP_Message(*code, "      mov byte ptr [%s+%s],%s",x86_Name(AddrReg1), x86_Name(AddrReg2), x86Byte_Name(x86reg));

	switch (x86reg) {
	case x86_EAX: PUTDST16(*code,0x0488); break;
	case x86_EBX: PUTDST16(*code,0x1C88); break;
	case x86_ECX: PUTDST16(*code,0x0C88); break;
	case x86_EDX: PUTDST16(*code,0x1488); break;
	case x86_ESI: PUTDST16(*code,0x3488); break;
	case x86_EDI: PUTDST16(*code,0x3C88); break;
	case x86_ESP: PUTDST16(*code,0x2488); break;
	case x86_EBP: PUTDST16(*code,0x2C88); break;
	default:
		DisplayError("MoveX86regToX86regPointer\nUnhandled x86 Register");
	}

	switch (AddrReg1) {
	case x86_EAX: Param = 0x00; break;
	case x86_EBX: Param = 0x03; break;
	case x86_ECX: Param = 0x01; break;
	case x86_EDX: Param = 0x02; break;
	case x86_ESI: Param = 0x06; break;
	case x86_EDI: Param = 0x07; break;
	default:
		DisplayError("MoveX86regToX86regPointer\nUnhandled x86 Register");
	}

	switch (AddrReg2) {
	case x86_EAX: Param += 0x00; break;
	case x86_EBX: Param += 0x18; break;
	case x86_ECX: Param += 0x08; break;
	case x86_EDX: Param += 0x10; break;
	case x86_ESI: Param += 0x30; break;
	case x86_EDI: Param += 0x38; break;
	case x86_ESP: Param += 0x20; break;
	case x86_EBP: Param += 0x28; break;
	default:
		DisplayError("MoveX86regByteToX86regPointer\nUnhandled x86 Register");
	}
	PUTDST8(*code,Param);
}

static void MoveX86regHalfToBaseMem(BYTE** code, int x86reg, int AddrReg, BYTE* base, char* baseName) {
	WORD x86Command = 0x0;

	CPU_OR_RSP_Message(*code, "      mov word ptr [%s+%s], %s", x86_Name(AddrReg), baseName, x86Half_Name(x86reg)); \
	PUTDST8(*code, 0x66);
	switch (AddrReg) {
	case x86_EAX: x86Command = 0x0089; break;
	case x86_EBX: x86Command = 0x0389; break;
	case x86_ECX: x86Command = 0x0189; break;
	case x86_EDX: x86Command = 0x0289; break;
	case x86_ESI: x86Command = 0x0689; break;
	case x86_EDI: x86Command = 0x0789; break;
	case x86_ESP: x86Command = 0x0489; break;
	case x86_EBP: x86Command = 0x0589; break;
	}
	switch (x86reg) {
	case x86_EAX: x86Command += 0x8000; break;
	case x86_EBX: x86Command += 0x9800; break;
	case x86_ECX: x86Command += 0x8800; break;
	case x86_EDX: x86Command += 0x9000; break;
	case x86_ESI: x86Command += 0xB000; break;
	case x86_EDI: x86Command += 0xB800; break;
	case x86_ESP: x86Command += 0xA000; break;
	case x86_EBP: x86Command += 0xA800; break;
	}
	PUTDST16(*code, x86Command);
	PUTDST32(*code, base);
}

void MoveX86regHalfToN64Mem(BYTE** code, int x86reg, int AddrReg) {
	MoveX86regHalfToBaseMem(code, x86reg, AddrReg, N64MEM, "N64MEM");
}

void MoveX86regHalfToDMem(BYTE** code, int x86reg, int AddrReg) {
	MoveX86regHalfToBaseMem(code, x86reg, AddrReg, DMEM, "DMEM");
}

void MoveX86regHalfToVariable(BYTE** code, int x86reg, void * Variable, char * VariableName) {
	CPU_OR_RSP_Message(*code, "      mov word ptr [%s], %s",VariableName,x86Half_Name(x86reg));
	PUTDST8(*code,0x66);
	switch (x86reg) {
	case x86_EAX: PUTDST8(*code,0xA3); break;
	case x86_EBX: PUTDST16(*code,0x1D89); break;
	case x86_ECX: PUTDST16(*code,0x0D89); break;
	case x86_EDX: PUTDST16(*code,0x1589); break;
	case x86_ESI: PUTDST16(*code,0x3589); break;
	case x86_EDI: PUTDST16(*code,0x3D89); break;
	case x86_ESP: PUTDST16(*code,0x2589); break;
	case x86_EBP: PUTDST16(*code,0x2D89); break;
	default:
		DisplayError("MoveX86regToVariable\nUnknown x86 Register");
	}
    PUTDST32(*code,Variable);
}

void MoveX86regHalfToX86regPointer(BYTE** code, int x86reg, int AddrReg1, int AddrReg2) {
	BYTE Param = 0x0;

	CPU_OR_RSP_Message(*code, "      mov word ptr [%s+%s],%s",x86_Name(AddrReg1), x86_Name(AddrReg2), x86Half_Name(x86reg));

	PUTDST8(*code,0x66);
	switch (x86reg) {
	case x86_EAX: PUTDST16(*code,0x0489); break;
	case x86_EBX: PUTDST16(*code,0x1C89); break;
	case x86_ECX: PUTDST16(*code,0x0C89); break;
	case x86_EDX: PUTDST16(*code,0x1489); break;
	case x86_ESI: PUTDST16(*code,0x3489); break;
	case x86_EDI: PUTDST16(*code,0x3C89); break;
	case x86_ESP: PUTDST16(*code,0x2489); break;
	case x86_EBP: PUTDST16(*code,0x2C89); break;
	default:
		DisplayError("MoveX86regHalfToX86regPointer\nUnhandled x86 Register");
	}

	switch (AddrReg1) {
	case x86_EAX: Param = 0x00; break;
	case x86_EBX: Param = 0x03; break;
	case x86_ECX: Param = 0x01; break;
	case x86_EDX: Param = 0x02; break;
	case x86_ESI: Param = 0x06; break;
	case x86_EDI: Param = 0x07; break;
	default:
		DisplayError("MoveX86regHalfToX86regPointer\nUnhandled x86 Register");
	}

	switch (AddrReg2) {
	case x86_EAX: Param += 0x00; break;
	case x86_EBX: Param += 0x18; break;
	case x86_ECX: Param += 0x08; break;
	case x86_EDX: Param += 0x10; break;
	case x86_ESI: Param += 0x30; break;
	case x86_EDI: Param += 0x38; break;
	case x86_ESP: Param += 0x20; break;
	case x86_EBP: Param += 0x28; break;
	default:
		DisplayError("MoveX86regHalfToX86regPointer\nUnhandled x86 Register");
	}
	PUTDST8(*code,Param);
}

void MoveX86PointerToX86reg(BYTE** code, int x86reg, int X86Pointer) {
	WORD x86Command = 0x0;

	CPU_OR_RSP_Message(*code, "      mov %s, dword ptr [%s]",x86_Name(x86reg),x86_Name(X86Pointer));

	switch (X86Pointer) {
	case x86_EAX: x86Command = 0x008B; break;
	case x86_EBX: x86Command = 0x038B; break;
	case x86_ECX: x86Command = 0x018B; break;
	case x86_EDX: x86Command = 0x028B; break;
	case x86_ESI: x86Command = 0x068B; break;
	case x86_EDI: x86Command = 0x078B; break;
	}
	
	switch (x86reg) {
	case x86_EAX: x86Command += 0x0000; break;
	case x86_EBX: x86Command += 0x1800; break;
	case x86_ECX: x86Command += 0x0800; break;
	case x86_EDX: x86Command += 0x1000; break;
	case x86_ESI: x86Command += 0x3000; break;
	case x86_EDI: x86Command += 0x3800; break;
	case x86_ESP: x86Command += 0x2000; break;
	case x86_EBP: x86Command += 0x2800; break;
	}
	PUTDST16(*code,x86Command);
}

void MoveX86regPointerToX86reg(BYTE** code, int AddrReg1, int AddrReg2, int x86reg) {
	BYTE Param = 0x0;

	CPU_OR_RSP_Message(*code, "      mov %s, dword ptr [%s+%s]",x86_Name(x86reg),x86_Name(AddrReg1), x86_Name(AddrReg2));

	switch (x86reg) {
	case x86_EAX: PUTDST16(*code,0x048B); break;
	case x86_EBX: PUTDST16(*code,0x1C8B); break;
	case x86_ECX: PUTDST16(*code,0x0C8B); break;
	case x86_EDX: PUTDST16(*code,0x148B); break;
	case x86_ESI: PUTDST16(*code,0x348B); break;
	case x86_EDI: PUTDST16(*code,0x3C8B); break;
	case x86_ESP: PUTDST16(*code,0x248B); break;
	case x86_EBP: PUTDST16(*code,0x2C8B); break;
	default:
		DisplayError("MoveX86regPointerToX86reg\nUnhandled x86 Register");
	}

	switch (AddrReg1) {
	case x86_EAX: Param = 0x00; break;
	case x86_EBX: Param = 0x03; break;
	case x86_ECX: Param = 0x01; break;
	case x86_EDX: Param = 0x02; break;
	case x86_ESI: Param = 0x06; break;
	case x86_EDI: Param = 0x07; break;
	default:
		DisplayError("MoveX86regPointerToX86reg\nUnhandled x86 Register");
	}

	switch (AddrReg2) {
	case x86_EAX: Param += 0x00; break;
	case x86_EBX: Param += 0x18; break;
	case x86_ECX: Param += 0x08; break;
	case x86_EDX: Param += 0x10; break;
	case x86_ESI: Param += 0x30; break;
	case x86_EDI: Param += 0x38; break;
	case x86_ESP: Param += 0x20; break;
	case x86_EBP: Param += 0x28; break;
	default:
		DisplayError("MoveX86regPointerToX86reg\nUnhandled x86 Register");
	}
	PUTDST8(*code,Param);
}

void MoveX86regPointerToX86regDisp8(BYTE** code, int AddrReg1, int AddrReg2, int x86reg, BYTE offset) {
	BYTE Param = 0x0;

	CPU_OR_RSP_Message(*code, "      mov %s, dword ptr [%s+%s]",x86_Name(x86reg),x86_Name(AddrReg1), x86_Name(AddrReg2));

	switch (x86reg) {
	case x86_EAX: PUTDST16(*code,0x448B); break;
	case x86_EBX: PUTDST16(*code,0x5C8B); break;
	case x86_ECX: PUTDST16(*code,0x4C8B); break;
	case x86_EDX: PUTDST16(*code,0x548B); break;
	case x86_ESI: PUTDST16(*code,0x748B); break;
	case x86_EDI: PUTDST16(*code,0x7C8B); break;
	case x86_ESP: PUTDST16(*code,0x648B); break;
	case x86_EBP: PUTDST16(*code,0x6C8B); break;
	default:
		DisplayError("MoveX86regPointerToX86reg\nUnhandled x86 Register");
	}

	switch (AddrReg1) {
	case x86_EAX: Param = 0x00; break;
	case x86_EBX: Param = 0x03; break;
	case x86_ECX: Param = 0x01; break;
	case x86_EDX: Param = 0x02; break;
	case x86_ESI: Param = 0x06; break;
	case x86_EDI: Param = 0x07; break;
	default:
		DisplayError("MoveX86regPointerToX86reg\nUnhandled x86 Register");
	}

	switch (AddrReg2) {
	case x86_EAX: Param += 0x00; break;
	case x86_EBX: Param += 0x18; break;
	case x86_ECX: Param += 0x08; break;
	case x86_EDX: Param += 0x10; break;
	case x86_ESI: Param += 0x30; break;
	case x86_EDI: Param += 0x38; break;
	case x86_ESP: Param += 0x20; break;
	case x86_EBP: Param += 0x28; break;
	default:
		DisplayError("MoveX86regPointerToX86reg\nUnhandled x86 Register");
	}
	PUTDST8(*code,Param);
	PUTDST8(*code,offset);
}

void MoveX86regToMemory(BYTE** code, int x86reg, int AddrReg, DWORD Disp) {
	WORD x86Command = 0x0;

	CPU_OR_RSP_Message(*code, "      mov dword ptr [%s+%X], %s",x86_Name(AddrReg),Disp,x86_Name(x86reg));
	switch (AddrReg) {
	case x86_EAX: x86Command = 0x0089; break;
	case x86_EBX: x86Command = 0x0389; break;
	case x86_ECX: x86Command = 0x0189; break;
	case x86_EDX: x86Command = 0x0289; break;
	case x86_ESI: x86Command = 0x0689; break;
	case x86_EDI: x86Command = 0x0789; break;
	case x86_ESP: x86Command = 0x0489; break;
	case x86_EBP: x86Command = 0x0589; break;
	}
	switch (x86reg) {
	case x86_EAX: x86Command += 0x8000; break;
	case x86_EBX: x86Command += 0x9800; break;
	case x86_ECX: x86Command += 0x8800; break;
	case x86_EDX: x86Command += 0x9000; break;
	case x86_ESI: x86Command += 0xB000; break;
	case x86_EDI: x86Command += 0xB800; break;
	case x86_ESP: x86Command += 0xA000; break;
	case x86_EBP: x86Command += 0xA800; break;
	}
	PUTDST16(*code,x86Command);
	PUTDST32(*code,Disp);
}

static void MoveX86regToBaseMem(BYTE** code, int x86reg, int AddrReg, BYTE* base, char* baseName) {
	WORD x86Command = 0x0;

	CPU_OR_RSP_Message(*code, "      mov dword ptr [%s+%s], %s", x86_Name(AddrReg), baseName, x86_Name(x86reg)); \
		switch (AddrReg) {
		case x86_EAX: x86Command = 0x0089; break;
		case x86_EBX: x86Command = 0x0389; break;
		case x86_ECX: x86Command = 0x0189; break;
		case x86_EDX: x86Command = 0x0289; break;
		case x86_ESI: x86Command = 0x0689; break;
		case x86_EDI: x86Command = 0x0789; break;
		case x86_ESP: x86Command = 0x0489; break;
		case x86_EBP: x86Command = 0x0589; break;
		}
	switch (x86reg) {
	case x86_EAX: x86Command += 0x8000; break;
	case x86_EBX: x86Command += 0x9800; break;
	case x86_ECX: x86Command += 0x8800; break;
	case x86_EDX: x86Command += 0x9000; break;
	case x86_ESI: x86Command += 0xB000; break;
	case x86_EDI: x86Command += 0xB800; break;
	case x86_ESP: x86Command += 0xA000; break;
	case x86_EBP: x86Command += 0xA800; break;
	}
	PUTDST16(*code, x86Command);
	PUTDST32(*code, base);
}

void MoveX86regToN64Mem(BYTE** code, int x86reg, int AddrReg) {
	MoveX86regToBaseMem(code, x86reg, AddrReg, N64MEM, "N64MEM");
}

void MoveX86regToDMem(BYTE** code, int x86reg, int AddrReg) {
	MoveX86regToBaseMem(code, x86reg, AddrReg, DMEM, "DMEM");
}

void MoveX86regToN64MemDisp(BYTE** code, int x86reg, int AddrReg, BYTE Disp) {
	WORD x86Command = 0x0;

	CPU_OR_RSP_Message(*code, "      mov dword ptr [%s+N64mem+%d], %s",x86_Name(AddrReg),Disp,x86_Name(x86reg));\
	switch (AddrReg) {
	case x86_EAX: x86Command = 0x0089; break;
	case x86_EBX: x86Command = 0x0389; break;
	case x86_ECX: x86Command = 0x0189; break;
	case x86_EDX: x86Command = 0x0289; break;
	case x86_ESI: x86Command = 0x0689; break;
	case x86_EDI: x86Command = 0x0789; break;
	case x86_ESP: x86Command = 0x0489; break;
	case x86_EBP: x86Command = 0x0589; break;
	}
	switch (x86reg) {
	case x86_EAX: x86Command += 0x8000; break;
	case x86_EBX: x86Command += 0x9800; break;
	case x86_ECX: x86Command += 0x8800; break;
	case x86_EDX: x86Command += 0x9000; break;
	case x86_ESI: x86Command += 0xB000; break;
	case x86_EDI: x86Command += 0xB800; break;
	case x86_ESP: x86Command += 0xA000; break;
	case x86_EBP: x86Command += 0xA800; break;
	}
	PUTDST16(*code,x86Command);
	PUTDST32(*code,N64MEM+Disp);
}

void MoveX86regToVariable(BYTE** code, int x86reg, void * Variable, char * VariableName) {
	CPU_OR_RSP_Message(*code, "      mov dword ptr [%s], %s",VariableName,x86_Name(x86reg));
	switch (x86reg) {
	case x86_EAX: PUTDST8(*code,0xa3); break;
	case x86_EBX: PUTDST16(*code,0x1D89); break;
	case x86_ECX: PUTDST16(*code,0x0D89); break;
	case x86_EDX: PUTDST16(*code,0x1589); break;
	case x86_ESI: PUTDST16(*code,0x3589); break;
	case x86_EDI: PUTDST16(*code,0x3D89); break;
	case x86_ESP: PUTDST16(*code,0x2589); break;
	case x86_EBP: PUTDST16(*code,0x2D89); break;
	default:
		DisplayError("MoveX86regToVariable\nUnknown x86 Register");
	}
    PUTDST32(*code,Variable);
}

void MoveX86RegToX86Reg(BYTE** code, int Source, int Destination) {
	WORD x86Command = 0x0;
	
	CPU_OR_RSP_Message(*code, "      mov %s, %s",x86_Name(Destination),x86_Name(Source));

	switch (Destination) {\
	case x86_EAX: x86Command = 0x0089; break;
	case x86_EBX: x86Command = 0x0389; break;
	case x86_ECX: x86Command = 0x0189; break;
	case x86_EDX: x86Command = 0x0289; break;
	case x86_ESI: x86Command = 0x0689; break;
	case x86_EDI: x86Command = 0x0789; break;
	case x86_ESP: x86Command = 0x0489; break;
	case x86_EBP: x86Command = 0x0589; break;
	}
	
	switch (Source) {
	case x86_EAX: x86Command += 0xC000; break;
	case x86_EBX: x86Command += 0xD800; break;
	case x86_ECX: x86Command += 0xC800; break;
	case x86_EDX: x86Command += 0xD000; break;
	case x86_ESI: x86Command += 0xF000; break;
	case x86_EDI: x86Command += 0xF800; break;
	case x86_ESP: x86Command += 0xE000; break;
	case x86_EBP: x86Command += 0xE800; break;
	}
	PUTDST16(*code,x86Command);
}

void MoveX86regToX86Pointer(BYTE** code, int x86reg, int X86Pointer) {
	WORD x86Command = 0x0;

	CPU_OR_RSP_Message(*code, "      mov dword ptr [%s], %s",x86_Name(X86Pointer),x86_Name(x86reg));

	switch (X86Pointer) {
	case x86_EAX: x86Command = 0x0089; break;
	case x86_EBX: x86Command = 0x0389; break;
	case x86_ECX: x86Command = 0x0189; break;
	case x86_EDX: x86Command = 0x0289; break;
	case x86_ESI: x86Command = 0x0689; break;
	case x86_EDI: x86Command = 0x0789; break;
	}
	
	switch (x86reg) {
	case x86_EAX: x86Command += 0x0000; break;
	case x86_EBX: x86Command += 0x1800; break;
	case x86_ECX: x86Command += 0x0800; break;
	case x86_EDX: x86Command += 0x1000; break;
	case x86_ESI: x86Command += 0x3000; break;
	case x86_EDI: x86Command += 0x3800; break;
	case x86_ESP: x86Command += 0x2000; break;
	case x86_EBP: x86Command += 0x2800; break;
	}
	PUTDST16(*code,x86Command);
}

void MoveX86regToX86regPointer(BYTE** code, int x86reg, int AddrReg1, int AddrReg2) {
	BYTE Param = 0x0;

	CPU_OR_RSP_Message(*code, "      mov dword ptr [%s+%s],%s",x86_Name(AddrReg1), x86_Name(AddrReg2), x86_Name(x86reg));

	switch (x86reg) {
	case x86_EAX: PUTDST16(*code,0x0489); break;
	case x86_EBX: PUTDST16(*code,0x1C89); break;
	case x86_ECX: PUTDST16(*code,0x0C89); break;
	case x86_EDX: PUTDST16(*code,0x1489); break;
	case x86_ESI: PUTDST16(*code,0x3489); break;
	case x86_EDI: PUTDST16(*code,0x3C89); break;
	case x86_ESP: PUTDST16(*code,0x2489); break;
	case x86_EBP: PUTDST16(*code,0x2C89); break;
	default:
		DisplayError("MoveX86regToX86regPointer\nUnhandled x86 Register");
	}

	switch (AddrReg1) {
	case x86_EAX: Param = 0x00; break;
	case x86_EBX: Param = 0x03; break;
	case x86_ECX: Param = 0x01; break;
	case x86_EDX: Param = 0x02; break;
	case x86_ESI: Param = 0x06; break;
	case x86_EDI: Param = 0x07; break;
	default:
		DisplayError("MoveX86regToX86regPointer\nUnhandled x86 Register");
	}

	switch (AddrReg2) {
	case x86_EAX: Param += 0x00; break;
	case x86_EBX: Param += 0x18; break;
	case x86_ECX: Param += 0x08; break;
	case x86_EDX: Param += 0x10; break;
	case x86_ESI: Param += 0x30; break;
	case x86_EDI: Param += 0x38; break;
	case x86_ESP: Param += 0x20; break;
	case x86_EBP: Param += 0x28; break;
	default:
		DisplayError("MoveX86regToX86regPointer\nUnhandled x86 Register");
	}
	PUTDST8(*code,Param);
}

void MoveZxByteX86regPointerToX86reg(BYTE** code, int AddrReg1, int AddrReg2, int x86reg) {
	BYTE Param = 0x0;

	CPU_OR_RSP_Message(*code, "      movzx %s, byte ptr [%s+%s]",x86_Name(x86reg),x86_Name(AddrReg1), x86_Name(AddrReg2));

	PUTDST16(*code,0xB60F);
	switch (x86reg) {
	case x86_EAX: PUTDST8(*code,0x04); break;
	case x86_EBX: PUTDST8(*code,0x1C); break;
	case x86_ECX: PUTDST8(*code,0x0C); break;
	case x86_EDX: PUTDST8(*code,0x14); break;
	case x86_ESI: PUTDST8(*code,0x34); break;
	case x86_EDI: PUTDST8(*code,0x3C); break;
	case x86_ESP: PUTDST8(*code,0x24); break;
	case x86_EBP: PUTDST8(*code,0x2C); break;
	default:
		DisplayError("MoveZxByteX86regPointerToX86reg\nUnhandled x86 Register");
	}

	switch (AddrReg1) {
	case x86_EAX: Param = 0x00; break;
	case x86_EBX: Param = 0x03; break;
	case x86_ECX: Param = 0x01; break;
	case x86_EDX: Param = 0x02; break;
	case x86_ESI: Param = 0x06; break;
	case x86_EDI: Param = 0x07; break;
	default:
		DisplayError("MoveZxByteX86regPointerToX86reg\nUnhandled x86 Register");
	}

	switch (AddrReg2) {
	case x86_EAX: Param += 0x00; break;
	case x86_EBX: Param += 0x18; break;
	case x86_ECX: Param += 0x08; break;
	case x86_EDX: Param += 0x10; break;
	case x86_ESI: Param += 0x30; break;
	case x86_EDI: Param += 0x38; break;
	case x86_ESP: Param += 0x20; break;
	case x86_EBP: Param += 0x28; break;
	default:
		DisplayError("MoveZxByteX86regPointerToX86reg\nUnhandled x86 Register");
	}
	PUTDST8(*code,Param);
}

void MoveZxHalfX86regPointerToX86reg(BYTE** code, int AddrReg1, int AddrReg2, int x86reg) {
	BYTE Param = 0x0;

	CPU_OR_RSP_Message(*code, "      movzx %s, word ptr [%s+%s]",x86_Name(x86reg),x86_Name(AddrReg1), x86_Name(AddrReg2));

	PUTDST16(*code,0xB70F);
	switch (x86reg) {
	case x86_EAX: PUTDST8(*code,0x04); break;
	case x86_EBX: PUTDST8(*code,0x1C); break;
	case x86_ECX: PUTDST8(*code,0x0C); break;
	case x86_EDX: PUTDST8(*code,0x14); break;
	case x86_ESI: PUTDST8(*code,0x34); break;
	case x86_EDI: PUTDST8(*code,0x3C); break;
	case x86_ESP: PUTDST8(*code,0x24); break;
	case x86_EBP: PUTDST8(*code,0x2C); break;
	default:
		DisplayError("MoveZxHalfX86regPointerToX86reg\nUnhandled x86 Register");
	}

	switch (AddrReg1) {
	case x86_EAX: Param = 0x00; break;
	case x86_EBX: Param = 0x03; break;
	case x86_ECX: Param = 0x01; break;
	case x86_EDX: Param = 0x02; break;
	case x86_ESI: Param = 0x06; break;
	case x86_EDI: Param = 0x07; break;
	default:
		DisplayError("MoveZxHalfX86regPointerToX86reg\nUnhandled x86 Register");
	}

	switch (AddrReg2) {
	case x86_EAX: Param += 0x00; break;
	case x86_EBX: Param += 0x18; break;
	case x86_ECX: Param += 0x08; break;
	case x86_EDX: Param += 0x10; break;
	case x86_ESI: Param += 0x30; break;
	case x86_EDI: Param += 0x38; break;
	case x86_ESP: Param += 0x20; break;
	case x86_EBP: Param += 0x28; break;
	default:
		DisplayError("MoveZxHalfX86regPointerToX86reg\nUnhandled x86 Register");
	}
	PUTDST8(*code,Param);
}

static void MoveZxBaseMemToX86regByte(BYTE** code, int x86reg, int AddrReg, BYTE* base, char* baseName) {
	WORD x86Command = 0x0;

	CPU_OR_RSP_Message(*code, "      movzx %s, byte ptr [%s+%s]", x86_Name(x86reg), x86_Name(AddrReg), baseName);
	switch (AddrReg) {
	case x86_EAX: x86Command = 0x00B6; break;
	case x86_EBX: x86Command = 0x03B6; break;
	case x86_ECX: x86Command = 0x01B6; break;
	case x86_EDX: x86Command = 0x02B6; break;
	case x86_ESI: x86Command = 0x06B6; break;
	case x86_EDI: x86Command = 0x07B6; break;
	case x86_ESP: x86Command = 0x04B6; break;
	case x86_EBP: x86Command = 0x05B6; break;
	}
	switch (x86reg) {
	case x86_EAX: x86Command += 0x8000; break;
	case x86_EBX: x86Command += 0x9800; break;
	case x86_ECX: x86Command += 0x8800; break;
	case x86_EDX: x86Command += 0x9000; break;
	case x86_ESI: x86Command += 0xB000; break;
	case x86_EDI: x86Command += 0xB800; break;
	case x86_ESP: x86Command += 0xA000; break;
	case x86_EBP: x86Command += 0xA800; break;
	default:
		DisplayError("MoveZxN64MemToX86regByte\nInvalid x86 Register");
		break;
	}
	PUTDST8(*code, 0x0f);
	PUTDST16(*code, x86Command);
	PUTDST32(*code, base);
}

void MoveZxN64MemToX86regByte(BYTE** code, int x86reg, int AddrReg) {
	MoveZxBaseMemToX86regByte(code, x86reg, AddrReg, N64MEM, "N64MEM");
}

void MoveZxDMemToX86regByte(BYTE** code, int x86reg, int AddrReg) {
	MoveZxBaseMemToX86regByte(code, x86reg, AddrReg, DMEM, "DMEM");
}

static void MoveZxBaseMemToX86regHalf(BYTE** code, int x86reg, int AddrReg, BYTE* base, char* baseName) {
	WORD x86Command = 0x0;

	CPU_OR_RSP_Message(*code, "      movzx %s, word ptr [%s+%s]", x86_Name(x86reg), x86_Name(AddrReg), baseName);

	switch (AddrReg) {
	case x86_EAX: x86Command = 0x00B7; break;
	case x86_EBX: x86Command = 0x03B7; break;
	case x86_ECX: x86Command = 0x01B7; break;
	case x86_EDX: x86Command = 0x02B7; break;
	case x86_ESI: x86Command = 0x06B7; break;
	case x86_EDI: x86Command = 0x07B7; break;
	case x86_ESP: x86Command = 0x04B7; break;
	case x86_EBP: x86Command = 0x05B7; break;
	}
	switch (x86reg) {
	case x86_EAX: x86Command += 0x8000; break;
	case x86_EBX: x86Command += 0x9800; break;
	case x86_ECX: x86Command += 0x8800; break;
	case x86_EDX: x86Command += 0x9000; break;
	case x86_ESI: x86Command += 0xB000; break;
	case x86_EDI: x86Command += 0xB800; break;
	case x86_ESP: x86Command += 0xA000; break;
	case x86_EBP: x86Command += 0xA800; break;
	}

	PUTDST8(*code, 0x0f);
	PUTDST16(*code, x86Command);
	PUTDST32(*code, base);
}

void MoveZxN64MemToX86regHalf(BYTE** code, int x86reg, int AddrReg) {
	MoveZxBaseMemToX86regHalf(code, x86reg, AddrReg, N64MEM, "N64MEM");
}

void MoveZxDMemToX86regHalf(BYTE** code, int x86reg, int AddrReg) {
	MoveZxBaseMemToX86regHalf(code, x86reg, AddrReg, DMEM, "DMEM");
}

void MoveZxVariableToX86regByte(BYTE** code, void *Variable, char *VariableName, int x86reg) {
	CPU_OR_RSP_Message(*code, "      movzx %s, byte ptr [%s]",x86_Name(x86reg),VariableName);

	PUTDST16(*code, 0xb60f);

	switch (x86reg) {
	case x86_EAX: PUTDST8(*code,0x05); break;
	case x86_EBX: PUTDST8(*code,0x1D); break;
	case x86_ECX: PUTDST8(*code,0x0D); break;
	case x86_EDX: PUTDST8(*code,0x15); break;
	case x86_ESI: PUTDST8(*code,0x35); break;
	case x86_EDI: PUTDST8(*code,0x3D); break;
	case x86_ESP: PUTDST8(*code,0x25); break;
	case x86_EBP: PUTDST8(*code,0x2D); break;
	default: DisplayError("MoveZxVariableToX86regHalf\nUnknown x86 Register");
	}
    PUTDST32(*code,Variable);
}

void MoveZxVariableToX86regHalf(BYTE** code, void *Variable, char *VariableName, int x86reg) {
	CPU_OR_RSP_Message(*code, "      movzx %s, word ptr [%s]",x86_Name(x86reg),VariableName);

	PUTDST16(*code, 0xb70f);

	switch (x86reg) {
	case x86_EAX: PUTDST8(*code,0x05); break;
	case x86_EBX: PUTDST8(*code,0x1D); break;
	case x86_ECX: PUTDST8(*code,0x0D); break;
	case x86_EDX: PUTDST8(*code,0x15); break;
	case x86_ESI: PUTDST8(*code,0x35); break;
	case x86_EDI: PUTDST8(*code,0x3D); break;
	case x86_ESP: PUTDST8(*code,0x25); break;
	case x86_EBP: PUTDST8(*code,0x2D); break;
	default: DisplayError("MoveZxVariableToX86regHalf\nUnknown x86 Register");
	}
    PUTDST32(*code,Variable);
}

void MulX86reg(BYTE** code, int x86reg) {
	CPU_OR_RSP_Message(*code, "      mul %s",x86_Name(x86reg));\
	switch (x86reg) {
	case x86_EAX: PUTDST16(*code,0xE0F7); break;
	case x86_EBX: PUTDST16(*code,0xE3F7); break;
	case x86_ECX: PUTDST16(*code,0xE1F7); break;
	case x86_EDX: PUTDST16(*code,0xE2F7); break;
	case x86_ESI: PUTDST16(*code,0xE6F7); break;
	case x86_EDI: PUTDST16(*code,0xE7F7); break;
	case x86_ESP: PUTDST16(*code,0xE4F7); break;
	case x86_EBP: PUTDST16(*code,0xE5F7); break;
	default:
		DisplayError("MulX86reg\nUnknown x86 Register");
	}
}

void NotX86Reg(BYTE** code, int  x86Reg) {
	CPU_OR_RSP_Message(*code, "      not %s",x86_Name(x86Reg));
	switch (x86Reg) {
	case x86_EAX: PUTDST16(*code,0xD0F7); break;
	case x86_EBX: PUTDST16(*code,0xD3F7); break;
	case x86_ECX: PUTDST16(*code,0xD1F7); break;
	case x86_EDX: PUTDST16(*code,0xD2F7); break;
	case x86_ESI: PUTDST16(*code,0xD6F7); break;
	case x86_EDI: PUTDST16(*code,0xD7F7); break;
	case x86_ESP: PUTDST16(*code,0xD4F7); break;
	case x86_EBP: PUTDST16(*code,0xD5F7); break;
	}
}

void OrConstToVariable(BYTE** code, DWORD Const, void * Variable, char * VariableName) {
	CPU_OR_RSP_Message(*code, "      or dword ptr [%s], 0x%X",VariableName, Const);
	if ((Const & 0xFFFFFF80) != 0 && (Const & 0xFFFFFF80) != 0xFFFFFF80) {
		PUTDST16(*code, 0x0D81);
		PUTDST32(*code, Variable);
		PUTDST32(*code, Const);
	} else {
		PUTDST16(*code, 0x0D83);
		PUTDST32(*code, Variable);
		PUTDST8(*code, Const);
	}
}

void OrConstToX86Reg(BYTE** code, DWORD Const, int  x86Reg) {
	CPU_OR_RSP_Message(*code, "      or %s, %Xh",x86_Name(x86Reg),Const);
	if ((Const & 0xFFFFFF80) != 0 && (Const & 0xFFFFFF80) != 0xFFFFFF80) {
		switch (x86Reg) {
		case x86_EAX: PUTDST8(*code,0x0D); break;
		case x86_EBX: PUTDST16(*code,0xCB81); break;
		case x86_ECX: PUTDST16(*code,0xC981); break;
		case x86_EDX: PUTDST16(*code,0xCA81); break;
		case x86_ESI: PUTDST16(*code,0xCE81); break;
		case x86_EDI: PUTDST16(*code,0xCF81); break;
		case x86_ESP: PUTDST16(*code,0xCC81); break;
		case x86_EBP: PUTDST16(*code,0xCD81); break;
		}
		PUTDST32(*code, Const);
	} else {
		switch (x86Reg) {
		case x86_EAX: PUTDST16(*code,0xC883); break;
		case x86_EBX: PUTDST16(*code,0xCB83); break;
		case x86_ECX: PUTDST16(*code,0xC983); break;
		case x86_EDX: PUTDST16(*code,0xCA83); break;
		case x86_ESI: PUTDST16(*code,0xCE83); break;
		case x86_EDI: PUTDST16(*code,0xCF83); break;
		case x86_ESP: PUTDST16(*code,0xCC83); break;
		case x86_EBP: PUTDST16(*code,0xCD83); break;
		}
		PUTDST8(*code, Const);
	}
}

void OrVariableToX86Reg(BYTE** code, void * Variable, char * VariableName, int x86Reg) {
	CPU_OR_RSP_Message(*code, "      or %s, dword ptr [%s]",x86_Name(x86Reg),VariableName);
	switch (x86Reg) {
	case x86_EAX: PUTDST16(*code,0x050B); break;
	case x86_EBX: PUTDST16(*code,0x1D0B); break;
	case x86_ECX: PUTDST16(*code,0x0D0B); break;
	case x86_EDX: PUTDST16(*code,0x150B); break;
	case x86_ESI: PUTDST16(*code,0x350B); break;
	case x86_EDI: PUTDST16(*code,0x3D0B); break;
	case x86_ESP: PUTDST16(*code,0x250B); break;
	case x86_EBP: PUTDST16(*code,0x2D0B); break;
	}
	PUTDST32(*code,Variable);
}

void OrX86RegToVariable(BYTE** code, void * Variable, char * VariableName, int x86Reg) {
	CPU_OR_RSP_Message(*code, "      or dword ptr [%s], %s",VariableName, x86_Name(x86Reg));
	switch (x86Reg) {
	case x86_EAX: PUTDST16(*code,0x0509); break;
	case x86_EBX: PUTDST16(*code,0x1D09); break;
	case x86_ECX: PUTDST16(*code,0x0D09); break;
	case x86_EDX: PUTDST16(*code,0x1509); break;
	case x86_ESI: PUTDST16(*code,0x3509); break;
	case x86_EDI: PUTDST16(*code,0x3D09); break;
	case x86_ESP: PUTDST16(*code,0x2509); break;
	case x86_EBP: PUTDST16(*code,0x2D09); break;
	}
	PUTDST32(*code,Variable);
}

void OrX86RegToX86Reg(BYTE** code, int Destination, int Source) {
	WORD x86Command = 0x0;

	CPU_OR_RSP_Message(*code, "      or %s, %s",x86_Name(Destination),x86_Name(Source));
	switch (Source) {
	case x86_EAX: x86Command = 0x000B; break;
	case x86_EBX: x86Command = 0x030B; break;
	case x86_ECX: x86Command = 0x010B; break;
	case x86_EDX: x86Command = 0x020B; break;
	case x86_ESI: x86Command = 0x060B; break;
	case x86_EDI: x86Command = 0x070B; break;
	case x86_ESP: x86Command = 0x040B; break;
	case x86_EBP: x86Command = 0x050B; break;
	}
	switch (Destination) {
	case x86_EAX: x86Command += 0xC000; break;
	case x86_EBX: x86Command += 0xD800; break;
	case x86_ECX: x86Command += 0xC800; break;
	case x86_EDX: x86Command += 0xD000; break;
	case x86_ESI: x86Command += 0xF000; break;
	case x86_EDI: x86Command += 0xF800; break;
	case x86_ESP: x86Command += 0xE000; break;
	case x86_EBP: x86Command += 0xE800; break;
	}
	PUTDST16(*code,x86Command);
}

void Popad(BYTE** code) {
	CPU_OR_RSP_Message(*code, "      popad");
	PUTDST8(*code,0x61);
}

void Pushad(BYTE** code) {
	CPU_OR_RSP_Message(*code, "      pushad");
	PUTDST8(*code,0x60);
}

void Push(BYTE** code, int x86reg) {
	CPU_OR_RSP_Message(*code, "      push %s", x86_Name(x86reg));

	switch(x86reg) {
	case x86_EAX: PUTDST8(*code, 0x50); break;
	case x86_EBX: PUTDST8(*code, 0x53); break;
	case x86_ECX: PUTDST8(*code, 0x51); break;
	case x86_EDX: PUTDST8(*code, 0x52); break;
	case x86_ESI: PUTDST8(*code, 0x56); break;
	case x86_EDI: PUTDST8(*code, 0x57); break;
	case x86_ESP: PUTDST8(*code, 0x54); break;
	case x86_EBP: PUTDST8(*code, 0x55); break;
	}
}

void Pop(BYTE** code, int x86reg) {
	CPU_OR_RSP_Message(*code, "      pop %s", x86_Name(x86reg));

	switch(x86reg) {
	case x86_EAX: PUTDST8(*code, 0x58); break;
	case x86_EBX: PUTDST8(*code, 0x5B); break;
	case x86_ECX: PUTDST8(*code, 0x59); break;
	case x86_EDX: PUTDST8(*code, 0x5A); break;
	case x86_ESI: PUTDST8(*code, 0x5E); break;
	case x86_EDI: PUTDST8(*code, 0x5F); break;
	case x86_ESP: PUTDST8(*code, 0x5C); break;
	case x86_EBP: PUTDST8(*code, 0x5D); break;
	}
}

void PushImm32(BYTE** code, char * String, DWORD Value) {
	CPU_OR_RSP_Message(*code, "      push %s",String);
	PUTDST8(*code,0x68);
	PUTDST32(*code,Value);
}

void Ret(BYTE** code) {
	CPU_OR_RSP_Message(*code, "      ret");
	PUTDST8(*code,0xC3);
}

void Seta(BYTE** code, int x86reg) {
	CPU_OR_RSP_Message(*code, "      seta %s",x86Byte_Name(x86reg));
	PUTDST16(*code,0x970F);
	switch (x86reg) {
	case x86_EAX: PUTDST8(*code,0xC0); break;
	case x86_EBX: PUTDST8(*code,0xC3); break;
	case x86_ECX: PUTDST8(*code,0xC1); break;
	case x86_EDX: PUTDST8(*code,0xC2); break;
	default:
		DisplayError("Seta\nUnknown x86 Register");
	}
}

void SetaVariable(BYTE** code, void * Variable, char * VariableName) {
	CPU_OR_RSP_Message(*code, "      seta byte ptr [%s]",VariableName);
	PUTDST16(*code,0x970F);
	PUTDST8(*code,0x05);
	PUTDST32(*code,Variable);
}

void Setae(BYTE** code, int x86reg) {
	CPU_OR_RSP_Message(*code, "      setae %s",x86Byte_Name(x86reg));
	PUTDST16(*code,0x930F);
	switch (x86reg) {
	case x86_EAX: PUTDST8(*code,0xC0); break;
	case x86_EBX: PUTDST8(*code,0xC3); break;
	case x86_ECX: PUTDST8(*code,0xC1); break;
	case x86_EDX: PUTDST8(*code,0xC2); break;
	default:
		DisplayError("Seta\nUnknown x86 Register");
	}
}

void Setb(BYTE** code, int x86reg) {
	CPU_OR_RSP_Message(*code, "      setb %s",x86Byte_Name(x86reg));
	PUTDST16(*code,0x920F);
	switch (x86reg) {
	case x86_EAX: PUTDST8(*code,0xC0); break;
	case x86_EBX: PUTDST8(*code,0xC3); break;
	case x86_ECX: PUTDST8(*code,0xC1); break;
	case x86_EDX: PUTDST8(*code,0xC2); break;
	default:
		DisplayError("Setb\nUnknown x86 Register");
	}
}

void SetbVariable(BYTE** code, void * Variable, char * VariableName) {
	CPU_OR_RSP_Message(*code, "      setb byte ptr [%s]",VariableName);
	PUTDST16(*code,0x920F);
	PUTDST8(*code,0x05);
	PUTDST32(*code,Variable);
}

void Setg(BYTE** code, int x86reg) {
	CPU_OR_RSP_Message(*code, "      setg %s",x86Byte_Name(x86reg));
	PUTDST16(*code,0x9F0F);
	switch (x86reg) {
	case x86_EAX: PUTDST8(*code,0xC0); break;
	case x86_EBX: PUTDST8(*code,0xC3); break;
	case x86_ECX: PUTDST8(*code,0xC1); break;
	case x86_EDX: PUTDST8(*code,0xC2); break;
	default:
		DisplayError("Setg\nUnknown x86 Register");
	}
}

void SetgVariable(BYTE** code, void * Variable, char * VariableName) {
	CPU_OR_RSP_Message(*code, "      setg byte ptr [%s]",VariableName);
	PUTDST16(*code,0x9F0F);
	PUTDST8(*code,0x05);
	PUTDST32(*code,Variable);
}

void SetgeVariable(BYTE** code, void * Variable, char * VariableName) {
	CPU_OR_RSP_Message(*code, "      setge byte ptr [%s]", VariableName);
	PUTDST16(*code, 0x9D0F);
	PUTDST8(*code, 0x05);
	PUTDST32(*code, Variable);
}

void Setl(BYTE** code, int x86reg) {
	CPU_OR_RSP_Message(*code, "      setl %s",x86Byte_Name(x86reg));
	PUTDST16(*code,0x9C0F);
	switch (x86reg) {
	case x86_EAX: PUTDST8(*code,0xC0); break;
	case x86_EBX: PUTDST8(*code,0xC3); break;
	case x86_ECX: PUTDST8(*code,0xC1); break;
	case x86_EDX: PUTDST8(*code,0xC2); break;
	default:
		DisplayError("Setl\nUnknown x86 Register");
	}
}

void SetlVariable(BYTE** code, void * Variable, char * VariableName) {
	CPU_OR_RSP_Message(*code, "      setl byte ptr [%s]",VariableName);
	PUTDST16(*code,0x9C0F);
	PUTDST8(*code,0x05);
	PUTDST32(*code,Variable);
}

void SetleVariable(BYTE** code, void* Variable, char* VariableName) {
	CPU_OR_RSP_Message(*code, "      setle byte ptr [%s]", VariableName);
	PUTDST16(*code, 0x9E0F);
	PUTDST8(*code, 0x05);
	PUTDST32(*code, Variable);
}

void Setz(BYTE** code, int x86reg) {
	CPU_OR_RSP_Message(*code, "      setz %s",x86Byte_Name(x86reg));
	PUTDST16(*code,0x940F);
	switch (x86reg) {
	case x86_EAX: PUTDST8(*code,0xC0); break;
	case x86_EBX: PUTDST8(*code,0xC3); break;
	case x86_ECX: PUTDST8(*code,0xC1); break;
	case x86_EDX: PUTDST8(*code,0xC2); break;
	default:
		DisplayError("Setz\nUnknown x86 Register");
	}
}

void SetzVariable(BYTE** code, void* Variable, char* VariableName) {
	CPU_OR_RSP_Message(*code, "      setz byte ptr [%s]", VariableName);
	PUTDST16(*code, 0x940F);
	PUTDST8(*code, 0x05);
	PUTDST32(*code, Variable);
}

void Setnz(BYTE** code, int x86reg) {
	CPU_OR_RSP_Message(*code, "      setnz %s",x86Byte_Name(x86reg));
	PUTDST16(*code,0x950F);
	switch (x86reg) {
	case x86_EAX: PUTDST8(*code,0xC0); break;
	case x86_EBX: PUTDST8(*code,0xC3); break;
	case x86_ECX: PUTDST8(*code,0xC1); break;
	case x86_EDX: PUTDST8(*code,0xC2); break;
	default:
		DisplayError("Setnz\nUnknown x86 Register");
	}
}

void SetnzVariable(BYTE** code, void* Variable, char* VariableName) {
	CPU_OR_RSP_Message(*code, "      setnz byte ptr [%s]", VariableName);
	PUTDST16(*code, 0x950F);
	PUTDST8(*code, 0x05);
	PUTDST32(*code, Variable);
}

void ShiftLeftDouble(BYTE** code, int Destination, int Source) {
	unsigned char s = 0xC0;

	CPU_OR_RSP_Message(*code, "      shld %s, %s, cl", x86_Name(Destination),x86_Name(Source));
	PUTDST16(*code,0xA50F);

	switch (Destination) {
	case x86_EAX: s |= 0x00; break;
	case x86_EBX: s |= 0x03; break;
	case x86_ECX: s |= 0x01; break;
	case x86_EDX: s |= 0x02; break;
	case x86_ESI: s |= 0x06; break;
	case x86_EDI: s |= 0x07; break;
	case x86_ESP: s |= 0x04; break;
	case x86_EBP: s |= 0x05; break;
	}

	switch (Source) {
	case x86_EAX: s |= 0x00 << 3; break;
	case x86_EBX: s |= 0x03 << 3; break;
	case x86_ECX: s |= 0x01 << 3; break;
	case x86_EDX: s |= 0x02 << 3; break;
	case x86_ESI: s |= 0x06 << 3; break;
	case x86_EDI: s |= 0x07 << 3; break;
	case x86_ESP: s |= 0x04 << 3; break;
	case x86_EBP: s |= 0x05 << 3; break;
	}

	PUTDST8(*code,s);
}

void ShiftLeftDoubleImmed(BYTE** code, int Destination, int Source, BYTE Immediate) {
	unsigned char s = 0xC0;

	CPU_OR_RSP_Message(*code, "      shld %s, %s, %Xh", x86_Name(Destination),x86_Name(Source), Immediate);
	PUTDST16(*code,0xA40F);

	switch (Destination) {
	case x86_EAX: s |= 0x00; break;
	case x86_EBX: s |= 0x03; break;
	case x86_ECX: s |= 0x01; break;
	case x86_EDX: s |= 0x02; break;
	case x86_ESI: s |= 0x06; break;
	case x86_EDI: s |= 0x07; break;
	case x86_ESP: s |= 0x04; break;
	case x86_EBP: s |= 0x05; break;
	}

	switch (Source) {
	case x86_EAX: s |= 0x00 << 3; break;
	case x86_EBX: s |= 0x03 << 3; break;
	case x86_ECX: s |= 0x01 << 3; break;
	case x86_EDX: s |= 0x02 << 3; break;
	case x86_ESI: s |= 0x06 << 3; break;
	case x86_EDI: s |= 0x07 << 3; break;
	case x86_ESP: s |= 0x04 << 3; break;
	case x86_EBP: s |= 0x05 << 3; break;
	}

	PUTDST8(*code,s);
	PUTDST8(*code,Immediate);
}

void ShiftLeftSign(BYTE** code, int x86reg) {
	CPU_OR_RSP_Message(*code, "      shl %s, cl",x86_Name(x86reg));
	switch (x86reg) {
	case x86_EAX: PUTDST16(*code,0xE0D3); break;
	case x86_EBX: PUTDST16(*code,0xE3D3); break;
	case x86_ECX: PUTDST16(*code,0xE1D3); break;
	case x86_EDX: PUTDST16(*code,0xE2D3); break;
	case x86_ESI: PUTDST16(*code,0xE6D3); break;
	case x86_EDI: PUTDST16(*code,0xE7D3); break;
	case x86_ESP: PUTDST16(*code,0xE4D3); break;
	case x86_EBP: PUTDST16(*code,0xE5D3); break;
	}
}

void ShiftLeftSignImmed(BYTE** code, int x86reg, BYTE Immediate) {
	CPU_OR_RSP_Message(*code, "      shl %s, %Xh",x86_Name(x86reg),Immediate);

	if (Immediate != 1) {
		switch (x86reg) {
		case x86_EAX: PUTDST16(*code, 0xE0C1); break;
		case x86_EBX: PUTDST16(*code, 0xE3C1); break;
		case x86_ECX: PUTDST16(*code, 0xE1C1); break;
		case x86_EDX: PUTDST16(*code, 0xE2C1); break;
		case x86_ESI: PUTDST16(*code, 0xE6C1); break;
		case x86_EDI: PUTDST16(*code, 0xE7C1); break;
		case x86_ESP: PUTDST16(*code, 0xE4C1); break;
		case x86_EBP: PUTDST16(*code, 0xE5C1); break;
		}
		PUTDST8(*code, Immediate);
	} else {
		switch (x86reg) {
		case x86_EAX: PUTDST16(*code, 0xE0D1); break;
		case x86_EBX: PUTDST16(*code, 0xE3D1); break;
		case x86_ECX: PUTDST16(*code, 0xE1D1); break;
		case x86_EDX: PUTDST16(*code, 0xE2D1); break;
		case x86_ESI: PUTDST16(*code, 0xE6D1); break;
		case x86_EDI: PUTDST16(*code, 0xE7D1); break;
		case x86_ESP: PUTDST16(*code, 0xE4D1); break;
		case x86_EBP: PUTDST16(*code, 0xE5D1); break;
		}
	}
}

void ShiftLeftSignVariableImmed(BYTE** code, void* Variable, char* VariableName, BYTE Immediate) {
	CPU_OR_RSP_Message(*code, "      shl dword ptr [%s], %Xh", VariableName, Immediate);

	if (Immediate != 1) {
		PUTDST16(*code, 0x25C1);
		PUTDST32(*code, Variable);
		PUTDST8(*code, Immediate);
	} else {
		PUTDST16(*code, 0x25D1);
		PUTDST32(*code, Variable);
	}
}

void ShiftLeftSignVariable(BYTE** code, void* Variable, char* VariableName) {
	CPU_OR_RSP_Message(*code, "      shl dword ptr [%s], cl", VariableName);

	PUTDST16(*code, 0x25D3);
	PUTDST32(*code, Variable);
}

void ShiftRightSign(BYTE** code, int x86reg) {
	CPU_OR_RSP_Message(*code, "      sar %s, cl",x86_Name(x86reg));
	switch (x86reg) {
	case x86_EAX: PUTDST16(*code,0xF8D3); break;
	case x86_EBX: PUTDST16(*code,0xFBD3); break;
	case x86_ECX: PUTDST16(*code,0xF9D3); break;
	case x86_EDX: PUTDST16(*code,0xFAD3); break;
	case x86_ESI: PUTDST16(*code,0xFED3); break;
	case x86_EDI: PUTDST16(*code,0xFFD3); break;
	case x86_ESP: PUTDST16(*code,0xFCD3); break;
	case x86_EBP: PUTDST16(*code,0xFDD3); break;
	}
}

void ShiftRightSignImmed(BYTE** code, int x86reg, BYTE Immediate) {
	CPU_OR_RSP_Message(*code, "      sar %s, %Xh",x86_Name(x86reg),Immediate);
	if (Immediate != 1) {
		switch (x86reg) {
		case x86_EAX: PUTDST16(*code, 0xF8C1); break;
		case x86_EBX: PUTDST16(*code, 0xFBC1); break;
		case x86_ECX: PUTDST16(*code, 0xF9C1); break;
		case x86_EDX: PUTDST16(*code, 0xFAC1); break;
		case x86_ESI: PUTDST16(*code, 0xFEC1); break;
		case x86_EDI: PUTDST16(*code, 0xFFC1); break;
		case x86_ESP: PUTDST16(*code, 0xFCC1); break;
		case x86_EBP: PUTDST16(*code, 0xFDC1); break;
		default:
			DisplayError("ShiftRightSignImmed\nUnknown x86 Register");
		}
		PUTDST8(*code, Immediate);
	} else {
		switch (x86reg) {
		case x86_EAX: PUTDST16(*code, 0xF8D1); break;
		case x86_EBX: PUTDST16(*code, 0xFBD1); break;
		case x86_ECX: PUTDST16(*code, 0xF9D1); break;
		case x86_EDX: PUTDST16(*code, 0xFAD1); break;
		case x86_ESI: PUTDST16(*code, 0xFED1); break;
		case x86_EDI: PUTDST16(*code, 0xFFD1); break;
		case x86_ESP: PUTDST16(*code, 0xFCD1); break;
		case x86_EBP: PUTDST16(*code, 0xFDD1); break;
		default:
			DisplayError("ShiftRightSignImmed\nUnknown x86 Register");
		}
	}
}

void ShiftRightSignVariableImmed(BYTE** code, void* Variable, char* VariableName, BYTE Immediate) {
	CPU_OR_RSP_Message(*code, "      sar dword ptr [%s], %Xh", VariableName, Immediate);

	if (Immediate != 1) {
		PUTDST16(*code, 0x3DC1);
		PUTDST32(*code, Variable);
		PUTDST8(*code, Immediate);
	} else {
		PUTDST16(*code, 0x3DD1);
		PUTDST32(*code, Variable);
	}
}

void ShiftRightSignVariable(BYTE** code, void* Variable, char* VariableName) {
	CPU_OR_RSP_Message(*code, "      sar dword ptr [%s], cl", VariableName);

	PUTDST16(*code, 0x3DD3);
	PUTDST32(*code, Variable);
}

void ShiftRightUnsign(BYTE** code, int x86reg) {
	CPU_OR_RSP_Message(*code, "      shr %s, cl",x86_Name(x86reg));
	switch (x86reg) {
	case x86_EAX: PUTDST16(*code,0xE8D3); break;
	case x86_EBX: PUTDST16(*code,0xEBD3); break;
	case x86_ECX: PUTDST16(*code,0xE9D3); break;
	case x86_EDX: PUTDST16(*code,0xEAD3); break;
	case x86_ESI: PUTDST16(*code,0xEED3); break;
	case x86_EDI: PUTDST16(*code,0xEFD3); break;
	case x86_ESP: PUTDST16(*code,0xECD3); break;
	case x86_EBP: PUTDST16(*code,0xEDD3); break;
	}
}

void ShiftRightDouble(BYTE** code, int Destination, int Source) {
	unsigned char s = 0xC0;

	CPU_OR_RSP_Message(*code, "      shrd %s, %s, cl", x86_Name(Destination),x86_Name(Source));
	PUTDST16(*code,0xAD0F);

	switch (Destination) {
	case x86_EAX: s |= 0x00; break;
	case x86_EBX: s |= 0x03; break;
	case x86_ECX: s |= 0x01; break;
	case x86_EDX: s |= 0x02; break;
	case x86_ESI: s |= 0x06; break;
	case x86_EDI: s |= 0x07; break;
	case x86_ESP: s |= 0x04; break;
	case x86_EBP: s |= 0x05; break;
	}

	switch (Source) {
	case x86_EAX: s |= 0x00 << 3; break;
	case x86_EBX: s |= 0x03 << 3; break;
	case x86_ECX: s |= 0x01 << 3; break;
	case x86_EDX: s |= 0x02 << 3; break;
	case x86_ESI: s |= 0x06 << 3; break;
	case x86_EDI: s |= 0x07 << 3; break;
	case x86_ESP: s |= 0x04 << 3; break;
	case x86_EBP: s |= 0x05 << 3; break;
	}

	PUTDST8(*code,s);
}

void ShiftRightDoubleImmed(BYTE** code, int Destination, int Source, BYTE Immediate) {
	unsigned char s = 0xC0;

	CPU_OR_RSP_Message(*code, "      shrd %s, %s, %Xh", x86_Name(Destination),x86_Name(Source), Immediate);
	PUTDST16(*code,0xAC0F);

	switch (Destination) {
	case x86_EAX: s |= 0x00; break;
	case x86_EBX: s |= 0x03; break;
	case x86_ECX: s |= 0x01; break;
	case x86_EDX: s |= 0x02; break;
	case x86_ESI: s |= 0x06; break;
	case x86_EDI: s |= 0x07; break;
	case x86_ESP: s |= 0x04; break;
	case x86_EBP: s |= 0x05; break;
	}

	switch (Source) {
	case x86_EAX: s |= 0x00 << 3; break;
	case x86_EBX: s |= 0x03 << 3; break;
	case x86_ECX: s |= 0x01 << 3; break;
	case x86_EDX: s |= 0x02 << 3; break;
	case x86_ESI: s |= 0x06 << 3; break;
	case x86_EDI: s |= 0x07 << 3; break;
	case x86_ESP: s |= 0x04 << 3; break;
	case x86_EBP: s |= 0x05 << 3; break;
	}

	PUTDST8(*code,s);
	PUTDST8(*code,Immediate);
}

void ShiftRightUnsignImmed(BYTE** code, int x86reg, BYTE Immediate) {
	CPU_OR_RSP_Message(*code, "      shr %s, %Xh",x86_Name(x86reg),Immediate);
	if (Immediate != 1) {
		switch (x86reg) {
		case x86_EAX: PUTDST16(*code, 0xE8C1); break;
		case x86_EBX: PUTDST16(*code, 0xEBC1); break;
		case x86_ECX: PUTDST16(*code, 0xE9C1); break;
		case x86_EDX: PUTDST16(*code, 0xEAC1); break;
		case x86_ESI: PUTDST16(*code, 0xEEC1); break;
		case x86_EDI: PUTDST16(*code, 0xEFC1); break;
		case x86_ESP: PUTDST16(*code, 0xECC1); break;
		case x86_EBP: PUTDST16(*code, 0xEDC1); break;
		}
		PUTDST8(*code, Immediate);
	} else {
		switch (x86reg) {
		case x86_EAX: PUTDST16(*code, 0xE8D1); break;
		case x86_EBX: PUTDST16(*code, 0xEBD1); break;
		case x86_ECX: PUTDST16(*code, 0xE9D1); break;
		case x86_EDX: PUTDST16(*code, 0xEAD1); break;
		case x86_ESI: PUTDST16(*code, 0xEED1); break;
		case x86_EDI: PUTDST16(*code, 0xEFD1); break;
		case x86_ESP: PUTDST16(*code, 0xECD1); break;
		case x86_EBP: PUTDST16(*code, 0xEDD1); break;
		}
	}
}

void ShiftRightUnsignVariableImmed(BYTE** code, void* Variable, char* VariableName, BYTE Immediate) {
	CPU_OR_RSP_Message(*code, "      shr dword ptr [%s], %Xh", VariableName, Immediate);

	if (Immediate != 1) {
		PUTDST16(*code, 0x2DC1);
		PUTDST32(*code, Variable);
		PUTDST8(*code, Immediate);
	} else {
		PUTDST16(*code, 0x2DD1);
		PUTDST32(*code, Variable);
	}
}

void ShiftRightUnsignVariable(BYTE** code, void* Variable, char* VariableName) {
	CPU_OR_RSP_Message(*code, "      shr dword ptr [%s], cl", VariableName);

	PUTDST16(*code, 0x2DD3);
	PUTDST32(*code, Variable);
}

void SbbConstFromX86Reg (BYTE** code, int x86Reg, DWORD Const) {
	CPU_OR_RSP_Message(*code, "      sbb %s, %Xh",x86_Name(x86Reg),Const);
	if ((Const & 0xFFFFFF80) != 0 && (Const & 0xFFFFFF80) != 0xFFFFFF80) {
		switch (x86Reg) {
		case x86_EAX: PUTDST16(*code,0xD881); break;
		case x86_EBX: PUTDST16(*code,0xDB81); break;
		case x86_ECX: PUTDST16(*code,0xD981); break;
		case x86_EDX: PUTDST16(*code,0xDA81); break;
		case x86_ESI: PUTDST16(*code,0xDE81); break;
		case x86_EDI: PUTDST16(*code,0xDF81); break;
		case x86_ESP: PUTDST16(*code,0xDC81); break;
		case x86_EBP: PUTDST16(*code,0xDD81); break;
		}
		PUTDST32(*code, Const);
	} else {
		switch (x86Reg) {
		case x86_EAX: PUTDST16(*code,0xD883); break;
		case x86_EBX: PUTDST16(*code,0xDB83); break;
		case x86_ECX: PUTDST16(*code,0xD983); break;
		case x86_EDX: PUTDST16(*code,0xDA83); break;
		case x86_ESI: PUTDST16(*code,0xDE83); break;
		case x86_EDI: PUTDST16(*code,0xDF83); break;
		case x86_ESP: PUTDST16(*code,0xDC83); break;
		case x86_EBP: PUTDST16(*code,0xDD83); break;
		}
		PUTDST8(*code, Const);
	}
}

void SbbVariableFromX86reg(BYTE** code, int x86reg, void * Variable, char * VariableName) {
	CPU_OR_RSP_Message(*code, "      sbb %s, dword ptr [%s]",x86_Name(x86reg),VariableName);
	switch (x86reg) {
	case x86_EAX: PUTDST16(*code,0x051B); break;
	case x86_EBX: PUTDST16(*code,0x1D1B); break;
	case x86_ECX: PUTDST16(*code,0x0D1B); break;
	case x86_EDX: PUTDST16(*code,0x151B); break;
	case x86_ESI: PUTDST16(*code,0x351B); break;
	case x86_EDI: PUTDST16(*code,0x3D1B); break;
	case x86_ESP: PUTDST16(*code,0x251B); break;
	case x86_EBP: PUTDST16(*code,0x2D1B); break;
	default:
		DisplayError("SbbVariableFromX86reg\nUnknown x86 Register");
	}
    PUTDST32(*code,Variable);
}

void SbbX86RegToX86Reg(BYTE** code, int Destination, int Source) {
	WORD x86Command = 0x0;
	CPU_OR_RSP_Message(*code, "      sbb %s, %s",x86_Name(Destination),x86_Name(Source));
	switch (Source) {
	case x86_EAX: x86Command = 0x001B; break;
	case x86_EBX: x86Command = 0x031B; break;
	case x86_ECX: x86Command = 0x011B; break;
	case x86_EDX: x86Command = 0x021B; break;
	case x86_ESI: x86Command = 0x061B; break;
	case x86_EDI: x86Command = 0x071B; break;
	case x86_ESP: x86Command = 0x041B; break;
	case x86_EBP: x86Command = 0x051B; break;
	}
	switch (Destination) {
	case x86_EAX: x86Command += 0xC000; break;
	case x86_EBX: x86Command += 0xD800; break;
	case x86_ECX: x86Command += 0xC800; break;
	case x86_EDX: x86Command += 0xD000; break;
	case x86_ESI: x86Command += 0xF000; break;
	case x86_EDI: x86Command += 0xF800; break;
	case x86_ESP: x86Command += 0xE000; break;
	case x86_EBP: x86Command += 0xE800; break;
	}
	PUTDST16(*code,x86Command);
}

void SubConstFromVariable (BYTE** code, DWORD Const, void *Variable, char *VariableName) {
	CPU_OR_RSP_Message(*code, "      sub dword ptr [%s], 0x%X",VariableName, Const);\
	PUTDST16(*code,0x2D81);
	PUTDST32(*code,Variable);
	PUTDST32(*code,Const);
}

void SubConstFromX86Reg (BYTE** code, int x86Reg, DWORD Const) {
	CPU_OR_RSP_Message(*code, "      sub %s, %Xh",x86_Name(x86Reg),Const);
	if ((Const & 0xFFFFFF80) != 0 && (Const & 0xFFFFFF80) != 0xFFFFFF80) {
		switch (x86Reg) {
		case x86_EAX: PUTDST8(*code,0x2D); break;
		case x86_EBX: PUTDST16(*code,0xEB81); break;
		case x86_ECX: PUTDST16(*code,0xE981); break;
		case x86_EDX: PUTDST16(*code,0xEA81); break;
		case x86_ESI: PUTDST16(*code,0xEE81); break;
		case x86_EDI: PUTDST16(*code,0xEF81); break;
		case x86_ESP: PUTDST16(*code,0xEC81); break;
		case x86_EBP: PUTDST16(*code,0xED81); break;
		}
		PUTDST32(*code, Const);
	} else {
		switch (x86Reg) {
		case x86_EAX: PUTDST16(*code,0xE883); break;
		case x86_EBX: PUTDST16(*code,0xEB83); break;
		case x86_ECX: PUTDST16(*code,0xE983); break;
		case x86_EDX: PUTDST16(*code,0xEA83); break;
		case x86_ESI: PUTDST16(*code,0xEE83); break;
		case x86_EDI: PUTDST16(*code,0xEF83); break;
		case x86_ESP: PUTDST16(*code,0xEC83); break;
		case x86_EBP: PUTDST16(*code,0xED83); break;
		}
		PUTDST8(*code, Const);
	}
}

void SubVariableFromX86reg(BYTE** code, int x86reg, void * Variable, char * VariableName) {
	CPU_OR_RSP_Message(*code, "      sub %s, dword ptr [%s]",x86_Name(x86reg),VariableName);
	switch (x86reg) {
	case x86_EAX: PUTDST16(*code,0x052B); break;
	case x86_EBX: PUTDST16(*code,0x1D2B); break;
	case x86_ECX: PUTDST16(*code,0x0D2B); break;
	case x86_EDX: PUTDST16(*code,0x152B); break;
	case x86_ESI: PUTDST16(*code,0x352B); break;
	case x86_EDI: PUTDST16(*code,0x3D2B); break;
	case x86_ESP: PUTDST16(*code,0x252B); break;
	case x86_EBP: PUTDST16(*code,0x2D2B); break;
	default:
		DisplayError("SubVariableFromX86reg\nUnknown x86 Register");
	}
    PUTDST32(*code,Variable);
}

void SubX86RegToX86Reg(BYTE** code, int Destination, int Source) {
	WORD x86Command = 0x0;
	CPU_OR_RSP_Message(*code, "      sub %s, %s",x86_Name(Destination),x86_Name(Source));
	switch (Source) {
	case x86_EAX: x86Command = 0x002B; break;
	case x86_EBX: x86Command = 0x032B; break;
	case x86_ECX: x86Command = 0x012B; break;
	case x86_EDX: x86Command = 0x022B; break;
	case x86_ESI: x86Command = 0x062B; break;
	case x86_EDI: x86Command = 0x072B; break;
	case x86_ESP: x86Command = 0x042B; break;
	case x86_EBP: x86Command = 0x052B; break;
	}
	switch (Destination) {
	case x86_EAX: x86Command += 0xC000; break;
	case x86_EBX: x86Command += 0xD800; break;
	case x86_ECX: x86Command += 0xC800; break;
	case x86_EDX: x86Command += 0xD000; break;
	case x86_ESI: x86Command += 0xF000; break;
	case x86_EDI: x86Command += 0xF800; break;
	case x86_ESP: x86Command += 0xE000; break;
	case x86_EBP: x86Command += 0xE800; break;
	}
	PUTDST16(*code,x86Command);
}

void SubX86regFromVariable(BYTE** code, int x86reg, void* Variable, char* VariableName) {
	CPU_OR_RSP_Message(*code, "      sub dword ptr [%s], %s", VariableName, x86_Name(x86reg));
	switch (x86reg) {
	case x86_EAX: PUTDST16(*code, 0x0529); break;
	case x86_EBX: PUTDST16(*code, 0x1D29); break;
	case x86_ECX: PUTDST16(*code, 0x0D29); break;
	case x86_EDX: PUTDST16(*code, 0x1529); break;
	case x86_ESI: PUTDST16(*code, 0x3529); break;
	case x86_EDI: PUTDST16(*code, 0x3D29); break;
	case x86_ESP: PUTDST16(*code, 0x2529); break;
	case x86_EBP: PUTDST16(*code, 0x2D29); break;
	default:
		DisplayError("SubX86regFromVariable\nUnknown x86 Register");
	}
	PUTDST32(*code, Variable);
}

void TestConstToX86Reg(BYTE** code, DWORD Const, int x86reg) {
	if ((Const & 0xFFFFFF80) != 0 || x86reg == x86_ESI || x86reg == x86_EDI || x86reg == x86_ESP || x86reg == x86_EBP) {
		CPU_OR_RSP_Message(*code, "      test %s, 0x%X", x86_Name(x86reg), Const);

		switch (x86reg) {
		case x86_EAX: PUTDST8(*code, 0xA9); break;
		case x86_EBX: PUTDST16(*code, 0xC3F7); break;
		case x86_ECX: PUTDST16(*code, 0xC1F7); break;
		case x86_EDX: PUTDST16(*code, 0xC2F7); break;
		case x86_ESI: PUTDST16(*code, 0xC6F7); break;
		case x86_EDI: PUTDST16(*code, 0xC7F7); break;
		case x86_ESP: PUTDST16(*code, 0xC4F7); break;
		case x86_EBP: PUTDST16(*code, 0xC5F7); break;
		}
		PUTDST32(*code, Const);
	} else {
		CPU_OR_RSP_Message(*code, "      test %s, 0x%X", x86Byte_Name(x86reg), Const);
		switch (x86reg) {
		case x86_EAX: PUTDST8(*code, 0xA8); break;
		case x86_EBX: PUTDST16(*code, 0xC3F6); break;
		case x86_ECX: PUTDST16(*code, 0xC1F6); break;
		case x86_EDX: PUTDST16(*code, 0xC2F6); break;
		}
		PUTDST8(*code, Const);
	}
}

void TestVariable(BYTE** code, DWORD Const, void * Variable, char * VariableName) {
	CPU_OR_RSP_Message(*code, "      test dword ptr ds:[%s], 0x%X",VariableName, Const);
	PUTDST16(*code,0x05F7);
	PUTDST32(*code,Variable);
	PUTDST32(*code,Const);
}

void TestX86RegToX86Reg(BYTE** code, int Destination, int Source) {
	WORD x86Command = 0x0;
	CPU_OR_RSP_Message(*code, "      test %s, %s",x86_Name(Destination),x86_Name(Source));
	switch (Source) {
	case x86_EAX: x86Command = 0x0085; break;
	case x86_EBX: x86Command = 0x0385; break;
	case x86_ECX: x86Command = 0x0185; break;
	case x86_EDX: x86Command = 0x0285; break;
	case x86_ESI: x86Command = 0x0685; break;
	case x86_EDI: x86Command = 0x0785; break;
	case x86_ESP: x86Command = 0x0485; break;
	case x86_EBP: x86Command = 0x0585; break;
	}
	switch (Destination) {
	case x86_EAX: x86Command += 0xC000; break;
	case x86_EBX: x86Command += 0xD800; break;
	case x86_ECX: x86Command += 0xC800; break;
	case x86_EDX: x86Command += 0xD000; break;
	case x86_ESI: x86Command += 0xF000; break;
	case x86_EDI: x86Command += 0xF800; break;
	case x86_ESP: x86Command += 0xE000; break;
	case x86_EBP: x86Command += 0xE800; break;
	}
	PUTDST16(*code,x86Command);
}

void XorConstToX86Reg(BYTE** code, int x86Reg, DWORD Const) {
	CPU_OR_RSP_Message(*code, "      xor %s, %Xh",x86_Name(x86Reg),Const);
	if ((Const & 0xFFFFFF80) != 0 && (Const & 0xFFFFFF80) != 0xFFFFFF80) {
		switch (x86Reg) {
		case x86_EAX: PUTDST8(*code,0x35); break;
		case x86_EBX: PUTDST16(*code,0xF381); break;
		case x86_ECX: PUTDST16(*code,0xF181); break;
		case x86_EDX: PUTDST16(*code,0xF281); break;
		case x86_ESI: PUTDST16(*code,0xF681); break;
		case x86_EDI: PUTDST16(*code,0xF781); break;
		case x86_ESP: PUTDST16(*code,0xF481); break;
		case x86_EBP: PUTDST16(*code,0xF581); break;
		}
		PUTDST32(*code, Const);
	} else {
		switch (x86Reg) {
		case x86_EAX: PUTDST16(*code,0xF083); break;
		case x86_EBX: PUTDST16(*code,0xF383); break;
		case x86_ECX: PUTDST16(*code,0xF183); break;
		case x86_EDX: PUTDST16(*code,0xF283); break;
		case x86_ESI: PUTDST16(*code,0xF683); break;
		case x86_EDI: PUTDST16(*code,0xF783); break;
		case x86_ESP: PUTDST16(*code,0xF483); break;
		case x86_EBP: PUTDST16(*code,0xF583); break;
		}
		PUTDST8(*code, Const);
	}
}

void XorX86RegToX86Reg(BYTE** code, int Source, int Destination) {
	WORD x86Command = 0x0;

	CPU_OR_RSP_Message(*code, "      xor %s, %s",x86_Name(Source),x86_Name(Destination));
		
	switch (Source) {
	case x86_EAX: x86Command = 0x0031; break;
	case x86_EBX: x86Command = 0x0331; break;
	case x86_ECX: x86Command = 0x0131; break;
	case x86_EDX: x86Command = 0x0231; break;
	case x86_ESI: x86Command = 0x0631; break;
	case x86_EDI: x86Command = 0x0731; break;
	case x86_ESP: x86Command = 0x0431; break;
	case x86_EBP: x86Command = 0x0531; break;
	}
	switch (Destination) {
	case x86_EAX: x86Command += 0xC000; break;
	case x86_EBX: x86Command += 0xD800; break;
	case x86_ECX: x86Command += 0xC800; break;
	case x86_EDX: x86Command += 0xD000; break;
	case x86_ESI: x86Command += 0xF000; break;
	case x86_EDI: x86Command += 0xF800; break;
	case x86_ESP: x86Command += 0xE000; break;
	case x86_EBP: x86Command += 0xE800; break;
	}
	PUTDST16(*code,x86Command);
}

void XorVariableToX86reg(BYTE** code, void *Variable, char *VariableName, int x86reg) {
	CPU_OR_RSP_Message(*code, "      Xor %s, dword ptr [%s]",x86_Name(x86reg),VariableName);
	switch (x86reg) {
	case x86_EAX: PUTDST16(*code,0x0533); break;
	case x86_EBX: PUTDST16(*code,0x1D33); break;
	case x86_ECX: PUTDST16(*code,0x0D33); break;
	case x86_EDX: PUTDST16(*code,0x1533); break;
	case x86_ESI: PUTDST16(*code,0x3533); break;
	case x86_EDI: PUTDST16(*code,0x3D33); break;
	case x86_ESP: PUTDST16(*code,0x2533); break;
	case x86_EBP: PUTDST16(*code,0x2D33); break;
	default: DisplayError("XorVariableToX86reg\nUnknown x86 Register");
	}
    PUTDST32(*code,Variable);
}

void XorX86RegToVariable(BYTE** code, void* Variable, char* VariableName, int x86reg) {
	CPU_OR_RSP_Message(*code, "      xor dword ptr [%s], %s", VariableName, x86_Name(x86reg));
	switch (x86reg) {
	case x86_EAX: PUTDST16(*code, 0x0531); break;
	case x86_EBX: PUTDST16(*code, 0x1D31); break;
	case x86_ECX: PUTDST16(*code, 0x0D31); break;
	case x86_EDX: PUTDST16(*code, 0x1531); break;
	case x86_ESI: PUTDST16(*code, 0x3531); break;
	case x86_EDI: PUTDST16(*code, 0x3D31); break;
	case x86_ESP: PUTDST16(*code, 0x2531); break;
	case x86_EBP: PUTDST16(*code, 0x2D31); break;
	default: DisplayError("XorX86RegToVariable\nUnknown x86 Register");
	}
	PUTDST32(*code, Variable);
}

void XorConstToVariable(BYTE** code, void* Variable, char* VariableName, DWORD Const) {

	CPU_OR_RSP_Message(*code, "      xor dword ptr [%s], 0x%X", VariableName, Const);

	if ((Const & 0xFFFFFF80) != 0 && (Const & 0xFFFFFF80) != 0xFFFFFF80) {
		PUTDST16(*code, 0x3581);
		PUTDST32(*code, Variable);
		PUTDST32(*code, Const);
	} else {
		PUTDST16(*code, 0x3583);
		PUTDST32(*code, Variable);
		PUTDST8(*code, Const);
	}
}
