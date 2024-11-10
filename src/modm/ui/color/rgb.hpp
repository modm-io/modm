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
 * @brief 			Color in RGB space. Each channel has a memoryaddress on its own.
 *
 * @tparam DR 		Digits for red channel
 * @tparam DG 		Digits for green channel
 * @tparam DB 		Digits for blue channel
 *
 * @author			Thomas Sommer
 * @ingroup			modm_ui_color
 */
template<int DR, int DG = DR, int DB = DR>
requires (DR > 0) && (DG > 0) && (DB > 0)
class Rgb
{
public:
	using RedType = Gray<DR>;
	using GreenType = Gray<DG>;
	using BlueType = Gray<DB>;

	// using RgbSumValueType = modm::fits_any<RedType::T, GreenType::T, BlueType::T>;

	constexpr Rgb() = default;

	// TODO Support for https://en.cppreference.com/w/cpp/utility/initializer_list

	constexpr Rgb(RedType red, GreenType green, BlueType blue)
		: red_(red), green_(green), blue_(blue)
	{}

	template<class C>
	requires ColorRgb<C> || ColorRgbPallete<C>
	constexpr Rgb(const C& other)
		: red_(other.red()), green_(other.green()), blue_(other.blue())
	{}

	template<ColorGray C>
	constexpr Rgb(const C &gray)
		: red_(gray), green_(gray), blue_(gray)
	{}

	template<ColorHsv C>
	constexpr Rgb(const C& hsv)
	{
		// No need to calculate sharper than the output
		// OPTIMIZE Develop CalcType from types of conversion target: RedType, GreenType and BlueType.
		using CalcType = C::ValueType;
		using T = CalcType::T;

		using WideType = modm::WideType<T>;
		using WideWideType = modm::WideType<WideType>;
		static_assert(!std::is_same_v<WideType, WideWideType>, "C::T too big");

		const T hue = CalcType(Gray<std::numeric_limits<typename C::HueType>::digits>(hsv.hue()));
		const T saturation = CalcType(hsv.saturation());
		const T value = CalcType(hsv.value());

		const WideType vs = value * saturation;
		const WideType h6 = 6 * hue;

		T i = h6 >> CalcType::digits;
		WideType f = ((i | 1) << CalcType::digits) - h6;
		if (i & 1) f = -f;

		CalcType p(((value << CalcType::digits) - vs) >> CalcType::digits);
		CalcType u(((WideWideType(value) << 2 * CalcType::digits) - WideWideType(vs) * f) >> 2 * CalcType::digits);

		switch (i)
		{
			case 0: red_ = hsv.value(); green_ = u; blue_ = p; break;
			case 1: red_ = u; green_ = hsv.value(); blue_ = p; break;
			case 2: red_ = p; green_ = hsv.value(); blue_ = u; break;
			case 3: red_ = p; green_ = u; blue_ = hsv.value(); break;
			case 4: red_ = u; green_ = p; blue_ = hsv.value(); break;
			case 5: red_ = hsv.value(); green_ = p; blue_ = u; break;
		}
	}

	// accessors
	constexpr RedType red() const { return red_; }
	constexpr GreenType green() const { return green_; }
	constexpr BlueType blue() const { return blue_; }

	RedType& red() { return red_; }
	GreenType& green() { return green_; }
	BlueType& blue() { return blue_; }

	// operator +=, -=, *=, /=
	Rgb& operator+=(const Rgb& other) {
		red_ += other.red();
		green_ += other.green();
		blue_ += other.blue();
		return *this;
	}

	Rgb& operator-=(const Rgb& other) {
		red_ -= other.red();
		green_ -= other.green();
		blue_ -= other.blue();
		return *this;
	}

	template <typename S>
	Rgb operator*= (S scale) {
		red_ *= scale;
		green_ *= scale;
		blue_ *= scale;
		return *this;
	}

	template <typename S>
	Rgb operator/= (S scale) {
		red_ *= scale;
		green_ *= scale;
		blue_ *= scale;
		return *this;
	}

	// operator +, -, *, /
	constexpr Rgb
	operator+(const Rgb& rgb) const {
		return {
			red_ + rgb.red(),
			green_ + rgb.green(),
			blue_ + rgb.blue()
		};
	}

	constexpr Rgb
	operator-(const Rgb& rgb) const {
		return {
			red_ - rgb.red(),
			green_ - rgb.green(),
			blue_ - rgb.blue()
		};
	}

	template <typename S>
	constexpr Rgb
	operator*(S scale) const {
		return {
			red_ * scale,
			green_ * scale,
			blue_ * scale
		};
	}

	template <typename S>
	constexpr Rgb
	operator/(S scale) const {
		return {
			red_ / scale,
			green_ / scale,
			blue_ / scale
		};
	}

	constexpr bool
	operator==(const Rgb& other) const = default;

	// Compare perceived brightness. For performance and simplicity reasons,
	// the intermediate brightness type is hardcoded to Gray8.
	constexpr auto
	operator<=>(const Gray8& gray) const noexcept {
		return Gray8(*this) <=> gray;
	};

	void invert() {
		red_.invert();
		green_.invert();
		blue_.invert();
	}

private:
	RedType red_{0};
	GreenType green_{0};
	BlueType blue_{0};

	template<int ER, int EG, int EB>
	requires (ER > 0) && (EG > 0) && (EB > 0)
	friend class Rgb;
};

template<std::unsigned_integral U>
using RgbT = Rgb<std::numeric_limits<U>::digits>;

/// @ingroup modm_ui_color
using Rgb888 = RgbT<uint8_t>;
using Rgb161616 = RgbT<uint16_t>;

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
	// OPTIMIZE This should also work with fixed point and Colors integrated operator* and operator/
	const IntermediateType sum = IntermediateType(rgb.red() + rgb.green() + rgb.blue());
	return {
		IntermediateType(rgb.red()) * multiplier / sum,
		IntermediateType(rgb.green()) * multiplier / sum,
		IntermediateType(rgb.blue()) * multiplier / sum
	};
}

#if __has_include(<modm/io/iostream.hpp>)
#include <modm/io/iostream.hpp>

/// @ingroup modm_ui_color
template<ColorRgb C>
IOStream&
operator<<(IOStream& os, const C& rgb)
{
	os << rgb.red() << "\t" << rgb.green() << "\t" << rgb.blue();
	return os;
}
#endif

}  // namespace modm::color