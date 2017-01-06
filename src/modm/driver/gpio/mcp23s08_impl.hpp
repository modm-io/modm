/*
 * Copyright (c) 2009-2011, Fabian Greif
 * Copyright (c) 2009, Martin Rosekeit
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

#ifndef MODM_MCP23S08_HPP
	#error	"Don't include this file directly, use 'mcp23s08.hpp' instead!"
#endif

// ----------------------------------------------------------------------------
template <typename Spi, typename Cs, typename Int>
Spi modm::Mcp23s08<Spi, Cs, Int>::spi;

template <typename Spi, typename Cs, typename Int>
Cs modm::Mcp23s08<Spi, Cs, Int>::cs;

template <typename Spi, typename Cs, typename Int>
Int modm::Mcp23s08<Spi, Cs, Int>::interrupt;

// ----------------------------------------------------------------------------
template <typename Spi, typename Cs, typename Int>
void
modm::Mcp23s08<Spi, Cs, Int>::initialize()
{
	spi.initialize();
	cs.set();
	cs.setOutput();
	interrupt.setInput();
	
	modm::delayMicroseconds(1);
	
	// disable address pins (as they are by default) and enable the
	// open-drain output from the interrupt line
	cs.reset();
	spi.write(deviceAddress | WRITE);
	spi.write(MCP_IOCON);
	spi.write(1 << 2);
	cs.set();
	
	modm::delayMicroseconds(1);
}

template <typename Spi, typename Cs, typename Int>
void
modm::Mcp23s08<Spi, Cs, Int>::configure(uint8_t inputMask, uint8_t pullupMask)
{
	cs.reset();
	spi.write(deviceAddress | WRITE);
	spi.write(MCP_IODIR);
	spi.write(inputMask);
	cs.set();
	
	modm::delayMicroseconds(1);
	
	cs.reset();
	spi.write(deviceAddress | WRITE);
	spi.write(MCP_GPPU);
	spi.write(pullupMask);
	cs.set();
	
	modm::delayMicroseconds(1);
}

//void
//configureInterrupt();

// ----------------------------------------------------------------------------
template <typename Spi, typename Cs, typename Int>
uint8_t
modm::Mcp23s08<Spi, Cs, Int>::read()
{
	cs.reset();
	spi.write(deviceAddress | READ);
	spi.write(MCP_GPIO);
	
	uint8_t value = spi.write(0x00);
	cs.set();
	
	modm::delayMicroseconds(1);
	
	return value;
}

template <typename Spi, typename Cs, typename Int>
void
modm::Mcp23s08<Spi, Cs, Int>::write(uint8_t output)
{
	cs.reset();
	spi.write(deviceAddress | WRITE);
	spi.write(MCP_GPIO);
	spi.write(output);
	cs.set();
	
	modm::delayMicroseconds(1);
}
