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

#include <modm/ui/shape/point.hpp>
#include <modm/ui/shape/section.hpp>

namespace modm::graphic
{
/**
 * Baseclass for 2D Graphic Objects like Display and BufferMemory
 *
 * @tparam	R	Resolution - R.x: horizontal, R.y: vertical
 *
 * @author		Thomas Sommer
 * @ingroup		modm_ui_graphic
 */
template <color::Color C, shape::Size R>
class Canvas
{
public:
	static constexpr shape::Size ResolutionVector = R;

	constexpr uint32_t
	getPixels() const { return R.x * R.y; }

	virtual constexpr int16_t
	getWidth() const { return R.x; }

	virtual constexpr int16_t
	getHeight() const { return R.y; }

	virtual constexpr shape::Size
	getSize() const { return R; }

	constexpr shape::Section
	asSection() const
	{ return {{0, 0}, getSize()}; }

 	constexpr shape::Section
	getIntersection(shape::Section section) {
		return {{
			std::clamp<int16_t>(section.topLeft.x, 0, getWidth()),
			std::clamp<int16_t>(section.topLeft.y, 0, getHeight())
		},
		{
			std::clamp<int16_t>(section.bottomRight.x, 0, getWidth()),
			std::clamp<int16_t>(section.bottomRight.y, 0, getHeight())
		}};
	}

	void
	setColormap(C* colormap)
	{ this->colormap = colormap; }

	// Set drawing color form colormap
	void
	setColor(const std::size_t color_idx)
	{ this->color = colormap[color_idx]; }

	// Set drawing color
	void
	setColor(const C color)
	{ this->color = color; }

protected:
	Canvas(C* colormap = nullptr) : colormap(colormap) {}

	C* colormap;
	C color;
	
	static constexpr shape::Point
	getSourceOrigin(shape::Point placement) {
		return {
			placement.x < 0 ? -placement.x : 0,
			placement.y < 0 ? -placement.y : 0
		};
	}

	constexpr bool
	xInCanvas(int16_t x) const
	{ return x >= 0 and x < int16_t(getWidth()); }

	constexpr bool
	yInCanvas(int16_t y) const
	{ return y >= 0 and y < int16_t(getHeight()); }

	constexpr bool
	pointInCanvas(shape::Point position) const
	{ return xInCanvas(position.x) and yInCanvas(position.y); }
};
}  // namespace modm::graphic