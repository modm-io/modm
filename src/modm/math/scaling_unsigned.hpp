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

#include <algorithm>

#include <modm/math/utils/integer_traits.hpp>
// #include <modm/architecture/interface/assert.hpp>

namespace modm {

/**
 * @brief 		Unsigned integer with arbitrary digits and scaling value on conversion
 * 				between instances with different digits.
 *
 * @tparam D 	Digits
 *
 * @author		Thomas Sommer
 */
template<int D>
class ScalingUnsigned {
public:
	static constexpr int Digits = D;
	using ValueType = uint_t<D>::least;

	static constexpr ValueType min = 0;
	static constexpr ValueType max = bitmask<D>();

	constexpr ScalingUnsigned() = default;

	// FIXME want both:
	// COMPTIME: static_assert(value <= max, "value out of range")
	// RUNTIME: std::min(value, max) or modm_assert(value <= max, ...)
	constexpr ScalingUnsigned(ValueType value) : value(std::min(value, max)) {
		// TODO disable via lbuild option
		// modm_assert_continue_fail_debug(value <= max, "ScalingUnsigned", "constructor", "value out of range");
	}

	// Construct from bigger or equal ColorGray
	template <int E, std::enable_if_t<(D <= E), void*> = nullptr>
	constexpr ScalingUnsigned(const ScalingUnsigned<E>& other)
		: value(other.value >> (E - D)) {}

	template <int E, std::enable_if_t<(D <= E), void*> = nullptr>
	constexpr ScalingUnsigned(ScalingUnsigned<E> &&other)
		: value(other.value >> (E - D)) {}

	// Construct from smaller ColorGray
	template <int E, std::enable_if_t<(D > E), void*> = nullptr>
	constexpr ScalingUnsigned(const ScalingUnsigned<E>& other)
		: value(other.value * max / other.max)
	{}

	template <int E, std::enable_if_t<(D > E), void*> = nullptr>
	constexpr ScalingUnsigned(ScalingUnsigned<E> &&other)
		: value(other.value * max / other.max)
	{}

	/* 	// Faster construction from from single digit
	constexpr ScalingUnsigned(const ScalingUnsigned<1> &other) : value(other.value ? bitmask<D>() : 0){}

	// constexpr ScalingUnsigned(ScalingUnsigned<1> &&other) : value(other.value ? bitmask<D>() : 0){}
	constexpr ScalingUnsigned& operator=(const ScalingUnsigned<1> &other) {
		value = other.value ? bitmask<D>() : 0;
		return *this;
	} */

	// Assign ScalingUnsigned with more or equal Depth
	template <int E, std::enable_if_t<(D <= E), void*> = nullptr>
	void operator=(const ScalingUnsigned<E>& other) {
		value = other.value >> (E - D);
	}

	// Assign ScalingUnsigned with less Depth
	template <int E, std::enable_if_t<(D > E), void*> = nullptr>
	void operator=(const ScalingUnsigned<E>& other) {
		value = other.value * max / other.max;
	}

	// FIXME want both:
	// COMPTIME: static_assert(value <= max, "value out of range")
	// RUNTIME: std::min(value, max) or modm_assert(value <= max, ...)
	void setValue(ValueType value) {
		this->value = std::min(value, max);
		// TODO disable via lbuild option
		// modm_assert_continue_fail_debug(value <= max, "modm::ScalingUnsigned", "setValue()", "value out of range");
	}

	ValueType getValue() const { return value; }

		bool isSaturated() const {
		return value == max;
	}

	constexpr auto operator<=>(const ScalingUnsigned<D> &) const = default;

protected:
	ValueType value = 0;

	inline void max_cutoff() { value = std::min(value, max); }
private:
	template<int>
	friend class ScalingUnsigned;
};

}