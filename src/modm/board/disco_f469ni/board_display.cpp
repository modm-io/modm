/*
 * Copyright (c) 2016-2018, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include "board.hpp"
#include <modm/architecture/interface/memory.hpp>

#include <algorithm>

extern void
board_initialize_display(uint8_t);

// Basic implementation of display running on memory mapped buffer
class DsiDisplay : public modm::ColorGraphicDisplay
{
public:
	DsiDisplay() : buffer(new (modm::MemoryExternal) uint16_t[800*480])
	{
		Board::setDisplayBuffer((void *) buffer);
	}

	uint16_t
	getWidth() const override
	{ return 800; }

	uint16_t
	getHeight() const override
	{ return 480; }

	inline std::size_t
	getBufferWidth() const final
	{
		return 800;
	}

	inline std::size_t
	getBufferHeight() const final
	{
		return 480;
	}

	void
	clear() final
	{
		std::fill(buffer, buffer + this->getBufferWidth()*this->getBufferHeight(), this->backgroundColor.color);
	}

	void
	update() final
	{
		// FIXME: avoid tearing by using double buffering!
	}

	void
	setPixel(int16_t x, int16_t y) final
	{
		if (x < 0 or 800 <= x or y < 0 or 480 <= y) return;
		buffer[y * 800 + x] = this->foregroundColor.color;
	}

	void
	clearPixel(int16_t x, int16_t y) final
	{
		if (x < 0 or 800 <= x or y < 0 or 480 <= y) return;
		buffer[y * 800 + x] = this->backgroundColor.color;
	}

	modm::color::Rgb565
	getPixel(int16_t x, int16_t y) const final
	{
		if (x < 0 or 800 <= x or y < 0 or 480 <= y) return false;
		return buffer[y * 800 + x];
	}

protected:
	uint16_t * const buffer;
};

void
Board::initializeDisplay()
{
	board_initialize_display(2);
}

void
Board::setDisplayBuffer(void * buffer)
{
	// ColorCoding: 0 = ARGB8888, 2 = RGB565
	board_initialize_display(2);
	// Configures the color frame buffer start address
	LTDC_Layer1->CFBAR = uint32_t(buffer);
	// Enable LTDC_Layer by setting LEN bit
	LTDC_Layer1->CR = LTDC_LxCR_LEN;
	// Sets the Reload type
	LTDC->SRCR = LTDC_SRCR_IMR;
}

void *
Board::getDisplayBuffer()
{
	return (void *) LTDC_Layer1->CFBAR;
}

modm::ColorGraphicDisplay&
Board::getDisplay()
{
	static DsiDisplay display;
	return display;
}
