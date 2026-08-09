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
constexpr modm::color::HsvT<T>::HsvT(const modm::color::RgbT<T> &rgb)
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

template<std::unsigned_integral T>
constexpr modm::color::HsvT<T>::operator RgbT<T>() const
	requires std::is_same_v<T, uint8_t>
{
	uint16_t vs = value * saturation;
	uint16_t h6 = 6 * hue;

	T p = ((value << 8) - vs) >> 8;
	T i = h6 >> 8;
	uint16_t f = ((i | 1) << 8) - h6;
	if (i & 1) { f = -f; }
	T u = (((uint32_t)value << 16) - (uint32_t)vs * f) >> 16;

	uint8_t red = 0;
	uint8_t green = 0;
	uint8_t blue = 0;

	switch (i)
	{
		case 0: red = value; green = u; blue = p; break;
		case 1: red = u; green = value; blue = p; break;
		case 2: red = p; green = value; blue = u; break;
		case 3: red = p; green = u; blue = value; break;
		case 4: red = u; green = p; blue = value; break;
		case 5: red = value; green = p; blue = u; break;
	}

	return RgbT<T>(red, green, blue);
}
