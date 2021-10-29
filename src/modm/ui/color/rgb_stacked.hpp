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

#include "concepts.hpp"

namespace modm::color {
/**
 * @brief 			Stores a RGB color in a stacked. Consumes potentialy less space but
 * 					does not offer immediate access to each channel making it Inefficient
 * 					to calc with. Primary application as display- or storage-format.
 *
 * @tparam DR 		Bits for red channel
 * @tparam DG 		Bits for green channel
 * @tparam DB 		Bits for blue channel
 *
 * @author			Thomas Sommer
 * @ingroup			modm_ui_color
 */
template <int DR, int DG, int DB>
class RgbStackedD
{
public:
	using RedType = GrayD<DR>;
	using GreenType = GrayD<DG>;
	using BlueType = GrayD<DB>;

	using ValueType = uint_t<DR + DG + DB>::least;

	constexpr RgbStackedD() = default;

	constexpr RgbStackedD(const ValueType value) : value(value){};

	constexpr RgbStackedD(RedType red, GreenType green, BlueType blue)
		: value(red.getValue() << (DG + DB) | green.getValue() << DB | blue.getValue()) {}

	template<ColorRgbStacked C>
	constexpr RgbStackedD(const C &other)
		: RgbStackedD(other.getRed(), other.getGreen(), other.getBlue()) {}

	template<ColorRgbStacked C>
	constexpr RgbStackedD(C &&other)
		: RgbStackedD(other.getRed(), other.getGreen(), other.getBlue()) {}

	template<ColorGray C>
	constexpr RgbStackedD(const C &gray)
		: RgbStackedD(RgbD<C::Digits, C::Digits, C::Digits>(gray)) {}

	template<ColorRgb C>
	constexpr RgbStackedD(const C &rgb)
		: RgbStackedD(rgb.getRed(), rgb.getGreen(), rgb.getBlue()) {}

	template<ColorHsv C>
	constexpr RgbStackedD(const C &hsv)
		: RgbStackedD(RgbD<5,6,5>(hsv)) {}

	void setRed(RedType red)
	{ value = (value & ~(RedType::mask << (DG + DB))) | red.value << (DG + DB); }
	void setGreen(GreenType green)
	{ value = (value & ~(GreenType::mask << DB)) | green.value << DB; }
	void setBlue(BlueType blue)
	{ value = (value & ~BlueType::mask) | blue.value; }

	RedType getRed() const { return value >> (DG + DB); }
	GreenType getGreen() const { return value >> DB & GreenType::max;}
	BlueType getBlue() const { return value & BlueType::max; }

	ValueType getValue() const { return value; }

	template <typename RgbStackedT_>
	constexpr RgbStackedD operator+(RgbStackedT_ &other) {
		return {
			getRed() + other.getRed(),
			getGreen() + other.getGreen(),
			getBlue() + other.getBlue()
		};
	}

	// OPTIMIZE Do move semantics help for these operators?
	template <typename RgbStackedT_>
	constexpr RgbStackedD operator-(RgbStackedT_ &other) {
		return {
			getRed() - other.getRed(),
			getGreen() - other.getGreen(),
			getBlue() - other.getBlue()
		};
	}

	template <typename RgbStackedT_>
	RgbStackedD& operator+=(RgbStackedT_ &other) {
		setRed(getRed() + other.getRed());
		setGreen(getGreen() + other.getGreen());
		setBlue(getBlue() + other.getBlue());
		return *this;
	}

	template <typename RgbStackedT_>
	RgbStackedD& operator-=(RgbStackedT_ &other) {
		setRed(getRed() - other.getRed());
		setGreen(getGreen() - other.getGreen());
		setBlue(getBlue() - other.getBlue());
		return *this;
	}

	// IMPLEMENT missing operators
	// operator+
	// operator-

	constexpr bool
	operator==(const RgbStackedD& other) const = default;

	void invert() {
		value ^= bitmask<DR + DG + DB>();
	}

	template <typename ScaleType>
	constexpr RgbStackedD operator*(ScaleType &scale) {
		return {
			getRed() * scale,
			getGreen() * scale,
			getBlue() * scale
		};
	}

private:
	ValueType value;

	template<ColorRgbStacked C>
	friend IOStream&
	operator<<(IOStream&, const C&);
};

using Rgb565 = RgbStackedD<5,6,5>;

#if __has_include(<modm/io/iostream.hpp>)
#include <modm/io/iostream.hpp>

template<ColorRgbStacked C>
IOStream&
operator<<(IOStream& os, const C& rgb)
{
	os << rgb.getRed() << "\t" << rgb.getGreen() << "\t" << rgb.getBlue();
	return os;
}
#endif

}