/*
 * Copyright (c) 2009, Martin Rosekeit
 * Copyright (c) 2009-2013, Fabian Greif
 * Copyright (c) 2012-2013, 2015, Niklas Hauser
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

#ifndef MODM_COLOR_RGB_HPP
#define MODM_COLOR_RGB_HPP

#include <stdint.h>
#include <bit>

#include <concepts>
#include <modm/io/iostream.hpp>
#include <modm/math/utils/arithmetic_traits.hpp>
#include <type_traits>

#include "brightness.hpp"
#include "hsv.hpp"
#include "rgb565.hpp"

namespace modm::color
{

// forward declarations for convertion constructors
template<std::unsigned_integral T>
class HsvT;

template<std::unsigned_integral T>
class BrightnessT;

class Rgb565;

/**
 * Color in HSV Colorspace
 *
 * @author		Martin Rosekeit, Fabian Greif, Niklas Hauser, David Hebbeker, Thomas Sommer
 * @ingroup		modm_ui_color
 */
template<std::unsigned_integral T>
class RgbT
{
public:
	T red{0};
	T green{0};
	T blue{0};

	using TSum = modm::WideType<T>;

	constexpr RgbT() = default;

	constexpr RgbT(T red, T green, T blue) : red(red), green(green), blue(blue) {}

	/**
	 * Copy Constructor 8bit->16bit
	 */
	template<std::unsigned_integral U>
		requires std::is_same_v<T, uint16_t> && std::is_same_v<U, uint8_t>
	constexpr RgbT(const RgbT<U> &rgb_other)
		: red(rgb_other.red << 8), green(rgb_other.green << 8), blue(rgb_other.blue << 8)
	{}

	/**
	 * Copy Constructor 16bit->8bit
	 */
	template<std::unsigned_integral U>
		requires std::is_same_v<T, uint8_t> && std::is_same_v<U, uint16_t>
	constexpr RgbT(const RgbT<U> &rgb_other)
		: red(rgb_other.red >> 8), green(rgb_other.green >> 8), blue(rgb_other.blue >> 8)
	{}

	/**
	 * Convertion Constructor for HSV Color
	 *
	 * @param hsv	HSV Color
	 */
	template<std::unsigned_integral U>
	constexpr RgbT(const HsvT<U>& hsv);

	/**
	 * Convertion Constructor for Brightness
	 *
	 * @param brightness	Brightness 'Color'-object
	 */
	// TODO Plump conversion, implement the right way
	template<std::unsigned_integral U>
	constexpr RgbT(const BrightnessT<U> brightness)
		: red(brightness), green(brightness), blue(brightness)
	{}

	/**
	 * Convertion Constructor for RGB565 Color
	 *
	 * @param rgb565	RGB565 Color
	 */
	constexpr RgbT(const Rgb565& rgb565)
		: red((rgb565.color >> 8) & 0xF8),
		  green((rgb565.color >> 3) & 0xFC),
		  blue(rgb565.color << 3)
	{}

	constexpr bool
	operator==(const RgbT<T>& other) const = default;

private:
	template<std::unsigned_integral U>
	friend IOStream&
	operator<<(IOStream&, const RgbT<U>&);
};

/// @ingroup modm_ui_color
using Rgb = RgbT<uint8_t>;


/**
 * Normalize color values based on a clear value
 *
 * Imagine a low band light, for example a green laser. In case the filters
 * of a sensor do not transfer this wavelength well, it might result in all
 * colors being very low. An aditional clear value (provided f.e. by TCS3472)
 * will not filter colors and thus it will see a bright light (intensity).
 * In order to still have some signal the very low green value can be
 * amplified with the clear value.
 *
 * @ingroup modm_ui_color
 */
template<std::unsigned_integral T, typename IntermediateType = float, std::unsigned_integral ReturnType = T>
	requires std::is_fundamental_v<IntermediateType>
constexpr RgbT<ReturnType>
normalizeColor(RgbT<T> rgb, IntermediateType multiplier = 1)
{
	const IntermediateType sum = IntermediateType(rgb.red) + rgb.green + rgb.blue;
	return RgbT<ReturnType>(IntermediateType(rgb.red) * multiplier / sum,
							IntermediateType(rgb.green) * multiplier / sum,
							IntermediateType(rgb.blue) * multiplier / sum);
}


#if __has_include(<modm/io/iostream.hpp>)
#include <modm/io/iostream.hpp>

/// @ingroup modm_ui_color
template<std::unsigned_integral U>
IOStream&
operator<<(IOStream& os, const color::RgbT<U>& color)
{
	os << color.red << "\t" << color.green << "\t" << color.blue;
	return os;
}
#endif

}  // namespace modm::color

#include "rgb_impl.hpp"

#endif  // MODM_COLOR_RGB_HPP
