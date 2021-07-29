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

#include <concepts>

#include "hsv.hpp"
#include "rgb.hpp"
#include "rgb565.hpp"

namespace modm::color
{

// forward declarations for convertion constructors
template<std::unsigned_integral T>
class RgbT;

template<std::unsigned_integral T>
class HsvT;

template<bool BigEndian>
class Rgb565;

/**
 * @brief		Grayscale as unsigned integral. Add's the math for conversion to and from
 *				Color-Types. Use with: Grayscale Buffers, Dimmed LEDs, Grayscale sensors
 *
 * @author		Thomas Sommer
 *
 * @tparam	T	Unsigned integral for the grayscale-value
 * @ingroup		modm_ui_color
 */
template<std::unsigned_integral T>
class GrayscaleT
{
public:
	T value{0};

	constexpr GrayscaleT() = default;

	constexpr GrayscaleT(T value) : value(value) {}

	/**
	 * Copy Constructor 8bit->16bit
	 */
	template<std::unsigned_integral U>
		requires std::is_same_v<T, uint16_t> && std::is_same_v<U, uint8_t>
	constexpr GrayscaleT(const GrayscaleT<U> &other)
		: value(other.value << 8)
	{}

	/**
	 * Copy Constructor 16bit->8bit
	 */
	template<std::unsigned_integral U>
		requires std::is_same_v<T, uint8_t> && std::is_same_v<U, uint16_t>
	constexpr GrayscaleT(const GrayscaleT<U> &other)
		: value(other.value >> 8)
	{}

	/**
	 * Convertion Constructor for RGB Color
	 *
	 * @param rgb	RGB Color
	 */
	template<std::unsigned_integral U>
	constexpr GrayscaleT(RgbT<U> rgb)
		: value((0.2125 * float(rgb.red)) + (0.7154 * float(rgb.green)) +
				(0.0721 * float(rgb.blue)))
	{}

	/**
	 * Convertion Constructor for HSV Color
	 *
	 * @param hsv	HSV Color
	 */
	template<std::unsigned_integral U>
	constexpr GrayscaleT(HsvT<U> hsv) : value(hsv.value)
	{}

	/**
	 * Convertion Constructor for RGB565 Color
	 *
	 * @param rgb565	RGB565 Color
	 */
	template<bool BigEndian>
	constexpr GrayscaleT(Rgb565<BigEndian> rgb565) : GrayscaleT<T>(RgbT<uint8_t>(rgb565)) {}

	constexpr bool
	operator==(const GrayscaleT<T> &other) const = default;
};

/// @ingroup modm_ui_color
using Grayscale = GrayscaleT<uint8_t>;

}  // namespace modm::color
