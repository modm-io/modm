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
#include <modm/math/utils/bit_constants.hpp>

#include <modm/ui/color.hpp>
#include "font.hpp"

namespace modm
{

/// @ingroup	modm_ui_display
namespace display
{

/// @ingroup modm_ui_display
class Point : public Vector<int16_t, 2>
{
public:
	Point() = default;

	// Allow arbitray arithmetic Types for Point-construction
	// This suspresses "narrowing conversion" compiler warnings
	template<typename T, typename U>
	requires std::is_arithmetic_v<T> and std::is_arithmetic_v<U>
	Point(T x, U y) : Vector<int16_t, 2>(x, y){};

	template<typename T>
	requires std::is_arithmetic_v<T>
	Point(Vector<T, 2> vector) : Vector<int16_t, 2>(vector){};
};

/**
 * Helper class for 2d-drawing functions and buffer-conversion
 */
class Intersection
{
public:
	Point pos, topLeft, bottomRight;

	Intersection() = default;

	Intersection(Point pos, int16_t width_source, int16_t height_source, int16_t width_target, int16_t height_target)
		: pos(pos),
		topLeft({
			std::clamp<int16_t>(pos.x, 0, width_target),
			std::clamp<int16_t>(pos.y, 0, height_target)
		}),
		bottomRight({
			std::clamp<int16_t>(pos.x + width_source, 0, width_target),
			std::clamp<int16_t>(pos.y + height_source, 0, height_target)
		})
	{}

	// Optional getters

	Point
	getSourceTopLeft() const
	{ return {pos.x < 0 ? -pos.x : 0, pos.y < 0 ? -pos.y : 0}; }

	uint32_t getPixelCount() const
	{
		const Point diagonal = bottomRight - topLeft;
		return diagonal.x * diagonal.y;
	}

	bool noIntersection() const
	{
		return topLeft == bottomRight;
	}
};

/// @ingroup modm_ui_display
enum OrientationFlags : uint8_t
{
	Portrait = Bit0,
	Rotate180 = Bit1
};

/// @ingroup modm_ui_display
enum Orientation : uint8_t
{
	Landscape0 = 0,
	Portrait90 = Portrait,
	Landscape180 = Rotate180,
	Portrait270 = Portrait | Rotate180
};
}  // namespace display

using namespace display;

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

	/**
	 * Set a pixel to color
	 *
	 * \param p		point
	 */
	virtual void
	setPixel(Point p) = 0;

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
	// setClippingWindow(Point start, Point end);

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
	void
	drawLine(Point start, Point end);

	/**
	 * Draw a rectangle.
	 *
	 * \param start 	Upper left corner
	 * \param width		Width of rectangle
	 * \param height	Height of rectangle
	 */
	void
	drawRectangle(Point start, uint16_t width, uint16_t height);

	/**
	 * Draw a filled rectangle.
	 *
	 * \param start 	Upper left corner
	 * \param width		Width of rectangle
	 * \param height	Height of rectangle
	 */
	void
	fillRectangle(Point start, uint16_t width, uint16_t height);

	/**
	 * Draw a rectangle with rounded corners
	 *
	 * \param start 	Upper left corner
	 * \param width		Width of rectangle
	 * \param height	Height of rectangle
	 * \param radius	Rounding radius
	 */
	void
	drawRoundedRectangle(Point start, uint16_t width, uint16_t height, uint16_t radius);

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
	// fillRoundedRectangle(Point start, uint16_t width, uint16_t height, uint16_t radius);

	/**
	 * Draw a circle
	 *
	 * Uses the midpoint circle algorithm.
	 *
	 * \param center	Center of the circle
	 * \param radius	Radius of the circle
	 */
	void
	drawCircle(Point center, uint16_t radius);

	/**
	 * Draw a filled circle.
	 *
	 * \param center	Center of the circle
	 * \param radius	Radius of the circle
	 */
	virtual void
	fillCircle(Point center, uint16_t radius);

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
	drawEllipse(Point center, int16_t rx, int16_t ry);

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
	drawImage(Point start, modm::accessor::Flash<uint8_t> image);

	/**
	 * Draw an image.
	 *
	 * \param start		Upper left corner
	 * \param width		Image width
	 * \param height	Image height
	 * \param data		Image data in Flash without any size information.
	 */
	virtual void
	drawImageRaw(Point start, uint16_t width, uint16_t height,
				 modm::accessor::Flash<uint8_t> data);

	/**
	 * Set the cursor for text drawing.
	 *
	 * \param position	Cursor position
	 */
	inline void
	setCursor(Point position)
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
		this->cursor = Point(x, y);
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

	inline Point
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
	drawCircle4(Point center, int16_t x, int16_t y);

	virtual void
	drawHorizontalLine(Point start, uint16_t length);

	virtual void
	drawVerticalLine(Point start, uint16_t length);

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

	Point cursor;

	Orientation orientation = Orientation::Landscape0;
};
}  // namespace modm

#endif  // MODM_GRAPHIC_DISPLAY_HPP
