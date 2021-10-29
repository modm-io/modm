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

#include <stdint.h>
#include <bit>

#include <concepts>
#include <modm/io/iostream.hpp>
#include <modm/math/utils/arithmetic_traits.hpp>
#include <type_traits>

#include "concepts.hpp"
#include "gray.hpp"

namespace modm::color
{
/**
 * @brief 			Color in RGB space. Each channel has it's own Memoryaddress.
 *
 * @tparam DR 		Digits for red channel
 * @tparam DG 		Digits for green channel
 * @tparam DB 		Digits for blue channel
 *
 * @author			Thomas Sommer
 * @ingroup			modm_ui_color
 */
template<int DR, int DG = DR, int DB = DR>
class RgbD
{
public:
	using RedType = GrayD<DR>;
	using GreenType = GrayD<DG>;
	using BlueType = GrayD<DB>;

	// using RgbSumValueType = modm::fits_any_t<RedType::ValueType, GreenType::ValueType, BlueType::ValueType>;

	constexpr RgbD() = default;

	constexpr RgbD(RedType red, GreenType green, BlueType blue)
		: red(red), green(green), blue(blue)
	{}

	template<ColorRgb C>
	constexpr RgbD(const C& rgb_other)
		: red(rgb_other.red), green(rgb_other.green), blue(rgb_other.blue)
	{}

	template<ColorGray C>
	constexpr RgbD(const C &gray)
		: red(gray), green(gray), blue(gray) {}

 	template<ColorRgbStacked C>
 	constexpr RgbD(const C& rgbs)
		: red(rgbs.getRed()), green(rgbs.getGreen()), blue(rgbs.getBlue())
	{}

	template<ColorHsv C>
	constexpr RgbD(const C& hsv);

	void setRed(RedType red) { this->red = red;}
	void setGreen(GreenType green) { this->green = green;}
	void setBlue(BlueType blue) { this->blue = blue;}

	RedType getRed() const { return red; }
	GreenType getGreen() const { return green; }
	BlueType getBlue() const { return blue; }

	// IMPLEMENT missing operators
	// operator+=()
	// operator-=()
	// operator*=()

	constexpr bool
	operator==(const RgbD& other) const = default;

	void invert() {
		red.invert();
		green.invert();
		blue.invert();
	}

private:
	RedType red{0};
	GreenType green{0};
	BlueType blue{0};

	template<int, int, int>
	friend class RgbD;

	template<ColorRgb C>
	friend IOStream&
	operator<<(IOStream&, const C&);
};

template<std::unsigned_integral U>
using RgbT = RgbD<std::numeric_limits<U>::digits>;

/// @ingroup modm_ui_color
using Rgb888 = RgbD<8>; // Alternative using Rgb888 = RgbT<uint8_t>
using Rgb161616 = RgbD<16>; // Alternative using Rgb161616 = RgbT<uint16_t>

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
template<ColorRgb C, typename IntermediateType = float, ColorRgb ReturnColor = C>
	requires std::is_fundamental_v<IntermediateType>
constexpr ReturnColor
normalizeColor(C rgb, IntermediateType multiplier = 1)
{
	const IntermediateType sum = IntermediateType(rgb.red) + rgb.green + rgb.blue;
	return {
		IntermediateType(rgb.getRed()) * multiplier / sum,
		IntermediateType(rgb.getGreen()) * multiplier / sum,
		IntermediateType(rgb.getBlue()) * multiplier / sum
	};
}

#if __has_include(<modm/io/iostream.hpp>)
#include <modm/io/iostream.hpp>

template<ColorRgb C>
IOStream&
operator<<(IOStream& os, const C& rgb)
{
	os << rgb.getRed() << "\t" << rgb.getGreen() << "\t" << rgb.getBlue();
	return os;
}
#endif

}  // namespace modm::color

#include "rgb_impl.hpp"