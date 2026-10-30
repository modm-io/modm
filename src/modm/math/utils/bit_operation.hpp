/*
 * Copyright (c) 2009, Martin Rosekeit
 * Copyright (c) 2009-2011, 2013, Fabian Greif
 * Copyright (c) 2012-2016, Niklas Hauser
 * Copyright (c) 2016, Kevin Läufer
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef	MODM_BIT_OPERATION_HPP
#define	MODM_BIT_OPERATION_HPP

#include <bit>
#include <concepts>
#include <cstddef>
#include <stdint.h>
#include <utility>

#include <modm/architecture/detect.hpp>
#include <modm/architecture/utils.hpp>

namespace modm
{
	/// @ingroup modm_math_utils
	/// @{

	/**
	 * Exchange the two nibbles of a byte
	 *
	 * \code
	 * 0xab => 0xba
	 * \endcode
	 */
	[[deprecated("use std::rotl(n, 4)")]] // DEPRECATED: 2027q4
	constexpr uint8_t
	swap(uint8_t n)
	{
		return std::rotl(n, 4);
	}

	/// Exchange two byte
	[[deprecated("use std::swap")]] // DEPRECATED: 2027q4
	constexpr void
	swap(uint8_t& a, uint8_t& b)
	{
		std::swap(a, b);
	}

	/**
	 * Exchange the two bytes of a 16-bit integer
	 *
	 * \code
	 * 0xabcd => 0xcdab
	 * \endcode
	 */
	[[deprecated("use std::byteswap")]] // DEPRECATED: 2027q4
	constexpr uint16_t
	swap(uint16_t n)
	{
		return std::byteswap(n);
	}

	/**
	 * Exchange the four bytes of a 32-bit integer
	 *
	 * \code
	 * 0xabcdefgh => 0xghefcdab
	 * \endcode
	 */
	[[deprecated("use std::byteswap")]] // DEPRECATED: 2027q4
	constexpr uint32_t
	swap(uint32_t n)
	{
		return std::byteswap(n);
	}

	[[deprecated("use std::swap")]] // DEPRECATED: 2027q4
	constexpr void
	swap(int16_t& a, int16_t& b)
	{
		std::swap(a, b);
	}

	// --------------------------------------------------------------------
	/**
	 * Reverse the bits in a byte
	 *
	 * \code
	 * 0b01110100 => 0b00101110
	 * \endcode
	 *
	 * 15 clock cycles on an AVR, without call + return.
	 */
	constexpr uint8_t
	bitReverse(uint8_t n)
	{
#if defined(MODM_CPU_ARM) && !defined(MODM_CPU_CORTEX_M0)
		if !consteval
		{
			asm volatile(
				"rbit %0,%0"	"\n\t"
				"rev %0,%0"		"\n\t"
				 : "=r" (n)
				 : "0" (n)
			);
			return n;
		}
#endif
		n = ((uint8_t) (n >> 1) & 0x55) | ((uint8_t) (n << 1) & 0xaa);
		n = ((uint8_t) (n >> 2) & 0x33) | ((uint8_t) (n << 2) & 0xcc);

		return std::rotl(n, 4);
	}

	/// Reverse the bits in a 16-bit integer
	constexpr uint16_t
	bitReverse(uint16_t n)
	{
#if defined(MODM_CPU_ARM) && !defined(MODM_CPU_CORTEX_M0)
		if !consteval
		{
			asm volatile(
				"rbit %0,%0"	"\n\t"
				"rev16 %0,%0"	"\n\t"
				"rev %0,%0"		"\n\t"
				 : "=r" (n)
				 : "0" (n)
			);
			return n;
		}
#endif
		n = ((n >>  1) & 0x5555) | ((n <<  1) & 0xaaaa);
		n = ((n >>  2) & 0x3333) | ((n <<  2) & 0xcccc);
		n = ((n >>  4) & 0x0f0f) | ((n <<  4) & 0xf0f0);

		return std::byteswap(n);
	}

	/// Reverse the bits in a 32-bit integer
	constexpr uint32_t
	bitReverse(uint32_t n)
	{
		if !consteval
		{
#if defined(MODM_CPU_ARM) && !defined(MODM_CPU_CORTEX_M0) && !defined(__aarch64__)
			asm volatile(
				"rbit %0,%0"	"\n\t"
				 : "=r" (n)
				 : "0" (n)
			);
			return n;
#elif defined(__aarch64__)
			asm volatile(
				"rbit %0,%0"	"\n\t"
				"rev32 %0,%0"	"\n\t"
				"rev %0,%0"		"\n\t"
				 : "=r" (n)
				 : "0" (n)
			);
			return n;
#endif
		}
		n = ((n >>  1) & 0x55555555) | ((n <<  1) & 0xaaaaaaaa);
		n = ((n >>  2) & 0x33333333) | ((n <<  2) & 0xcccccccc);
		n = ((n >>  4) & 0x0f0f0f0f) | ((n <<  4) & 0xf0f0f0f0);
		n = ((n >>  8) & 0x00ff00ff) | ((n <<  8) & 0xff00ff00);
		n = ((n >> 16) & 0x0000ffff) | ((n << 16) & 0xffff0000);

		return n;
	}

	/// Returns position of leftmost bit.
	/// @retval -1  if no bit set in value
	/// @retval n   position of leftmost bit
	[[deprecated("use std::bit_width(value) - 1")]] // DEPRECATED: 2027q4
	constexpr int8_t
	leftmostBit(uint32_t value)
	{
		return std::bit_width(value) - 1;
	}

	/// Count the number of bits set
	template< std::unsigned_integral T >
	[[deprecated("use std::popcount")]] // DEPRECATED: 2027q4
	constexpr std::size_t
	bitCount(T n)
	{
		return std::popcount(n);
	}

	/// @}
}

#endif	// MODM_BIT_OPERATION_HPP
