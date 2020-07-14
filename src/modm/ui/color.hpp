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

namespace modm
{

/// @ingroup modm_ui_color
namespace color
{

template<typename T>
class HsvT;

template<typename T>
class RgbT;

/// @ingroup modm_ui_color
template<class UnderlyingType = uint8_t>
class RgbT
{
public:
	UnderlyingType red{0};
	UnderlyingType green{0};
	UnderlyingType blue{0};

	constexpr RgbT() = default;

	constexpr RgbT(UnderlyingType red, UnderlyingType green, UnderlyingType blue)
	:	red(red), green(green), blue(blue)
	{
	}

	template<typename IntermediateType = float, unsigned int multiplier = 100, typename ReturnType = UnderlyingType>
	ReturnType getRelative(const UnderlyingType color) const
	{
		return static_cast<ReturnType>(
				(static_cast<IntermediateType>(color) *
				static_cast<IntermediateType>(multiplier)) /
				static_cast<IntermediateType>(red + green + blue));
	}

	template<typename IntermediateType = float, unsigned int multiplier = 100, typename ReturnType = UnderlyingType>
	ReturnType getRelativeRed() const
	{
		return getRelative<IntermediateType, multiplier, ReturnType>(red);
	}

	template<typename IntermediateType = float, unsigned int multiplier = 100, typename ReturnType = UnderlyingType>
	ReturnType getRelativeGreen() const
	{
		return getRelative<IntermediateType, multiplier, ReturnType>(green);
	}

	template<typename IntermediateType = float, unsigned int multiplier = 100, typename ReturnType = UnderlyingType>
	ReturnType getRelativeBlue() const
	{
		return getRelative<IntermediateType, multiplier, ReturnType>(blue);
	}

	template<typename IntermediateType = float, unsigned int multiplier = 100, typename ReturnType = UnderlyingType>
	RgbT<ReturnType> getRelativeColors() const
	{
		return RgbT<ReturnType>(
			getRelativeRed	<IntermediateType, multiplier, ReturnType>(),
			getRelativeGreen<IntermediateType, multiplier, ReturnType>(),
			getRelativeBlue	<IntermediateType, multiplier, ReturnType>()
		);
	}

	template<typename T> void
	toHsv(HsvT<T>* color) const;

	constexpr bool
	operator == (const RgbT<UnderlyingType>& other) const
	{
		return (red == other.red and green == other.green and blue == other.blue);
	}

private:
	template<typename T>
	friend IOStream&
	operator << ( IOStream& os, const RgbT<T>&);
};

/// @ingroup modm_ui_color
typedef RgbT<>	Rgb;

/// @ingroup modm_ui_color
template<class UnderlyingType = uint8_t>
class HsvT
{
public:
	UnderlyingType hue{0};
	UnderlyingType saturation{0};
	UnderlyingType value{0};

	constexpr HsvT() = default;

	constexpr HsvT(UnderlyingType hue, UnderlyingType saturation, UnderlyingType value)
	:	hue(hue), saturation(saturation), value(value)
	{
	}

	template<typename T=UnderlyingType>
	void
	toRgb(RgbT<T>* color) const;
};

/// @ingroup modm_ui_color
typedef HsvT<>	Hsv;

template <typename UnderlyingType>
IOStream& operator << ( IOStream& os, const color::RgbT<UnderlyingType>& color);

} // namespace color

} // namespace modm

#include "color_impl.hpp"

#endif // MODM_COLOR_HPP
