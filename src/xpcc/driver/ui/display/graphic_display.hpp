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
 */
// ----------------------------------------------------------------------------

#ifndef XPCC__GRAPHIC_DISPLAY_HPP
#define XPCC__GRAPHIC_DISPLAY_HPP

#include <xpcc/architecture/driver/accessor.hpp>
#include <xpcc/math/geometry.hpp>

#include <xpcc/io/iodevice.hpp>
#include <xpcc/io/iostream.hpp>

#include "font.hpp"

namespace xpcc
{
	namespace glcd
	{
		typedef Vector<int16_t, 2> Point;
		
		// RGB16 (565) Format
		class Color
		{
		public:
			static ALWAYS_INLINE Color white()   { return Color(0xffff); };
			static ALWAYS_INLINE Color yellow()  { return Color(0xFFE0); };
			static ALWAYS_INLINE Color magenta() { return Color(0xF81F); };
			static ALWAYS_INLINE Color red()     { return Color(0xF800); };
			static ALWAYS_INLINE Color orange()  { return Color(0xFD20); };
			static ALWAYS_INLINE Color sliver()  { return Color(0xC618); };
			static ALWAYS_INLINE Color gray()    { return Color(0x8410); };
			static ALWAYS_INLINE Color maroon()  { return Color(0x8000); };
			static ALWAYS_INLINE Color lime()    { return Color(0x07E0); };
			static ALWAYS_INLINE Color green()   { return Color(0x0400); };
			static ALWAYS_INLINE Color blue()    { return Color(0x001F); };
			static ALWAYS_INLINE Color navy()    { return Color(0x0010); };
			static ALWAYS_INLINE Color black()   { return Color(0x0000); };
			
			/**
			 * @param	red
			 * 		Range [0..255]
			 * @param	green
			 * 		Range [0..255]
			 * @param	blue
			 * 		Range [0..255]
			 */
			Color(uint8_t red, uint8_t green, uint8_t blue) :
				color(((static_cast<uint16_t>(red >> 3) << 11) |
						(static_cast<uint16_t>(green >> 2) << 5) |
						static_cast<uint16_t>(blue >> 3)))
			{
			}
			
			Color(uint16_t color) :
				color(color)
			{
			}
			
			inline uint16_t
			getValue() const
			{
				return color;
			}
			
			bool
			operator == (const Color& other) const {
				return (color == other.color);
			}
			
		private:
			uint16_t color;
		};
	}
	
	// TODO
//	enum class Orientation : uint8_t
//	{
//		Portrait, 				//< Connector top
//		LandscapeRight,			//< Connector right
//		LandscapeLeft,			//< Connector left
//		PortraitUpsideDown,		//< Connector bottom
//	};
	
	/**
	 * Base class for graphical displays. 
	 * 
	 * Coordinate System:
	 * \code
	 * (0, 0)
	 *    +---------------------+
	 *    |  ----> X            |
	 *    | |                   |
	 *    | |                   |
	 *    | V Y                 |
	 *    |                     |
	 *    |                     |
	 *    |                     |
	 *    +---------------------+
	 *                e.g. (127, 63)
	 * \endcode
	 * 
	 * The size (width and height) of a graphics primitive always correspond
	 * to its mathematical model, ignoring the rendered with. As everything
	 * is drawn one pixel wide, the pixels will be rendered to the right and
	 * below the mathematically defined points.
	 * 
	 * \ingroup	lcd
	 */
	/*
	 * TODO
	 * 
	 * setBrush() and setPen() for filling and the border?
	 * possible Brush/Pen modes:
	 * - NONE
	 * - BLACK
	 * - WHITE
	 * - INVERT
	 * 
	 * Text mode:
	 * - left adjusted (default)
	 * - right adjusted
	 * - centered
	 * 
	 * All modes relative to the current viewport. This would make
	 * drawing a menu system easier.
	 */
	class GraphicDisplay : public IOStream
	{
	public:

		friend class VirtualGraphicDisplay;

		GraphicDisplay();
		
		virtual
		~GraphicDisplay()
		{
		}
		
		/**
		 * Number of pixel in horizontal direction.
		 */
		virtual uint16_t
		getWidth() const = 0;

		/**
		 * Number of pixel in vertical direction.
		 */
		virtual uint16_t
		getHeight() const = 0;

		/**
		 * Clear screen and reset the cursor.
		 */
		virtual void
		clear() = 0;
		
		/**
		 * Transfer the content of the RAM buffer to the real display.
		 */
		virtual void
		update() = 0;
		
		/**
		 * Set a new foreground color.
		 * 
		 * The foreground color is used for all drawing operations. Default
		 * is white.
		 * 
		 * @see	setBackgroundColor()
		 */
		void
		setColor(const glcd::Color& color);
		
		inline glcd::Color
		getForegroundColor() const
		{
			return this->foregroundColor;
		}
		
		/**
		 * Set new background color.
		 * 
		 * The background color used when clearing the screen. Default is black.
		 * 
		 * @see	setColor()
		 */
		void
		setBackgroundColor(const glcd::Color& color);
		
		/**
		 * Limit the coordinate system to a smaller area.
		 * 
		 * The default viewport is the complete screen.
		 */
		//void
		//setViewport();
		
		// TODO Set a clipping area
		// Everything drawn outside this area will be discarded.
		//void
		//setClippingWindow();
		
		/**
		 * Draw a pixel in currently active foreground color.
		 * 
		 * \param	x	x-position
		 * \param	y	y-position
		 */
		inline void
		drawPixel(int16_t x, int16_t y)
		{
			(this->*draw)(x, y);
		}
		
		inline void
		drawPixel(glcd::Point center)
		{
			(this->*draw)(center.x, center.y);
		}
		
		/**
		 * Draw a line.
		 * 
		 * Uses the faster drawHorizontalLine() or drawVerticalLine() if
		 * possible, otherwise the line is rastered with the Bresenham line
		 * algorithm.
		 * 
		 * \param start	first point
		 * \param end	second point
		 */
		inline void
		drawLine(glcd::Point start, glcd::Point end)
		{
			this->drawLine(start.getX(), start.getY(), end.getX(), end.getY());
		}
		
		/**
		 * Draw a line specified by x and y coordinates of both points.
		 */
		void
		drawLine(int16_t x1, int16_t y1, int16_t x2, int16_t y2);
		
		inline void
		drawRectangle(int16_t x, int16_t y, uint16_t width, uint16_t height)
		{
			drawRectangle(glcd::Point(x, y), width, height);
		}
		
		/**
		 * Draw a rectangle.
		 */
		void
		drawRectangle(glcd::Point upperLeft, uint16_t width, uint16_t height);
		
		/**
		 * Draw a rectangle with rounded corners.
		 */
		void
		drawRoundedRectangle(glcd::Point upperLeft,
				uint16_t width, uint16_t height,
				uint16_t radius);
		
		/**
		 * Draw a circle
		 * 
		 * Uses the midpoint circle algorithm.
		 * 
		 * \param center	Center of the circle
		 * \param radius	Radius of the circle
		 */
		void
		drawCircle(glcd::Point center, uint16_t radius);
		
		/**
		 * Draw an ellipse.
		 * 
		 * Uses a variation of the midpoint algorithm. May be improved through
		 * simplification of the used formula.
		 * 
		 * \param center	Center of the ellipse
		 * \param rx		radius in x-direction
		 * \param ry		radius in y-direction
		 */
		void
		drawEllipse(glcd::Point center, int16_t rx, int16_t ry);
		
		/**
		 * Draw an image.
		 * 
		 * The first byte in the image data specifies the with, the second
		 * byte the height. Afterwards the actual image data.
		 * 
		 * \param upperLeft		Upper left corner
		 * \param image			Image data
		 * 
		 * \see	drawImage()
		 */
		void
		drawImage(glcd::Point upperLeft, xpcc::accessor::Flash<uint8_t> image);
		
		/**
		 * Draw an image.
		 * 
		 * \p data is the actual image data without any size information.
		 */
		virtual void
		drawImageRaw(glcd::Point upperLeft,
				uint16_t width, uint16_t height,
				xpcc::accessor::Flash<uint8_t> data);
		
		/**
		 * Fill a rectangle.
		 */
		void
		fillRectangle(glcd::Point upperLeft, uint16_t width, uint16_t height);
		
		inline void
		fillRectangle(int16_t x, int16_t y, uint16_t width, uint16_t height)
		{
			fillRectangle(glcd::Point(x, y), width, height);
		}
		
		/**
		 * Fill a circle.
		 */
		void
		fillCircle(glcd::Point center, uint16_t radius);
		
	public:
		/**
		 * Set a new font.
		 * 
		 * Default font is xpcc::font::FixedWidth5x8.
		 * 
		 * \param	newFont	Active font
		 * \see		xpcc::font
		 */
		inline void
		setFont(const uint8_t *newFont)
		{
			this->font = xpcc::accessor::asFlash(newFont);
		}
		
		/**
		 * Get the height of a character.
		 */
		uint8_t
		getFontHeight() const;
		
		/**
		 * Set the cursor for text drawing.
		 * 
		 * \param	position	Cursor position
		 */
		inline void
		setCursor(glcd::Point position)
		{
			this->cursor = position;
		}
		
		/**
		 * Set the cursor for text drawing.
		 * 
		 * \param	x	Cursor x-position
		 * \param	y	Cursor y-position
		 */
		inline void
		setCursor(int16_t x, int16_t y)
		{
			this->cursor = glcd::Point(x, y);
		}
		
		inline void
		setCursorX(int16_t x)
		{
			this->cursor.x = x;
		}
		
		inline void
		setCursorY(int16_t y)
		{
			this->cursor.y = y;
		}
		
		inline glcd::Point
		getCursor() const
		{
			return this->cursor;
		}
		
		/**
		 * Write a single character.
		 */
		void
		write(char c);
		
	protected:
		/// helper method for drawCircle() and drawEllipse()
		void
		drawCircle4(glcd::Point center, int16_t x, int16_t y);
		
		virtual void
		drawHorizontalLine(glcd::Point start, uint16_t length);
		
		virtual void
		drawVerticalLine(glcd::Point start, uint16_t length);
		
		virtual void
		setPixel(int16_t x, int16_t y) = 0;
		
		virtual void
		clearPixel(int16_t x, int16_t y) = 0;
		
		virtual bool
		getPixel(int16_t x, int16_t y) = 0;
		
	protected:
		// Interface class for the IOStream
		class Writer : public IODevice
		{
		public:
			Writer(GraphicDisplay *parent) :
				parent(parent)
			{
			}
			
			/// Draw a single character
			virtual void
			write(char c);
			
			using IODevice::write;
			
			// unused
			virtual void
			flush();
			
			// unused, returns always `false`
			virtual bool
			read(char& c);
			
		private:
			GraphicDisplay *parent;
		};
		
	protected:
		Writer writer;
		
		// callback function for drawing pixels
		void (GraphicDisplay::*draw)(int16_t x, int16_t y);
		
		glcd::Color foregroundColor;
		glcd::Color backgroundColor;
		xpcc::accessor::Flash<uint8_t> font;
		glcd::Point cursor;
	};
}

#endif	// XPCC__GRAPHIC_DISPLAY_HPP
