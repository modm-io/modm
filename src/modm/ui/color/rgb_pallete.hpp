/*
 * Copyright (c) 2023, Thomas Sommer
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------
#pragma once

#include "concepts.hpp"

namespace modm::color
{

/**
 * @brief 			Color in RGB space - mandatory for most graphic displays and stored images.
 * 					Requires less space than unstacked rgb types. Calculations without dedicated graphics
 * 					acceleration hardware are very inefficient.
 * 					If theres no dedicated graphics acceleration but lots of RAM, using modm::Rgb<> for
 * 					calculations and afterwards conversion to modm::RgbPallete<> may be an option.
 *
 * @tparam DR 		Digits for red channel
 * @tparam DG 		Digits for green channel
 * @tparam DB 		Digits for blue channel
 *
 * @author			Thomas Sommer
 * @ingroup			modm_ui_color
 */
template <int DR, int DG, int DB>
requires (DR > 0) && (DG > 0) && (DB > 0)
class RgbPallete
{
public:
	using RedType = Gray<DR>;
	using GreenType = Gray<DG>;
	using BlueType = Gray<DB>;

	using T = least_uint<DR + DG + DB>;

	constexpr RgbPallete() = default;

	constexpr RgbPallete(T value)
		: value_(value)
	{}

	constexpr RgbPallete(RedType red, GreenType green, BlueType blue)
		: value_(red << (DG + DB) | green << DB | blue)
	{}

	constexpr RgbPallete(const RgbPallete &rgbstacked)
		: value_(rgbstacked.value_)
	{}

	template<ColorGray C>
	constexpr RgbPallete(const C &gray)
		: RgbPallete(Rgb<C::digits>(gray))
	{}

	template<ColorRgb C>
	constexpr RgbPallete(const C &rgb)
		: RgbPallete(rgb.red(), rgb.green(), rgb.blue())
	{}

	template<ColorRgbPallete C>
	constexpr RgbPallete(const C &rgstacked)
		: RgbPallete(rgstacked.red(), rgstacked.green(), rgstacked.blue())
	{}

	template<ColorHsv C>
	constexpr RgbPallete(const C &hsv)
		: RgbPallete(Rgb<5,6,5>(hsv))
	{}

	/**
	 * @brief 			Interface to channels of stacked color types
	 * 					May be used for StackedHsv as well
	 *
	 * @tparam TS 		unsigned integral type of stacked color value
	 * @tparam TC 		Gray<> type of channel
	 * @tparam Shift 	lshift of channel in stacked color value
	 *
	 * @ingroup			modm_ui_color
	 */
	template<std::unsigned_integral TS, ColorGray TC, int Shift>
	struct channel_accessor {
		TS &value_;

		void operator= (const TC new_value)
		{ value_ = (value_ & ~(TC::max << Shift)) | new_value << Shift; }

		TC value() const { return (value_ >> Shift) & TC::max; }
	};

	// accessors
	constexpr RedType red() const { return value_ >> (DG + DB); }
	constexpr GreenType green() const { return value_ >> DB & GreenType::max;}
	constexpr BlueType blue() const { return value_ & BlueType::max; }

	auto red() { return channel_accessor<T, RedType, DG + DB>{value_}; }
	auto green() { return channel_accessor<T, GreenType, DB>{value_}; }
	auto blue() { return channel_accessor<T, BlueType, 0>{value_}; }

	// assignment
	void operator=(const RgbPallete other) {
		value_ = other.value_;
	}

	RgbPallete& operator+=(const Rgb<DR, DG, DB>& rgb) {
		operator=({
			red() + rgb.red(),
			green() + rgb.green(),
			blue() + rgb.blue()
		});
		return *this;
	}

	RgbPallete& operator-=(const Rgb<DR, DG, DB>& rgb) {
		operator=({
			red() - rgb.red(),
			green() - rgb.green(),
			blue() - rgb.blue()
		});
		return *this;
	}

	RgbPallete& operator*=(const Rgb<DR, DG, DB>& rgb) {
		operator=({
			red() * rgb.red(),
			green() * rgb.green(),
			blue() * rgb.blue()
		});
		return *this;
	}

	RgbPallete& operator/=(const Rgb<DR, DG, DB>& rgb) {
		operator=({
			red() * rgb.red(),
			green() * rgb.green(),
			blue() * rgb.blue()
		});
		return *this;
	}

	constexpr RgbPallete
	operator+(const Rgb<DR, DG, DB>& rgb) {
		return {
			red() + rgb.red(),
			green() + rgb.green(),
			blue() + rgb.blue()
		};
	}

	constexpr RgbPallete
	operator-(const Rgb<DR, DG, DB>& rgb) {
		return {
			red() - rgb.red(),
			green() - rgb.green(),
			blue() - rgb.blue()
		};
	}

	constexpr RgbPallete
	operator*(const Rgb<DR, DG, DB>& rgb) {
		return {
			red() * rgb.red(),
			green() * rgb.green(),
			blue() * rgb.blue()
		};
	}

	constexpr RgbPallete
	operator/(const Rgb<DR, DG, DB>& rgb) {
		return {
			red() / rgb.red(),
			green() / rgb.green(),
			blue() / rgb.blue()
		};
	}

	constexpr bool
	operator==(const RgbPallete& other) const = default;

	// Remaining comparison operators are server by color::Rgb<D> and implicit type conversion

	void invert() {
		value_ ^= T(std::pow(2, DR + DG + DB) - 1);
	}

	constexpr T value() const { return value_; }
private:
	T value_{0};
};

using Rgb565 = RgbPallete<5,6,5>;
using Rgb666 = RgbPallete<6,6,6>;

#if __has_include(<modm/io/iostream.hpp>)
#include <modm/io/iostream.hpp>

template<ColorRgbPallete C>
IOStream&
operator<<(IOStream& os, const C& rgb)
{
	os << rgb.red() << "\t" << rgb.green() << "\t" << rgb.blue();
	return os;
}
#endif

}  // namespace modm::color