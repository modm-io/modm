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
 *
 * $Id: spi.hpp 750 2012-03-01 17:01:46Z salkinium $
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_ATMEGA__SPI_HPP
#define XPCC_ATMEGA__SPI_HPP

#include <avr/io.h>
#include <stdint.h>
#include <xpcc/driver/spi/spi_master.hpp>

namespace xpcc
{
	namespace atmega
	{
		/**
		 * \brief	SPI master module
		 * \ingroup	atmega
		 */
		class SpiMaster : public xpcc::SpiMaster
		{
		public:
			enum Mode
			{
				MODE_0 = 0,				///< SCK normal, sample on rising edge
				MODE_1 = (1 << CPHA),	///< SCK normal, sample on falling edge
				MODE_2 = (1 << CPOL),	///< SCK inverted, sample on falling edge
				MODE_3 = (1 << CPOL) | (1 << CPHA),	
										///< SCK inverted, sample on rising edge
			};
			
			enum Prescaler
			{
				PRESCALER_2 = 0x80 | 0,
				PRESCALER_4 = 0,
				PRESCALER_8 = 0x80 | (1 << SPR0),
				PRESCALER_16 = (1 << SPR0),
				PRESCALER_32 = 0x80 | (1 << SPR1),
				PRESCALER_64 = (1 << SPR1),
				PRESCALER_128 = (1 << SPR1) | (1 << SPR0),
			};
			
		public:
			/**
			 * \brief	Initialize SPI module
			 * 
			 * \warning	This won't set the directions for the I/O pins. You have
			 * 			to do this by yourself.
			 */
			static void
			initialize(Mode mode, Prescaler prescaler);
			
			static uint8_t
			write(uint8_t data);
			
			static bool
			setBuffer(uint16_t length,
					  uint8_t* transmit=0, uint8_t* receive=0,
					  bool transmitIncr=true, bool receiveIncr=true);
			
			static bool
			transfer(bool send=true, bool receive=false, bool wait=true);
			
			static bool
			isFinished();
		};
	}
}

#endif // XPCC_ATMEGA__SPI_HPP
