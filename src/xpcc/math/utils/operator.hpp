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
 * $Id$
 */
// ----------------------------------------------------------------------------

#ifndef	XPCC_MATH__OPERATOR_HPP
#define	XPCC_MATH__OPERATOR_HPP

#include <cmath>
#include <xpcc/architecture/platform.hpp>

namespace xpcc
{
	namespace math
	{
		/**
		 * \brief	Fast AVR integer square root assembly routines
		 * 
		 * Square root calculation based on a implementation by Ruud v Gessel.
		 * The maximum execution time is 310 clock cycles (inclusive CALL+RET)
		 * 
		 * \see		<a href="http://www.mikrocontroller.net/articles/AVR_Arithmetik#avr-gcc_Implementierung_.2832_Bit.29" target="_blank">
		 * 			Article on microcontroller.net</a>
		 * \see		<a href="http://members.chello.nl/j.beentjes3/Ruud/sqrt32avr.htm" target="_blank">
		 * 			Original implementation</a>
		 * 
		 * \ingroup	math
		 */
		extern "C" uint16_t
		sqrt32(uint32_t);

		/**
		 * \brief	unsigned 16bit x 16bit = 32bit multiplication
		 * 
		 * \see		AVR201
		 * \ingroup	math
		 */
		inline uint32_t
		mul32(uint16_t a, uint16_t b);

		/**
		 * \brief	signed 16bit x 16bit = 32bit multiplication
		 * 
		 * \see		AVR201
		 * \ingroup	math
		 */
		inline int32_t
		mul32(int16_t a, int16_t b);

		/**
		 * \brief	Signed multiply accumulate of two 16bits numbers with
		 * 			a 32bits result
		 * 
		 * \code
		 * result += a * b;
		 * \endcode
		 * 
		 * \see		AVR201
		 * \ingroup	math
		 */
		inline int32_t
		mac32(int32_t result, int16_t a, int16_t b);
	}
}

#if defined(__AVR__) && defined(__AVR_HAVE_MUL__)
	#include "operator_avr_impl.hpp"
#else
	#include "operator_impl.hpp"
#endif

#endif	// XPCC_MATH__OPERATOR_HPP
