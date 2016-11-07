/*
 * Copyright (c) 2009-2011, Fabian Greif
 * Copyright (c) 2012-2014, Niklas Hauser
 * Copyright (c) 2014, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC__ST7036_HPP
	#error	"Don't include this file directly, use 'st7036.hpp' instead!"
#endif

// ----------------------------------------------------------------------------

namespace xpcc {
	namespace st7036 {
		EXTERN_FLASH_STORAGE(uint8_t configuration[10]);
	}
}

// ----------------------------------------------------------------------------

template <typename SPI, typename CS, typename RS, unsigned int Width, unsigned int Heigth>
xpcc::St7036<SPI, CS, RS, Width, Heigth>::St7036() :
	CharacterDisplay(Width, Heigth)
{
}

// ----------------------------------------------------------------------------

template <typename SPI, typename CS, typename RS, unsigned int Width, unsigned int Heigth>
void
xpcc::St7036<SPI, CS, RS, Width, Heigth>::initialize()
{
	accessor::Flash<uint8_t> config(st7036::configuration);
	for (uint8_t i = 0; i < 10; ++i)
	{
		writeCommand(config[i]);
	}
}

template <typename SPI, typename CS, typename RS, unsigned int Width, unsigned int Heigth>
void
xpcc::St7036<SPI, CS, RS, Width, Heigth>::writeRaw(char c)
{
	RS::set();

	CS::reset();
	SPI::transferBlocking(c);
	CS::set();
}

template <typename SPI, typename CS, typename RS, unsigned int Width, unsigned int Heigth>
void
xpcc::St7036<SPI, CS, RS, Width, Heigth>::execute(Command command)
{
	writeCommand(command);
}

template <typename SPI, typename CS, typename RS, unsigned int Width, unsigned int Heigth>
void
xpcc::St7036<SPI, CS, RS, Width, Heigth>::setCursor(uint8_t newLine, uint8_t newColumn)
{
	this->column = newColumn;
	this->line = newLine;

	newColumn += 0x40 * newLine;
	writeCommand(0x80 | newColumn);
}

// ----------------------------------------------------------------------------

template <typename SPI, typename CS, typename RS, unsigned int Width, unsigned int Heigth>
void
xpcc::St7036<SPI, CS, RS, Width, Heigth>::writeCommand(uint8_t inCommand)
{
	RS::reset();

	CS::reset();
	SPI::transferBlocking(inCommand);
	CS::set();

	// check if the command is 'clear display' oder 'return home', these
	// commands take a bit longer until they are finished.
	if ((inCommand & 0xfc) == 0) {
		xpcc::delayMicroseconds(1200);
	}
	else {
		xpcc::delayMicroseconds(27);
	}
}
