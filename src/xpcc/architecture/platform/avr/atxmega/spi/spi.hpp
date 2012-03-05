// coding: utf-8
// ----------------------------------------------------------------------------
/* Copyright (c) 2012, Roboterclub Aachen e.V.
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
 * $Id: spi.hpp 751 2012-03-02 21:38:40Z salkinium $
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_ATXMEGA__SPI_HPP
#define XPCC_ATXMEGA__SPI_HPP

namespace xpcc
{
	namespace atxmega
	{
		namespace spi
		{
			enum Mode
			{
				MODE_0 = SPI_MODE_0_gc,	///< SCK normal, sample on rising edge
				MODE_1 = SPI_MODE_1_gc, ///< SCK normal, sample on falling edge
				MODE_2 = SPI_MODE_2_gc, ///< SCK inverted, sample on falling edge
				MODE_3 = SPI_MODE_3_gc	///< SCK inverted, sample on rising edge
			};
			
			enum Prescaler
			{
				PRESCALER_2 = SPI_CLK2X_bm | SPI_PRESCALER_DIV4_gc,
				PRESCALER_4 = SPI_PRESCALER_DIV4_gc,
				PRESCALER_8 = SPI_CLK2X_bm | SPI_PRESCALER_DIV16_gc,
				PRESCALER_16 = SPI_PRESCALER_DIV16_gc,
				PRESCALER_32 = SPI_CLK2X_bm | SPI_PRESCALER_DIV64_gc,
				PRESCALER_64 = SPI_PRESCALER_DIV64_gc,
				PRESCALER_128 = SPI_PRESCALER_DIV128_gc
			};
		}
	}
}

#endif	// XPCC_ATXMEGA__SPI_HPP
