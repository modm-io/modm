/*
 * Copyright (c) 2010-2011, 2013, Fabian Greif
 * Copyright (c) 2012-2013, Niklas Hauser
 * Copyright (c) 2014, Daniel Krebs
 * Copyright (c) 2021, Thomas Sommer
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#pragma once
#include "buffer.hpp"

using namespace modm::color;
using namespace modm::shape;

namespace modm::graphic {

template<Color C, Size R>
void
Buffer<C, R>::setFont(const uint8_t* font)
{
	if (modm::accessor::asFlash(font).isValid())
	{
		this->font = modm::accessor::asFlash(font);
		font_metadata.update(this->font);
	}
}

template<Color C, Size R>
uint16_t
Buffer<C, R>::getStringWidth(const char* s) const
{
	uint16_t width = 0;

	while (*s)
	{
		width += (*font)[offsetWidthTable + (uint8_t(*s) - font_metadata.first)];
		width += font_metadata.vspace;
		s++;
	}

	return width;
}

template<Color C, Size R>
uint16_t
Buffer<C, R>::getStringWidth(
	const char* s, const modm::accessor::Flash<uint8_t>* font)
{
	uint16_t width = 0;

	const uint8_t vspace 			= (*font)[5];
	const uint8_t first 			= (*font)[6];

	while (*s)
	{
		width += (*font)[offsetWidthTable + (uint8_t(*s) - first)];
		width += vspace;
		s++;
	}

	return width;
}

template<Color C, Size R>
void
Buffer<C, R>::writeChar(char character)
{
	if (character == '\n')
	{
		linebreak();
		return;
	}

	if (character == '\t')
	{
		// TODO
		const uint8_t tab_width = 2 * font_metadata.height;
		if (cursor.x < R.x + tab_width)
			cursor.x += tab_width;
		else
			linebreak();
		return;
	}

	// Character not contained in this font?
	if (character < font_metadata.first or character >= (font_metadata.first + font_metadata.count))
		return;

	const uint8_t usedRows = (font_metadata.height + 7) / 8;  // round up

	const uint8_t position = character - font_metadata.first + offsetWidthTable;
	const uint8_t width = font[position];

	uint16_t offset = font_metadata.count + offsetWidthTable;
	for (uint8_t i = offsetWidthTable; i < position; i++) { offset += font[i] * usedRows; }

	if (auto_linebreak and cursor.x > R.x - width)
		linebreak();

	this->writeImage(ImageAccessor<color::Monochrome, modm::accessor::Flash>(font.getPointer() + offset, {width, font_metadata.height}, cursor));

	// Add Whitespace
	cursor.x += character < 128 ? width + font_metadata.vspace : width;
}

} // namespace modm::graphic