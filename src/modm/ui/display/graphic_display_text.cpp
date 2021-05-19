/*
 * Copyright (c) 2010-2011, 2013, Fabian Greif
 * Copyright (c) 2012-2013, Niklas Hauser
 * Copyright (c) 2014, Daniel Krebs
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include "graphic_display.hpp"

// ----------------------------------------------------------------------------
uint8_t
modm::GraphicDisplay::getFontHeight() const
{
	return modm::GraphicDisplay::getFontHeight(&(this->font));
}

uint8_t
modm::GraphicDisplay::getFontHeight(const modm::accessor::Flash<uint8_t> *font)
{
	if (!font->isValid())
		return 0;

	return (*font)[3];
}

// ----------------------------------------------------------------------------
uint16_t
modm::GraphicDisplay::getStringWidth(const char* s) const
{
	return modm::GraphicDisplay::getStringWidth(s, &(this->font));
}


uint16_t
modm::GraphicDisplay::getStringWidth(const char* s, const modm::accessor::Flash<uint8_t> *font)
{
	if (!font->isValid())
		return 0;

	const uint8_t offsetWidthTable 	= 8;
	const uint8_t vspace 			= (*font)[5];
	const uint8_t first 			= (*font)[6];

	uint16_t width = 0;

	while(*s) {
		width += (*font)[offsetWidthTable + (static_cast<uint8_t>(*s) - first)];
		width += vspace;
		s++;
	}

	return width;
}

// ----------------------------------------------------------------------------
void
modm::GraphicDisplay::write(char c)
{
	if (!this->font.isValid())
		return;

	const uint8_t character = static_cast<uint8_t>(c);
	const uint8_t height = font[3];
	const uint8_t hspace = font[4];
	const uint8_t vspace = font[5];

	if (character == '\n') {
		this->cursor.set(0, this->cursor.y + height + hspace);
		return;
	}

	const uint8_t first = font[6];
	const uint8_t count = font[7];

	if (character >= (first + count) || character < first) {
		// character is not contained in this font set
		return;
	}

	const uint8_t offsetWidthTable = 8;

	uint16_t offset = count + offsetWidthTable;
	uint8_t position = character - first + offsetWidthTable;
	const uint8_t usedRows = (height + 7) / 8;	// round up
	for (uint8_t i = offsetWidthTable; i < position; i++)
	{
		offset += font[i] * usedRows;
	}
	uint8_t width = font[position];

	this->drawImageRaw(cursor, width, height,
			accessor::asFlash(font.getPointer() + offset));

	cursor.setX(cursor.x + width);

	// all characters below 128 have whitespace afterwards (number given
	// by vspace).
	if (character < 128) {
		//color::Rgb565 oldColor = this->color;

		//this->setColor(color::html::White);
		for (uint_fast8_t i = 0; i < vspace; ++i) {
			//this->drawVerticalLine(cursor, height);
			cursor.setX(cursor.x + 1);
		}

		// restore color
		//this->setColor(oldColor);
	}
}

// ----------------------------------------------------------------------------
void
modm::GraphicDisplay::Writer::write(char c)
{
	this->parent->write(c);
}

void
modm::GraphicDisplay::Writer::flush()
{
}

bool
modm::GraphicDisplay::Writer::read(char&)
{
	return false;
}
