/*
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

#include <algorithm>
#include <modm/processing/resumable.hpp>

#include "canvas.hpp"
#include "style.hpp"
#include <modm/ui/shape.hpp>

namespace modm::graphic
{

/**
 * @brief			Painter for external attached memory, e.g. a Display.
 *					Can draw fundamental shapes: shape::Point, Line, Rectangle, Circle, Ellipse
 *					Can write other buffers from RAM and images from FLASH.
 *
 * @tparam	GD		Recipient display for the paintings
 *
 * @author			Thomas Sommer
 * @ingroup			modm_ui_graphic
 *
 * @see				graphic::LocalPainter (same without Resumable Functions)
 */

template<GraphicDisplay GD>
class RemotePainter : public GD
{
public:
	using ColorType = GD::ColorType;
	
	void drawBlocking(shape::Point point)
	{ RF_CALL_BLOCKING(draw(point)); }

	void drawBlocking(shape::Line line)
	{ RF_CALL_BLOCKING(draw(line)); }

	void drawBlocking(shape::Rectangle rectangle, Style style = Style::Outline)
	{ RF_CALL_BLOCKING(draw(rectangle, style)); }

	void drawBlocking(shape::Rectangle rectangle, uint16_t radius, Style style = Style::Outline)
	{ RF_CALL_BLOCKING(draw(rectangle, radius, style)); }

	void drawBlocking(shape::Circle circle, Style style = Style::Outline)
	{ RF_CALL_BLOCKING(draw(circle, style)); }

	void drawBlocking(shape::Ellipse ellipse, Style style = Style::Outline)
	{ RF_CALL_BLOCKING(draw(ellipse, style)); }

	ColorType getBlocking(shape::Point point)
	{ return RF_CALL_BLOCKING(get(point)); }

	// TODO Export these Interfaces
	template<color::Color CO>
	void writeBlocking(BufferInterface<CO> &buffer, shape::Point placement = {0, 0})
	{ RF_CALL_BLOCKING(write(buffer, placement)); }

	template<color::Color CO>
	void writeBlocking(const uint8_t *addr, shape::Point placement = {0, 0})
	{ RF_CALL_BLOCKING(write(addr, placement)); }

	/**
	 * @brief 	Fundamental drawing routines. Should be self-explained.
	 */
	ResumableResult<void>
	draw(shape::Point point);

	ResumableResult<void>
	draw(shape::Line line);

	ResumableResult<void>
	draw(shape::Rectangle rectangle, Style style = Style::Outline);

	ResumableResult<void>
	draw(shape::Rectangle rectangle, uint16_t radius, Style style = Style::Outline);

	ResumableResult<void>
	draw(shape::Circle circle, Style style = Style::Outline);

	modm	::ResumableResult<void>
	draw(shape::Ellipse ellipse, Style style = Style::Outline);

	ResumableResult<ColorType>
	get(Point point) const;

	/**
	 * @brief 				Writes a buffer to remote location. If the other buffer is grayscale,
	 * 						the colors will be mapped using the colormap.
	 *
	 * @tparam CO			Colortype of buffer
	 * @param buffer 		Interface to Buffer
	 * @param placement 	Write buffer to this position
	 */
	template<color::Color CO>
	ResumableResult<void>
	write(BufferInterface<CO> &buffer, shape::Point placement = {0, 0});

	/**
	 * @brief 				Writes an image to remote location. If the image is grayscale,
	 * 						the colors will be mapped using the colormap.
	 *
	 * @tparam CO			Colortype of image
	 * @param buffer 		Address of the image in flash
	 * @param placement 	Write image to this position
	 */
	template<color::Color CO>
	ResumableResult<void>
	write(const uint8_t *addr, shape::Point placement = {0, 0});
private:
	ResumableResult<void>
	drawQuadPoints(shape::Point center, shape::Point point);

	// Static variables for resumable functions
	shape::Section section;

	union {
		struct {
			Line line;
			Point delta;
			bool steep;
			int16_t step, error;
		} l; // Line bresenhem

		struct {
			Point bottomRight;
		} r;
		struct {
			shape::Point point;
			int16_t error;
		} co; // Circle Outline

		struct {
			int16_t f, ddF_x, ddF_y, x, y;
			shape::Point start;
		} cf; // Circle Filled
	};

	// Quad Points
	Point qpoint;
	bool drawXpos, drawXneg;
};

} // namespace modm::graphic

#include "painter_remote_impl.hpp"