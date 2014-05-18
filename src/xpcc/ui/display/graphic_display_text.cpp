// coding: utf-8
// ----------------------------------------------------------------------------
/* Copyright (c) 2009, Roboterclub Aachen e.V.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 *	 * Redistributions of source code must retain the above copyright
 *	   notice, this list of conditions and the following disclaimer.
 *	 * Redistributions in binary form must reproduce the above copyright
 *	   notice, this list of conditions and the following disclaimer in the
 *	   documentation and/or other materials provided with the distribution.
 *	 * Neither the name of the Roboterclub Aachen e.V. nor the
 *	   names of its contributors may be used to endorse or promote products
 *	   derived from this software without specific prior written permission.
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
 */
// ----------------------------------------------------------------------------

#include "graphic_display.hpp"

// ----------------------------------------------------------------------------
uint8_t
xpcc::GraphicDisplay::getFontHeight() const
{
	if (!this->font.isValid())
		return 0;
	
	return font[3];
}

// ----------------------------------------------------------------------------
uint16_t
xpcc::GraphicDisplay::getStringWidth(const char* s) const
{
	if (!this->font.isValid())
		return 0;

	const uint8_t offsetWidthTable 	= 8;
	const uint8_t vspace 			= this->font[5];
	const uint8_t first 			= this->font[6];

	uint16_t width = 0;

	while(*s) {
		width += this->font[offsetWidthTable + (static_cast<uint8_t>(*s) - first)];
		width += vspace;
		s++;
	}

	return width;
}

// ----------------------------------------------------------------------------
void
xpcc::GraphicDisplay::write(char c)
{
	if (!this->font.isValid())
		return;
	
	const uint8_t character = static_cast<uint8_t>(c);
	const uint8_t height = font[3];
	const uint8_t hspace = font[4];
	const uint8_t vspace = font[5];
	
	if (character == '\n') {
		this->cursor.set(0, this->cursor.getY() + height + hspace);
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
	
	cursor.setX(cursor.getX() + width);
	
	// all characters below 128 have whitespace afterwards (number given
	// by vspace).
	if (character < 128) {
		//glcd::Color oldColor = this->color;
		
		//this->setColor(glcd::Color::white());
		for (uint_fast8_t i = 0; i < vspace; ++i) {
			//this->drawVerticalLine(cursor, height);
			cursor.setX(cursor.getX() + 1);
		}
		
		// restore color
		//this->setColor(oldColor);
	}
}

// ----------------------------------------------------------------------------
void
xpcc::GraphicDisplay::Writer::write(char c)
{
	this->parent->write(c);
}

void
xpcc::GraphicDisplay::Writer::flush()
{
}

bool
xpcc::GraphicDisplay::Writer::read(char&)
{
	return false;
}
