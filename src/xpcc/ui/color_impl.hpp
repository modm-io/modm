/*
 * Copyright (c) 2013-2015, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_COLOR_HPP
#	error	"Don't include this file directly, use 'color.hpp' instead!"
#endif

#include <modm/math/utils.hpp>

/**
 * @see http://de.wikipedia.org/wiki/HSV-Farbraum#Umrechnung_RGB_in_HSV.2FHSL
 * @param color
 */
template<typename UnderlyingType> template<typename T>
inline void
modm::color::RgbT<UnderlyingType>::toHsv(HsvT<T>* color) const
{
	typedef float CalcType;
	const CalcType maxValue = modm::ArithmeticTraits<T>::max;
	const CalcType _red		= static_cast<CalcType>(red) / maxValue;
	const CalcType _blue	= static_cast<CalcType>(blue) / maxValue;
	const CalcType _green	= static_cast<CalcType>(green) / maxValue;
	const CalcType _max = max(_red, max(_green, _blue));
	const CalcType _min = min(_red, min(_green, _blue));
	const CalcType _diff = _max - _min;

	CalcType hue_temp;

	// CALCULATE HUE
	if(_max == _min) { // all three color values are the same
		hue_temp = 0;
		color->value = _max * maxValue;
	}
	else if(_max == _red) {
		hue_temp = 60 * (0 + (_green - _blue)	/ _diff );
		color->value = red;
	}
	else if(_max == _green) {
		hue_temp = 60 * (2 + (_blue - _red)		/ _diff );
		color->value = green;
	}
	else /*if(_max == _blue)*/ {
		hue_temp = 60 * (4 + (_red - _green)	/ _diff );
		color->value = blue;
	}

	if(hue_temp < 0) {
		color->hue = (hue_temp + 360	) * (maxValue / 360);
	}
	else {
		color->hue = (hue_temp			) * (maxValue / 360);
	}

	// CALCULATE SATURATION
	if(_max == 0) {
		color->saturation = 0;
	} else {
		color->saturation = _diff / _max * maxValue;
	}
}

template <typename UnderlyingType>
modm::IOStream&
modm::color::operator << ( modm::IOStream& os, const modm::color::RgbT<UnderlyingType>& color)
{
	os << color.red << "\t" << color.green << "\t" << color.blue;
	return os;
}
