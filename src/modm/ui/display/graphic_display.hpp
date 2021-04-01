/*
 * Copyright (c) 2009-2010, Martin Rosekeit
 * Copyright (c) 2009-2011, 2013, Fabian Greif
 * Copyright (c) 2011, 2013, Thorsten Lajewski
 * Copyright (c) 2012-2014, 2016, Niklas Hauser
 * Copyright (c) 2013, Hans Schily
 * Copyright (c) 2014, Daniel Krebs
 * Copyright (c) 2015, Niclas Rohrer
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_GRAPHIC_DISPLAY_HPP
#define MODM_GRAPHIC_DISPLAY_HPP

#include <modm/architecture/interface/accessor.hpp>
#include <modm/math/geometry.hpp>

#include <modm/io/iodevice.hpp>
#include <modm/io/iostream.hpp>

#include "font.hpp"

namespace modm
{
	/// @ingroup	modm_ui_display
	namespace glcd
	{
		/// @ingroup modm_ui_display
		typedef Vector<int16_t, 2> Point;
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
	 * \ingroup	modm_ui_display
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
		drawImage(glcd::Point upperLeft, modm::accessor::Flash<uint8_t> image);

		/**
		 * Draw an image.
		 *
		 * \p data is the actual image data without any size information.
		 */
		virtual void
		drawImageRaw(glcd::Point upperLeft,
				uint16_t width, uint16_t height,
				modm::accessor::Flash<uint8_t> data);

		/**
		 * Fill a rectangle.
		 */
		virtual void
		fillRectangle(glcd::Point upperLeft, uint16_t width, uint16_t height);

		inline void
		fillRectangle(int16_t x, int16_t y, uint16_t width, uint16_t height)
		{
			fillRectangle(glcd::Point(x, y), width, height);
		}

		/**
		 * Fill a circle.
		 */
		virtual void
		fillCircle(glcd::Point center, uint16_t radius);

	public:
		/**
		 * Set a new font.
		 *
		 * Default font is modm::font::FixedWidth5x8.
		 *
		 * \param	newFont	Active font
		 * \see		modm::font
		 */
		inline void
		setFont(const uint8_t *newFont)
		{
			this->font = modm::accessor::asFlash(newFont);
		}

		inline void
		setFont(const modm::accessor::Flash<uint8_t> *font)
		{
			this->font = *font;
		}

		/**
		 * Get the height of a character.
		 */
		uint8_t
		getFontHeight() const;

		static uint8_t
		getFontHeight(const modm::accessor::Flash<uint8_t> *font);

		/**
		* Get the width of (null terminated) string.
		*/
		uint16_t
		getStringWidth(const char* s) const;

		static uint16_t
		getStringWidth(const char* s, const modm::accessor::Flash<uint8_t> *font);

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

		modm::accessor::Flash<uint8_t> font;
		glcd::Point cursor;
	};
}

#endif	// MODM_GRAPHIC_DISPLAY_HPP
