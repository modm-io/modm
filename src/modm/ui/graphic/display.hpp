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

#include <type_traits>

#include <modm/ui/color/concepts.hpp>

#include <modm/ui/shape/point.hpp>
#include <modm/ui/shape/section.hpp>

#include "canvas.hpp"
#include "buffer.hpp"
#include "concepts.hpp"

namespace modm::graphic
{

enum OrientationFlags : uint8_t
{
	Portrait = Bit0,
	TopDown = Bit1
};

enum Orientation : uint8_t
{
	Landscape0,
	Portrait90 = Portrait,
	Landscape180 = TopDown,
	Portrait270 = Portrait | TopDown
};

/**
 * Baseclass for Graphic Display
 *
 * @tparam	C			One of color::* or bool
 * @tparam	R			Resolution - R.x: horizontal, R.y: vertical
 * @tparam	CanPortrait	Wether the Display supports Portrait90 / Portrait270 Orientation
 *
 * @author				Thomas Sommer
 * @ingroup				modm_ui_graphic
 */

// TODO Replace <.., bool CanPortrait> with <.., Orientation Supported>
// so constexpr and runtime getWidth() / getHeight() can be enabled more cleverly
// May inherit on modm::Matrix<..> or provide a conversion constructor for modm::Matrix<..>
template <color::Color C, shape::Size R, bool CanPortrait>
class Display : virtual public Canvas<C, R>
{
public:
	Orientation getOrientation() const
	{ return orientation; }

	int16_t
	getWidth() const override
	{ return (orientation & OrientationFlags::Portrait) ? R.y : R.x; }

	int16_t
	getHeight() const override
	{ return (orientation & OrientationFlags::Portrait) ? R.x : R.y; }

	Size
	getSize() const override
	{ return (orientation & OrientationFlags::Portrait) ? R.swapped() : R; }

protected:
	Display() = default;

	Orientation orientation = Orientation::Landscape0;

	// Static variables for Resumable Functions
	shape::Section clipping;
};

// Same like Display<C, R, true> but doesn't overwrite getWidth() and getHeight()
template <color::Color C, shape::Size R>
class Display<C, R, false> : virtual public Canvas<C, R>
{
public:
	C color;

	Orientation getOrientation() const
	{ return orientation; }

protected:
	Display(C color) : color(color) {};

	Orientation orientation = Orientation::Landscape0;

	// Static variables for Resumable Functions
	Section clipping;
};

} // namespace modm