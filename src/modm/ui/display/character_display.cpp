/*
 * Copyright (c) 2009-2011, Fabian Greif
 * Copyright (c) 2012-2013, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include "character_display.hpp"

modm::CharacterDisplay::CharacterDisplay(uint8_t width, uint8_t height) :
	IOStream(writer),
	writer(this),
	lineWidth(width), lineCount(height),
	column(0), line(0)
{
}

void
modm::CharacterDisplay::write(char c)
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
modm::CharacterDisplay::Writer::write(char c)
{
	this->parent->write(c);
}

void
modm::CharacterDisplay::Writer::flush()
{
}

bool
modm::CharacterDisplay::Writer::read(char&)
{
	return false;
}
