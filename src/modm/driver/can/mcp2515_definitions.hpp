/*
 * Copyright (c) 2009-2011, Fabian Greif
 * Copyright (c) 2010, Martin Rosekeit
 * Copyright (c) 2010, Thorsten Lajewski
 * Copyright (c) 2012-2013, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef	MODM_MCP2515_DEFINITIONS_HPP
#define	MODM_MCP2515_DEFINITIONS_HPP

namespace modm
{
	namespace mcp2515
	{
		/**
		 * \brief	Adressen der Register des MCP2515
		 * 
		 * Die Redundanten Adressen von z.B. dem Register CANSTAT 
		 * (0x0E, 0x1E, 0x2E, ...) wurden dabei nicht mit aufgelistet.
		 * 
		 * \ingroup	mcp2515
		 */
		enum Register
		{
			RXF0SIDH = 0x00,
			RXF0SIDL = 0x01,
			RXF0EID8 = 0x02,
			RXF0EID0 = 0x03,
			RXF1SIDH = 0x04,
			RXF1SIDL = 0x05,
			RXF1EID8 = 0x06,
			RXF1EID0 = 0x07,
			RXF2SIDH = 0x08,
			RXF2SIDL = 0x09,
			RXF2EID8 = 0x0A,
			RXF2EID0 = 0x0B,
			BFPCTRL = 0x0C,
			TXRTSCTRL = 0x0D,
			CANSTAT = 0x0E,
			CANCTRL = 0x0F,
			
			RXF3SIDH = 0x10,
			RXF3SIDL = 0x11,
			RXF3EID8 = 0x12,
			RXF3EID0 = 0x13,
			RXF4SIDH = 0x14,
			RXF4SIDL = 0x15,
			RXF4EID8 = 0x16,
			RXF4EID0 = 0x17,
			RXF5SIDH = 0x18,
			RXF5SIDL = 0x19,
			RXF5EID8 = 0x1A,
			RXF5EID0 = 0x1B,
			TEC =  0x1C,
			REC = 0x1D,
			
			RXM0SIDH = 0x20,
			RXM0SIDL = 0x21,
			RXM0EID8 = 0x22,
			RXM0EID0 = 0x23,
			RXM1SIDH = 0x24,
			RXM1SIDL = 0x25,
			RXM1EID8 = 0x26,
			RXM1EID0 = 0x27,
			CNF3 = 0x28,
			CNF2 = 0x29,
			CNF1 = 0x2A,
			CANINTE = 0x2B,
			CANINTF = 0x2C,
			EFLG = 0x2D,
			
			TXB0CTRL = 0x30,
			TXB0SIDH = 0x31,
			TXB0SIDL = 0x32,
			TXB0EID8 = 0x33,
			TXB0EID0 = 0x34,
			TXB0DLC = 0x35,
			TXB0D0 = 0x36,
			TXB0D1 = 0x37,
			TXB0D2 = 0x38,
			TXB0D3 = 0x39,
			TXB0D4 = 0x3A,
			TXB0D5 = 0x3B,
			TXB0D6 = 0x3C,
			TXB0D7 = 0x3D,
			
			TXB1CTRL = 0x40,
			TXB1SIDH = 0x41,
			TXB1SIDL = 0x42,
			TXB1EID8 = 0x43,
			TXB1EID0 = 0x44,
			TXB1DLC = 0x45,
			TXB1D0 = 0x46,
			TXB1D1 = 0x47,
			TXB1D2 = 0x48,
			TXB1D3 = 0x49,
			TXB1D4 = 0x4A,
			TXB1D5 = 0x4B,
			TXB1D6 = 0x4C,
			TXB1D7 = 0x4D,
			
			TXB2CTRL = 0x50,
			TXB2SIDH = 0x51,
			TXB2SIDL = 0x52,
			TXB2EID8 = 0x53,
			TXB2EID0 = 0x54,
			TXB2DLC = 0x55,
			TXB2D0 = 0x56,
			TXB2D1 = 0x57,
			TXB2D2 = 0x58,
			TXB2D3 = 0x59,
			TXB2D4 = 0x5A,
			TXB2D5 = 0x5B,
			TXB2D6 = 0x5C,
			TXB2D7 = 0x5D,
			
			RXB0CTRL = 0x60,
			RXB0SIDH = 0x61,
			RXB0SIDL = 0x62,
			RXB0EID8 = 0x63,
			RXB0EID0 = 0x64,
			RXB0DLC = 0x65,
			RXB0D0 = 0x66,
			RXB0D1 = 0x67,
			RXB0D2 = 0x68,
			RXB0D3 = 0x69,
			RXB0D4 = 0x6A,
			RXB0D5 = 0x6B,
			RXB0D6 = 0x6C,
			RXB0D7 = 0x6D,
			
			RXB1CTRL = 0x70,
			RXB1SIDH = 0x71,
			RXB1SIDL = 0x72,
			RXB1EID8 = 0x73,
			RXB1EID0 = 0x74,
			RXB1DLC = 0x75,
			RXB1D0 = 0x76,
			RXB1D1 = 0x77,
			RXB1D2 = 0x78,
			RXB1D3 = 0x79,
			RXB1D4 = 0x7A,
			RXB1D5 = 0x7B,
			RXB1D6 = 0x7C,
			RXB1D7 = 0x7D
		};
		
		/**
		 * \brief	Bitdefinition von BFPCTRL
		 * \ingroup	mcp2515
		 */
		enum BFPCTRL
		{
			B1BFS = (1 << 5),
			B0BFS = (1 << 4),
			B1BFE = (1 << 3),
			B0BFE = (1 << 2),
			B1BFM = (1 << 1),
			B0BFM = (1 << 0)
		};
		
		/**
		 * \brief	Bitdefinition von TXRTSCTRL
		 * \ingroup	mcp2515
		 */
		enum TXRTSCTRL
		{
			B2RTS = (1 << 5),
			B1RTS = (1 << 4),
			B0RTS = (1 << 3),
			B2RTSM = (1 << 2),
			B1RTSM = (1 << 1),
			B0RTSM = (1 << 0)
		};
		
		/**
		 * \brief	Bitdefinition von CANSTAT
		 * \ingroup	mcp2515
		 */
		enum CANSTAT
		{
			OPMOD2 = (1 << 7),
			OPMOD1 = (1 << 6),
			OPMOD0 = (1 << 5),
			ICOD2 = (1 << 3),
			ICOD1 = (1 << 2),
			ICOD0 = (1 << 1)
		};
		
		/**
		 * \brief	Bitdefinition von CANCTRL
		 * \ingroup	mcp2515
		 */
		enum CANCTRL
		{
			REQOP2 = (1 << 7),
			REQOP1 = (1 << 6),
			REQOP0 = (1 << 5),
			ABAT = (1 << 4),
			CLKEN = (1 << 2),
			CLKPRE1 = (1 << 1),
			CLKPRE0 = (1 << 0)
		};
		
		/**
		 * \brief	Bitdefinition von CNF3
		 * \ingroup	mcp2515
		 */
		enum CNF3
		{
			WAKFIL = (1 << 6),
			PHSEG22 = (1 << 2),
			PHSEG21 = (1 << 1),
			PHSEG20 = (1 << 0)
		};
		
		/**
		 * \brief	Bitdefinition von CNF2
		 * \ingroup	mcp2515
		 */
		enum CNF2
		{
			BTLMODE = (1 << 7),
			SAM = (1 << 6),
			PHSEG12 = (1 << 5),
			PHSEG11 = (1 << 4),
			PHSEG10 = (1 << 3),
			PHSEG2 = (1 << 2),
			PHSEG1 = (1 << 1),
			PHSEG0 = (1 << 0)
		};
		
		/**
		 * \brief	Bitdefinition von CNF1
		 * \ingroup	mcp2515
		 */
		enum CNF1
		{
			MCP2515_SJW1 = (1 << 7),	// also defined by the AT90CAN
			MCP2515_SJW0 = (1 << 6),
			MCP2515_BRP5 = (1 << 5),
			MCP2515_BRP4 = (1 << 4),
			MCP2515_BRP3 = (1 << 3),
			MCP2515_BRP2 = (1 << 2),
			MCP2515_BRP1 = (1 << 1),
			MCP2515_BRP0 = (1 << 0)
		};
		
		/**
		 * \brief	Bitdefinition von CANINTE
		 * \ingroup	mcp2515
		 */
		enum CANINTE
		{
			MERRE = (1 << 7),
			WAKIE = (1 << 6),
			ERRIE = (1 << 5),
			TX2IE = (1 << 4),
			TX1IE = (1 << 3),
			TX0IE = (1 << 2),
			RX1IE = (1 << 1),
			RX0IE = (1 << 0)
		};
		
		/**
		 * \brief	Bitdefinition von CANINTF
		 * \ingroup	mcp2515
		 */
		enum CANINTF
		{
			MERRF = (1 << 7),
			WAKIF = (1 << 6),
			ERRIF = (1 << 5),
			TX2IF = (1 << 4),
			TX1IF = (1 << 3),
			TX0IF = (1 << 2),
			RX1IF = (1 << 1),
			RX0IF = (1 << 0)
		};
		
		/**
		 * \brief	Bitdefinition von EFLG
		 * \ingroup	mcp2515
		 */
		enum EFLG
		{
			RX1OVR = (1 << 7),
			RX0OVR = (1 << 6),
			TXB0 = (1 << 5),
			TXEP = (1 << 4),
			RXEP = (1 << 3),
			TXWAR = (1 << 2),
			RXWAR = (1 << 1),
			EWARN = (1 << 0)
		};
		
		/**
		 * \brief	Bitdefinition von TXBnCTRL (n = 0, 1), 2)
		 * \ingroup	mcp2515
		 */
		enum TXBnCTRL
		{
			ABTF = (1 << 6),
			MLOA = (1 << 5),
			TXERR = (1 << 4),
			TXREQ = (1 << 3),
			TXP1 = (1 << 1),
			TXP0 = (1 << 0)
		};
		
		/**
		 * \brief	Bitdefinition von RXB0CTRL
		 * \ingroup	mcp2515
		 */
		enum RXB0CTRL
		{
			RXM1 = (1 << 6),
			RXM0 = (1 << 5),
			RXRTR = (1 << 3),
			BUKT = (1 << 2),
			BUKT1 = (1 << 1),
			FILHIT0 = (1 << 0)
		};
		
		/**
		 * \brief	Bitdefinition von TXBnSIDL (n = 0, 1)
		 * \ingroup	mcp2515
		 */
		enum TXBnSIDL
		{
			EXIDE = (1 << 3)
		};
		
		/**
		 * \brief	Bitdefinition von RXB1CTRL
		 * 
		 * \see		RXM1, RXM0, RXRTR und FILHIT0 sind schon
		 * 			für RXB0CTRL definiert
		 * \ingroup	mcp2515
		 */
		enum RXB1CTRL
		{
			FILHIT2 = (1 << 2),
			FILHIT1 = (1 << 1)
		};
		
		/**
		 * \brief	Bitdefinition von RXBnSIDL (n = 0, 1)
		 * \ingroup	mcp2515
		 */
		enum RXBnSIDL
		{
			MCP2515_SRR = (1 << 4),	// also defined by the AT90CAN
			MCP2515_IDE = (1 << 3)
		};
		
		/**
		 * \brief	Bitdefinition von RXBnDLC (n = 0, 1)
		 * 
		 * \see		TXBnDLC (gleiche Bits)
		 * \ingroup	mcp2515
		 */
		enum RXBnDLC
		{
			MCP2515_RTR = (1 << 6),	// also defined by the AT90CAN
			MCP2515_DLC3 = (1 << 3),
			MCP2515_DLC2 = (1 << 2),
			MCP2515_DLC1 = (1 << 1),
			MCP2515_DLC0 = (1 << 0)
		};
		
		/**
		 * \brief	Definition of the status register
		 * \ingroup	mcp2515
		 */
		enum Status
		{
			CANINTF_TX2IF = (1 << 7),
			TXB2CNTRL_TXREQ = (1 << 6),
			CANINTF_TX1IF = (1 << 5),
			TXB1CNTRL_TXREQ = (1 << 4),
			CANINTF_TX0IF = (1 << 3),
			TXB0CNTRL_TXREQ = (1 << 2),
			CANINTF_RX1IF = (1 << 1),
			CANINTF_RX0IF = (1 << 0)
		};
		
		/**
		 * \brief	Definition of the receive status register
		 * \ingroup	mcp2515
		 */
		enum RxStatus
		{
			FLAG_RXB1_FULL = (1 << 7),
			FLAG_RXB0_FULL = (1 << 6),
			
			FLAG_EXTENDED = (1 << 4),
			FLAG_RTR = (1 << 3),
			
			TYPE_EXTENDED_REMOTE_FRAME = (1 << 4) | (1 << 3),
			TYPE_EXTENDED_DATA_FRAME = (1 << 4),
			TYPE_STANDARD_REMOTE_FRAME = (1 << 3),
			TYPE_STANDARD_DATA_FRAME = 0,
					
			MATCH_RXF1_ROLLOVER_TO_RXB1 = 7,
			MATCH_RXF0_ROLLOVER_TO_RXB1 = 6,
			MATCH_RXF5 = 5,
			MATCH_RXF4 = 4,
			MATCH_RXF3 = 3,
			MATCH_RXF2 = 2,
			MATCH_RXF1 = 1,
			MATCH_RXF0 = 0,
		};
	}
}

#endif	// MODM_MCP2515_DEFINITIONS_HPP
