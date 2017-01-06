/*
 * Copyright (c) 2009, Martin Rosekeit
 * Copyright (c) 2009-2013, Fabian Greif
 * Copyright (c) 2012-2013, 2015, Niklas Hauser
 * Copyright (c) 2013, David Hebbeker
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_COLOR_HPP
#define MODM_COLOR_HPP

#include <stdint.h>
#include <modm/io/iostream.hpp>
#include <modm/utils/arithmetic_traits.hpp>

namespace modm
{

namespace color
{

template<typename T>
class HsvT;

template<typename T>
class RgbT;

template<class UnderlyingType = uint8_t>
class RgbT
{
public:
	UnderlyingType red;
	UnderlyingType green;
	UnderlyingType blue;

	RgbT(UnderlyingType red, UnderlyingType green, UnderlyingType blue)
	:	red(red), green(green), blue(blue)
	{
	}

	RgbT()
	:	red(0), green(0), blue(0)
	{
	}

	template<typename IntermediateType = float, unsigned int multiplier = 100, typename ReturnType = UnderlyingType>
	inline ReturnType getRelative(const UnderlyingType color) const
	{
		return static_cast<ReturnType>(
				(static_cast<IntermediateType>(color) *
				static_cast<IntermediateType>(multiplier)) /
				static_cast<IntermediateType>(red + green + blue));
	}

	template<typename IntermediateType = float, unsigned int multiplier = 100, typename ReturnType = UnderlyingType>
	inline ReturnType getRelativeRed() const
	{
		return getRelative<IntermediateType, multiplier, ReturnType>(red);
	}

	template<typename IntermediateType = float, unsigned int multiplier = 100, typename ReturnType = UnderlyingType>
	inline ReturnType getRelativeGreen() const
	{
		return getRelative<IntermediateType, multiplier, ReturnType>(green);
	}

	template<typename IntermediateType = float, unsigned int multiplier = 100, typename ReturnType = UnderlyingType>
	inline ReturnType getRelativeBlue() const
	{
		return getRelative<IntermediateType, multiplier, ReturnType>(blue);
	}

	template<typename IntermediateType = float, unsigned int multiplier = 100, typename ReturnType = UnderlyingType>
	inline RgbT<ReturnType> getRelativeColors() const
	{
		return RgbT<ReturnType>(
			getRelativeRed	<IntermediateType, multiplier, ReturnType>(),
			getRelativeGreen<IntermediateType, multiplier, ReturnType>(),
			getRelativeBlue	<IntermediateType, multiplier, ReturnType>()
		);
	}

	template<typename T> void
	toHsv(HsvT<T>* color) const;

private:
	template<typename T>
	friend IOStream&
	operator << ( IOStream& os, const RgbT<T>&);
};

typedef RgbT<>	Rgb;

template<class UnderlyingType = uint8_t>
class HsvT
{
public:
	UnderlyingType hue;
	UnderlyingType saturation;
	UnderlyingType value;

	HsvT(UnderlyingType hue, UnderlyingType saturation, UnderlyingType value)
	:	hue(hue), saturation(saturation), value(value)
	{
	}

	HsvT()
	:	hue(0), saturation(0), value(0)
	{
	}

	template<typename T=UnderlyingType>
	void
	toRgb(RgbT<T>* color) const;
};

typedef HsvT<>	Hsv;

template <typename UnderlyingType>
IOStream& operator << ( IOStream& os, const color::RgbT<UnderlyingType>& color);

} // namespace color

} // namespace modm

#include "color_impl.hpp"

#endif // MODM_COLOR_HPP
