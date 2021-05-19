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

#ifndef MODM_COLOR_HSV_HPP
#error "Don't include this file directly, use 'hsv.hpp' instead!"
#endif

#include <algorithm>

/**
 * @see http://de.wikipedia.org/wiki/HSV-Farbraum#Umrechnung_RGB_in_HSV.2FHSL
 * @param rgb
 */
template<std::unsigned_integral T>
template<std::unsigned_integral U>
constexpr modm::color::HsvT<T>::HsvT(const modm::color::RgbT<U> &rgb)
{
	using CalcType = float;
	const CalcType maxValue = std::numeric_limits<T>::max();
	const CalcType _red = CalcType(rgb.red) / maxValue;
	const CalcType _blue = CalcType(rgb.blue) / maxValue;
	const CalcType _green = CalcType(rgb.green) / maxValue;
	const CalcType _max = std::max(_red, std::max(_green, _blue));
	const CalcType _min = std::min(_red, std::min(_green, _blue));
	const CalcType _diff = _max - _min;

	CalcType hue_temp;

	// CALCULATE HUE
	if (_max == _min)
	{
		// all three color values are the same
		hue_temp = 0;
		value = _max * maxValue;
	} else if (_max == _red)
	{
		hue_temp = 60 * (0 + (_green - _blue) / _diff);
		value = rgb.red;
	} else if (_max == _green)
	{
		hue_temp = 60 * (2 + (_blue - _red) / _diff);
		value = rgb.green;
	} else /*if(_max == _blue)*/
	{
		hue_temp = 60 * (4 + (_red - _green) / _diff);
		value = rgb.blue;
	}

	if (hue_temp < 0)
		hue = (hue_temp + 360) * (maxValue / 360);
	else
		hue = (hue_temp) * (maxValue / 360);

	// CALCULATE SATURATION
	if (_max == 0)
		saturation = 0;
	else
		saturation = _diff / _max * maxValue;
}
