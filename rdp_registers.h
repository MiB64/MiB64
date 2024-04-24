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

#define DPC_START_REG			RegDPC[0]
#define DPC_END_REG				RegDPC[1]
#define DPC_CURRENT_REG			RegDPC[2]
#define DPC_STATUS_REG			RegDPC[3]
#define DPC_CLOCK_REG			RegDPC[4]
#define DPC_BUFBUSY_REG			RegDPC[5]
#define DPC_PIPEBUSY_REG		RegDPC[6]
#define DPC_TMEM_REG			RegDPC[7]

#define DPC_CLR_XBUS_DMEM_DMA	0x0001		/* Bit 0: clear xbus_dmem_dma */
#define DPC_SET_XBUS_DMEM_DMA	0x0002		/* Bit 1: set xbus_dmem_dma */
#define DPC_CLR_FREEZE			0x0004		/* Bit 2: clear freeze */
#define DPC_SET_FREEZE			0x0008		/* Bit 3: set freeze */
#define DPC_CLR_FLUSH			0x0010		/* Bit 4: clear flush */
#define DPC_SET_FLUSH			0x0020		/* Bit 5: set flush */
#define DPC_CLR_TMEM_CTR		0x0040		/* Bit 6: clear tmem ctr */
#define DPC_CLR_PIPE_CTR		0x0080		/* Bit 7: clear pipe ctr */
#define DPC_CLR_CMD_CTR			0x0100		/* Bit 8: clear cmd ctr */
#define DPC_CLR_CLOCK_CTR		0x0200		/* Bit 9: clear clock ctr */

#define DPC_STATUS_XBUS_DMEM_DMA	0x001	/* Bit  0: xbus_dmem_dma */
#define DPC_STATUS_FREEZE			0x002	/* Bit  1: freeze */
#define DPC_STATUS_FLUSH			0x004	/* Bit  2: flush */
#define DPC_STATUS_START_GCLK		0x008	/* Bit  3: start gclk */
#define DPC_STATUS_TMEM_BUSY		0x010	/* Bit  4: tmem busy */
#define DPC_STATUS_PIPE_BUSY		0x020	/* Bit  5: pipe busy */
#define DPC_STATUS_CMD_BUSY			0x040	/* Bit  6: cmd busy */
#define DPC_STATUS_CBUF_READY		0x080	/* Bit  7: cbuf ready */
#define DPC_STATUS_DMA_BUSY			0x100	/* Bit  8: dma busy */
#define DPC_STATUS_END_VALID		0x200	/* Bit  9: end valid */
#define DPC_STATUS_START_VALID		0x400	/* Bit 10: start valid */

extern DWORD* RegDPC;
