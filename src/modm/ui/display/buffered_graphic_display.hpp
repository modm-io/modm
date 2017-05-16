/*
 * Copyright (c) 2009, Georgi Grinshpun
 * Copyright (c) 2009-2011, 2013, Fabian Greif
 * Copyright (c) 2010, Martin Rosekeit
 * Copyright (c) 2011, Thorsten Lajewski
 * Copyright (c) 2012-2015, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_BUFFERED_GRAPHIC_DISPLAY_HPP
#define MODM_BUFFERED_GRAPHIC_DISPLAY_HPP

#include <stdlib.h>
#include "graphic_display.hpp"

namespace modm
{
	/**
	 * Base class for graphical displays with a RAM buffer.
	 *
	 * Every operation works on the internal RAM buffer, therefore the content
	 * of the real display is not changed until a call of update().
	 *
	 * \tparam	Width	Width of the display.
	 * \tparam	Height	Height of the display. Must be a multiple of 8!
	 *
	 * \author	Fabian Greif
	 * \ingroup	graphics
	 */
	template <uint16_t Width, uint16_t Height>
	class BufferedGraphicDisplay : public GraphicDisplay
	{
		// Height must be a multiple of 8
		static_assert((Height % 8) == 0, "height must be a multiple of 8");

	public:
		static constexpr uint16_t DisplayBufferWidth = Width;
		static constexpr uint16_t DisplayBufferHeight = Height / 8;

	public:
		virtual
		~BufferedGraphicDisplay()
		{
		}

		virtual  inline uint16_t
		getWidth() const
		{
			return Width;
		}

		virtual inline uint16_t
		getHeight() const
		{
			return Height;
		}

		/**
		 * \brief	Clear the complete screen
		 *
		 * Use fillRectangle() to clear certain areas of the screen.
		 */
		virtual void
		clear();

		// Faster version adapted for the RAM buffer
		virtual void
		drawImageRaw(glcd::Point upperLeft,
				uint16_t width, uint16_t height,
				modm::accessor::Flash<uint8_t> data);

	protected:
		// Faster version adapted for the RAM buffer
		virtual void
		drawHorizontalLine(glcd::Point start, uint16_t length);

		// TODO Faster version adapted for the RAM buffer
		//virtual void
		//drawVerticalLine(glcd::Point start, uint8_t length);

		virtual void
		setPixel(int16_t x, int16_t y);

		virtual void
		clearPixel(int16_t x, int16_t y);

		virtual bool
		getPixel(int16_t x, int16_t y);

		uint8_t display_buffer[DisplayBufferWidth][DisplayBufferHeight];
	};
}

#include "buffered_graphic_display_impl.hpp"

#endif	// MODM_GRAPHIC_DISPLAY_HPP
