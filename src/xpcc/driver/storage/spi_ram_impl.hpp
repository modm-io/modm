/*
 * Copyright (c) 2011, Fabian Greif
 * Copyright (c) 2012, Niklas Hauser
 * Copyright (c) 2014, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_SPI_RAM_HPP
	#error	"Don't include this file directly, use 'spi_ram.hpp' instead!"
#endif

#include <modm/architecture/driver/delay.hpp>

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
	
	xpcc::delayMicroseconds(1);
	
	cs.reset();
	spi.write(WRITE_STATUS_REGISTER);
	spi.write(SEQUENTIAL_MODE);
	cs.set();
	
	xpcc::delayMicroseconds(1);
	
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
