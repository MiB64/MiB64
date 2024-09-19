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

enum mmxRegValues {
	x86_MM0 = 0, x86_MM1 = 1, x86_MM2 = 2, x86_MM3 = 3,
	x86_MM4 = 4, x86_MM5 = 5, x86_MM6 = 6, x86_MM7 = 7
};

enum sseRegValues {
	x86_XMM0 = 0, x86_XMM1 = 1, x86_XMM2 = 2, x86_XMM3 = 3,
	x86_XMM4 = 4, x86_XMM5 = 5, x86_XMM6 = 6, x86_XMM7 = 7
};

enum avxRegValues {
	x86_YMM0 = 0, x86_YMM1 = 1, x86_YMM2 = 2, x86_YMM3 = 3,
	x86_YMM4 = 4, x86_YMM5 = 5, x86_YMM6 = 6, x86_YMM7 = 7
};

#define x86_Name(Reg)   (Reg) == x86_EAX  ? "eax" : (Reg) == x86_EBX  ? "ebx" :\
						(Reg) == x86_ECX  ? "ecx" : (Reg) == x86_EDX  ? "edx" :\
						(Reg) == x86_ESI  ? "esi" :	(Reg) == x86_EDI  ? "edi" :\
						(Reg) == x86_ESP  ? "esp" : (Reg) == x86_EBP  ? "ebp" :\
						"Unknown x86 Register"

#define x86Byte_Name(Reg)	(Reg) == x86_EAX  ? "al" : (Reg) == x86_EBX  ? "bl" :\
							(Reg) == x86_ECX  ? "cl" : (Reg) == x86_EDX  ? "dl" :\
							"Unknown x86 Register"

#define x86HighByte_Name(Reg)	(Reg) == x86_EAX  ? "ah" : (Reg) == x86_EBX  ? "bh" :\
								(Reg) == x86_ECX  ? "ch" : (Reg) == x86_EDX  ? "dh" :\
								"Unknown x86 Register"

#define x86Half_Name(Reg)   (Reg) == x86_EAX  ? "ax" : (Reg) == x86_EBX  ? "bx" :\
							(Reg) == x86_ECX  ? "cx" : (Reg) == x86_EDX  ? "dx" :\
							(Reg) == x86_ESI  ? "si" :	(Reg) == x86_EDI  ? "di" :\
							(Reg) == x86_ESP  ? "sp" : (Reg) == x86_EBP  ? "bp" :\
							"Unknown x86 Register"

void DetectCpuSpecs(void);
BOOL IsMMXSupported(void);
BOOL IsMMX2Supported(void);
BOOL IsSSESupported(void);
BOOL IsSSE2Supported(void);
BOOL IsSSE41Supported(void);
BOOL IsAVXSupported(void);
BOOL IsAVX2Supported(void);

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
void AndConstToX86RegHalf            ( BYTE** code, int x86Reg, WORD Const );
void AndVariableToX86Reg             ( BYTE** code, void * Variable, char * VariableName, int x86Reg );
void AndVariableDispToX86Reg         ( BYTE** code, void * Variable, char * VariableName, int x86Reg, int AddrReg, int Multiplier);
void AndX86RegToVariable             ( BYTE** code, void * Variable, char * VariableName, int x86Reg );
void AndX86RegToX86Reg               ( BYTE** code, int Destination, int Source );
void BreakPoint                      ( BYTE** code );
void Call_Direct                     ( BYTE** code, void * FunctAddress, char * FunctName );
void Call_Indirect                   ( BYTE** code, void * FunctAddress, char * FunctName );
void Cdq                             ( BYTE** code );
void CompConstToVariable             ( BYTE** code, DWORD Const, void * Variable, char * VariableName );
void CompConstToX86reg               ( BYTE** code, int x86Reg, DWORD Const );
void CompX86regToVariable            ( BYTE** code, int x86Reg, void * Variable, char * VariableName );
void CompVariableToX86reg	         ( BYTE** code, int x86Reg, void * Variable, char * VariableName );
void CompX86RegToX86Reg              ( BYTE** code, int Destination, int Source );
void CondMoveEqual                   ( BYTE** code, int Destination, int Source );
void Cwde                            ( BYTE** code );
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
void MoveConstByteToDMem             ( BYTE** code, BYTE Const, int AddrReg );
void MoveConstHalfToN64Mem           ( BYTE** code, WORD Const, int AddrReg );
void MoveConstHalfToDMem             ( BYTE** code, WORD Const, int AddrReg );
void MoveConstByteToVariable         ( BYTE** code, BYTE Const,void *Variable, char *VariableName );
void MoveConstByteToX86regPointer    ( BYTE** code, BYTE Const, int AddrReg1, int AddrReg2 );
void MoveConstHalfToVariable         ( BYTE** code, WORD Const, void *Variable, char *VariableName );
void MoveConstHalfToX86regPointer    ( BYTE** code, WORD Const, int AddrReg1, int AddrReg2 );
void MoveConstToMemoryDisp           ( BYTE** code, DWORD Const, int AddrReg, DWORD Disp );
void MoveConstToN64Mem               ( BYTE** code, DWORD Const, int AddrReg );
void MoveConstToDMem                 ( BYTE** code, DWORD Const, int AddrReg );
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
void MoveDMemToX86regHighByte        ( BYTE** code, int x86reg, int AddrReg );
void MoveSxByteX86regPointerToX86reg ( BYTE** code, int AddrReg1, int AddrReg2, int x86reg );
void MoveSxHalfX86regPointerToX86reg ( BYTE** code, int AddrReg1, int AddrReg2, int x86reg );
void MoveSxHalfX86regToX86reg        ( BYTE** code, int Source, int Destination );
void MoveSxN64MemToX86regByte        ( BYTE** code, int x86reg, int AddrReg );
void MoveSxN64MemToX86regHalf        ( BYTE** code, int x86reg, int AddrReg );
void MoveSxDMemToX86regByte          ( BYTE** code, int x86reg, int AddrReg );
void MoveSxDMemToX86regHalf          ( BYTE** code, int x86reg, int AddrReg );
void MoveSxVariableToX86regByte      ( BYTE** code, void *Variable, char *VariableName, int x86reg );
void MoveSxVariableToX86regHalf      ( BYTE** code, void *Variable, char *VariableName, int x86reg );
void MoveVariableDispToX86Reg        ( BYTE** code, void *Variable, char *VariableName, int x86Reg, int AddrReg, int Multiplier );
void MoveVariableToX86reg            ( BYTE** code, void *Variable, char *VariableName, int x86reg );
void MoveVariableToX86regByte        ( BYTE** code, void *Variable, char *VariableName, int x86reg );
void MoveVariableToX86regHighByte    ( BYTE** code, void* Variable, char* VariableName, int x86reg );
void MoveVariableToX86regHalf        ( BYTE** code, void *Variable, char *VariableName, int x86reg );
void MoveX86PointerToX86reg          ( BYTE** code, int x86reg, int X86Pointer );
void MoveX86regByteToN64Mem          ( BYTE** code, int x86reg, int AddrReg );
void MoveX86regByteToDMem            ( BYTE** code, int x86reg, int AddrReg );
void MoveX86regHighByteToDMem        ( BYTE** code, int x86reg, int AddrReg );
void MoveX86regByteToVariable        ( BYTE** code, int x86reg, void * Variable, char * VariableName );
void MoveX86regHighByteToVariable    ( BYTE** code, int x86reg, void * Variable, char * VariableName );
void MoveX86regByteToX86regPointer   ( BYTE** code, int x86reg, int AddrReg1, int AddrReg2 );
void MoveX86regHalfToN64Mem          ( BYTE** code, int x86reg, int AddrReg );
void MoveX86regHalfToDMem			 ( BYTE** code, int x86reg, int AddrReg );
void MoveX86regHalfToVariable        ( BYTE** code, int x86reg, void * Variable, char * VariableName );
void MoveX86regHalfToX86regPointer   ( BYTE** code, int x86reg, int AddrReg1, int AddrReg2 );
void MoveX86regPointerToX86reg       ( BYTE** code, int AddrReg1, int AddrReg2, int x86reg );
void MoveX86regPointerToX86regDisp8  ( BYTE** code, int AddrReg1, int AddrReg2, int x86reg, BYTE offset );
void MoveX86regToMemory              ( BYTE** code, int x86reg, int AddrReg, DWORD Disp );
void MoveX86regToN64Mem              ( BYTE** code, int x86reg, int AddrReg );
void MoveX86regToDMem                ( BYTE** code, int x86reg, int AddrReg );
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
void ShiftLeftSignVariable           ( BYTE** code, void * Variable, char * VariableName );
void ShiftRightDouble                ( BYTE** code, int Destination, int Source );
void ShiftRightDoubleImmed           ( BYTE** code, int Destination, int Source, BYTE Immediate );
void ShiftRightSign                  ( BYTE** code, int x86reg );
void ShiftRightSignImmed             ( BYTE** code, int x86reg, BYTE Immediate );
void ShiftRightSignVariableImmed     ( BYTE** code, void * Variable, char * VariableName, BYTE Immediate );
void ShiftRightSignVariable          ( BYTE** code, void * Variable, char * VariableName );
void ShiftRightUnsign                ( BYTE** code, int x86reg );
void ShiftRightUnsignImmed           ( BYTE** code, int x86reg, BYTE Immediate );
void ShiftRightUnsignVariable        ( BYTE** code, void * Variable, char * VariableName );
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

#define _MMX_SHUFFLE(a, b, c, d)	\
	((BYTE)(((a) << 6) | ((b) << 4) | ((c) << 2) | (d)))

void MmxMoveRegToReg( BYTE** code, int Dest, int Source );
void MmxMoveQwordRegToVariable( BYTE** code, int Dest, void * Variable, char * VariableName );
void MmxMoveQwordVariableToReg( BYTE** code, int Dest, void * Variable, char * VariableName );
void MmxPandRegToReg( BYTE** code, int Dest, int Source );
void MmxPandnRegToReg( BYTE** code, int Dest, int Source );
/*void MmxPandVariableToReg(void* Variable, char* VariableName, int Dest);*/
void MmxPorRegToReg( BYTE** code, int Dest, int Source );
/*void MmxPorVariableToReg(void* Variable, char* VariableName, int Dest);
void MmxXorRegToReg(int Dest, int Source);*/
void MmxShuffleMemoryToReg( BYTE** code, int Dest, void* Variable, char* VariableName, BYTE Immed );
void MmxPmullwRegToReg( BYTE** code, int Dest, int Source );
/*void MmxPmullwVariableToReg(int Dest, void* Variable, char* VariableName);
void MmxPmulhuwRegToReg(int Dest, int Source);*/
void MmxPmulhwRegToReg( BYTE** code, int Dest, int Source );
void MmxPmulhwRegToVariable( BYTE** code, int Dest, void * Variable, char * VariableName );
void MmxPsrlwImmed( BYTE** code,int Dest, BYTE Immed );
/*void MmxPsrawImmed(int Dest, BYTE Immed);*/
void MmxPsllwImmed( BYTE** code, int Dest, BYTE Immed );
/*void MmxPaddswRegToReg(int Dest, int Source);
void MmxPaddswVariableToReg(int Dest, void* Variable, char* VariableName);*/
void MmxPaddwRegToReg( BYTE** code, int Dest, int Source );
/*void MmxPackSignedDwords(int Dest, int Source);
void MmxUnpackLowWord(int Dest, int Source);
void MmxUnpackHighWord(int Dest, int Source);
void MmxCompareGreaterWordRegToReg(int Dest, int Source);*/
void MmxCompareEqualWordRegToReg( BYTE** code, int Dest, int Source );
void MmxEmptyMultimediaState( BYTE** code );

enum SseDataType {
	SseType_QuadWord
};

void SseMoveAlignedVariableToReg( BYTE** code, void * Variable, char * VariableName, int sseReg, int sseDataType, BOOL SS2Supported );
void SseMoveAlignedRegToVariable( BYTE** code, int sseReg, void * Variable, char * VariableName, int sseDataType, BOOL SS2Supported );
/*void SseMoveAlignedN64MemToReg(int sseReg, int AddrReg);
void SseMoveAlignedRegToN64Mem(int sseReg, int AddrReg);*/
void SseMoveLowRegToHighReg( BYTE** code, int Dest, int Source );
void SseMoveHighRegToLowReg( BYTE** code, int Dest, int Source );
/*void SseMoveUnalignedVariableToReg(void* Variable, char* VariableName, int sseReg);
void SseMoveUnalignedRegToVariable(int sseReg, void* Variable, char* VariableName);
void SseMoveUnalignedN64MemToReg(int sseReg, int AddrReg);
void SseMoveUnalignedRegToN64Mem(int sseReg, int AddrReg);*/
void SseMoveRegToReg( BYTE** code, int Dest, int Source, int sseDataType, BOOL SSE2Supported );
/*void SseXorRegToReg(int Dest, int Source);*/

void Sse2CompareEqualDWordRegToReg( BYTE** code, int Dest, int Source );
void Sse2CompareEqualDWordVariableToReg( BYTE** code, int Dest, void * Variable, char * VariableName );
void Sse2CompareEqualWordRegToReg( BYTE** code, int Dest, int Source );
void Sse2MoveQWordRegToReg( BYTE** code, int Dest, int Source );
void Sse2MoveSxWordRegToDWordReg( BYTE** code, int Dest, int Source, BOOL SSE41Supported );
void Sse2PadddRegToReg( BYTE** code, int Dest, int Source );
void Sse2PadddVariableToReg( BYTE** code, int Dest, void * Variable, char * VariableName );
void Sse2PaddwRegToReg( BYTE** code, int Dest, int Source );
void Sse2PandVariableToReg( BYTE** code, int Dest, void * Variable, char * VariableName );
void Sse2PandRegToReg( BYTE** code, int Dest, int Source );
void Sse2PandnRegToReg( BYTE** code, int Dest, int Source );
void Sse2PmulldRegToReg( BYTE** code, int Dest, int Source );
void Sse2PmullwRegToReg( BYTE** code, int Dest, int Source );
void Sse2PmulhwRegToReg( BYTE** code, int Dest, int Source );
void Sse2PorRegToReg( BYTE** code, int Dest, int Source );
void Sse2PorVariableToReg( BYTE** code, int Dest, void * Variable, char * VariableName );
void Sse2PslldImmed( BYTE** code, int Dest, BYTE Immed );
void Sse2PsllwImmed( BYTE** code, int Dest, BYTE Immed );
void Sse2PsradImmed( BYTE** code, int Dest, BYTE Immed );
void Sse2PsrawImmed( BYTE** code, int Dest, BYTE Immed );
void Sse2PsrlwImmed( BYTE** code, int Dest, BYTE Immed );
void Sse2PunpckHighWordsRegToReg(BYTE** code, int Dest, int Source);
void Sse2PunpckLowWordsRegToReg( BYTE** code, int Dest, int Source );
void Sse2ShuffleDWordsRegToReg( BYTE** code, int Dest, int Source, BYTE Immed );
void Sse2ShuffleLowWordsMemoryToReg( BYTE** code, int Dest, void * Variable, char * VariableName, BYTE Immed );
void Sse2ShuffleLowWordsRegToReg( BYTE** code, int Dest, int Source, BYTE Immed );
void Sse2ShuffleHighWordsMemoryToReg( BYTE** code, int Dest, void * Variable, char * VariableName, BYTE Immed );
void Sse2ShuffleHighWordsRegToReg( BYTE** code, int Dest, int Source, BYTE Immed );

void Sse41PackUnsignedDWordRegToWordReg( BYTE** code, int Dest, int Source );
void Sse41PBlendVariableToRegWithXMM0Mask( BYTE** code, int Dest, void * Variable, char * VariableName );

/*typedef struct {
	union {
		struct {
			unsigned Reg0 : 2;
			unsigned Reg1 : 2;
			unsigned Reg2 : 2;
			unsigned Reg3 : 2;
		};
		unsigned UB : 8;
	};
} SHUFFLE;

void SseShuffleReg(int Dest, int Source, BYTE Immed);*/

void AvxCompareEqualDWordRegToReg256( BYTE** code, int Dest, int Src1, int Src2 );
void AvxVExtracti128RegToReg( BYTE** code , int Dest, int Src, BOOL msb );
void AvxVPackUnsignedDWordRegToWordReg128( BYTE** code, int Dest, int Src1, int Src2 );
void AvxVPAdddRegToReg256( BYTE** code, int Dest, int Src1, int Src2 );
void AvxVPandnRegToReg256( BYTE** code, int Dest, int Src1, int Src2 );
void AvxVPBlendvbRegToReg256( BYTE** code, int Dest, int Src1, int Src2, int Src3Mask );
void AvxVPBroadcastdVariableToReg256( BYTE** code, int Dest, void * Variable, char * VariableName );
void AvxVPBroadcastwVariableToReg128( BYTE** code, int Dest, void * Variable, char * VariableName );
void AvxVPMovesxWordReg128ToDwordReg256( BYTE** code, int Dest, int Source );
void AvxVPMovesxWordVariableToDWordReg256( BYTE** code, int Dest, void * Variable, char * VariableName );
void AvxVPMulldRegToReg256( BYTE** code, int Dest, int Src1, int Src2 );
void AvxVPorRegToReg256( BYTE** code, int Dest, int Src1, int Src2 );
void AvxVPSlldRegToReg256Immed( BYTE** code, int Dest, int Src, BYTE Immed );
void AvxVPSrldRegToReg256Immed( BYTE** code, int Dest, int Src, BYTE Immed );

void x86_SetBranch8b(void* JumpByte, void* Destination);
void x86_SetBranch32b(void* JumpByte, void* Destination);
