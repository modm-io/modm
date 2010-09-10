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

#ifndef XPCC__GRAPHIC_DISPLAY_HPP
#define XPCC__GRAPHIC_DISPLAY_HPP

#include <stdint.h>

namespace xpcc
{
	/**
	 * \brief	Base class for graphical displays 
	 * 
	 * \ingroup	lcd
	 */
	class GraphicDisplay
	{
	public:
		virtual
		~GraphicDisplay()
		{
		}
		
		virtual void
		clear() = 0;
		
		virtual void
		clear(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2) = 0;
		
		virtual void
		setPixel(uint8_t x, uint8_t y) = 0;
		
		virtual void
		clearPixel(uint8_t x, uint8_t y) = 0;
		
		/**
		 * \brief	Draw line
		 * 
		 * Uses the faster drawHorizontalLine() or drawVerticalLine() if
		 * possible, otherwise the line is rastered with the bresenham line
		 * algorithm.
		 * 
		 * \param x1	first point
		 * \param y1	first point
		 * \param x2	second point
		 * \param y2	second point
		 */
		void
		drawLine(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2);
		
		virtual void
		drawHorizontalLine(uint8_t x1, uint8_t y1, uint8_t length);
		
		virtual void
		drawVerticalLine(uint8_t x1, uint8_t y1, uint8_t length);
		
		void
		drawRectangle(uint8_t x, uint8_t y, uint8_t width, uint8_t height);
		
		void
		drawRoundedRectangle(uint8_t x, uint8_t y,
				uint8_t width, uint8_t height,
				uint8_t radius);
		
		/**
		 * \brief	Draw a cricle
		 * 
		 * Uses the midpoint circle algorithm.
		 * 
		 * \param cx	x-coordinate of the center
		 * \param cy	y-coordinate of the center
		 */
		void
		drawCircle(uint8_t cx, uint8_t cy, uint8_t radius);
		
		void
		drawEllipse(uint8_t cx, uint8_t cy, uint8_t rx, uint8_t ry);
		
		void
		fillRectangle(uint8_t x, uint8_t y, uint8_t width, uint8_t height);
		
	protected:
		/// helper method for drawCircle()
		void
		drawCircle4(uint8_t cx, uint8_t cy, uint8_t x, uint8_t y);
	};
}

#endif	// XPCC__GRAPHIC_DISPLAY_HPP
