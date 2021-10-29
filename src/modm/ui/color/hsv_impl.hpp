/*
 * Copyright (c) 2009-2010, 2012, Fabian Greif
 * Copyright (c) 2010, Martin Rosekeit
 * Copyright (c) 2012-2013, Niklas Hauser
 * Copyright (c) 2013, David Hebbeker
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
#include "hsv.hpp"

#include <algorithm>

/**
 * @see https://en.wikipedia.org/wiki/HSL_and_HSV
 */
template<int DH, int DS, int DV>
template<modm::color::ColorRgb C>
constexpr modm::color::HsvD<DH, DS, DV>::HsvD(const C& rgb)
{
	// OPTIMIZE No need to calculate sharper than the output
	// Develop CalcType from target types: HueType, SaturationType and ValueType
	using CalcType = float;

	const CalcType maxValue = ValueType::max;

	const CalcType red = CalcType(ValueType(rgb.getRed()).getValue()) / maxValue;
	const CalcType green = CalcType(ValueType(rgb.getGreen()).getValue()) / maxValue;
	const CalcType blue = CalcType(ValueType(rgb.getBlue()).getValue()) / maxValue;
	const CalcType max = modm::vmax(red, green, blue);
	const CalcType min = modm::vmin(red, green, blue);
	const CalcType diff = max - min;

	CalcType hue_temp;

 	if (max == min)
	{
		// all three color values are the same
		hue_temp = 0;
		value = max * maxValue;
	} else if (max == red)
	{
		hue_temp = 60 * (0 + (green - blue) / diff);
		value = rgb.getRed();
	} else if (max == green)
	{
		hue_temp = 60 * (2 + (blue - red) / diff);
		value = rgb.getGreen();
	} else // max == blue
	{
		hue_temp = 60 * (4 + (red - green) / diff);
		value = rgb.getBlue();
	}

	hue = hue_temp < 0 ? (hue_temp + 360) * (maxValue / 360) : (hue_temp) * (maxValue / 360);
	saturation = max ? diff / max * maxValue : 0;
}