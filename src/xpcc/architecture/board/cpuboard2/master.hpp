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
 *
 * $Id$
 */
// ----------------------------------------------------------------------------

#ifndef XPCC__CPU_BOARD2_MASTER_HPP
#define XPCC__CPU_BOARD2_MASTER_HPP

#include <xpcc/architecture/platform.hpp>
#include <xpcc/architecture/driver.hpp>

#include <xpcc/driver/software_spi.hpp>
#include <xpcc/driver/at45db0x1d.hpp>
#include <xpcc/driver/serial_ram.hpp>
#include <xpcc/driver/can/mcp2515.hpp>

#include <xpcc/driver/interconnect/interconnect_interface.hpp>

namespace xpcc
{
	/**
	 * \brief	CPU Board 2
	 * 
	 * <h2>Pin assignment:</h2>
	 * 
	 * CAN (MCP2515):
	 * - PC4 - CS
	 * - PC5 - SCK  (SPIC)
	 * - PC6 - MISO (SPIC)
	 * - PC7 - MOSI (SPIC)
	 * - PB0 - INT
	 * - PB1 - TX_RTS0
	 * - PB2 - TX_RTS1
	 * - PB3 - TX_RTS2
	 * - PQ0 - RX_BF0
	 * - PQ1 - RX_BF1
	 * 
	 * RAM (23K256) & Flash (AT45DB041):
	 * - PQ2 - HOLD_RAM
	 * - PQ3 - CS_RAM
	 * - PF4 - CS_FLASH
	 * - PF5 - SCK  (SPIF)
	 * - PF6 - MISO (SPIF)
	 * - PF7 - MOSI (SPIF)
	 * 
	 * BoardLink (to Slave):
	 * - PC0 - MS  (output -> PD5)
	 * - PC1 - SM  (input  -> PD4)
	 * - PC2 - RXD (USARTC0)
	 * - PC3 - TXD (USARTC0)
	 * 
	 * Jtag:
	 * - PB4 - TMS
	 * - PB5 - TDI
	 * - PB6 - TCK
	 * - PB7 - TDO
	 * 
	 * Other:
	 * - PR0 - Button (active low, inverted via software)
	 * - PR1 - ext. clock (8 MHz)
	 * - PH0..7 - LEDs
	 * 
	 * Available on the headers:
	 * - PA0..7
	 * - PD0..7
	 * - PE0..7
	 * - PF0..7 (4-7 also used for the Flash)
	 * - PJ0..7
	 * - PK0..7
	 * 
	 * \ingroup	board
	 * \author	Fabian Greif
	 */
	template <typename Transmit = Empty, typename Receive = Empty>
	class CpuBoard2
	{
	protected:
		// Mcp2515
		GPIO__OUTPUT(Sck, C, 5);
		GPIO__INPUT(Miso, C, 6);
		GPIO__OUTPUT(Mosi, C, 7);
		
		GPIO__OUTPUT(Cs, C, 4);
		GPIO__INPUT(Int, B, 0);
		
		typedef xpcc::SoftwareSpi<Sck, Mosi, Miso> SpiCan;
		
		GPIO__INPUT(RxBf0, Q, 0);
		GPIO__INPUT(RxBf1, Q, 1);
		
		GPIO__OUTPUT(TxRts0, B, 1);
		GPIO__OUTPUT(TxRts1, B, 2);
		GPIO__OUTPUT(TxRts2, B, 3);
		
		// Flash & RAM
		GPIO__OUTPUT(CsRam, Q, 3);
		GPIO__OUTPUT(HoldRam, Q, 2);
		GPIO__OUTPUT(CsFlash, F, 4);
		GPIO__OUTPUT(MosiFlash, F, 5);
		GPIO__INPUT(MisoFlash, F, 6);
		GPIO__OUTPUT(SckFlash, F, 7);
		
		typedef xpcc::SoftwareSpi<SckFlash, MosiFlash, MisoFlash> SpiFlash;
		
	public:
		/// Atmel DataFlash (4 MBit)
		typedef xpcc::At45db0x1d<SpiFlash, CsFlash> Flash;
		
		/// 32 kB serial RAM
		typedef xpcc::SerialRam<SpiFlash, CsRam, HoldRam> Ram;
		
		/// CAN Controller
		typedef xpcc::Mcp2515<SpiCan, Cs, Int> Can;
		
		/// Connection to the Slave CPU
		typedef InterconnectInterface<Transmit, Receive> Interconnect;
		
		// other
		GPIO__PORT(Leds, H);
		
		GPIO__OUTPUT(Led0, H, 0);	///< Green
		GPIO__OUTPUT(Led1, H, 1);	///< Green
		GPIO__OUTPUT(Led2, H, 2);	///< Green
		GPIO__OUTPUT(Led3, H, 3);	///< Green
		GPIO__OUTPUT(Led4, H, 4);	///< Yellow
		GPIO__OUTPUT(Led5, H, 5);	///< Yellow
		GPIO__OUTPUT(Led6, H, 6);	///< Red
		GPIO__OUTPUT(Led7, H, 7);	///< Red
		
		GPIO__INPUT(Button, R, 0);	///< 1=pressed, 0=not pressed
		
	public:
		/**
		 * \brief	Initialize all peripheral devices
		 * 
		 * \return	\c true if the initialization was successfull, \c false
		 * 			if one of the chips didn't respond.
		 */
		static bool
		initialize();
		
	protected:
		/// Enable internal PLL (x4) to generate the 32 MHz clock frequency
		static void
		enableExternalClock();
	};
}

#include "master_impl.hpp"

#endif // XPCC__CPU_BOARD2_MASTER_HPP
