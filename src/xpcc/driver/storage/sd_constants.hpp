/*
 * Copyright (c) 2009-2011, Fabian Greif
 * Copyright (c) 2009, Georgi Grinshpun
 * Copyright (c) 2010, Martin Rosekeit
 * Copyright (c) 2011-2012, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_SD_CONSTANTS_HPP
#define XPCC_SD_CONSTANTS_HPP

// Definitions for MMC/SDC command
#define CMD0	(0x40 + 0)	// GO_IDLE_STATE
#define CMD1	(0x40 + 1)	// SEND_OP_COND (MMC)
#define ACMD41	(0xC0 + 41)	// SEND_OP_COND (SDC)
#define CMD8	(0x40 + 8)	// SEND_IF_COND
#define CMD9	(0x40 + 9)	// SEND_CSD
#define CMD10	(0x40 + 10)	// SEND_CID
#define CMD12	(0x40 + 12)	// STOP_TRANSMISSION
#define ACMD13	(0xC0 + 13)	// SD_STATUS (SDC)
#define CMD16	(0x40 + 16)	// SET_BLOCKLEN
#define CMD17	(0x40 + 17)	// READ_SINGLE_BLOCK
#define CMD18	(0x40 + 18)	// READ_MULTIPLE_BLOCK
#define CMD23	(0x40 + 23)	// SET_BLOCK_COUNT (MMC)
#define ACMD23	(0xC0 + 23)	// SET_WR_BLK_ERASE_COUNT (SDC)
#define CMD24	(0x40 + 24)	// WRITE_BLOCK
#define CMD25	(0x40 + 25)	// WRITE_MULTIPLE_BLOCK
#define CMD55	(0x40 + 55)	// APP_CMD
#define CMD58	(0x40 + 58)	// READ_OCR

// Card type flags (CardType)
#define CT_MMC		0x01		// MMC ver 3
#define CT_SD1		0x02		// SD ver 1
#define CT_SD2		0x04		// SD ver 2
#define CT_SDC		(CT_SD1|CT_SD2)	// SD
#define CT_BLOCK	0x08		// Block addressing

#endif // XPCC_SD_CONSTANTS_HPP
