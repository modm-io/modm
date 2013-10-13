// coding: utf-8
/* Copyright (c) 2009, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#include "character_display.hpp"

xpcc::CharacterDisplay::CharacterDisplay(uint8_t width, uint8_t height) :
	IOStream(writer),
	writer(this),
	lineWidth(width), lineCount(height),
	column(0), line(0)
{
}

void
xpcc::CharacterDisplay::write(char c)
{
	if (c == '\n')
	{
		line++;
		if (line >= lineCount) {
			line = 0;
		}
		column = 0;
		this->setCursor(column, line);
	}
	else {
		if (column >= lineWidth) {
			this->write('\n');
		}
		this->writeRaw(c);
		column++;
	}
}

// ----------------------------------------------------------------------------
void
xpcc::CharacterDisplay::Writer::write(char c)
{
	this->parent->write(c);
}

void
xpcc::CharacterDisplay::Writer::flush()
{
}

bool
xpcc::CharacterDisplay::Writer::read(char&)
{
	return false;
}
