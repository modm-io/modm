/*
 * Copyright (c) 2012, Sascha Schade
 * Copyright (c) 2013, Kevin Laeufer
 * Copyright (c) 2014, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_LPC11_CAN_REGISTERS
#define MODM_LPC11_CAN_REGISTERS

// ---- Bitnames for LPC_CAN IF1 and IF2 command register
#define	CAN_IFn_CMDMSK_DATAB	(1 << 0)	/* 1 is transfer data byte 4-7 to message object, 0 is not */
#define	CAN_IFn_CMDMSK_DATAA	(1 << 1)	/* 1 is transfer data byte 0-3 to message object, 0 is not */
#define	CAN_IFn_CMDMSK_TREQ		(1 << 2)	/* 1 is set the TxRqst bit, 0 is not */
#define	CAN_IFn_CMDMSK_INTPND	(1 << 3)	/* This bit is ignored in the write direction.  */
#define	CAN_IFn_CMDMSK_CTRL		(1 << 4)	/* 1 is transfer the CTRL bit to the message object, 0 is not */
#define	CAN_IFn_CMDMSK_ARB		(1 << 5)	/* 1 is transfer the ARB bits to the message object, 0 is not */
#define	CAN_IFn_CMDMSK_MASK		(1 << 6)	/* 1 is transfer the MASK bit to the message object, 0 is not */
#define	CAN_IFn_CMDMSK_WR		(1 << 7)	/* 0 is READ, 1 is WRITE */

// ---- Bitnames for LPC_CAN IF1 and IF2 mask 2 register IFn_MSK2
#define	CAN_IFn_MSK2_MXTD		(1 << 15)	/* 1 extended identifier bit is used in the RX filter unit, 0 is not */
#define	CAN_IFn_MSK2_MDIR		(1 << 14)	/* 1 direction bit is used in the RX filter unit, 0 is not */

// ---- Bitnames for LPC_CAN IF1 and IF2 arbitration 2 register CAN_IFn_ARB2
#define CAN_IFn_ARB2_MSGVAL		(1 << 15)	/* Message valid bit, 1 is valid in the MO handler, 0 is ignored */
#define	CAN_IFn_ARB2_XTD		(1 << 14)     /* 1 extended identifier bit is used in the RX filter unit, 0 is not */
#define	CAN_IFn_ARB2_DIR		(1 << 13)     /* 1 direction bit is used in the RX filter unit, 0 is not */

// ---- Bitnames for LPC_CAN IF1 and IF2 Message control register CAN_IFn_MCTRL
#define	CAN_IFn_MCTRL_NEWD		(1 << 15)     /* 1 indicates new data is in the message buffer.  */
#define	CAN_IFn_MCTRL_MSGLST	(1 << 14)     /* 1 indicates a message loss. */
#define	CAN_IFn_MCTRL_INTPND	(1 << 13)     /* 1 indicates message object is an interrupt source */
#define CAN_IFn_MCTRL_UMASK    	(1 << 12)     /* 1 is to use the mask for the receive filter mask. */
#define	CAN_IFn_MCTRL_TXIE		(1 << 11)     /* 1 is TX interrupt enabled */
#define	CAN_IFn_MCTRL_RXIE		(1 << 10)     /* 1 is RX interrupt enabled */
#define CAN_IFn_MCTRL_RMTEN		(1 << 9)      /* 1 is remote frame enabled */
#define CAN_IFn_MCTRL_TXRQST   	(1 << 8)      /* 1 is TxRqst enabled */
#define	CAN_IFn_MCTRL_EOB		(1 << 7)      /* End of buffer, always write to 1 */
#define	CAN_IFn_MCTRL_DLC		0x000F        /* bit mask for DLC */
#define CAN_IFn_MCTRL_DLC_MAX	8

// ---- Bitnames for LPC_CAN IF1 and IF2 Command request register CAN_IFn_CMDREQ_BUSY
#define	CAN_IFn_CMDREQ_BUSY		(1 << 15)

#endif // MODM_LPC11_CAN_REGISTERS
