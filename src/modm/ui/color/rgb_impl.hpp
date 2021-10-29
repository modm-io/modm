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
#include "rgb.hpp"

template<int DR, int DG, int DB>
template<modm::color::ColorHsv C>
constexpr modm::color::RgbD<DR, DG, DB>::RgbD(const C& hsv)
{
	// OPTIMIZE No need to calculate sharper than the output
	// Develop CalcType from types of conversion target: RedType, GreenType and BlueType.
	using CalcType = C::ValueType;
	using ValueType = CalcType::ValueType;

	using WideType = modm::WideType<ValueType>;
	using WideWideType = modm::WideType<WideType>;
	static_assert(!std::is_same_v<WideType, WideWideType>, "C::ValueType too big");

	const ValueType hue = CalcType(hsv.getHue()).getValue();
	const ValueType saturation = CalcType(hsv.getSaturation()).getValue();
	const ValueType value = CalcType(hsv.getValue()).getValue();

	const WideType vs = value * saturation;
	const WideType h6 = 6 * hue;

	ValueType i = h6 >> CalcType::Digits;
	WideType f = ((i | 1) << CalcType::Digits) - h6;
	if (i & 1) f = -f;

	CalcType p(((value << CalcType::Digits) - vs) >> CalcType::Digits);
	CalcType u(((WideWideType(value) << 2 * CalcType::Digits) - WideWideType(vs) * f) >> 2 * CalcType::Digits);

	switch (i)
	{
		case 0: red = hsv.getValue(); green = u; blue = p; break;
		case 1: red = u; green = hsv.getValue(); blue = p; break;
		case 2: red = p; green = hsv.getValue(); blue = u; break;
		case 3: red = p; green = u; blue = hsv.getValue(); break;
		case 4: red = u; green = p; blue = hsv.getValue(); break;
		case 5: red = hsv.getValue(); green = p; blue = u; break;
	}
}