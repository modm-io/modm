// coding: utf-8
// ----------------------------------------------------------------------------
/* Copyright (c) 2009, Roboterclub Aachen e.V.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Roboterclub Aachen e.V. nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
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
 *
 * $Id$
 */
// ----------------------------------------------------------------------------

#ifndef XPCC__BUFFERED_GRAPHIC_DISPLAY_HPP
#define XPCC__BUFFERED_GRAPHIC_DISPLAY_HPP

#include <stdlib.h>
#include "graphic_display.hpp"

namespace xpcc
{
	/**
	 * \brief	Base class for graphical displays with a RAM buffer
	 * 
	 * Every operation works on the internal RAM buffer, therefore the content
	 * of the real display is not changed until a call of update().
	 * 
	 * \tparam	Width	Width of the display.
	 * \tparam	Rows	Number of rows. The height of the display is 'Rows * 8'.
	 * 
	 * \author	Fabian Greif
	 * \ingroup	lcd
	 */
	template <uint8_t Width, uint8_t Rows>
	class BufferedGraphicDisplay : public GraphicDisplay
	{
	public:
		virtual
		~BufferedGraphicDisplay()
		{
		}
		
		/**
		 * \brief	Clear the complete screen
		 * 
		 * Use fillRectangle() to clear certain areas of the screen.
		 */
		virtual void
		clear();
		
		/**
		 * \brief	Transfer the content of the RAM buffer to the real display.
		 */
		virtual void
		update() = 0;
		
		// Faster version adapted for the RAM buffer
		virtual void
		drawImageRaw(glcd::Point upperLeft,
				uint8_t width, uint8_t height,
				xpcc::accessor::Flash<uint8_t> data);
		
	protected:
		// Faster version adapted for the RAM buffer
		virtual void
		drawHorizontalLine(glcd::Point start, uint8_t length);
		
		// TODO Faster version adapted for the RAM buffer
		//virtual void
		//drawVerticalLine(glcd::Point start, uint8_t length);
		
		virtual void
		setPixel(uint8_t x, uint8_t y);
		
		virtual void
		clearPixel(uint8_t x, uint8_t y);
		
		virtual bool
		getPixel(uint8_t x, uint8_t y);
		
		uint8_t buffer[Width][Rows];
	};
}

#include "buffered_graphic_display_impl.hpp"

#endif	// XPCC__GRAPHIC_DISPLAY_HPP
