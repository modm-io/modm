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

#include <algorithm>
#include <concepts>
#include <functional>

#include <modm/math/utils/integer_traits.hpp>
#include <modm/math/saturated.hpp>

#include <modm/math/utils/arithmetic_traits.hpp>
#include <modm/math/utils/misc.hpp>

#include "concepts.hpp"

namespace modm::color
{

/**
 * @brief 		Unsigned integer with arbitrary number of digits, proportional conversion
 * 				and saturating arithemtics.
 *
 * @tparam D 	Number of Digits
 *
 * @author		Thomas Sommer
 *
 * @ingroup		modm_ui_color
 */
template <int D>
requires (D > 0)
class Gray
{
public:
	using T = least_uint<D>;
	static constexpr T max = std::pow(2, D) - 1;
	static constexpr int digits = D;

	/// constructors
	constexpr Gray() = default;

	constexpr Gray(T value)
	{
#ifdef MODM_DEBUG_BUILD
		if (not modm_assert_continue_ignore_debug(value <= max, "Gray.ctor",
												  "value exceeded max", max))
		{
			value_ = std::min(max, value);
			return;
		}
#endif

		value_ = value;
	}

	// Construct from Gray with less digits: scale up
	template <int E>
	requires (E < D)
	constexpr Gray(const Gray<E>& other)
		: value_(other.value_ * max / other.max)
	{}

	// Construct from Gray with more digits: scale down
	/// @warning Scaling down may infer loss of accuracy!
	template <int E>
	requires (E > D)
	constexpr Gray(const Gray<E>& other)
		: value_(other.value_ >> (E - D))
	{}

	// OPTIMIZE Check the colors continuoty of this conversion algorithm,
	// once buffers can be viewed on displays.
	// @details: https://github.com/modm-io/modm/pull/781#discussion_r820142167

	// @see: https://en.wikipedia.org/wiki/Grayscale#Converting_colour_to_grayscale
  	template<ColorRgb C>
	constexpr Gray(const C& rgb)
		: value_(
			( 1742 * Gray(rgb.red())
			+ 5859 * Gray(rgb.green())
			+ 591 * Gray(rgb.blue())
		) >> 13)
	{}

	template<ColorRgbPallete C>
 	constexpr Gray(const C& rgbpallete)
	 	: Gray(Rgb<C::RedType::digits, C::GreenType::digits, C::BlueType::digits>(rgbpallete))
	{}

	// Conversion from Hsv does not reflect human brightness perception like conversion from Rgb above
	// e.g. converting Hsv->Rgb->Gray will not produce the same result like Hsv->Gray
	// However, using hsv serves the purpose
 	template<ColorHsv C>
	constexpr Gray(const C& hsv) : value_(hsv.value())
	{}


	/// assignment operator
	void
	operator=(T value)
	{
#ifdef MODM_DEBUG_BUILD
		if (not modm_assert_continue_ignore_debug(value <= max, "Gray.assign",
												  "value exceeded max", max))
		{
			value_ = std::min(max, value);
			return;
		}
#endif

		value_ = value;
	}

	// Assign from Gray with less digits: scale up
	template <int E>
	requires (E < D)
	constexpr void operator=(const Gray<E>& other) {
		value_ = other.value_ * max / other.max;
	}

	// Assign from Gray with more digits: scale down
	/// @warning Scaling down may infer loss of accuracy!
	template <int E>
	requires (E > D)
	constexpr void operator=(const Gray<E>& other) {
		value_ = other.value_ >> (E - D);
	}

	// operator +=, -=, *=, /=
	template <std::integral I>
	Gray&
	operator+=(const I integral) {
		modm::Saturated<T&> saturated(value_);
		saturated += integral;

		value_ = std::min(value_, max);

		return *this;
	}

	template <int I>
 	Gray&
	operator+=(const Gray<I>& other)
	{
		return this->operator+=(other.value_);
	}

	template <std::integral I>
	Gray&
	operator-=(const I integral) {
		modm::Saturated<T&> saturated(value_);
		saturated -= integral;

		value_ = std::min(value_, max);

		return *this;
	}

	template <int I>
 	Gray&
	operator-=(const Gray<I>& other)
	{
		return this->operator-=(other.value_);
	}

	template<std::integral I>
	Gray&
	operator*=(const I scale) {
		modm::Saturated<T&> saturated(value_);
		saturated *= scale;

		value_ = std::min(value_, max);

		return *this;
	}

	template <int I>
 	Gray&
	operator*=(const Gray<I>& other)
	{
		return this->operator*=(other.value_);
	}

	template<std::integral I>
	Gray&
	operator/=(const I scale) {
		modm::Saturated<T&> saturated(value_);
		saturated /= scale;

		value_ = std::min(value_, max);

		return *this;
	}

	template <int I>
 	Gray&
	operator/=(const Gray<I>& other)
	{
		return this->operator*=(other.value_);
	}

	// operator +, -, *, /
	template<std::integral I>
	constexpr Gray
	operator+(const I integral) const {
		modm::Saturated<T> saturated(value_);
		saturated += integral;

		return std::min(saturated.value(), max);
	}

	constexpr Gray
	operator+(const Gray& other) const
	{
		return this->operator+(other.value_);
	}

	template<std::integral I>
	constexpr Gray
	operator-(const I integral) const {
		modm::Saturated<T> saturated(value_);
		saturated -= integral;

		return std::min(saturated.value(), max);
	}

	constexpr Gray
	operator-(const Gray& other) const
	{
		return this->operator-(other.value_);
	}

	template<std::integral I>
	constexpr Gray
	operator*(const I scale) const {
		modm::Saturated<I> saturated(value_);
		saturated *= scale;

		return std::min<T>(saturated.value(), max);
	}

	constexpr Gray
	operator*(const Gray& other) const
	{
		return this->operator*(other.value_);
	}

	template<std::integral I>
	constexpr Gray
	operator/(const I scale) const {
		modm::Saturated<I> saturated(value_);
		saturated /= scale;
		return saturated.value();
	}

	constexpr Gray
	operator/(const Gray& other) const
	{
		return this->operator/(other.value_);
	}

	template<std::floating_point F>
	constexpr Gray
	operator*(const F scale) const
	{
		// OPTIMIZE develop optimal decimals from D
		static constexpr int decimals = 10;

		using WideType = modm::WideType<T>;
		WideType saturated = value_ * T(scale * decimals) / decimals;
		return {T(std::min<WideType>(saturated, max))};
	}

	void
	invert() {
		value_ ^= T(std::pow(2, D) - 1);
	}

	bool
	constexpr isSaturated() const
	{
		return value_ == max;
	}

	constexpr operator T() const {
		return value_;
	}

protected:
	T value_{0};

private:
	template<int E>
	requires (E > 0)
	friend class Gray;

	template <int E>
	friend modm::IOStream &
	operator<<(modm::IOStream &, const Gray<E> &);
};

// If the user wants to provide a type rather than digits
template<typename E>
using GrayT = Gray<std::numeric_limits<E>::digits>;

using Monochrome = Gray<1>;
using Gray2 = Gray<2>;
using Gray4 = Gray<4>;
using Gray8 = GrayT<uint8_t>;
using Gray16 = GrayT<uint16_t>;
// Also fine
// using Gray8 = Gray<8>;
// using Gray16 = Gray<16>;

#if __has_include(<modm/io/iostream.hpp>)
#include <modm/io/iostream.hpp>

template <int E>
modm::IOStream &
operator<<(modm::IOStream &os, const Gray<E> &color)
{
	os << color.value_;
	return os;
}
#endif

}  // namespace modm::color