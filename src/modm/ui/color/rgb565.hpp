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

#include "brightness.hpp"
#include "hsv.hpp"
#include "rgb.hpp"

namespace modm::color
{

// forward declarations for convertion constructors
template<std::unsigned_integral T>
class RgbT;

template<std::unsigned_integral T>
class HsvT;

template<std::unsigned_integral T>
class BrightnessT;

/**
 * Color in RGB Colorspace, 16 bits: RRRR RGGG GGGB BBBB
 *
 * @author		Fabian Greif, Thomas Sommer
 * @ingroup		modm_ui_color
 */
class Rgb565
{
public:
	uint16_t color{0x0000};

	using RgbCalcType = RgbT<uint8_t>;

	constexpr Rgb565() = default;

	/**
	 * Constructor for preformatted Rgb565 Color
	 *
	 * @param color		Preformatted RGB-color in 565-format: RRRR RGGG GGGB BBBB
	 */
	constexpr Rgb565(uint16_t color) : color(color) {}

	/**
	 * Constructor for components: red, green, blue
	 *
	 * @param red 		Red color component
	 * @param green 	Green color component
	 * @param blue 		Blue color component
	 */
	constexpr Rgb565(uint8_t red, uint8_t green, uint8_t blue)
		: color(uint16_t(red & 0xF8) << 8 | uint16_t(green & 0xFC) << 3 | uint16_t(blue >> 3))
	{}

	/**
	 * Convertion Constructor for RGB Color
	 *
	 * @param rgb	RGB Color
	 */
	template<std::unsigned_integral U>
	constexpr Rgb565(const RgbT<U> &rgb) : Rgb565(rgb.red, rgb.green, rgb.blue)
	{}

	/**
	 * Convertion Constructor for HSV Color
	 *
	 * @param hsv	HSV Color
	 */
	template<std::unsigned_integral U>
	constexpr Rgb565(const HsvT<U> &hsv) : Rgb565(RgbCalcType(hsv))
	{}

	/**
	 * Convertion Constructor for Brightness
	 *
	 * @param brightness	Brightness 'Color'-object
	 */
	template<std::unsigned_integral U>
	constexpr Rgb565(const BrightnessT<U> brightness) : Rgb565(RgbCalcType(brightness))
	{}

	constexpr bool
	operator==(const Rgb565 &other) const = default;

	/// Saturated addition ⊕
	Rgb565
	operator+(const Rgb565 other) const
	{
		const int8_t red_raw = (color >> 11) + (other.color >> 11);
		const uint16_t red = std::clamp<int8_t>(red_raw, 0, 31) << 11;

		const int8_t green_raw = (color >> 5 & 0x3F) + (other.color >> 5 & 0x3F);
		const uint16_t green = std::clamp<int8_t>(green_raw, 0, 63) << 5;

		const int8_t blue_raw = (color & 0x1F) + (other.color & 0x1F);
		const uint16_t blue = std::clamp<int8_t>(blue_raw, 0, 31);

		return Rgb565(red | green | blue);
	}

	/// Saturated substraction ⊖
	Rgb565
	operator-(const Rgb565 other) const
	{
		const int8_t red_raw = (color >> 11) - (other.color >> 11);
		const uint16_t red = std::clamp<int8_t>(red_raw, 0, 0x1F) << 11;

		const int8_t green_raw = (color >> 5 & 0x3F) - (other.color >> 5 & 0x3F);
		const uint16_t green = std::clamp<int8_t>(green_raw, 0, 0x3F) << 5;

		const int8_t blue_raw = (color & 0x1F) - (other.color & 0x1F);
		const uint16_t blue = std::clamp<int8_t>(blue_raw, 0, 0x1F);

		return Rgb565(red | green | blue);
	}
};

}  // namespace modm::color
