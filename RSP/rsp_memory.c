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
#include "rsp_config.h"
#include "rsp_memory.h"
#include "rsp_registers.h"
#include "../Main.h"

BYTE * RspRecompCode = NULL, * RspRecompCodeSecondary, * RspRecompPos;

int AllocateRspMemory (void) {
	if (RspRecompCode == NULL) {
		RspRecompCode = (BYTE*)VirtualAlloc(NULL, 0x01800004, MEM_RESERVE, PAGE_EXECUTE_READWRITE);
		RspRecompCode = (BYTE*)VirtualAlloc(RspRecompCode, 0x01800000, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	}
	
	if(RspRecompCode == NULL) {
		DisplayError("Not enough memory for RSP RecompCode!");
		return FALSE;
	}

	RspRecompCodeSecondary = (BYTE *)VirtualAlloc( NULL, 0x00200000, MEM_COMMIT, PAGE_EXECUTE_READWRITE );
	if(RspRecompCodeSecondary == NULL) {
		DisplayError("Not enough memory for RSP RecompCode Secondary!");
		return FALSE;
	}

	ClearJumpTables();

	RspRecompPos = RspRecompCode;
	return TRUE;
}

/*void FreeMemory (void) {
	VirtualFree( RecompCode, 0 , MEM_RELEASE);
	VirtualFree( JumpTable, 0 , MEM_RELEASE);
	VirtualFree( RecompCodeSecondary, 0 , MEM_RELEASE);
}*/

void RSP_LB_DMEM ( DWORD Addr, BYTE * Value ) {
	* Value = *(BYTE *)(DMEM + ((Addr ^ 3) & 0xFFF)) ;
}

void RSP_LBV_DMEM ( DWORD Addr, int vect, int element ) {
	RSP_Vect[vect].B[15 - element] = *(DMEM + ((Addr ^ 3) & 0xFFF));
}

void RSP_LDV_DMEM ( DWORD Addr, int vect, int element ) {
	int length, Count;
	
	length = 8;
	if (length > 16 - element) {
		length = 16 - element;
	}
	for (Count = element; Count < (length + element); Count ++ ){
		RSP_Vect[vect].B[15 - Count] = *(DMEM + ((Addr ^ 3) & 0xFFF));
		Addr += 1;
	}

}

void RSP_LFV_DMEM ( DWORD Addr, int vect, int element ) {
	int length, count;
	VECTOR Temp;
	int alignedAddress = Addr & ~7;
	int misalignment = Addr & 7;

	length = 8;
	if (length > 16 - element) {
		length = 16 - element;
	}

	Temp.HW[7] = ((short)*(DMEM + (alignedAddress + ((misalignment + element) & 0xF) ^ 3))) << 7;
	Temp.HW[6] = ((short)*(DMEM + (alignedAddress + ((misalignment + 4 - element) & 0xF) ^ 3))) << 7;
	Temp.HW[5] = ((short)*(DMEM + (alignedAddress + ((misalignment + 8 - element) & 0xF) ^ 3))) << 7;
	Temp.HW[4] = ((short)*(DMEM + (alignedAddress + ((misalignment + 12 - element) & 0xF) ^ 3))) << 7;
	Temp.HW[3] = ((short)*(DMEM + (alignedAddress + ((misalignment + 8 - element) & 0xF) ^ 3))) << 7;
	Temp.HW[2] = ((short)*(DMEM + (alignedAddress + ((misalignment + 12 - element) & 0xF) ^ 3))) << 7;
	Temp.HW[1] = ((short)*(DMEM + (alignedAddress + ((misalignment - element) & 0xF) ^ 3))) << 7;
	Temp.HW[0] = ((short)*(DMEM + (alignedAddress + ((misalignment + 4 - element) & 0xF) ^ 3))) << 7;

	for (count = element; count < (length + element); count++) {
		RSP_Vect[vect].B[15 - count] = Temp.B[15 - count];
	}
}

void RSP_LH_DMEM ( DWORD Addr, WORD * Value ) {
	if ((Addr & 0x1) != 0) {
		Addr &= 0xFFF;
		*Value = *(BYTE *)(DMEM + (Addr^ 3)) << 8;		
		*Value += *(BYTE *)(DMEM + (((Addr + 1) & 0xFFF)^ 3));
		return;
	}
	* Value = *(WORD *)(DMEM + ((Addr ^ 2 ) & 0xFFF));	
}

void RSP_LHV_DMEM ( DWORD Addr, int vect, int element ) {
	int alignedAddr = Addr & ~7;
	int misalignment = Addr & 7;

	for (int i = 0; i < 8; ++i) {
		RSP_Vect[vect].HW[7-i] = *(DMEM + ((alignedAddr + ((misalignment - element + i*2) & 0xF) ^ 3) & 0xFFF)) << 7;
	}
}

void RSP_LLV_DMEM ( DWORD Addr, int vect, int element ) {
	int length, Count;
	
	length = 4;
	if (length > 16 - element) {
		length = 16 - element;
	}
	for (Count = element; Count < (length + element); Count ++ ){
		RSP_Vect[vect].B[15 - Count] = *(DMEM + ((Addr ^ 3) & 0xFFF));
		Addr += 1;
	}

}

void RSP_LPV_DMEM ( DWORD Addr, int vect, int element ) {	
	int alignedAddr = Addr & ~7;
	int misalignment = Addr & 7;
	for (int i = 0; i < 8; ++i) {
		int elementOffset = (0x10 - element + i + misalignment) & 0xF;
		int elemAddr = alignedAddr + elementOffset;
		RSP_Vect[vect].HW[7 - i] = *(DMEM + ((elemAddr & 0xFFF) ^ 3)) << 8;
	}
}

void RSP_LRV_DMEM ( DWORD Addr, int vect, int element ) {	
	int length, Count, offset;

	offset = (Addr & 0xF) - 1;
	length = (Addr & 0xF) - element;
	Addr &= 0xFF0;
	for (Count = element; Count < (length + element); Count ++ ){
		RSP_Vect[vect].B[offset - Count] = *(DMEM + ((Addr ^ 3) & 0xFFF));
		Addr += 1;
	}

}

void RSP_LQV_DMEM ( DWORD Addr, int vect, int element ) {
	int length, Count;
	
	length = ((Addr + 0x10) & ~0xF) - Addr;
	if (length > 16 - element) {
		length = 16 - element;
	}
	for (Count = element; Count < (length + element); Count ++ ){
		RSP_Vect[vect].B[15 - Count] = *(DMEM + ((Addr ^ 3) & 0xFFF));
		Addr += 1;
	}

}

void RSP_LSV_DMEM ( DWORD Addr, int vect, int element ) {
	int length, Count;
	
	length = 2;
	if (length > 16 - element) {
		length = 16 - element;
	}
	for (Count = element; Count < (length + element); Count ++ ){
		RSP_Vect[vect].B[15 - Count] = *(DMEM + ((Addr ^ 3) & 0xFFF));
		Addr += 1;
	}
}

void RSP_LTV_DMEM ( DWORD Addr, int vect, int element ) {
	int firstReg = (vect & ~7);
	int regOffset = element / 2;
	int currentAddress = (Addr & ~7) + ((element + (Addr & 8)) & 15);
	int maxAddress = (Addr & ~7) + 16;
		
	for(int count = 0; count < 8; count++) {
		RSP_Vect[firstReg + regOffset].B[15 - (count * 2)]     = *(DMEM + (currentAddress++ ^ 3));
		if (currentAddress == maxAddress) currentAddress = Addr & ~7;
		RSP_Vect[firstReg + regOffset].B[15 - ((count * 2)+1)] = *(DMEM + (currentAddress++ ^ 3));
		if (currentAddress == maxAddress) currentAddress = Addr & ~7;
		regOffset = (regOffset + 1) & 7;
	}
}

void RSP_LUV_DMEM ( DWORD Addr, int vect, int element ) {
	int alignedAddr = Addr & ~7;
	int misalignment = Addr & 7;
	for (int i = 0; i < 8; ++i) {
		int elementOffset = (0x10 - element + i + misalignment) & 0xF;
		int elemAddr = alignedAddr + elementOffset;
		RSP_Vect[vect].HW[7-i] = *(DMEM + ((elemAddr & 0xFFF) ^ 3)) << 7;
	}
}

void RSP_LW_DMEM ( DWORD Addr, DWORD * Value ) {
	if ((Addr & 0x3) != 0) {
		Addr &= 0xFFF;
		*Value = *(BYTE *)(DMEM + (Addr^ 3)) << 0x18;
		*Value += *(BYTE *)(DMEM + (((Addr + 1) & 0xFFF)^ 3)) << 0x10;
		*Value += *(BYTE *)(DMEM + (((Addr + 2) & 0xFFF)^ 3)) << 8;
		*Value += *(BYTE *)(DMEM + (((Addr + 3) & 0xFFF)^ 3));
		return;
	}
	* Value = *(DWORD *)(DMEM + (Addr & 0xFFF));
}

void RSP_LW_IMEM ( DWORD Addr, DWORD * Value ) {
	if ((Addr & 0x3) != 0) {
		DisplayError("Unaligned RSP_LW_IMEM");
	}
	* Value = *(DWORD *)(IMEM + (Addr & 0xFFF));
}

void RSP_SB_DMEM ( DWORD Addr, BYTE Value ) {
	*(BYTE *)(DMEM + ((Addr ^ 3) & 0xFFF)) = Value;
}

void RSP_SBV_DMEM ( DWORD Addr, int vect, int element ) {
	*(DMEM + ((Addr ^ 3) & 0xFFF)) = RSP_Vect[vect].B[15 - element];
}

void RSP_SDV_DMEM ( DWORD Addr, int vect, int element ) {
	int Count;

	for (Count = element; Count < (8 + element); Count ++ ){
		*(DMEM + ((Addr ^ 3) & 0xFFF)) = RSP_Vect[vect].B[15 - (Count & 0xF)];
		Addr += 1;
	}
}

void RSP_SFV_DMEM ( DWORD Addr, int vect, int element ) {	
	int offset = Addr & 0x7;
	Addr &= 0xFF8;

	switch (element) {
	case 0:
		*(DMEM + ((Addr + ((offset    )       ))^3)) = (BYTE)(RSP_Vect[vect].UHW[7] >> 7);
		*(DMEM + ((Addr + ((offset + 4)  & 0xF))^3)) = (BYTE)(RSP_Vect[vect].UHW[6] >> 7);
		*(DMEM + ((Addr + ((offset + 8)  & 0xF))^3)) = (BYTE)(RSP_Vect[vect].UHW[5] >> 7);
		*(DMEM + ((Addr + ((offset + 12) & 0xF))^3)) = (BYTE)(RSP_Vect[vect].UHW[4] >> 7);
		break;
	case 1:
		*(DMEM + ((Addr + ((offset    )       ))^3)) = (BYTE)(RSP_Vect[vect].UHW[1] >> 7);
		*(DMEM + ((Addr + ((offset + 4)  & 0xF))^3)) = (BYTE)(RSP_Vect[vect].UHW[0] >> 7);
		*(DMEM + ((Addr + ((offset + 8)  & 0xF))^3)) = (BYTE)(RSP_Vect[vect].UHW[3] >> 7);
		*(DMEM + ((Addr + ((offset + 12) & 0xF))^3)) = (BYTE)(RSP_Vect[vect].UHW[2] >> 7);
		break;
	case 2:
		*(DMEM + ((Addr + ((offset     )      ))^3)) = 0;
		*(DMEM + ((Addr + ((offset + 4 ) & 0xF))^3)) = 0;
		*(DMEM + ((Addr + ((offset + 8 ) & 0xF))^3)) = 0;
		*(DMEM + ((Addr + ((offset + 12) & 0xF))^3)) = 0;
		break;
	case 3:
		*(DMEM + ((Addr + ((offset     )      ))^3)) = 0;
		*(DMEM + ((Addr + ((offset + 4 ) & 0xF))^3)) = 0;
		*(DMEM + ((Addr + ((offset + 8 ) & 0xF))^3)) = 0;
		*(DMEM + ((Addr + ((offset + 12) & 0xF)^3))) = 0;
		break;
	case 4:
		*(DMEM + ((Addr + ((offset     )      ))^3)) = (BYTE)(RSP_Vect[vect].UHW[6] >> 7);
		*(DMEM + ((Addr + ((offset + 4 ) & 0xF))^3)) = (BYTE)(RSP_Vect[vect].UHW[5] >> 7);
		*(DMEM + ((Addr + ((offset + 8 ) & 0xF))^3)) = (BYTE)(RSP_Vect[vect].UHW[4] >> 7);
		*(DMEM + ((Addr + ((offset + 12) & 0xF))^3)) = (BYTE)(RSP_Vect[vect].UHW[7] >> 7);
		break;
	case 5:
		*(DMEM + ((Addr + ((offset     )      ))^3)) = (BYTE)(RSP_Vect[vect].UHW[0] >> 7);
		*(DMEM + ((Addr + ((offset + 4 ) & 0xF))^3)) = (BYTE)(RSP_Vect[vect].UHW[3] >> 7);
		*(DMEM + ((Addr + ((offset + 8 ) & 0xF))^3)) = (BYTE)(RSP_Vect[vect].UHW[2] >> 7);
		*(DMEM + ((Addr + ((offset + 12) & 0xF))^3)) = (BYTE)(RSP_Vect[vect].UHW[1] >> 7);
		break;
	case 6:
		*(DMEM + ((Addr + ((offset     )      ))^3)) = 0;
		*(DMEM + ((Addr + ((offset + 4 ) & 0xF))^3)) = 0;
		*(DMEM + ((Addr + ((offset + 8 ) & 0xF))^3)) = 0;
		*(DMEM + ((Addr + ((offset + 12) & 0xF))^3)) = 0;
		break;
	case 7:
		*(DMEM + ((Addr + ((offset     )      ))^3)) = 0;
		*(DMEM + ((Addr + ((offset + 4 ) & 0xF))^3)) = 0;
		*(DMEM + ((Addr + ((offset + 8 ) & 0xF))^3)) = 0;
		*(DMEM + ((Addr + ((offset + 12) & 0xF))^3)) = 0;
		break;
	case 8:
		*(DMEM + ((Addr + ((offset     )      ))^3)) = (BYTE)(RSP_Vect[vect].UHW[3] >> 7);
		*(DMEM + ((Addr + ((offset + 4 ) & 0xF))^3)) = (BYTE)(RSP_Vect[vect].UHW[2] >> 7);
		*(DMEM + ((Addr + ((offset + 8 ) & 0xF))^3)) = (BYTE)(RSP_Vect[vect].UHW[1] >> 7);
		*(DMEM + ((Addr + ((offset + 12) & 0xF))^3)) = (BYTE)(RSP_Vect[vect].UHW[0] >> 7);
		break;
	case 9:
		*(DMEM + ((Addr + ((offset     )      ))^3)) = 0;
		*(DMEM + ((Addr + ((offset + 4 ) & 0xF))^3)) = 0;
		*(DMEM + ((Addr + ((offset + 8 ) & 0xF))^3)) = 0;
		*(DMEM + ((Addr + ((offset + 12) & 0xF))^3)) = 0;
		break;
	case 10:
		*(DMEM + ((Addr + ((offset     )      ))^3)) = 0;
		*(DMEM + ((Addr + ((offset + 4 ) & 0xF))^3)) = 0;
		*(DMEM + ((Addr + ((offset + 8 ) & 0xF))^3)) = 0;
		*(DMEM + ((Addr + ((offset + 12) & 0xF))^3)) = 0;
		break;
	case 11:
		*(DMEM + ((Addr + ((offset     )      ))^3)) = (BYTE)(RSP_Vect[vect].UHW[4] >> 7);
		*(DMEM + ((Addr + ((offset + 4 ) & 0xF))^3)) = (BYTE)(RSP_Vect[vect].UHW[7] >> 7);
		*(DMEM + ((Addr + ((offset + 8 ) & 0xF))^3)) = (BYTE)(RSP_Vect[vect].UHW[6] >> 7);
		*(DMEM + ((Addr + ((offset + 12) & 0xF))^3)) = (BYTE)(RSP_Vect[vect].UHW[5] >> 7);
		break;
	case 12:
		*(DMEM + ((Addr + ((offset     )      ))^3)) = (BYTE)(RSP_Vect[vect].UHW[2] >> 7);
		*(DMEM + ((Addr + ((offset + 4 ) & 0xF))^3)) = (BYTE)(RSP_Vect[vect].UHW[1] >> 7);
		*(DMEM + ((Addr + ((offset + 8 ) & 0xF))^3)) = (BYTE)(RSP_Vect[vect].UHW[0] >> 7);
		*(DMEM + ((Addr + ((offset + 12) & 0xF))^3)) = (BYTE)(RSP_Vect[vect].UHW[3] >> 7);
		break;
	case 13:
		*(DMEM + ((Addr + ((offset     )      ))^3)) = 0;
		*(DMEM + ((Addr + ((offset + 4 ) & 0xF))^3)) = 0;
		*(DMEM + ((Addr + ((offset + 8 ) & 0xF))^3)) = 0;
		*(DMEM + ((Addr + ((offset + 12) & 0xF))^3)) = 0;
		break;
	case 14:
		*(DMEM + ((Addr + ((offset     )      ))^3)) = 0;
		*(DMEM + ((Addr + ((offset + 4 ) & 0xF))^3)) = 0;
		*(DMEM + ((Addr + ((offset + 8 ) & 0xF))^3)) = 0;
		*(DMEM + ((Addr + ((offset + 12) & 0xF))^3)) = 0;
		break;
	case 15:
		*(DMEM + ((Addr + ((offset     )      ))^3)) = (BYTE)(RSP_Vect[vect].UHW[7] >> 7);
		*(DMEM + ((Addr + ((offset + 4 ) & 0xF))^3)) = (BYTE)(RSP_Vect[vect].UHW[6] >> 7);
		*(DMEM + ((Addr + ((offset + 8 ) & 0xF))^3)) = (BYTE)(RSP_Vect[vect].UHW[5] >> 7);
		*(DMEM + ((Addr + ((offset + 12) & 0xF))^3)) = (BYTE)(RSP_Vect[vect].UHW[4] >> 7);
		break;
	}
}

void RSP_SH_DMEM ( DWORD Addr, WORD Value ) {
	if ((Addr & 0x1) != 0) {
		*(BYTE*)(DMEM + (Addr ^ 3)) = (BYTE)(Value >> 0x8);
		*(BYTE*)(DMEM + (((Addr + 1) & 0xFFF) ^ 3)) = (BYTE)(Value);
		return;
	}
	*(WORD *)(DMEM + ((Addr ^ 2) & 0xFFF)) = Value;
}

void RSP_SHV_DMEM ( DWORD Addr, int vect, int element ) {
	int alignedAddr = Addr & ~7;
	int misalignment = Addr & 7;

	for (int i = 0; i < 8; ++i) {
		int indexByte = i * 2 + element;
		int elemAddr = alignedAddr + ((misalignment + i * 2) & 0xF);
		*(DMEM + ((elemAddr ^ 3) & 0xFFF)) = (RSP_Vect[vect].UB[15 - ((indexByte + 0) & 0xF)] << 1) |
											 (RSP_Vect[vect].UB[15 - ((indexByte + 1) & 0xF)] >> 7);
	}
}

void RSP_SLV_DMEM ( DWORD Addr, int vect, int element ) {
	int Count;

	for (Count = element; Count < (4 + element); Count ++ ){
		*(DMEM + ((Addr ^3) & 0xFFF)) = RSP_Vect[vect].B[15 - (Count & 0xF)];
		Addr += 1;
	}
}

void RSP_SPV_DMEM ( DWORD Addr, int vect, int element ) {
	int Count;

	for (Count = element; Count < (8 + element); Count ++ ){
		if (((Count) & 0xF) < 8) {
			*(DMEM + ((Addr ^ 3) & 0xFFF)) = RSP_Vect[vect].UB[15 - ((Count & 0xF) << 1)];
		} else {
			*(DMEM + ((Addr ^ 3) & 0xFFF)) = (RSP_Vect[vect].UB[15 - ((Count & 0x7) << 1)] << 1) +
				(RSP_Vect[vect].UB[14 - ((Count & 0x7) << 1)] >> 7);
		}
		Addr += 1;
	}
}

void RSP_SQV_DMEM ( DWORD Addr, int vect, int element ) {
	int length, Count;
	
	length = ((Addr + 0x10) & ~0xF) - Addr;
	for (Count = element; Count < (length + element); Count ++ ){
		*(DMEM + ((Addr ^ 3) & 0xFFF)) = RSP_Vect[vect].B[15 - (Count & 0xF)];
		Addr += 1;
	}
}

void RSP_SRV_DMEM ( DWORD Addr, int vect, int element ) {
	int length, Count, offset;

	length = (Addr & 0xF);
	offset = (0x10 - length) & 0xF;
	Addr &= 0xFF0;
	for (Count = element; Count < (length + element); Count ++ ){
		*(DMEM + ((Addr ^ 3) & 0xFFF)) = RSP_Vect[vect].B[15 - ((Count + offset) & 0xF)];
		Addr += 1;
	}
}

void RSP_SSV_DMEM ( DWORD Addr, int vect, int element ) {
	int Count;

	for (Count = element; Count < (2 + element); Count ++ ){
		*(DMEM + ((Addr ^ 3) & 0xFFF)) = RSP_Vect[vect].B[15 - (Count & 0xF)];
		Addr += 1;
	}
}

void RSP_STV_DMEM ( DWORD Addr, int vect, int element ) {
	int firstReg = vect & ~7;
	int lastReg = firstReg + 7;
	int currentElement = 16 - (element & ~1);
	int alignedAddress = Addr & ~7;
	int offset = (Addr & 7) - (element & ~1);

	for (int reg = firstReg; reg <= lastReg; ++reg) {
		*(DMEM + (((alignedAddress + (offset++ & 15)) ^ 3) & 0xFFF)) = RSP_Vect[reg].UB[15 - (currentElement++ & 15)];
		*(DMEM + (((alignedAddress + (offset++ & 15)) ^ 3) & 0xFFF)) = RSP_Vect[reg].UB[15 - (currentElement++ & 15)];
	}
}

void RSP_SUV_DMEM ( DWORD Addr, int vect, int element ) {
	int Count;

	for (Count = element; Count < (8 + element); Count ++ ){
		if (((Count) & 0xF) < 8) {
			*(DMEM + ((Addr ^ 3) & 0xFFF)) = (RSP_Vect[vect].UB[15 - ((Count & 0x7) << 1)] << 1) +
				(RSP_Vect[vect].UB[14 - ((Count & 0x7) << 1)] >> 7);
		} else {
			*(DMEM + ((Addr ^ 3) & 0xFFF)) = RSP_Vect[vect].UB[15 - ((Count & 0x7) << 1)];
		}
		Addr += 1;
	}
}

void RSP_SW_DMEM ( DWORD Addr, DWORD Value ) {
	Addr &= 0xFFF;
	if ((Addr & 0x3) != 0) {
		*(BYTE *)(DMEM + (Addr ^ 3)) = (BYTE)(Value >> 0x18);
		*(BYTE *)(DMEM + (((Addr + 1) & 0xFFF) ^ 3)) = (BYTE)(Value >> 0x10);
		*(BYTE *)(DMEM + (((Addr + 2) & 0xFFF) ^ 3)) = (BYTE)(Value >> 0x8);
		*(BYTE *)(DMEM + (((Addr + 3) & 0xFFF) ^ 3)) = (BYTE)(Value);
		return;
	}
	*(DWORD *)(DMEM + Addr) = Value;
}

void RSP_SWV_DMEM ( DWORD Addr, int vect, int element ) {
	int Count, offset;

	offset = Addr & 7;
	Addr &= ~7;
	for (Count = element; Count < (16 + element); Count ++ ){
		*(DMEM + ((Addr + (offset & 0xF)) ^ 3)) = RSP_Vect[vect].B[15 - (Count & 0xF)];
		offset += 1;
	}
}
