// coding: utf-8
// ----------------------------------------------------------------------------
/* Copyright (c) 2013, Roboterclub Aachen e.V.
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

#ifndef XPCC_PERIPHERAL__SPI_HPP
#define XPCC_PERIPHERAL__SPI_HPP

#include "../peripheral.hpp"

namespace xpcc
{
	struct Spi
	{
		/// transfer options for the receive and transmit buffers
		/// @see SpiBlockMaster
		enum class BufferOptions : uint8_t
		{
			TxRxIncrement = 0b11,			///< Increments both Tx and Rx buffer
			TxIncrementRxDecrement = 0b10,	///< Increments Tx, but decrements Rx buffer
			TxDecrementRxIncrement = 0b01,	///< Decrements Tx, but increments Rx buffer
			TxRxDecrement = 0b00,			///< Decrements both Tx and Rx buffer
		};

		/// Spi Clock Mode, Mode0 is the most used mode
		enum class Mode : uint8_t
		{
			Mode0 = 0b00,	///< SCK normal,	sample on rising edge
			Mode1 = 0b01,	///< SCK normal,	sample on falling edge
			Mode2 = 0b10,	///< SCK inverted,	sample on falling edge
			Mode3 = 0b11,	///< SCK inverted,	sample on rising edge
		};
	};

	/**
	 * @brief	Spi Master
	 *
	 * @author	Niklas Hauser
	 * @ingroup	peripheral
	 * @ingroup	spi
	 */
	class SpiMaster : public ::xpcc::Peripheral, public Spi
	{
#ifdef __DOXYGEN__
	public:
		/**
		 * @brief	Write a single byte, wait for completion.
		 * @return	received byte.
		 */
		static uint8_t
		writeReadBlocking(uint8_t data);
		
		/// @brief	Write a single byte, wait for completion and discard received byte.
		static void
		writeBlocking(uint8_t data);
		
		/**
		 * @brief	Write a single byte, and return immediately.
		 *
		 * @return	`true` if data has been sent, `false` if buffer is full
		 */
		static bool
		write(uint8_t data);
		
		/// @return	last byte that has been received.
		static uint8_t
		getResult();
		
		/// @return	`true` if last byte has been sent and the swapped byte can be read.
		static bool
		isFinished();
#endif
	};

	/**
	 * @brief		Spi Master with DMA support
	 *
	 * @author		Niklas Hauser
	 * @ingroup		peripheral
	 */
	class SpiBlockMaster : public ::xpcc::Peripheral, public Spi
	{
#ifdef __DOXYGEN__
	public:
		/**
		 * @brief	Set the data buffers and length with options and starts a transfer
		 *
		 * @param[in]	tx			pointer to transmit buffer, set to zero to send dummy bytes
		 * @param[out]	rx			pointer to receive buffer, set to zero to discard received bytes
		 * @param		length		length of buffer or number of dummy bytes to be send
		 * @param		options		buffer options
		 *
		 * @return	`true`	if transfer request was successfully serviced,
		 *			`false` if another transfer is already progress.
		 */
		static bool
		start(uint8_t * tx, uint8_t * rx, std::size_t length, BufferOptions options=BufferOptions::TxRxIncrement);
		
		/**
		 * @return	`true` if previous transfer finished,
		 *			`false` if a transfer is in progress.
		 */
		static bool
		isFinished();
#endif
	};
}

#endif // XPCC_PERIPHERAL__SPI_HPP
