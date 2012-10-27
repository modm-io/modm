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
/*
 * WARNING: This file is generated automatically, do not edit!
 * Please modify the corresponding *.in file instead and rebuild this file. 
 */
// ----------------------------------------------------------------------------


#ifndef XPCC_ATXMEGA__SPI_F_HPP
#define XPCC_ATXMEGA__SPI_F_HPP

#include <stdint.h>
#include <avr/io.h>
#include "spi.hpp"
#include <xpcc/driver/connectivity/spi/spi_master.hpp>
#include <xpcc/architecture/utils.hpp>

namespace xpcc
{
	namespace xmega
	{
#if defined(SPIF) || defined(__DOXYGEN__)
		/**
		 * \brief		SPI Master for Port F
		 *
		 * This module only supports DMA read transfers in slave mode, however
		 * slave mode is not implemented here.
		 *
		 * \author		Niklas hauser
		 * \ingroup		xmega_spi
		 */
		class SpiMasterF : public SpiMaster
		{
		public:
			static void
			initialize(spi::Prescaler prescaler=spi::PRESCALER_16,
					   spi::Mode mode=spi::MODE_0);
			
			inline static void
			setDataOrder(bool lsbFirst=true)
			{
				SPIF_CTRL = (SPIF_CTRL & ~SPI_DORD_bm) | (lsbFirst ? SPI_DORD_bm : 0);
			}
			
			static uint8_t
			write(uint8_t data);
			
			static bool
			setBuffer(uint16_t length,
					  uint8_t* transmit=0, uint8_t* receive=0,
					  BufferIncrease bufferIncrease=BUFFER_INCR_BOTH);
			
			static bool
			transfer(TransferOptions options=TRANSFER_SEND_BUFFER_SAVE_RECEIVE);
			
			static ALWAYS_INLINE bool
			transferSync(TransferOptions options=TRANSFER_SEND_BUFFER_SAVE_RECEIVE);
			
			static bool
			isFinished();
			
			inline static SPI_t&
			getModuleBase()
			{
				return SPIF;
			}
		};
#endif // SPIF
		
#if defined(USARTF0) || defined(__DOXYGEN__)
		/**
		 * \brief		UART0 in SPI master mode
		 *
		 * The USART pins are mapped like this MOSI=>TXO, MISO=>RXI, SCK=>XCK.
		 * Be aware that this module has no SS pin, so no slave mode is possible.
		 *
		 * This module supports DMA transfers between memory (SRAM) and DATA
		 * register by defining which DMA channel to use for write/read.
		 * (SPI0_TX_DMA_CHANNEL, SPI0_RX_DMA_CHANNEL)
		 * In typical use, the DMA channels gets configured once and then several
		 * transfers get triggered, without CPU interference.
		 * 
		 * \author	Niklas Hauser
		 * \ingroup	xmega_spi
		 */
		class UartSpiMasterF0 : public SpiMaster
		{
		public:
			/**
			 * Sets pins and calculates the bsel value of the UART module.
			 * The parameter has to be constant at compile time, so that
			 * the compiler can do the calculation, and not the uC.
			 * The DMA channel is configured with a Burst Length of one byte,
			 * reloading of source address after block transfer completion,
			 * incr./decr. of source address after each byte transfer,
			 * no reloading of fixed destination address (=UART DATA register).
			 * 
			 * \warning The bitrate must be less than half F_CPU.
			 * \param	bitrate	SPI frequency in Hz
			 */
			static void
			initialize(uint32_t const bitrate=2000000);
			
			static uint8_t
			write(uint8_t data);
			
			static bool
			setBuffer(uint16_t length,
					  uint8_t* transmit=0, uint8_t* receive=0,
					  BufferIncrease bufferIncrease=BUFFER_INCR_BOTH);
					  
			static bool
			transfer(TransferOptions options=TRANSFER_SEND_BUFFER_SAVE_RECEIVE);
			
			static bool
			transferSync(TransferOptions options=TRANSFER_SEND_BUFFER_SAVE_RECEIVE);
			
			static bool
			isFinished();
			
			inline static USART_t&
			getModuleBase()
			{
				return USARTF0;
			}
			
			inline static void
			setDataOrder(bool lsbFirst=true)
			{
				USARTF0_CTRLC = (USARTF0_CTRLC & ~USART_CHSIZE2_bm) | (lsbFirst ? USART_CHSIZE2_bm : 0);
			}
			
		private:
			static bool
			isBusy();
		};
#endif	// USARTF0
		
#if defined(USARTF1) || defined(__DOXYGEN__)
		/**
		 * \brief		UART1 in SPI master mode
		 *
		 * The USART pins are mapped like this MOSI=>TXO, MISO=>RXI, SCK=>XCK.
		 * Be aware that this module has no SS pin, so no slave mode is possible.
		 *
		 * This module supports DMA transfers between memory (SRAM) and DATA
		 * register by defining which DMA channel to use for write/read.
		 * (SPI1_TX_DMA_CHANNEL, SPI1_RX_DMA_CHANNEL)
		 * In typical use, the DMA channels gets configured once and then several
		 * transfers get triggered, without CPU interference.
		 * 
		 * \author	Niklas Hauser
		 * \ingroup	xmega_spi
		 */
		class UartSpiMasterF1 : public SpiMaster
		{
		public:
			/**
			 * Sets pins and calculates the bsel value of the UART module.
			 * The parameter has to be constant at compile time, so that
			 * the compiler can do the calculation, and not the uC.
			 * The DMA channel is configured with a Burst Length of one byte,
			 * reloading of source address after block transfer completion,
			 * incr./decr. of source address after each byte transfer,
			 * no reloading of fixed destination address (=UART DATA register).
			 * 
			 * \warning The bitrate must be less than half F_CPU.
			 * \param	bitrate	SPI frequency in Hz
			 */
			static void
			initialize(uint32_t const bitrate=2000000);
			
			static uint8_t
			write(uint8_t data);
			
			static bool
			setBuffer(uint16_t length,
					  uint8_t* transmit=0, uint8_t* receive=0,
					  BufferIncrease bufferIncrease=BUFFER_INCR_BOTH);
					  
			static bool
			transfer(TransferOptions options=TRANSFER_SEND_BUFFER_SAVE_RECEIVE);
			
			static bool
			transferSync(TransferOptions options=TRANSFER_SEND_BUFFER_SAVE_RECEIVE);
			
			static bool
			isFinished();
			
			inline static USART_t&
			getModuleBase()
			{
				return USARTF1;
			}
			
			inline static void
			setDataOrder(bool lsbFirst=true)
			{
				USARTF1_CTRLC = (USARTF1_CTRLC & ~USART_CHSIZE2_bm) | (lsbFirst ? USART_CHSIZE2_bm : 0);
			}
			
		private:
			static bool
			isBusy();
		};
#endif	// USARTF1
	}
}

#endif // XPCC_ATXMEGA__SPI_F_HPP
