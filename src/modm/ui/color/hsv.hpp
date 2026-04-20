/*
 * Copyright (c) 2009, Martin Rosekeit
 * Copyright (c) 2009-2013, Fabian Greif
 * Copyright (c) 2012-2013, 2015, Niklas Hauser
 * Copyright (c) 2013, David Hebbeker
 * Copyright (c) 2021, Thomas Sommer
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_COLOR_HSV_HPP
#define MODM_COLOR_HSV_HPP

#include <stdint.h>
#include <concepts>

#include "rgb.hpp"

namespace modm::color
{

/**
 * @brief		Color in HSV Colorspace
 *
 * @author		Martin Rosekeit, Fabian Greif, Niklas Hauser, David Hebbeker, Thomas Sommer
 * @ingroup		modm_ui_color
 */
template<std::unsigned_integral T>
class HsvT
{
public:
	T hue{0};
	T saturation{0};
	T value{0};

	constexpr HsvT() = default;

	constexpr HsvT(T hue, T saturation, T value) : hue(hue), saturation(saturation), value(value) {}

	/**
	 * Copy Constructor 8bit->16bit
	 */
	template<std::unsigned_integral U>
		requires std::is_same_v<T, uint16_t> && std::is_same_v<U, uint8_t>
	constexpr HsvT(const HsvT<U> &hsv_other)
		: hue(hsv_other.hue << 8), saturation(hsv_other.saturation << 8), value(hsv_other.value << 8)
	{}

	/**
	 * Copy Constructor 16bit->8bit
	 */
	template<std::unsigned_integral U>
		requires std::is_same_v<T, uint8_t> && std::is_same_v<U, uint16_t>
	constexpr HsvT(const HsvT<U> &hsv_other)
		: hue(hsv_other.hue >> 8), saturation(hsv_other.saturation >> 8), value(hsv_other.value >> 8)
	{}

	/**
	 * Convertion Constructor for RGB Color
	 *
	 * @param rgb	RGB Color
	 */
	constexpr HsvT(const RgbT<T>& rgb);

	constexpr bool
	operator==(const HsvT<T>& other) const = default;

	constexpr operator RgbT<T>() const
		requires std::is_same_v<T, uint8_t>;

private:
	template<std::unsigned_integral U>
	friend IOStream&
	operator<<(IOStream&, const HsvT<U>&);
};

/// @ingroup modm_ui_color
using Hsv = HsvT<uint8_t>;


#if __has_include(<modm/io/iostream.hpp>)
#include <modm/io/iostream.hpp>

/// @ingroup modm_ui_color
template<std::unsigned_integral U>
IOStream&
operator<<(IOStream& os, const color::HsvT<U>& color)
{
	os << color.hue << "\t" << color.saturation << "\t" << color.value;
	return os;
}
#endif

}  // namespace modm::color

#include "hsv_impl.hpp"

#endif  // MODM_COLOR_HSV_HPP
