/*
 * Copyright (c) 2021-2023, Thomas Sommer
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
#include <concepts>

#include "concepts.hpp"
#include "gray.hpp"
#include <modm/math/uintn_t.hpp>

#include <modm/math/utils/arithmetic_traits.hpp>

namespace modm::color
{

/**
 * @brief			Color in HSV space. Each channel has a memoryaddress on its own.
 *
 * @tparam DH 		Digits for hue
 * @tparam DS 		Digits for saturation
 * @tparam DV 		Digits for value
 *
 * @author			Thomas Sommer
 * @ingroup			modm_ui_color
 */
template<int DH, int DS = DH, int DV = DH>
requires (DH > 0) && (DS > 0) && (DV > 0)
class Hsv
{
public:
	// Hue should wrap rather than saturate, thus an uintn_t is used
	using HueType = uintn_t<DH>;

	using SaturationType = Gray<DS>;
	using ValueType = Gray<DV>;

	constexpr Hsv() = default;

	// TODO Support for https://en.cppreference.com/w/cpp/utility/initializer_list

	constexpr Hsv(HueType hue, SaturationType saturation, ValueType value)
		: hue_(hue), saturation_(saturation), value_(value)
	{}

	template<ColorHsv C>
	constexpr Hsv(const C &other)
		: hue_(other.hue_), saturation_(other.saturation_), value_(other)
	{}

	template<ColorGray C>
	constexpr Hsv(const C &gray)
		: hue_(0), saturation_(0), value_(gray)
	{}

	template<ColorRgb C>
	constexpr Hsv(const C& rgb)
	{
		// OPTIMIZE No need to calculate sharper than the output
		// Develop CalcType from target types: HueType, SaturationType and ValueType
		using CalcType = float;

		const CalcType maxValue = ValueType::max;

		const CalcType red = CalcType(ValueType(rgb.red())) / maxValue;
		const CalcType green = CalcType(ValueType(rgb.green())) / maxValue;
		const CalcType blue = CalcType(ValueType(rgb.blue())) / maxValue;
		const CalcType max = modm::vmax(red, green, blue);
		const CalcType min = modm::vmin(red, green, blue);
		const CalcType diff = max - min;

		CalcType hue_temp;

		if (max == min) {
			// all three color values are the same
			hue_temp = 0;
			value_ = max * maxValue;
		} else if (max == red) {
			hue_temp = 60 * (0 + (green - blue) / diff);
			value_ = rgb.red();
		} else if (max == green) {
			hue_temp = 60 * (2 + (blue - red) / diff);
			value_ = rgb.green();
		} else {
			// max == blue
			hue_temp = 60 * (4 + (red - green) / diff);
			value_ = rgb.blue();
		}

		hue_ = hue_temp < 0 ? (hue_temp + 360) * (maxValue / 360) : (hue_temp) * (maxValue / 360);
		saturation_ = max ? diff / max * maxValue : 0;
	}

	template<ColorRgbPallete C>
 	constexpr Hsv(const C& rgbstacked)
	 	: Hsv(Rgb<C::RedType::digits, C::GreenType::digits, C::BlueType::digits>(rgbstacked))
	{}

	// accesors
	constexpr HueType hue() const { return hue_; }
	constexpr SaturationType saturation() const { return saturation_; }
	constexpr ValueType value() const { return value_; }

	HueType& hue() { return hue_; }
	SaturationType& saturation() { return saturation_; }
	ValueType& value() { return value_; }

	// TODO operator +=, -=, *=, /=
	// @see: https://gamedev.stackexchange.com/questions/26525/how-do-you-blend-multiple-colors-in-hsv-polar-color-space

	// TODO operator +, -, *, /

	// comparison operators
	constexpr bool
	operator==(const Hsv& other) const = default;

	// Compare perceived brightness. For performance and simplicity reasons,
	// the intermediate brightness type is hardcoded to Gray8.
	constexpr auto
	operator<=>(const Gray8& gray) const noexcept {
		return value_ <=> gray;
	};

	constexpr void invert()
	{ hue_.invert(); }

private:
	HueType hue_{0};
	SaturationType saturation_{0};
	ValueType value_{0};

	template<int EH, int ES, int EV>
	requires (EH > 0) && (ES > 0) && (EV > 0)
	friend class Hsv;
};

template<std::unsigned_integral T>
using HsvT = Hsv<std::numeric_limits<T>::digits>;

/// @ingroup modm_ui_color
using Hsv888 = HsvT<uint8_t>;
using Hsv161616 = HsvT<uint16_t>;

#if __has_include(<modm/io/iostream.hpp>)
#include <modm/io/iostream.hpp>

/// @ingroup modm_ui_color
template<ColorHsv C>
IOStream&
operator<<(IOStream& os, const C& hsv)
{
	os << hsv.hue() << "\t" << hsv.saturation() << "\t" << hsv.value();
	return os;
}

// Human friendly output:
// Hue in deg, Sat in pct, Value in pct
/// @ingroup modm_ui_color
template<ColorHsv C>
void ostream_human_friendly(IOStream& os, const C& hsv) {
	using CalcTypeHue = modm::WideType<typename C::HueType::T>;
	using CalcTypeSaturation = modm::WideType<typename C::SaturationType::T>;
	using CalcTypeValue = modm::WideType<typename C::ValueType::T>;

	os << (CalcTypeHue(hsv.hue()) * 360 / hsv.hue().max) << "deg\t";
	os << (CalcTypeSaturation(hsv.saturation()) * 100 / hsv.saturation().max) << "%\t";
	os << (CalcTypeValue(hsv.value()) * 100 / hsv.value().max) << "%";
}
#endif

}  // namespace modm::color
