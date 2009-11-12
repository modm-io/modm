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
 * $Id$
 */
// ----------------------------------------------------------------------------

#ifndef XPCC__SPI_HPP
#define XPCC__SPI_HPP

#include <avr/io.h>
#include <stdint.h>

namespace xpcc
{
	class Spi
	{
	public:
		typedef enum {
			MODE_0 = 0,				//!< SCK normal, sample on rising edge
			MODE_1 = (1 << CPHA),	//!< SCK normal, sample on falling edge
			MODE_2 = (1 << CPOL),	//!< SCK inverted, sample on falling edge
			MODE_3 = (1 << CPOL) | (1 << CPHA),	
									//!< SCK inverted, sample on rising edge
		} Mode;
		
		typedef enum {
			PRESCALER_2 = 0x80 | 0,
			PRESCALER_4 = 0,
			PRESCALER_8 = 0x80 | (1 << SPR0),
			PRESCALER_16 = (1 << SPR0),
			PRESCALER_32 = 0x80 | (1 << SPR1),
			PRESCALER_64 = (1 << SPR1),
			PRESCALER_128 = (1 << SPR1) | (1 << SPR0),
		} Prescaler;
	
	public:
		Spi(Mode mode, Prescaler prescaler);
		
		static uint8_t
		send(uint8_t data);
	};
}

#endif // XPCC__SPI_HPP
