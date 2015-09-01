// coding: utf-8
// ----------------------------------------------------------------------------
/* Copyright (c) 2009, Roboterclub Aachen e.V.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Roboterclub Aachen e.V. nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ROBOTERCLUB AACHEN E.V. ''AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL ROBOTERCLUB AACHEN E.V. BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
// ----------------------------------------------------------------------------

#ifndef	XPCC__BIT_OPERATION_HPP
#define	XPCC__BIT_OPERATION_HPP

#include <cstddef>
#include <cmath>
#include <stdint.h>

#include <xpcc/architecture/detect.hpp>
#include <xpcc/architecture/utils.hpp>

namespace xpcc
{
	/**
	 * \brief	Exchange the two nibbles of a byte
	 *
	 * \code
	 * 0xab => 0xba
	 * \endcode
	 *
	 * \ingroup	math
	 */
	ALWAYS_INLINE uint8_t
	swap(uint8_t n)
	{
#ifdef XPCC__CPU_AVR
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

	/**
	 * \brief	Exchange two byte
	 *
	 * \ingroup	math
	 */
	ALWAYS_INLINE void
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
	 *
	 * \ingroup	math
	 */
	ALWAYS_INLINE uint16_t
	swap(uint16_t n)
	{
#ifdef XPCC__CPU_ARM
		asm volatile(
			"rev16 %0,%0"	"\n\t"
			 : "=r" (n)
			 : "0" (n)
		);
		return n;
#elif defined XPCC__CPU_AVR
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
	 *
	 * \ingroup	math
	 */
	ALWAYS_INLINE uint32_t
	swap(uint32_t n)
	{
#ifdef XPCC__CPU_ARM
		asm volatile(
			"rev %0,%0"		"\n\t"
			 : "=r" (n)
			 : "0" (n)
		);
		return n;
#else
		n = (n << 24) | ((n << 8) & 0xff0000) | ((n >> 8) & 0xff00) | (n >> 24);
		return n;
#endif
	}

	ALWAYS_INLINE void
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
	 *
	 * \ingroup	math
	 */
	inline uint8_t
	bitReverse(uint8_t n)
	{
#ifdef XPCC__CPU_ARM
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
	 *
	 * \ingroup	math
	 */
	inline uint16_t
	bitReverse(uint16_t n)
	{
#ifdef XPCC__CPU_ARM
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
	 *
	 * \ingroup	math
	 */
	inline uint32_t
	bitReverse(uint32_t n)
	{
#ifdef XPCC__CPU_ARM
		asm volatile(
			"rbit %0,%0"	"\n\t"
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
	/// @ingroup math
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
	 *
	 * \ingroup	math
	 */
	std::size_t
	bitCount(uint8_t n);

	/**
	 * \brief	Count the number of bits set
	 *
	 * 33 clock cycles on an AVR, without call + return.
	 *
	 * \ingroup	math
	 */
	std::size_t
	bitCount(uint16_t n);

	/**
	 * \brief	Count the number of bits set
	 *
	 * \ingroup	math
	 */
	std::size_t
	bitCount(uint32_t n);
}

#endif	// XPCC__BIT_OPERATION_HPP
