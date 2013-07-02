// coding: utf-8
// ----------------------------------------------------------------------------
/* Copyright (c) 2009, Roboterclub Aachen e.V.
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

#ifndef XPCC__SOFTWARE_SPI_HPP
#define XPCC__SOFTWARE_SPI_HPP

#include <stdint.h>
#include "spi_master.hpp"
#include <xpcc/architecture/driver/delay.hpp>

namespace xpcc
{
	/**
	 * \brief	Software emulation of a SPI Master
	 * 
	 * \tparam	Clk			clock pin [output]
	 * \tparam	Mosi		master out slave in pin [output]
	 * \tparam	Miso		master in slave out pin [input]
	 * \tparam	Frequency	requested SPI frequency in Hz (default = 2 MHz)
	 * 
	 * \ingroup	connectivity
	 * \see		gpio
	 */
	template< typename Clk,
			  typename Mosi,
			  typename Miso,
			  int32_t Frequency = 2000000UL >
	class SoftwareSpiMaster : public ::xpcc::SpiMaster
	{
	public:
		static void
		initialize();
		
		static uint8_t
		writeReadBlocking(uint8_t data);

		static void
		writeBlocking(uint8_t data);

		static bool
		write(uint8_t data);

		static uint8_t
		getResult();

		static bool
		isFinished();
		
	protected:
		static ALWAYS_INLINE void
		delay();
		
		// calculate the delay in microseconds needed to achieve the
		// requested SPI frequency
		static const uint32_t delayTime = (1000000.0 / Frequency) / 2.0;
		
		static Clk clk;
		static Mosi mosi;
		static Miso miso;
		static bool finished;
		static uint8_t result;
	};

	template< typename Clk,
			  typename Mosi,
			  typename Miso,
			  int32_t Frequency = 2000000UL >
	class SoftwareSpiBlockMaster : public xpcc::SpiBlockMaster
	{
	public:
		/**
		 * \brief	Initialize SPI module
		 *
		 * This also sets the directions of the I/O pins.
		 */
		static ALWAYS_INLINE void
		initialize();

		static inline bool
		start(uint8_t * tx, uint8_t * rx, std::size_t length, BufferOptions options=BufferOptions::TxRxIncrement);

		static ALWAYS_INLINE bool
		isFinished();

	private:
		static bool finished;
	};
}

#include "spi_master_impl.hpp"

#endif // XPCC__SOFTWARE_SPI_HPP
