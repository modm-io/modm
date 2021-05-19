/*
 * Copyright (c) 2009-2010, Martin Rosekeit
 * Copyright (c) 2009-2011, 2013, Fabian Greif
 * Copyright (c) 2011, 2013, Thorsten Lajewski
 * Copyright (c) 2012-2014, 2016, Niklas Hauser
 * Copyright (c) 2013, Hans Schily
 * Copyright (c) 2014, Daniel Krebs
 * Copyright (c) 2015, Niclas Rohrer
 * Copyright (c) 2021, Thomas Sommer
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
#include <modm/io/iodevice.hpp>
#include <modm/io/iostream.hpp>
#include <modm/math/geometry.hpp>

#include <modm/ui/color.hpp>
#include "font.hpp"

namespace modm
{

/// @ingroup	modm_ui_display
namespace glcd
{

/// @ingroup modm_ui_display
using Point = Vector<int16_t, 2>;

enum Orientation : uint8_t
{
	Landscape0,
	Portrait90,
	Landscape180,
	Portrait270,
};

}  // namespace glcd

/**
 * Base class for graphical displays.
 *
 * \ingroup	modm_ui_display
 */

/*
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

	virtual ~GraphicDisplay() {}

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

	// TODO Requires all inherited drivers work with resumable functions
	// virtual modm::ResumableResult<bool>
	// setOrientation() = 0;

	/**
	 * Buffer-array size of first dimension
	 */
	virtual std::size_t
	getBufferWidth() const = 0;

	/**
	 * Buffer-array size of second dimension
	 */
	virtual std::size_t
	getBufferHeight() const = 0;

	/**
	 * Set a pixel to foregroundColor
	 *
	 * \param x		x-position
	 * \param y		y-position
	 */
	virtual void
	setPixel(int16_t x, int16_t y) = 0;

	/**
	 * Set a pixel to foregroundColor
	 *
	 * \param p		point
	 */
	inline void
	setPixel(glcd::Point p)
	{
		this->setPixel(p.x, p.y);
	}

	/**
	 * Set a pixel to backgroundColor
	 *
	 * \param x		x-position
	 * \param y		y-position
	 */
	virtual void
	clearPixel(int16_t x, int16_t y) = 0;

	/**
	 * Set a pixel to backgroundColor
	 *
	 * \param p		point
	 */
	inline void
	clearPixel(glcd::Point p)
	{
		this->setPixel(p.x, p.y);
	}

	/**
	 * Set whole screen to backgroundColor
	 */
	virtual void
	clear() = 0;

	/**
	 * Transfer the content of the RAM buffer to the real display.
	 */
	virtual void
	update() = 0;

	// TODO Requires all inherited drivers work with resumable functions
	// modm::ResumableResult<bool>
	// writeDisplay();

	// TODO Set a clipping area
	// Everything drawn outside this area will be discarded.
	// inline void
	// setClippingWindow(glcd::Point start, glcd::Point end);

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
		this->drawLine(start.x, start.y, end.x, end.y);
	}

	/**
	 * Draw a line
	 *
	 * \param x1	Start x-position
	 * \param y1	Start y-position
	 * \param x2	End x-position
	 * \param y3	End y-position
	 */
	void
	drawLine(int16_t x1, int16_t y1, int16_t x2, int16_t y2);

	/**
	 * Draw a rectangle.
	 *
	 * \param start 	Upper left corner
	 * \param width		Width of rectangle
	 * \param height	Height of rectangle
	 */
	void
	drawRectangle(glcd::Point start, uint16_t width, uint16_t height);

	/**
	 * Draw a rectangle.
	 *
	 * \param x 		Upper left corner x-position
	 * \param y 		Upper left corner y-position
	 * \param width		Width of rectangle
	 * \param height	Height of rectangle
	 */
	inline void
	drawRectangle(int16_t x, int16_t y, uint16_t width, uint16_t height)
	{
		drawRectangle(glcd::Point(x, y), width, height);
	}

	/**
	 * Draw a filled rectangle.
	 *
	 * \param start 	Upper left corner
	 * \param width		Width of rectangle
	 * \param height	Height of rectangle
	 */
	void
	fillRectangle(glcd::Point start, uint16_t width, uint16_t height);

	/**
	 * Draw a rectangle.
	 *
	 * \param x 		Upper left corner x-position
	 * \param y 		Upper left corner y-position
	 * \param width		Width of rectangle
	 * \param height	Height of rectangle
	 */
	inline void
	fillRectangle(int16_t x, int16_t y, uint16_t width, uint16_t height)
	{
		fillRectangle(glcd::Point(x, y), width, height);
	}

	/**
	 * Draw a rectangle with rounded corners
	 *
	 * \param start 	Upper left corner
	 * \param width		Width of rectangle
	 * \param height	Height of rectangle
	 * \param radius	Rounding radius
	 */
	void
	drawRoundedRectangle(glcd::Point start, uint16_t width, uint16_t height, uint16_t radius);

	/**
	 * Draw a filled rectangle with rounded corners
	 *
	 * \param start 	Upper left corner
	 * \param width		Width of rectangle
	 * \param height	Height of rectangle
	 * \param radius	Rounding radius
	 */
	// TODO Not yet implemented
	// void
	// fillRoundedRectangle(glcd::Point start, uint16_t width, uint16_t height, uint16_t radius);

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
	 * Draw a filled circle.
	 *
	 * \param center	Center of the circle
	 * \param radius	Radius of the circle
	 */
	virtual void
	fillCircle(glcd::Point center, uint16_t radius);

	/**
	 * Draw an ellipse.
	 *
	 * Uses a variation of the midpoint algorithm. May be improved through
	 * simplification of the used formula.
	 *
	 * \param center	Center of the ellipse
	 * \param rx		Radius in x-direction
	 * \param ry		Radius in y-direction
	 */
	void
	drawEllipse(glcd::Point center, int16_t rx, int16_t ry);

	/**
	 * Draw an image.
	 *
	 * The first byte in the image data specifies the with, the second
	 * byte the height. Afterwards the actual image data.
	 *
	 * \param start		Upper left corner
	 * \param image		Image data in Flash
	 *
	 * \see	drawImage()
	 */
	void
	drawImage(glcd::Point start, modm::accessor::Flash<uint8_t> image);

	/**
	 * Draw an image.
	 *
	 * \param start		Upper left corner
	 * \param width		Image width
	 * \param height	Image height
	 * \param data		Image data in Flash without any size information.
	 */
	virtual void
	drawImageRaw(glcd::Point start, uint16_t width, uint16_t height,
				 modm::accessor::Flash<uint8_t> data);

	/**
	 * Set the cursor for text drawing.
	 *
	 * \param position	Cursor position
	 */
	inline void
	setCursor(glcd::Point position)
	{
		this->cursor = position;
	}

	/**
	 * Set the cursor for text drawing.
	 *
	 * \param x		Cursor x-position
	 * \param y		Cursor y-position
	 */
	inline void
	setCursor(int16_t x, int16_t y)
	{
		this->cursor = glcd::Point(x, y);
	}

	/**
	 * Set the cursor x-position for text drawing.
	 *
	 * \param x		Cursor x-position
	 */
	inline void
	setCursorX(int16_t x)
	{
		this->cursor.x = x;
	}

	/**
	 * Set the cursor y-position for text drawing.
	 *
	 * \param y		Cursor y-position
	 */
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
	getStringWidth(const char *s) const;

	static uint16_t
	getStringWidth(const char *s, const modm::accessor::Flash<uint8_t> *font);

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

protected:
	// Interface class for the IOStream
	class Writer : public IODevice
	{
	public:
		Writer(GraphicDisplay *parent) : parent(parent) {}

		/// Draw a single character
		virtual void
		write(char c);

		using IODevice::write;

		// unused
		virtual void
		flush();

		// unused, returns always `false`
		virtual bool
		read(char &c);

	private:
		GraphicDisplay *parent;
	};

protected:
	Writer writer;
	modm::accessor::Flash<uint8_t> font;
	glcd::Point cursor;
};
}  // namespace modm

#endif  // MODM_GRAPHIC_DISPLAY_HPP
