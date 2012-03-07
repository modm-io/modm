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
 * $Id: spi.hpp 625 2011-11-15 17:47:41Z dergraaf $
 */
// ----------------------------------------------------------------------------

/**
 * \ingroup		atxmega
 * \defgroup	atxmega_spi	SPI Module
 *
 * Serial Peripheral Interface Module
 *
 * This SPI hardware implementation is compatible with both Software SPI as
 * well as Uart SPI.
 * The write method is blocking until the SPI Interrupt Flag is set (=> SPI
 * transfer is complete).
 * It can be used for higher SPI clock speeds than feasible with Software SPI.
 *
 * The MISO pin uses a pull-up resistor so that if the SPI slave is accidentally
 * disconnected, the write method does not block, but returns 0x00 or 0xff.
 * If your SPI slave has a "Data Ready" pin, which triggers a SPI readout, make
 * sure to use a pullup/pulldown on the input pin, to keep a disconnected slave
 * from causing contiguous readouts.
 *
 * \code
 * #include <xpcc/architecture/platform.hpp>
 *
 * // create a new typedef to shorten the name
 * typedef xpcc::atxmega::SpiMasterC Spi;
 *
 * int
 * main(void)
 * {
 *     ...
 *     Spi::initialize(SPI_PRESCALER_DIV64_gc);     // faster clock
 *     // or without the typedef
 *     // xpcc::atxmega::SpiMasterC::initialize(SPI_PRESCALER_DIV64_gc);
 *
 *     uint8_t result = Spi::write(0xf3);   // write a byte
 *     ...
 * }
 * \endcode
 *
 * \see		AVR1309
 * \see		xpcc::SoftwareSpi
 * \author	Niklas Hauser
 */

#include "spi/spi_c.hpp"
#include "spi/spi_d.hpp"
#include "spi/spi_e.hpp"
#include "spi/spi_f.hpp"
