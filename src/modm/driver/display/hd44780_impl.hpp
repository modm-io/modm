/*
 * Copyright (c) 2009-2011, Fabian Greif
 * Copyright (c) 2011-2013, Niklas Hauser
 * Copyright (c) 2014-2015, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_HD44780_HPP
	#error	"Don't include this file directly, use 'hd44780.hpp' instead!"
#endif

// ----------------------------------------------------------------------------
template <typename DATA, typename RW, typename RS, typename E>
modm::Hd44780<DATA, RW, RS, E>::Hd44780(uint8_t width, uint8_t height) :
	CharacterDisplay(width, height)
{
}

template <typename DATA, typename RW, typename RS, typename E>
void
modm::Hd44780<DATA, RW, RS, E>::initialize()
{
	driver::initialize(this->lineCount == 1 ?
			driver::LineMode::OneLine :
			driver::LineMode::TwoLines);
}

template <typename DATA, typename RW, typename RS, typename E>
void
modm::Hd44780<DATA, RW, RS, E>::writeRaw(char c)
{
	while(!driver::writeRAM(static_cast<uint8_t>(c)))
		;
}

template <typename DATA, typename RW, typename RS, typename E>
void
modm::Hd44780<DATA, RW, RS, E>::setCursor(uint8_t column, uint8_t line)
{
	this->column = (column <= this->lineWidth ? column : this->lineWidth);
	this->line   = (line   <= this->lineCount ? line   : this->lineCount);

	uint8_t address = this->column + 0x40 * this->line;
	if (this->line >= 2) {
		address += 20;
	}
	while(!driver::writeAddress(address))
		;
}

template <typename DATA, typename RW, typename RS, typename E>
void
modm::Hd44780<DATA, RW, RS, E>::execute(Command command)
{
	while(!driver::writeCommand(command))
		;
}

template <typename DATA, typename RW, typename RS, typename E>
void
modm::Hd44780<DATA, RW, RS, E>::writeCGRAM(uint8_t character, uint8_t *cg)
{
	// There are only 8 characters in CGRAM
	if (character > 8) {
		return;
	}
	while(not driver::writeCGRAM(character, cg))
			;
}

template <typename DATA, typename RW, typename RS, typename E>
void
modm::Hd44780<DATA, RW, RS, E>::clear()
{
	while(!driver::clear())
		;
	while(!driver::resetCursor())
		;
}


// ----------------------------------------------------------------------------
template <typename DATA, typename RW, typename RS, typename E1, typename E2>
modm::Hd44780Dual<DATA, RW, RS, E1, E2>::Hd44780Dual(uint8_t width, uint8_t height) :
	CharacterDisplay(width, height)
{
}

template <typename DATA, typename RW, typename RS, typename E1, typename E2>
void
modm::Hd44780Dual<DATA, RW, RS, E1, E2>::initialize()
{
	driver1::initialize(driver1::LineMode::TwoLines);
	driver2::initialize(driver2::LineMode::TwoLines);
}

template <typename DATA, typename RW, typename RS, typename E1, typename E2>
void
modm::Hd44780Dual<DATA, RW, RS, E1, E2>::writeRaw(char c)
{
	if (this->line < 2) {
		while(!driver1::writeRAM(static_cast<uint8_t>(c)))
			;
	} else {
		while(!driver2::writeRAM(static_cast<uint8_t>(c)))
			;
	}
}

template <typename DATA, typename RW, typename RS, typename E1, typename E2>
void
modm::Hd44780Dual<DATA, RW, RS, E1, E2>::setCursor(uint8_t column, uint8_t line)
{
	this->column = (column <= this->lineWidth ? column : this->lineWidth);
	this->line = (line <= this->lineCount ? line : this->lineCount);

	uint8_t address = this->column;
	if (this->line & 0x01) {
		address += 0x40;
	}

	if (this->line < 2) {
		while(!driver1::writeAddress(address))
			;
	} else {
		while(!driver2::writeAddress(address))
			;
	}
}

template <typename DATA, typename RW, typename RS, typename E1, typename E2>
void
modm::Hd44780Dual<DATA, RW, RS, E1, E2>::execute(Command command)
{
	while(!driver1::writeCommand(command))
		;
	while(!driver2::writeCommand(command))
		;
}

template <typename DATA, typename RW, typename RS, typename E1, typename E2>
void
modm::Hd44780Dual<DATA, RW, RS, E1, E2>::clear()
{
	while(!driver1::clear())
		;
	while(!driver1::resetCursor())
		;
	while(!driver2::clear())
		;
	while(!driver2::resetCursor())
		;
}
