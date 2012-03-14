// coding: utf-8
// ----------------------------------------------------------------------------
/* Copyright (c) 2011, Roboterclub Aachen e.V.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Roboterclub Aachen e.V. nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ROBOTERCLUB AACHEN E.V. ''AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL ROBOTERCLUB AACHEN E.V. BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC__SD_CONSTANTS_HPP
#define XPCC__SD_CONSTANTS_HPP

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

#endif // XPCC__SD_CONSTANTS_HPP
