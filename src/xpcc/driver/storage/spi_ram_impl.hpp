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
 * $Id: spi_ram_impl.hpp 605 2011-09-13 14:49:54Z dergraaf $
 */
// ----------------------------------------------------------------------------

#ifndef XPCC__SPI_RAM_HPP
	#error	"Don't include this file directly, use 'spi_ram.hpp' instead!"
#endif

#include <xpcc/architecture/driver/delay.hpp>

template <typename Spi, typename Cs, typename Hold>
Spi xpcc::SpiRam<Spi, Cs, Hold>::spi;

template <typename Spi, typename Cs, typename Hold>
Cs xpcc::SpiRam<Spi, Cs, Hold>::cs;

template <typename Spi, typename Cs, typename Hold>
Hold xpcc::SpiRam<Spi, Cs, Hold>::hold;

// ----------------------------------------------------------------------------
template <typename Spi, typename Cs, typename Hold>
bool
xpcc::SpiRam<Spi, Cs, Hold>::initialize()
{
	cs.set();
	cs.setOutput();
	
	spi.initialize();
	
	hold.set();
	hold.setOutput();
	
	xpcc::delay_us(1.0);
	
	cs.reset();
	spi.write(WRITE_STATUS_REGISTER);
	spi.write(SEQUENTIAL_MODE);
	cs.set();
	
	xpcc::delay_us(1.0);
	
	// Check if the status register has the right content. This also used
	// as a general check that the device is available.
	cs.reset();
	spi.write(READ_STATUS_REGISTER);
	bool result = (spi.write(0) == (SEQUENTIAL_MODE));
	cs.set();
	
	return result;
}

// ----------------------------------------------------------------------------
template <typename Spi, typename Cs, typename Hold>
void
xpcc::SpiRam<Spi, Cs, Hold>::startRead(uint16_t address)
{
	cs.reset();
	spi.write(READ);
	spi.write(address >> 8);
	spi.write(address & 0xff);	
}

// ----------------------------------------------------------------------------
template <typename Spi, typename Cs, typename Hold>
void
xpcc::SpiRam<Spi, Cs, Hold>::startWrite(uint16_t address)
{
	cs.reset();
	spi.write(WRITE);
	spi.write(address >> 8);
	spi.write(address & 0xff);
}

// ----------------------------------------------------------------------------
template <typename Spi, typename Cs, typename Hold>
void
xpcc::SpiRam<Spi, Cs, Hold>::writeByte(uint8_t data)
{
	spi.write(data);
}

// ----------------------------------------------------------------------------
template <typename Spi, typename Cs, typename Hold>
void
xpcc::SpiRam<Spi, Cs, Hold>::write(const uint8_t *data, std::size_t size)
{
	for (std::size_t i = 0; i < size; ++i) {
		spi.write(*data++);
	}
}

// ----------------------------------------------------------------------------
template <typename Spi, typename Cs, typename Hold>
uint8_t
xpcc::SpiRam<Spi, Cs, Hold>::readByte()
{
	return spi.write(0);
}

// ----------------------------------------------------------------------------
template <typename Spi, typename Cs, typename Hold>
void
xpcc::SpiRam<Spi, Cs, Hold>::read(uint8_t *data, std::size_t size)
{
	for (std::size_t i = 0; i < size; ++i) {
		*data++ = spi.write(0);
	}
}

// ----------------------------------------------------------------------------
template <typename Spi, typename Cs, typename Hold>
void
xpcc::SpiRam<Spi, Cs, Hold>::suspend()
{
	hold.reset();
}

// ----------------------------------------------------------------------------
template <typename Spi, typename Cs, typename Hold>
void
xpcc::SpiRam<Spi, Cs, Hold>::resume()
{
	hold.set();
}

// ----------------------------------------------------------------------------
template <typename Spi, typename Cs, typename Hold>
void
xpcc::SpiRam<Spi, Cs, Hold>::finish()
{
	resume();
	cs.set();
}

// ----------------------------------------------------------------------------
template <typename Spi, typename Cs, typename Hold>
void
xpcc::SpiRam<Spi, Cs, Hold>::writeData(uint16_t address,
		const uint8_t *data, std::size_t size)
{
	startWrite(address);
	write(data, size);
	finish();
}
	
// ----------------------------------------------------------------------------
template <typename Spi, typename Cs, typename Hold>
void
xpcc::SpiRam<Spi, Cs, Hold>::readData(uint16_t address,
		uint8_t *data, std::size_t size)
{
	startRead(address);
	read(data, size);
	finish();
}
