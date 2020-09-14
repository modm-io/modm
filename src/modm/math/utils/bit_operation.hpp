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

#include <cstddef>
#include <cmath>
#include <stdint.h>

#include <modm/architecture/detect.hpp>
#include <modm/architecture/utils.hpp>

namespace modm
{
	/// @ingroup modm_math_utils
	/// @{

	/**
	 * \brief	Exchange the two nibbles of a byte
	 *
	 * \code
	 * 0xab => 0xba
	 * \endcode
	 */
	modm_always_inline uint8_t
	swap(uint8_t n)
	{
#ifdef MODM_CPU_AVR
		if (__builtin_constant_p(n)) {
			n = (n << 4) | (n >> 4);
		}
		else {
			asm volatile ("swap %0" : "=r" (n) : "0" (n));
		}
		return n;
#else
		n = (n << 4) | (n >> 4);
		return n;
#endif
	}

	/// Exchange two byte
	modm_always_inline void
	swap(uint8_t& a, uint8_t& b)
	{
		uint8_t temp = a;
		a = b;
		b = temp;
	}

	/**
	 * \brief	Exchange the two bytes of a 16-bit integer
	 *
	 * \code
	 * 0xabcd => 0xcdab
	 * \endcode
	 */
	modm_always_inline uint16_t
	swap(uint16_t n)
	{
#ifdef MODM_CPU_ARM
		asm volatile(
			"rev16 %0,%0"	"\n\t"
			 : "=r" (n)
			 : "0" (n)
		);
		return n;
#elif defined MODM_CPU_AVR
		if (__builtin_constant_p(n)) {
			n = (n << 8) | (n >> 8);
		}
		else {
			asm volatile (
				"eor %A0, %B0"	"\n\t"
				"eor %B0, %A0"	"\n\t"
				"eor %A0, %B0"	"\n\t"
				: "=r" (n)
				: "0" (n)
			);
		}
		return n;
#else
		n = (n << 8) | (n >> 8);
		return n;
#endif
	}

	/**
	 * \brief	Exchange the four bytes of a 32-bit integer
	 *
	 * \code
	 * 0xabcdefgh => 0xghefcdab
	 * \endcode
	 */
	modm_always_inline uint32_t
	swap(uint32_t n)
	{
#if defined(MODM_CPU_ARM) && !defined(__aarch64__)
		asm volatile(
			"rev %0,%0"		"\n\t"
			 : "=r" (n)
			 : "0" (n)
		);
		return n;
#elif defined(MODM_CPU_ARM) && defined(__aarch64__)
		asm volatile(
			"rev32 %0,%0"	"\n\t"
			 : "=r" (n)
			 : "0" (n)
		);
		return n;
#else
		n = (n << 24) | ((n << 8) & 0xff0000) | ((n >> 8) & 0xff00) | (n >> 24);
		return n;
#endif
	}

	modm_always_inline void
	swap(int16_t& a, int16_t& b)
	{
		int16_t temp = a;
		a = b;
		b = temp;
	}

	// --------------------------------------------------------------------
	/**
	 * \brief	Reverse the bits in a byte
	 *
	 * \code
	 * 0b01110100 => 0b00101110
	 * \endcode
	 *
	 * 15 clock cycles on an AVR, without call + return.
	 */
	inline uint8_t
	bitReverse(uint8_t n)
	{
#if defined(MODM_CPU_ARM) && !defined(MODM_CPU_CORTEX_M0)
		asm volatile(
			"rbit %0,%0"	"\n\t"
			"rev %0,%0"		"\n\t"
			 : "=r" (n)
			 : "0" (n)
		);
		return n;
#else
		n = ((uint8_t) (n >> 1) & 0x55) | ((uint8_t) (n << 1) & 0xaa);
		n = ((uint8_t) (n >> 2) & 0x33) | ((uint8_t) (n << 2) & 0xcc);

		return swap(n);
#endif
	}

	/**
	 * \brief	Reverse the bits in a 16-bit integer
	 */
	inline uint16_t
	bitReverse(uint16_t n)
	{
#if defined(MODM_CPU_ARM) && !defined(MODM_CPU_CORTEX_M0)
		asm volatile(
			"rbit %0,%0"	"\n\t"
			"rev16 %0,%0"	"\n\t"
			"rev %0,%0"		"\n\t"
			 : "=r" (n)
			 : "0" (n)
		);
		return n;
#else
		n = ((n >>  1) & 0x5555) | ((n <<  1) & 0xaaaa);
		n = ((n >>  2) & 0x3333) | ((n <<  2) & 0xcccc);
		n = ((n >>  4) & 0x0f0f) | ((n <<  4) & 0xf0f0);

		return swap(n);
#endif
	}

	/**
	 * \brief	Reverse the bits in a 32-bit integer
	 */
	inline uint32_t
	bitReverse(uint32_t n)
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
#else
		n = ((n >>  1) & 0x55555555) | ((n <<  1) & 0xaaaaaaaa);
		n = ((n >>  2) & 0x33333333) | ((n <<  2) & 0xcccccccc);
		n = ((n >>  4) & 0x0f0f0f0f) | ((n <<  4) & 0xf0f0f0f0);
		n = ((n >>  8) & 0x00ff00ff) | ((n <<  8) & 0xff00ff00);
		n = ((n >> 16) & 0x0000ffff) | ((n << 16) & 0xffff0000);

		return n;
#endif
	}

	/// @cond
	constexpr int8_t
	leftmostBitHelper(uint32_t value)
	{
		return (value == 0)?  0  :  (1 + leftmostBitHelper(value >> 1));
	}
	/// @endcond

	/// Returns position of leftmost bit at compile time.
	/// @retval -1  if no bit set in value
	/// @retval n   position of leftmost bit
	constexpr int8_t
	leftmostBit(uint32_t value)
	{
		return leftmostBitHelper(value) - 1;
	}

	// --------------------------------------------------------------------
	/**
	 * \brief	Count the number of bits set
	 *
	 * 16 clock cycles on an AVR, without call + return.
	 */
	std::size_t
	bitCount(uint8_t n);

	/**
	 * \brief	Count the number of bits set
	 *
	 * 33 clock cycles on an AVR, without call + return.
	 */
	std::size_t
	bitCount(uint16_t n);

	/// Count the number of bits set
	std::size_t
	bitCount(uint32_t n);

	/// @}
}

#endif	// MODM_BIT_OPERATION_HPP
