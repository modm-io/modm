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

#ifndef XPCC__SOFTWARE_SPI_HPP
#define XPCC__SOFTWARE_SPI_HPP

#include <stdint.h>
#include <xpcc/architecture/general/time/delay.hpp>

namespace xpcc
{
	/**
	 * \brief	Software emulation of a SPI bus master
	 * 
	 * SPI stands for Serial Peripheral Interface Bus.
	 * 
	 * \todo	documentation
	 * 
	 * \tparam	CLK			clock pin [output]
	 * \tparam	MOSI		master out slave in pin [output]
	 * \tparam	MISO		master in slave out pin [input]
	 * \tparam	FREQUENCY	requested SPI frequency in Hz
	 * 
	 * \ingroup	driver
	 * \see		gpio
	 */
	template <typename CLK,
			  typename MOSI,
			  typename MISO,
			  int32_t FREQUENCY = 1000000>
	class SoftwareSpi
	{
	public:
		SoftwareSpi();
		
		static void
		initialize();
		
		static uint8_t
		write(uint8_t output);
		
	private:
		// calculate the delay in microseconds needed to achieve the
		// requested SPI frequency
		static const float delay = (1000000.0 / FREQUENCY) / 2.0;
	};
}

#include "software_spi_impl.hpp"

#endif // XPCC__SOFTWARE_SPI_HPP
