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
 * $Id: math.hpp 75 2009-10-14 22:48:12Z dergraaf $
 */
// ----------------------------------------------------------------------------

#ifndef	XPCC_AVR__MATH_HPP
#define	XPCC_AVR__MATH_HPP

#include <stdint.h>

namespace xpcc
{
	namespace avr
	{
		/**
		 * @ingroup	avr
		 * @brief	Fast and short 32 bits sqrt routine
		 * 
		 * Quadratwurzel basierend auf einer Implementierung von Ruud v Gessel,
		 * die zusammen mit avr-gcc verwendet werden kann. Je nach Algorithmus
		 * wird das Ergebnis zum Nächsten gerundet oder abgerundet. Abrunden
		 * ist dann angesagt, wenn die Wurzel aus einer großen Eingabe wie
		 * 0xffffffff zu ziehen ist, da bei Aufrunden hier das Ergebnis
		 * zu 0 überläuft.
		 * 
		 * Die Ausführungszeit ist maximal 310 Ticks (inclusive CALL+RET)
		 * 
		 * @see		http://www.mikrocontroller.net/articles/AVR_Arithmetik#avr-gcc_Implementierung_.2832_Bit.29
		 * @see		http://members.chello.nl/j.beentjes3/Ruud/sqrt32avr.htm
		 */
		extern "C" uint16_t
		sqrt32_round(uint32_t);

		/**
		 * @ingroup	avr
		 */
		extern "C" uint16_t
		sqrt32_floor(uint32_t);

		/**
		 * @ingroup	avr
		 * @brief	unsigned 16bit x 16bit = 32bit multiplication
		 * 
		 * @see		AVR201
		 */
		inline uint32_t
		mul32(uint16_t a, uint16_t b);

		/**
		 * @ingroup	avr
		 * @brief	signed 16bit x 16bit = 32bit multiplication
		 * 
		 * @see		AVR201
		 */
		inline int32_t
		mul32(int16_t a, int16_t b);

		/**
		 * @ingroup	avr
		 * @brief	Signed multiply accumulate of two 16bits numbers with
		 * 			a 32bits result
		 * 
		 * @verbatim
		 * result += a * b;
		 * @endverbatim
		 * 
		 * @see		AVR201
		 */
		inline int32_t
		mac32(int32_t result, int16_t a, int16_t b);
	}
}

#include "math_impl.hpp"

#endif	// XPCC_AVR__MATH_HPP
