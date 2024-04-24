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

#define MI_INIT_MODE_REG		RegMI[0]
#define MI_MODE_REG				RegMI[0]
#define MI_VERSION_REG			RegMI[1]
#define MI_NOOP_REG				RegMI[1]
#define MI_INTR_REG				RegMI[2]
#define MI_INTR_MASK_REG		RegMI[3]

#define MI_CLR_INIT				0x0080		/* Bit  7: clear init mode */
#define MI_SET_INIT				0x0100		/* Bit  8: set init mode */
#define MI_CLR_EBUS				0x0200		/* Bit  9: clear ebus test */
#define MI_SET_EBUS				0x0400		/* Bit 10: set ebus test mode */
#define MI_CLR_DP_INTR			0x0800		/* Bit 11: clear dp interrupt */
#define MI_CLR_RDRAM			0x1000		/* Bit 12: clear RDRAM reg */
#define MI_SET_RDRAM			0x2000		/* Bit 13: set RDRAM reg mode */

#define MI_MODE_INIT			0x0080		/* Bit  7: init mode */
#define MI_MODE_EBUS			0x0100		/* Bit  8: ebus test mode */
#define MI_MODE_RDRAM			0x0200		/* Bit  9: RDRAM reg mode */

#define MI_INTR_MASK_CLR_SP		0x0001		/* Bit  0: clear SP mask */
#define MI_INTR_MASK_SET_SP		0x0002		/* Bit  1: set SP mask */
#define MI_INTR_MASK_CLR_SI		0x0004		/* Bit  2: clear SI mask */
#define MI_INTR_MASK_SET_SI		0x0008		/* Bit  3: set SI mask */
#define MI_INTR_MASK_CLR_AI		0x0010		/* Bit  4: clear AI mask */
#define MI_INTR_MASK_SET_AI		0x0020		/* Bit  5: set AI mask */
#define MI_INTR_MASK_CLR_VI		0x0040		/* Bit  6: clear VI mask */
#define MI_INTR_MASK_SET_VI		0x0080		/* Bit  7: set VI mask */
#define MI_INTR_MASK_CLR_PI		0x0100		/* Bit  8: clear PI mask */
#define MI_INTR_MASK_SET_PI		0x0200		/* Bit  9: set PI mask */
#define MI_INTR_MASK_CLR_DP		0x0400		/* Bit 10: clear DP mask */
#define MI_INTR_MASK_SET_DP		0x0800		/* Bit 11: set DP mask */

#define MI_INTR_MASK_SP			0x01		/* Bit 0: SP intr mask */
#define MI_INTR_MASK_SI			0x02		/* Bit 1: SI intr mask */
#define MI_INTR_MASK_AI			0x04		/* Bit 2: AI intr mask */
#define MI_INTR_MASK_VI			0x08		/* Bit 3: VI intr mask */
#define MI_INTR_MASK_PI			0x10		/* Bit 4: PI intr mask */
#define MI_INTR_MASK_DP			0x20		/* Bit 5: DP intr mask */

#define MI_INTR_SP				0x01		/* Bit 0: SP intr */
#define MI_INTR_SI				0x02		/* Bit 1: SI intr */
#define MI_INTR_AI				0x04		/* Bit 2: AI intr */
#define MI_INTR_VI				0x08		/* Bit 3: VI intr */
#define MI_INTR_PI				0x10		/* Bit 4: PI intr */
#define MI_INTR_DP				0x20		/* Bit 5: DP intr */

extern DWORD* RegMI;
