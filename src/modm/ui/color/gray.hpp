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

#include <concepts>
#include <algorithm>
#include <functional>

#include <modm/math/scaling_unsigned.hpp>
#include <modm/math/saturation/saturated.hpp>

#include <modm/math/utils/arithmetic_traits.hpp>
#include <modm/math/utils/misc.hpp>

#include "concepts.hpp"

namespace modm::color {
/**
 * @brief 		Unsigned integer with arbitrary number of digits, symetric conversion
 * 				and saturating arithemtics. Used for grayscale
 *
 * @tparam D 	Digits
 * @tparam P	When forming a Buffer with this color, this would be the internal storage.
 * 				Should be MCUs fastest unsigned int -> just keep the default.
 * 				Some Displays require a specific type therefore it's a template argument.
 *
 *
 * @author		Thomas Sommer
 * @ingroup		modm_color_gray
 */
template <int D>
// template <int D, std::unsigned_integral P = uint8_t> // TODO
class GrayD : public modm::ScalingUnsigned<D>
{
	static_assert(D > 0, "Positive number of digits required for grayscale / colorchannel type.");
public:
	using ValueType = ScalingUnsigned<D>::ValueType;

	// using PalletType = uint32_t; // Best performance on STM32
	// using PalletType = uint16_t;
	using PalletType = uint8_t; // Must for Ssd1306, Sh1106
	// using PalletType = P; // TODO PalletType from template argument

	using ScalingUnsigned<D>::ScalingUnsigned;

  	template<ColorRgb C>
	constexpr GrayD(const C& rgb)
		: ScalingUnsigned<D>((
			2125 * (modm::WideType<ValueType>)(GrayD(rgb.getRed()).value) +
			7154 * (modm::WideType<ValueType>)(GrayD(rgb.getGreen()).value) +
			0721 * (modm::WideType<ValueType>)(GrayD(rgb.getBlue()).value)
			) / 10000
		)
	{}

 	template<ColorHsv C>
	constexpr GrayD(const C& hsv) : ScalingUnsigned<D>(hsv.getValue())
	{}

	GrayD& operator+=(const ValueType& value) {
		modm::Saturated<ValueType&> result(this->value);
		result+=value;
		this->max_cutoff();

		return *this;
	}

	GrayD& operator+=(const GrayD& other) {
		modm::Saturated<ValueType&> result(this->value);
		result+=other.value;
		this->max_cutoff();

		return *this;
	}

	GrayD& operator-=(const ValueType& value) {
		modm::Saturated<ValueType&> result(this->value);
		result-=value;
		this->max_cutoff();

		return *this;
	}

	GrayD& operator-=(const GrayD& other) {
		modm::Saturated<ValueType&> result(this->value);
		result-=other.value;
		this->max_cutoff();

		return *this;
	}

	// IMPLEMENT missing operators
	// operator*=() {}

	template<std::integral U>
	GrayD operator+(const U value) const {
		modm::Saturated<ValueType> result(this->value);
		result+=value;

		// When all digits of ValueType are used, std::min should optimize away
		return {std::min(result.getValue(), this->max)};
	}

	GrayD operator+(const GrayD& other) const {
		modm::Saturated<ValueType> result(this->value);
		result+=other.value;

		// When all digits of ValueType are used, std::min should optimize away
		return {std::min(result.getValue(), this->max)};
	}

	template<std::integral U>
	GrayD operator-(const U value) const {
		modm::Saturated<ValueType> result(this->value);
		result-=value;

		// When all digits of ValueType are used, std::min should optimize away
		return {std::min(result.getValue(), this->max)};
	}

	GrayD operator-(const GrayD& other) const {
		modm::Saturated<ValueType> result(this->value);
		result -= other.value;

		return result.getValue();
	}

	template<std::integral ScaleType>
	GrayD operator*(const ScaleType &scale) const {
		modm::Saturated<ValueType> result(this->value);
		result *= scale;

		// When all digits of ValueType are used, std::min should optimize away
		return {std::min(result.getValue(), this->max)};
	}

	template<std::floating_point ScaleType>
	GrayD
	operator*(const ScaleType &scale) const
	{
		// OPTIMIZE develop optimal decimals from D
		static constexpr int decimals = 10;
		using WideType = modm::WideType<ValueType>;
		WideType result = this->value * ValueType(scale * decimals) / decimals;
		return {ValueType(std::min<WideType>(result, max))};
	}

	void invert() {
		this->value ^= bitmask<D>();
		// alterantive: this->value ^= D - this->value;
		// alternative: this->value ^= this->max;
	}

private:

	// template<int, std::unsigned_integral> // TODO
	template<int>
	friend class GrayD;

	template <int V>
	friend modm::IOStream &
	operator<<(modm::IOStream &, const GrayD<V> &);
};

template<typename U>
using GrayT = GrayD<std::numeric_limits<U>::digits>;

using Monochrome = GrayD<1>;
using Gray2 = GrayD<2>;
using Gray4 = GrayD<4>;
using Gray8 = GrayT<uint8_t>;
using Gray16 = GrayT<uint16_t>;

#if __has_include(<modm/io/iostream.hpp>)
#include <modm/io/iostream.hpp>

template <int V>
modm::IOStream &
operator<<(modm::IOStream &os, const GrayD<V> &color)
{
	os << color.value;
	return os;
}
#endif

}