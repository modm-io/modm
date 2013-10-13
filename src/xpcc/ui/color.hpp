// coding: utf-8
/* Copyright (c) 2012, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_COLOR_HPP
#define XPCC_COLOR_HPP

#include <stdint.h>
#include <xpcc/io/iostream.hpp>
#include <xpcc/utils/arithmetic_traits.hpp>
#include <algorithm>

namespace xpcc
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

} // namespace xpcc

#include "color_impl.hpp"

#endif // XPCC_COLOR_HPP
