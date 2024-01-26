/*
 * Copyright (c) 2019, 2024, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_MATH_UNITS_HPP
#define MODM_MATH_UNITS_HPP

#include <stdint.h>
#include <modm/architecture/utils.hpp>
#include <chrono>

#ifdef __DOXYGEN__

namespace modm
{
	/// @ingroup modm_math_units
	/// @{
	using frequency_t = uint32_t;
	template<typename T> constexpr frequency_t Hz(T value);
	template<typename T> constexpr frequency_t kHz(T value);
	template<typename T> constexpr frequency_t MHz(T value);

	using baudrate_t = uint32_t;
	template<typename T> constexpr baudrate_t Bd(T value);
	template<typename T> constexpr baudrate_t kBd(T value);
	template<typename T> constexpr baudrate_t MBd(T value);

	using bitrate_t = uint32_t;
	template<typename T> constexpr bitrate_t bps(T value);
	template<typename T> constexpr bitrate_t kbps(T value);
	template<typename T> constexpr bitrate_t Mbps(T value);

	using percent_t = float;
	template<typename T> constexpr percent_t pct(T value);

	using seconds_t = std::chrono::seconds;
	using milliseconds_t = std::chrono::milliseconds;
	using microseconds_t = std::chrono::microseconds;
	/// @}

	namespace literals
	{
		/// @ingroup modm_math_units
		/// @{
		constexpr operator""_Hz(T value);
		constexpr operator""_kHz(T value);
		constexpr operator""_MHz(T value);

		constexpr operator""_Bd(T value);
		constexpr operator""_kBd(T value);
		constexpr operator""_MBd(T value);

		constexpr operator""_bps(T value);
		constexpr operator""_kbps(T value);
		constexpr operator""_Mbps(T value);

		constexpr operator""_pct(T value);
		/// @}
	}
}

#else

#define MODM_UNITS_LITERAL_DEFINITION(type, name, symbol) \
using MODM_CONCAT(name, _t) = type; \
template<typename T> constexpr MODM_CONCAT(name, _t) symbol(T value) { return value; } \
template<typename T> constexpr MODM_CONCAT(name, _t) MODM_CONCAT(k, symbol)(T value) { return value * 1'000ul; } \
template<typename T> constexpr MODM_CONCAT(name, _t) MODM_CONCAT(M, symbol)(T value) { return value * 1'000'000ul; } \
namespace literals { \
	constexpr MODM_CONCAT(name, _t) MODM_CONCAT(operator""_ , symbol)(unsigned long long int value) { return symbol(value); } \
	constexpr MODM_CONCAT(name, _t) MODM_CONCAT(operator""_k, symbol)(unsigned long long int value) { return MODM_CONCAT(k, symbol)(value); } \
	constexpr MODM_CONCAT(name, _t) MODM_CONCAT(operator""_k, symbol)(long double value)            { return MODM_CONCAT(k, symbol)(value); } \
	constexpr MODM_CONCAT(name, _t) MODM_CONCAT(operator""_M, symbol)(unsigned long long int value) { return MODM_CONCAT(M, symbol)(value); } \
	constexpr MODM_CONCAT(name, _t) MODM_CONCAT(operator""_M, symbol)(long double value)            { return MODM_CONCAT(M, symbol)(value); } \
}

#define MODM_UNITS_TIME_DEFINITION(type) \
struct MODM_CONCAT(type, _t) { \
	template< class Rep, class Period > \
	constexpr MODM_CONCAT(type, _t)(std::chrono::duration<Rep, Period> duration) \
	: count_{std::chrono::duration_cast<std::chrono::type>(duration).count()} {} \
	std::chrono::type::rep count_; \
	constexpr std::chrono::type::rep count() const { return count_; } \
};

namespace modm
{

MODM_UNITS_LITERAL_DEFINITION(uint32_t, frequency, Hz)
MODM_UNITS_LITERAL_DEFINITION(uint32_t, baudrate, Bd)
MODM_UNITS_LITERAL_DEFINITION(uint32_t, bitrate, bps)

using percent_t = float;
template<typename T> constexpr percent_t pct(T value) { return value / 100.f; }
// DEPRECATED: 2025q1
modm_deprecated("Access the value directly.") constexpr float pct2f(percent_t value) { return value; }
namespace literals
{
	constexpr percent_t operator""_pct(long double value) { return pct(value); }
	constexpr percent_t operator""_pct(unsigned long long int value) { return pct(value); }
}

MODM_UNITS_TIME_DEFINITION(seconds)
MODM_UNITS_TIME_DEFINITION(milliseconds)
MODM_UNITS_TIME_DEFINITION(microseconds)

using namespace ::modm::literals;
}

#endif	// __DOXYGEN__

#endif	// MODM_MATH_UNITS_HPP
