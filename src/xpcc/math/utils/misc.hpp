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
 * $Id: misc.hpp 607 2011-09-13 19:51:03Z dergraaf $
 */
// ----------------------------------------------------------------------------

#ifndef	XPCC_MATH__UTILS_HPP
#define	XPCC_MATH__UTILS_HPP

#include <cstddef>
#include <cmath>
#include <stdint.h>

#include <xpcc/architecture/utils.hpp>

namespace xpcc
{
	/**
	 * \brief	Fast check if a float variable is positive
	 *
	 * Checks only the sign bit for the AVR.
	 * 
	 * \ingroup	math
	 */
	inline bool
	isPositive(const float& a)
	{
#ifdef __AVR__
		// IEEE 754-1985: the most significant bit is the sign bit
		// sign = 0 => positive
		// sign = 1 => negative
		uint8_t *t = (uint8_t *) &a;
		if (*(t + 3) & 0x80) {
			return false;
		}
		else {
			return true;
		}
#else
		return (a > 0.0);
#endif
	}
	
	// --------------------------------------------------------------------
	/**
	 * \brief	Compile time exponentiation
	 * 
	 * Calculates B raised to the power of N, B and N must be compile-time
	 * constants.
	 * 
	 * \code
	 * int value = xpcc::Pow<10, 2>::value;
	 * \endcode
	 * 
	 * \tparam	B	Base
	 * \tparam	N	Exponent
	 * 
	 * \ingroup	math
	 */
	template <int B, int N>
	class Pow
	{
	public:
		enum { value = B * Pow<B, N - 1>::value };
	};
	
	// specialization for B^0 which is always 1
	// used to end the recursion in Pow<>
	template <int B>
	class Pow<B, 0>
	{
	public:
		enum { value = 1 };
	};
}

#endif	// XPCC_MATH__UTILS_HPP
