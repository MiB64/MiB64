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
enum x86RegValues {
	x86_Any	= 0,x86_EAX,x86_EBX,x86_ECX,x86_EDX,x86_ESI,x86_EDI,x86_EBP, x86_ESP, x86_Any8Bit
};

enum x86FpuValues {
	x86_ST0,x86_ST1,x86_ST2,x86_ST3,x86_ST4,x86_ST5,x86_ST6,x86_ST7
};

#define x86_Name(Reg)   (Reg) == x86_EAX  ? "eax" : (Reg) == x86_EBX  ? "ebx" :\
						(Reg) == x86_ECX  ? "ecx" : (Reg) == x86_EDX  ? "edx" :\
						(Reg) == x86_ESI  ? "esi" :	(Reg) == x86_EDI  ? "edi" :\
						(Reg) == x86_ESP  ? "esp" : (Reg) == x86_EBP  ? "ebp" :\
						"Unknown x86 Register"

#define x86Byte_Name(Reg)	(Reg) == x86_EAX  ? "al" : (Reg) == x86_EBX  ? "bl" :\
							(Reg) == x86_ECX  ? "cl" : (Reg) == x86_EDX  ? "dl" :\
							"Unknown x86 Register"

#define x86Half_Name(Reg)   (Reg) == x86_EAX  ? "ax" : (Reg) == x86_EBX  ? "bx" :\
							(Reg) == x86_ECX  ? "cx" : (Reg) == x86_EDX  ? "dx" :\
							(Reg) == x86_ESI  ? "si" :	(Reg) == x86_EDI  ? "di" :\
							(Reg) == x86_ESP  ? "sp" : (Reg) == x86_EBP  ? "bp" :\
							"Unknown x86 Register"

void AdcX86regToVariable             ( BYTE** code, int x86reg, void * Variable, char * VariableName );
void AdcConstToVariable              ( BYTE** code, void *Variable, char *VariableName, BYTE Constant );
void AdcConstToX86Reg                ( BYTE** code, int x86Reg, DWORD Const );
void AdcVariableToX86reg             ( BYTE** code, int x86reg, void * Variable, char * VariableName );
void AdcX86RegToX86Reg               ( BYTE** code, int Destination, int Source );
void AddConstToVariable              ( BYTE** code, DWORD Const, void *Variable, char *VariableName );
void AddConstToX86Reg                ( BYTE** code, int x86Reg, DWORD Const );
void AddVariableToX86reg             ( BYTE** code, int x86reg, void * Variable, char * VariableName );
void AddX86regToVariable             ( BYTE** code, int x86reg, void * Variable, char * VariableName );
void AddX86RegToX86Reg               ( BYTE** code, int Destination, int Source );
void AndConstToVariable              ( BYTE** code, DWORD Const, void *Variable, char *VariableName );
void AndConstToX86Reg                ( BYTE** code, int x86Reg, DWORD Const );
void AndVariableToX86Reg             ( BYTE** code, void * Variable, char * VariableName, int x86Reg );
void AndVariableDispToX86Reg         ( BYTE** code, void * Variable, char * VariableName, int x86Reg, int AddrReg, int Multiplier);
void AndX86RegToVariable             ( BYTE** code, void * Variable, char * VariableName, int x86Reg );
void AndX86RegToX86Reg               ( BYTE** code, int Destination, int Source );
void BreakPoint                      ( BYTE** code );
void Call_Direct                     ( BYTE** code, void * FunctAddress, char * FunctName );
void Call_Indirect                   ( BYTE** code, void * FunctAddress, char * FunctName );
void CompConstToVariable             ( BYTE** code, DWORD Const, void * Variable, char * VariableName );
void CompConstToX86reg               ( BYTE** code, int x86Reg, DWORD Const );
void CompX86regToVariable            ( BYTE** code, int x86Reg, void * Variable, char * VariableName );
void CompVariableToX86reg	         ( BYTE** code, int x86Reg, void * Variable, char * VariableName );
void CompX86RegToX86Reg              ( BYTE** code, int Destination, int Source );
void DecX86reg                       ( BYTE** code, int x86Reg );
void DivX86reg                       ( BYTE** code, int x86reg );
void idivX86reg                      ( BYTE** code, int x86reg );
void imulX86reg                      ( BYTE** code, int x86reg );
void IncX86reg                       ( BYTE** code, int x86Reg );
void JaeLabel8                       ( BYTE** code, char * Label, BYTE Value );
void JaeLabel32                      ( BYTE** code, char * Label, DWORD Value );
void JaLabel8                        ( BYTE** code, char * Label, BYTE Value );
void JaLabel32                       ( BYTE** code, char * Label, DWORD Value );
void JbLabel8                        ( BYTE** code, char * Label, BYTE Value );
void JbLabel32                       ( BYTE** code, char * Label, DWORD Value );
void JecxzLabel8                     ( BYTE** code, char * Label, BYTE Value );
void JeLabel8                        ( BYTE** code, char * Label, BYTE Value );
void JeLabel32                       ( BYTE** code, char * Label, DWORD Value );
void JgeLabel32                      ( BYTE** code, char * Label, DWORD Value );
void JgLabel8                        ( BYTE** code, char * Label, BYTE Value );
void JgLabel32                       ( BYTE** code, char * Label, DWORD Value );
void JleLabel8                       ( BYTE** code, char * Label, BYTE Value );
void JleLabel32                      ( BYTE** code, char * Label, DWORD Value );
void JlLabel8                        ( BYTE** code, char * Label, BYTE Value );
void JlLabel32                       ( BYTE** code, char * Label, DWORD Value );
void JmpDirectReg                    ( BYTE** code, int x86reg );
void JmpIndirectLabel32              ( BYTE** code, char * Label, DWORD location );
void JmpIndirectReg                  ( BYTE** code, int x86reg );
void JmpLabel8                       ( BYTE** code, char * Label, BYTE Value );
void JmpLabel32                      ( BYTE** code, char * Label, DWORD Value );
void JneLabel8                       ( BYTE** code, char * Label, BYTE Value );
void JneLabel32                      ( BYTE** code, char * Label, DWORD Value );
void JnsLabel8                       ( BYTE** code, char * Label, BYTE Value );
void JnsLabel32                      ( BYTE** code, char * Label, DWORD Value );
void JsLabel32                       ( BYTE** code, char * Label, DWORD Value );
void LeaRegReg                       ( BYTE** code, int x86RegDest, int x86RegSrc, int multiplier );
void LeaSourceAndOffset              ( BYTE** code, int x86DestReg, int x86SourceReg, int offset );
void MoveConstByteToN64Mem           ( BYTE** code, BYTE Const, int AddrReg );
void MoveConstHalfToN64Mem           ( BYTE** code, WORD Const, int AddrReg );
void MoveConstByteToVariable         ( BYTE** code, BYTE Const,void *Variable, char *VariableName );
void MoveConstByteToX86regPointer    ( BYTE** code, BYTE Const, int AddrReg1, int AddrReg2 );
void MoveConstHalfToVariable         ( BYTE** code, WORD Const, void *Variable, char *VariableName );
void MoveConstHalfToX86regPointer    ( BYTE** code, WORD Const, int AddrReg1, int AddrReg2 );
void MoveConstToMemoryDisp           ( BYTE** code, DWORD Const, int AddrReg, DWORD Disp );
void MoveConstToN64Mem               ( BYTE** code, DWORD Const, int AddrReg );
void MoveConstToN64MemDisp           ( BYTE** code, DWORD Const, int AddrReg, BYTE Disp );
void MoveConstToVariable             ( BYTE** code, DWORD Const, void *Variable, char *VariableName );
void MoveConstToX86Pointer           ( BYTE** code, DWORD Const, int X86Pointer );
void MoveConstToX86reg               ( BYTE** code, DWORD Const, int x86reg );
void MoveConstToX86regPointer        ( BYTE** code, DWORD Const, int AddrReg1, int AddrReg2 );
void MoveN64MemDispToX86reg          ( BYTE** code, int x86reg, int AddrReg, BYTE Disp );
void MoveN64MemToX86reg              ( BYTE** code, int x86reg, int AddrReg );
void MoveN64MemToX86regByte          ( BYTE** code, int x86reg, int AddrReg );
void MoveN64MemToX86regHalf          ( BYTE** code, int x86reg, int AddrReg );
void MoveDMemToX86reg                ( BYTE** code, int x86reg, int AddrReg );
void MoveDMemToX86regByte            ( BYTE** code, int x86reg, int AddrReg );
void MoveSxByteX86regPointerToX86reg ( BYTE** code, int AddrReg1, int AddrReg2, int x86reg );
void MoveSxHalfX86regPointerToX86reg ( BYTE** code, int AddrReg1, int AddrReg2, int x86reg );
void MoveSxN64MemToX86regByte        ( BYTE** code, int x86reg, int AddrReg );
void MoveSxN64MemToX86regHalf        ( BYTE** code, int x86reg, int AddrReg );
void MoveSxDMemToX86regByte          ( BYTE** code, int x86reg, int AddrReg );
void MoveSxDMemToX86regHalf          ( BYTE** code, int x86reg, int AddrReg );
void MoveSxVariableToX86regByte      ( BYTE** code, void *Variable, char *VariableName, int x86reg );
void MoveSxVariableToX86regHalf      ( BYTE** code, void *Variable, char *VariableName, int x86reg );
void MoveVariableDispToX86Reg        ( BYTE** code, void *Variable, char *VariableName, int x86Reg, int AddrReg, int Multiplier );
void MoveVariableToX86reg            ( BYTE** code, void *Variable, char *VariableName, int x86reg );
void MoveVariableToX86regByte        ( BYTE** code, void *Variable, char *VariableName, int x86reg );
void MoveVariableToX86regHalf        ( BYTE** code, void *Variable, char *VariableName, int x86reg );
void MoveX86PointerToX86reg          ( BYTE** code, int x86reg, int X86Pointer );
void MoveX86regByteToN64Mem          ( BYTE** code, int x86reg, int AddrReg );
void MoveX86regByteToVariable        ( BYTE** code, int x86reg, void * Variable, char * VariableName );
void MoveX86regByteToX86regPointer   ( BYTE** code, int x86reg, int AddrReg1, int AddrReg2 );
void MoveX86regHalfToN64Mem          ( BYTE** code, int x86reg, int AddrReg );
void MoveX86regHalfToVariable        ( BYTE** code, int x86reg, void * Variable, char * VariableName );
void MoveX86regHalfToX86regPointer   ( BYTE** code, int x86reg, int AddrReg1, int AddrReg2 );
void MoveX86regPointerToX86reg       ( BYTE** code, int AddrReg1, int AddrReg2, int x86reg );
void MoveX86regPointerToX86regDisp8  ( BYTE** code, int AddrReg1, int AddrReg2, int x86reg, BYTE offset );
void MoveX86regToMemory              ( BYTE** code, int x86reg, int AddrReg, DWORD Disp );
void MoveX86regToN64Mem              ( BYTE** code, int x86reg, int AddrReg );
void MoveX86regToN64MemDisp          ( BYTE** code, int x86reg, int AddrReg, BYTE Disp );
void MoveX86regToVariable            ( BYTE** code, int x86reg, void * Variable, char * VariableName );
void MoveX86RegToX86Reg              ( BYTE** code, int Source, int Destination );
void MoveX86regToX86Pointer          ( BYTE** code, int x86reg, int X86Pointer );
void MoveX86regToX86regPointer       ( BYTE** code, int x86reg, int AddrReg1, int AddrReg2 );
void MoveZxByteX86regPointerToX86reg ( BYTE** code, int AddrReg1, int AddrReg2, int x86reg );
void MoveZxHalfX86regPointerToX86reg ( BYTE** code, int AddrReg1, int AddrReg2, int x86reg );
void MoveZxN64MemToX86regByte        ( BYTE** code, int x86reg, int AddrReg );
void MoveZxDMemToX86regByte			 ( BYTE** code, int x86reg, int AddrReg );
void MoveZxN64MemToX86regHalf        ( BYTE** code, int x86reg, int AddrReg );
void MoveZxDMemToX86regHalf          ( BYTE** code, int x86reg, int AddrReg );
void MoveZxVariableToX86regByte      ( BYTE** code, void *Variable, char *VariableName, int x86reg );
void MoveZxVariableToX86regHalf      ( BYTE** code, void *Variable, char *VariableName, int x86reg );
void MulX86reg                       ( BYTE** code, int x86reg );
void NotX86Reg                       ( BYTE** code, int x86Reg );
void OrConstToVariable               ( BYTE** code, DWORD Const, void * Variable, char * VariableName );
void OrConstToX86Reg                 ( BYTE** code, DWORD Const, int  x86Reg );
void OrVariableToX86Reg              ( BYTE** code, void * Variable, char * VariableName, int x86Reg );
void OrX86RegToVariable              ( BYTE** code, void * Variable, char * VariableName, int x86Reg );
void OrX86RegToX86Reg                ( BYTE** code, int Destination, int Source );
void Popad                           ( BYTE** code );
void Pushad                          ( BYTE** code );
void Push					         ( BYTE** code, int x86reg );
void Pop					         ( BYTE** code, int x86reg );
void PushImm32                       ( BYTE** code, char * String, DWORD Value );
void Ret                             ( BYTE** code );
void Seta                            ( BYTE** code, int x86reg );
void Setae                           ( BYTE** code, int x86reg );
void SetaVariable                    ( BYTE** code, void * Variable, char * VariableName );
void Setb                            ( BYTE** code, int x86reg );
void SetbVariable                    ( BYTE** code, void * Variable, char * VariableName );
void Setg                            ( BYTE** code, int x86reg );
void SetgVariable                    ( BYTE** code, void * Variable, char * VariableName );
void SetgeVariable					 ( BYTE** code, void * Variable, char * VariableName );
void Setl                            ( BYTE** code, int x86reg );
void SetlVariable                    ( BYTE** code, void * Variable, char * VariableName );
void Setz					         ( BYTE** code, int x86reg );
void SetzVariable					 ( BYTE** code, void * Variable, char * VariableName );
void SetleVariable					 ( BYTE** code, void * Variable, char * VariableName);
void Setnz					         ( BYTE** code, int x86reg );
void SetnzVariable					 ( BYTE** code, void * Variable, char * VariableName );
void ShiftLeftDouble                 ( BYTE** code, int Destination, int Source );
void ShiftLeftDoubleImmed            ( BYTE** code, int Destination, int Source, BYTE Immediate );
void ShiftLeftSign                   ( BYTE** code, int x86reg );
void ShiftLeftSignImmed              ( BYTE** code, int x86reg, BYTE Immediate );
void ShiftLeftSignVariableImmed      ( BYTE** code, void * Variable, char * VariableName, BYTE Immediate );
void ShiftRightDouble                ( BYTE** code, int Destination, int Source );
void ShiftRightDoubleImmed           ( BYTE** code, int Destination, int Source, BYTE Immediate );
void ShiftRightSign                  ( BYTE** code, int x86reg );
void ShiftRightSignImmed             ( BYTE** code, int x86reg, BYTE Immediate );
void ShiftRightSignVariableImmed     ( BYTE** code, void * Variable, char * VariableName, BYTE Immediate );
void ShiftRightUnsign                ( BYTE** code, int x86reg );
void ShiftRightUnsignImmed           ( BYTE** code, int x86reg, BYTE Immediate );
void ShiftRightUnsignVariableImmed   ( BYTE** code, void * Variable, char * VariableName, BYTE Immediate );
void SbbConstFromX86Reg              ( BYTE** code, int x86Reg, DWORD Const );
void SbbVariableFromX86reg           ( BYTE** code, int x86reg, void * Variable, char * VariableName );
void SbbX86RegToX86Reg               ( BYTE** code, int Destination, int Source );
void SubConstFromVariable            ( BYTE** code, DWORD Const, void *Variable, char *VariableName );
void SubConstFromX86Reg              ( BYTE** code, int x86Reg, DWORD Const );
void SubVariableFromX86reg           ( BYTE** code, int x86reg, void * Variable, char * VariableName );
void SubX86RegToX86Reg               ( BYTE** code, int Destination, int Source );
void SubX86regFromVariable           ( BYTE** code, int x86reg, void * Variable, char * VariableName );
void TestConstToX86Reg               ( BYTE** code, DWORD Const, int x86reg );
void TestVariable                    ( BYTE** code, DWORD Const, void * Variable, char * VariableName );
void TestX86RegToX86Reg              ( BYTE** code, int Destination, int Source );
void XorConstToX86Reg                ( BYTE** code, int x86Reg, DWORD Const );
void XorX86RegToX86Reg               ( BYTE** code, int Source, int Destination );
void XorVariableToX86reg             ( BYTE** code, void * Variable, char * VariableName, int x86reg );
void XorX86RegToVariable             ( BYTE** code, void * Variable, char * VariableName, int x86reg );
void XorConstToVariable              ( BYTE** code, void * Variable, char * VariableName, DWORD Const );


void fpuAbs					         ( BYTE** code );
void fpuAddDword			         ( BYTE** code, void *Variable, char *VariableName );
void fpuAddDwordRegPointer           ( BYTE** code, int x86Pointer );
void fpuAddQword			         ( BYTE** code, void *Variable, char *VariableName );
void fpuAddQwordRegPointer           ( BYTE** code, int x86Pointer );
void fpuAddReg				         ( BYTE** code, int x86reg );
void fpuAddRegPop			         ( BYTE** code, int * StackPos, int x86reg );
void fpuComDword			         ( BYTE** code, void *Variable, char *VariableName, BOOL Pop );
void fpuComDwordRegPointer           ( BYTE** code, int x86Pointer, BOOL Pop );
void fpuComQword			         ( BYTE** code, void *Variable, char *VariableName, BOOL Pop );
void fpuComQwordRegPointer           ( BYTE** code, int x86Pointer, BOOL Pop );
void fpuComReg                       ( BYTE** code, int x86reg, BOOL Pop );
void fpuDivDword			         ( BYTE** code, void *Variable, char *VariableName );
void fpuDivDwordRegPointer           ( BYTE** code, int x86Pointer );
void fpuDivQword			         ( BYTE** code, void *Variable, char *VariableName );
void fpuDivQwordRegPointer           ( BYTE** code, int x86Pointer );
void fpuDivReg                       ( BYTE** code, int Reg );
void fpuDivRegPop			         ( BYTE** code, int x86reg );
void fpuExchange                     ( BYTE** code, int Reg );
void fpuFree                         ( BYTE** code, int Reg );
void fpuDecStack                     ( BYTE** code, int * StackPos );
void fpuIncStack                     ( BYTE** code, int * StackPos );
void fpuLoadControl			         ( BYTE** code, void *Variable, char *VariableName );
void fpuLoadDword			         ( BYTE** code, int * StackPos, void *Variable, char *VariableName );
void fpuLoadDwordFromX86Reg          ( BYTE** code, int * StackPos, int x86reg );
void fpuLoadDwordFromN64Mem          ( BYTE** code, int * StackPos, int x86reg );
void fpuLoadInt32bFromN64Mem         ( BYTE** code, int * StackPos, int x86reg );
void fpuLoadIntegerDword	         ( BYTE** code, int * StackPos, void *Variable, char *VariableName );
void fpuLoadIntegerDwordFromX86Reg   ( BYTE** code, int * StackPos,int x86Reg );
void fpuLoadIntegerQword	         ( BYTE** code, int * StackPos, void *Variable, char *VariableName );
void fpuLoadIntegerQwordFromX86Reg   ( BYTE** code, int * StackPos,int x86Reg );
void fpuLoadQword			         ( BYTE** code, int * StackPos, void *Variable, char *VariableName );
void fpuLoadQwordFromX86Reg          ( BYTE** code, int * StackPos, int x86Reg );
void fpuLoadQwordFromN64Mem          ( BYTE** code, int * StackPos, int x86reg );
void fpuLoadReg                      ( BYTE** code, int * StackPos, int Reg );
void fpuMulDword                     ( BYTE** code, void *Variable, char *VariableName);
void fpuMulDwordRegPointer           ( BYTE** code, int x86Pointer );
void fpuMulQword                     ( BYTE** code, void *Variable, char *VariableName);
void fpuMulQwordRegPointer           ( BYTE** code, int x86Pointer );
void fpuMulReg                       ( BYTE** code, int x86reg );
void fpuMulRegPop                    ( BYTE** code, int x86reg );
void fpuNeg					         ( BYTE** code );
void fpuRound				         ( BYTE** code );
void fpuSqrt				         ( BYTE** code );
void fpuStoreControl		         ( BYTE** code, void *Variable, char *VariableName );
void fpuStoreDword			         ( BYTE** code, int * StackPos, void *Variable, char *VariableName, BOOL pop );
void fpuStoreDwordFromX86Reg         ( BYTE** code, int * StackPos,int x86Reg, BOOL pop );
void fpuStoreDwordToN64Mem	         ( BYTE** code, int * StackPos, int x86reg, BOOL Pop );
void fpuStoreIntegerDword            ( BYTE** code, int * StackPos, void *Variable, char *VariableName, BOOL pop );
void fpuStoreIntegerDwordFromX86Reg  ( BYTE** code, int * StackPos,int x86Reg, BOOL pop );
void fpuStoreIntegerQword            ( BYTE** code, int * StackPos, void *Variable, char *VariableName, BOOL pop );
void fpuStoreIntegerQwordFromX86Reg  ( BYTE** code, int * StackPos, int x86Reg, BOOL pop );
void fpuStoreQwordFromX86Reg         ( BYTE** code, int * StackPos, int x86Reg, BOOL pop );
void fpuStoreStatus			         ( BYTE** code );
void fpuSubDword			         ( BYTE** code, void *Variable, char *VariableName );
void fpuSubDwordRegPointer           ( BYTE** code, int x86Pointer );
void fpuSubDwordReverse              ( BYTE** code, void *Variable, char *VariableName );
void fpuSubQword			         ( BYTE** code, void *Variable, char *VariableName );
void fpuSubQwordRegPointer           ( BYTE** code, int x86Pointer );
void fpuSubQwordReverse              ( BYTE** code, void *Variable, char *VariableName );
void fpuSubReg				         ( BYTE** code, int x86reg );
void fpuSubRegPop			         ( BYTE** code, int x86reg );

void x86_SetBranch32b(void* JumpByte, void* Destination);
