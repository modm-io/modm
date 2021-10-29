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

#include "concepts.hpp"
#include "gray.hpp"

#include <modm/math/utils/arithmetic_traits.hpp>

namespace modm::color
{
/**
 * @brief			Color in HSV space. Each channel has it's own Memoryaddress.
 *
 * @tparam DH 		Digits for hue
 * @tparam DS 		Digits for saturation
 * @tparam DV 		Digits for value
 *
 * @author			Thomas Sommer
 * @ingroup			modm_ui_color
 */
template<int DH, int DS = DH, int DV = DH>
class HsvD
{
public:
	// TODO HueType should wrap rather than saturate
	using HueType = GrayD<DH>;
	using SaturationType = GrayD<DS>;
	using ValueType = GrayD<DV>;

	constexpr HsvD() = default;

	constexpr HsvD(HueType hue, SaturationType saturation, ValueType value)
		: hue(hue), saturation(saturation), value(value)
	{}

	template<ColorHsv C>
	constexpr HsvD(const C &other)
		: hue(other.hue), saturation(other.saturation), value(other.value)
	{}

	template<ColorRgb C>
	constexpr HsvD(const C& rgb);

	template<ColorRgbStacked CS>
 	constexpr HsvD(const CS& rgbstacked) : HsvD(RgbD<CS::DR, CS::DG, CS::DB>(rgbstacked)) {}

	void setHue(HueType hue) { this->hue = hue;}
	void setSaturation(SaturationType saturation) { this->saturation = saturation;}
	void setValue(ValueType value) { this->value = value;}

	HueType getHue() const { return hue; }

	SaturationType getSaturation() const { return saturation; }
	ValueType getValue() const { return value; }

	constexpr bool
	operator==(const HsvD& other) const = default;

	// IMPLEMENT missing operators
	// operator+=()
	// operator-=()
	// operator*=()

	void invert() {
		hue.invert();
	}

	// Output in human readable format
	// Hue in deg, Sat in pct, Value in pct
	void iostream_human_readable(IOStream& io) {
		using CalcTypeHue = modm::WideType<typename HueType::ValueType>;
		io << (CalcTypeHue(hue.getValue()) * 360 / hue.max) << "deg\t";

		using CalcTypeSaturation = modm::WideType<typename SaturationType::ValueType>;
 		io << (CalcTypeSaturation(saturation.getValue()) * 100 / saturation.max) << "%\t";

		using CalcTypeValue = modm::WideType<typename ValueType::ValueType>;
		io << (CalcTypeValue(value.getValue()) * 100 / value.max) << "%";
	}

private:
	HueType hue{0};
	SaturationType saturation{0};
	ValueType value{0};

	template<int, int, int>
	friend class HsvD;

	template<ColorHsv C>
	friend IOStream&
	operator<<(IOStream&, const C&);
};

template<std::unsigned_integral T>
using HsvT = HsvD<std::numeric_limits<T>::digits>;

/// @ingroup modm_ui_color
using Hsv888 = HsvD<8>; // Alternative using Hsv888 = HsvT<uint8_t>;
using Hsv161616 = HsvD<16>; // Alternative using Hsv888 = HsvT<uint16_t>;

#if __has_include(<modm/io/iostream.hpp>)
#include <modm/io/iostream.hpp>

template<ColorHsv C>
IOStream&
operator<<(IOStream& os, const C& hsv)
{
	os << hsv.getHue() << "\t" << hsv.getSaturation() << "\t" << hsv.getValue();
	return os;
}
#endif

}  // namespace modm::color

#include "hsv_impl.hpp"
