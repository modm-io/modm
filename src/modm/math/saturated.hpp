/*
 * Copyright (c) 2022-2023, Thomas Sommer
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
#include <cmath>
#include <limits>
#include <type_traits>

#include <modm/math/utils/arithmetic_traits.hpp>
#include <modm/math/utils/integer_traits.hpp>

namespace modm
{

/**
 * @brief	Saturation arithmetic building on 'Integer-Overflow-Builtins'
 * 			Implementation works with integer, unsigned integer and float or reference to them.
 * 			Operators work with the same types or Saturated types of them.
 * @see		https://gcc.gnu.org/onlinedocs/gcc/Integer-Overflow-Builtins.html
 * @see		https://en.wikipedia.org/wiki/Saturation_arithmetic
 *
 * @author	Thomas Sommer
 *
 * @ingroup modm_math_saturation
 */
template<typename T>
requires std::integral<std::remove_reference_t<T>>
class Saturated
{
protected:
	using TP = std::remove_reference_t<T>;
	using TS = std::conditional_t<std::is_signed_v<T>, T, std::make_signed_t<modm::WideType<TP>>>;

	T value_{0};

private:
	static constexpr TP min = std::numeric_limits<TP>::min();
	static constexpr TP max = std::numeric_limits<TP>::max();

public:
	Saturated() = default;

	constexpr Saturated(const T& value) : value_(value){};
	constexpr Saturated(const Saturated<T>& other) : value_(other.value_){};

	template<typename U>
	requires std::integral<std::remove_reference_t<U>>
	constexpr Saturated(const U& value)
	{ value_ = std::clamp< modm::fits_any<TP, U> >(value, min, max); }

	template<typename U>
	requires std::floating_point<std::remove_reference_t<U>>
	constexpr Saturated(const U& value)
	{ value_ = std::clamp<float>(value, min, max); }

	template<typename U>
	requires std::integral<std::remove_reference_t<U>>
	constexpr Saturated(const Saturated<U>& other)
	{ value_ = std::clamp< modm::fits_any<TP, U> >(other.value, min, max); }

	T value() const
	{ return value_; }

	// Cast to underlying type. No more comparison operators required.
	// @see https://en.cppreference.com/w/cpp/language/cast_operator
	operator T() const
	{ return value_; }

	// operator=
	void
	operator=(const Saturated& other)
	{ value_ = other.value_; }

	template<typename U>
	requires std::integral<std::remove_reference_t<U>>
	void
	operator=(const U& other)
	{ value_ = std::clamp< modm::fits_any<TP, U> >(other, min, max); }

	template<typename U>
	requires std::integral<std::remove_reference_t<U>>
	void
	operator=(const Saturated<U>& other)
	{ value_ = std::clamp< modm::fits_any<TP, U> >(other.value_, min, max); }

	// Post: operator++, operator--
	Saturated&
	operator++()
	{
		if (value_ < max) value_++;
		return *this;
	}

	Saturated&
	operator--()
	{
		if (value_ > min) value_--;
		return *this;
	}

	// Pre: operator++(int), operator--(int)
	Saturated
	operator++(int)
	{
		Saturated ret(*this);
		if (value_ < max) value_++;
		return ret;
	}

	Saturated
	operator--(int)
	{
		Saturated ret(*this);
		if (value_ > min) value_--;
		return ret;
	}

	// operator+=, operator-=, operator*=
	template<typename U>
	requires std::unsigned_integral<std::remove_reference_t<U>>
	Saturated&
	operator+=(const U& other)
	{
		if (__builtin_add_overflow(value_, other, &value_))
			value_ = max;

		return *this;
	}

	template<typename U>
	requires std::signed_integral<std::remove_reference_t<U>>
	Saturated&
	operator+=(const U& other)
	{
		if (other < 0) {
			if (__builtin_sub_overflow(value_, -other, &value_))
				value_ = min;
		} else {
			if (__builtin_add_overflow(value_, other, &value_))
				value_ = max;
		}

		return *this;
	}

	template<typename U>
	requires std::integral<std::remove_reference_t<U>>
	Saturated&
	operator+=(const Saturated<U>& other)
	{ return this->operator+=(other.value_); }

	template<typename U>
	requires std::unsigned_integral<std::remove_reference_t<U>>
	Saturated&
	operator-=(const U& other)
	{
		if (__builtin_sub_overflow(value_, other, &value_))
			value_ = min;

		return *this;
	}

	template<typename U>
	requires std::signed_integral<std::remove_reference_t<U>>
	Saturated&
	operator-=(const U& other)
	{
		if (other < 0) {
			if (__builtin_add_overflow(value_, -other, &value_))
				value_ = max;
		} else {
			if (__builtin_sub_overflow(value_, other, &value_))
				value_ = min;
		}

		return *this;
	}

	template<typename U>
	requires std::integral<std::remove_reference_t<U>>
	Saturated&
	operator-=(const Saturated<U>& other)
	{ return this->operator-=(other.value_); }

	template<typename U>
	requires std::unsigned_integral<std::remove_reference_t<U>>
	Saturated&
	operator*=(const U& other)
	{
		if (__builtin_mul_overflow(value_, other, &value_))
			value_ = max;

		return *this;
	}

	template<typename U>
	requires std::signed_integral<std::remove_reference_t<U>>
	Saturated&
	operator*=(const U& other)
	{
		if (other < 0) {
			if (__builtin_mul_overflow(value_, -other, &value_))
				value_ = max;
			value_ = -value_;
		} else {
			if (__builtin_mul_overflow(value_, other, &value_))
				value_ = max;
		}

		return *this;
	}

	template<typename U>
	requires std::integral<std::remove_reference_t<U>>
	Saturated&
	operator*=(const Saturated<U>& other)
	{ return this->operator*=(other.value_); }

	// operator+, operator-, operator*
	template<typename U>
	requires std::unsigned_integral<std::remove_reference_t<U>>
	TP
	operator+(const U& other) const
	{
		Saturated<TP> ret;

		if (__builtin_add_overflow(value_, other, &ret.value_))
			ret.value_ = max;

		return ret;
	}

	template<typename U>
	requires std::signed_integral<std::remove_reference_t<U>>
	TP
	operator+(const U& other) const
	{
		Saturated<TP> ret;

		if (other < 0) {
			if (__builtin_sub_overflow(value_, -other, &ret.value_))
				ret.value_ = min;
		} else {
			if (__builtin_add_overflow(value_, other, &ret.value_))
				ret.value_ = max;
		}

		return ret;
	}

	template<typename U>
	requires std::integral<std::remove_reference_t<U>>
	Saturated<TP>
	operator+(const Saturated<U>& other) const
	{ return this->operator+(other.value_); }

	template<typename U>
	requires std::unsigned_integral<std::remove_reference_t<U>>
	Saturated<TP>
	operator-(const U& other) const
	{
		Saturated<TP> ret;

		if (__builtin_sub_overflow(value_, other, &ret.value_))
			ret.value_ = min;

		return ret;
	}

	template<typename U>
	requires std::signed_integral<std::remove_reference_t<U>>
	Saturated<TP>
	operator-(const U& other) const
	{
		Saturated<TP> ret;

		if (other < 0) {
			if (__builtin_add_overflow(value_, -other, &ret.value_))
				ret.value_ = max;
		} else {
			if (__builtin_sub_overflow(value_, other, &ret.value_))
				ret.value_ = min;
		}

		return ret;
	}

	template<typename U>
	requires std::integral<std::remove_reference_t<U>>
	Saturated<TP>
	operator-(const Saturated<U>& other) const
	{ return this->operator-(other.value_); }

	template<typename U>
	requires std::unsigned_integral<std::remove_reference_t<U>>
	Saturated<TP>
	operator*(const U& other) const
	{
		Saturated<TP> ret;

		if (__builtin_mul_overflow(value_, other, &ret.value_))
			ret.value_ = max;

		return ret;
	}

	template<typename U>
	requires std::signed_integral<std::remove_reference_t<U>>
	Saturated<TP>
	operator*(const U& other) const
	{
		Saturated<TP> ret;

		if (other < 0) {
			if (__builtin_mul_overflow(value_, -other, &ret.value_))
				ret.value_ = max;
			ret.value_ = -ret.value_;
		} else {
			if (__builtin_mul_overflow(value_, other, &ret.value_))
				ret.value_ = max;
		}

		return ret;
	}

	template<typename U>
	requires std::integral<std::remove_reference_t<U>>
	Saturated<TP>
	operator*(const Saturated<U>& other) const
	{ return this->operator*(other.value_); }

	template<typename U>
	requires std::integral<std::remove_reference_t<U>>
	Saturated<TP>
	operator/(const U& other) const
	{
		return Saturated<TP>(value_ / other);
	}

	template<typename U>
	requires std::integral<std::remove_reference_t<U>>
	Saturated<TP>
	operator/(const Saturated<U>& other) const
	{ return this->operator/(other.value_); }


	TS
	operator-() const
 	{ return -TS(value_); }

	void
	absolute()
	// Should be std::abs but that's troubelous for avr-gcc
	// @see: https://stackoverflow.com/questions/1374037/ambiguous-overload-call-to-absdouble
	{ value_ = abs(value_); }

	template<typename U>
	requires std::integral<std::remove_reference_t<U>>
	friend class Saturated;
};

}  // namespace modm
