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
 *
 * $Id: misc.hpp 88 2009-10-16 23:07:26Z dergraaf $
 */
// ----------------------------------------------------------------------------

#ifndef	MISC_HPP
#define	MISC_HPP

#include <stdint.h>

/**
 * @ingroup	utils
 * @brief	Force inlining
 * 
 * Macro to force inlining on function if needed. Compiling with -Os  does not
 * always inline them when declared only \c inline.
 */
#define ALWAYS_INLINE  inline __attribute__((always_inline))

#ifdef __DOXYGEN__

	/**
	 * @ingroup	utils
	 * @brief	Convert the argument into a C-String
	 */
	#define	STRINGIFY(s)	#s

	/**
	 * @ingroup	utils
	 * @brief	Concatenate the two arguments
	 */
	#define	CONCAT(a,b)		a ## b

#else // !__DOXYGEN__

	#define	STRINGIFY(s)	STRINGIFY2(s)
	#define	STRINGIFY2(s)	STRINGIFY3(s)
	#define	STRINGIFY3(s)	#s

	#define	CONCAT(a,b)		CONCAT2(a,b)
	#define	CONCAT2(a,b)	CONCAT3(a,b)
	#define	CONCAT3(a,b)	a ## b

	#ifndef	BASENAME
		#define	FILENAME	__FILE__
	#else
		#define	FILENAME	STRINGIFY(BASENAME)
	#endif

#endif

namespace xpcc
{
	namespace utils
	{
		/**
		 * @ingroup	utils
		 * @brief	Add volatile modifier to T
		 */
		template<typename T>
		volatile T&
		asVolatile(T& value)
		{
			return (volatile T&) value;
		}
		
		/**
		 * @ingroup	utils
		 * @brief	Exchange the two nibbles of a byte
		 */
		ALWAYS_INLINE uint8_t
		swap(uint8_t n)
		{
#ifdef __AVR__
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
		 * @ingroup	utils
		 * @brief	Exchange the two bytes of a 16-bit integer
		 */
		ALWAYS_INLINE uint16_t
		swap(uint16_t n)
		{
#ifdef __AVR__
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
		 * @ingroup	utils
		 * @brief	Reverse the bits in a byte
		 * 
		 * 15 cycles on an AVR, without call + return.
		 */
		uint8_t
		bitReverse(uint8_t n);
		
		/**
		 * @ingroup	utils
		 * @brief	Reverse the bits in a 16-bit integer
		 */
		uint16_t
		bitReverse(uint16_t n);
		
		/**
		 * @ingroup	utils
		 * @brief	Reverse the bits in a 32-bit integer
		 */
		uint32_t
		bitReverse(uint32_t n);
		
		/**
		 * @ingroup	utils
		 * @brief	Count the number of bit set to 1
		 * 
		 * 16 cycles on an AVR, without call + return.
		 * 
		 * @see		http://infolab.stanford.edu/~manku/bitReverse/bitReverse.html
		 */
		uint_fast8_t
		bitCount(uint8_t n);
		
		/**
		 * @ingroup	utils
		 * @brief	Count the number of bit set to 1
		 * 
		 * 33 cycles on an AVR, without call + return.
		 */
		uint_fast8_t
		bitCount(uint16_t n);
		
		/**
		 * @ingroup	utils
		 * @brief	Count the number of bit set to 1
		 */
		uint_fast8_t
		bitCount(uint32_t n);
	}
}

#endif	// MISC_HPP
