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

#ifndef XPCC__SATURATED_HPP
	#error	"Don't include this file directly use 'math/saturated.hpp' instead!"
#endif

namespace xpcc
{
	// ------------------------------------------------------------------------
	template<>
	Saturated<uint8_t>&
	Saturated<uint8_t>::operator+=(const Saturated<UnsignedType>& other) {
		asm (
			"add  %[x], %[y]"   "\n\t"
			"brcc 0f"           "\n\t"	// Falls es einen unsigned Überlauf gab
										// (C=1) Maximalwert laden
			"ldi  %[x], 0xff"   "\n"
			"0:"				"\n\t"
			: [x] "+d" (this->value)
			: [y] "r"  (other.value)
		);
		
		return *this;
	}

	// ------------------------------------------------------------------------
	// Diese Kombination ist am aufwändigsten in der Behandlung und geschieht
	// am einfachsten durch Umskalierung auf zwei signed-Werte,
	// signed-saturierter Operation und nachfolgender Rückskalierung. Zu
	// beachten ist, daß dieser Operator nicht kommutativ ist, d.h. a+b
	// liefert i.A. ein anderes Ergebnis als b+a. 
	template<>
	Saturated<uint8_t>&
	Saturated<uint8_t>::operator+=(const Saturated<SignedType>& other) {
		asm (
			"subi %[x], 0x80"	"\n\t"	// Transformation
										//   [0x00, 0xff] -> [0x80, 0x7f]
			"add  %[x], %[y]"   "\n\t"
			"brvc 0f"           "\n\t"	// Falls es einen signed Überlauf gab 
										// (V=1) Maximalwert laden
			"ldi  %[x], 0x7f"   "\n\t"
			"sbrc %[y], 7"		"\n\t"	// R0 ist negativ, daher muss der 
										// Minimalwert geladen werden
			"ldi  %[x], 0x80"	"\n"
			"0:"				"\n\t"
			"subi %[x], 0x80"	"\n\t"	// Rücktransformation 
										//   [0x80, 0x7f] -> [0x00, 0xff]
			: [x] "+d" (this->value)
			: [y] "r"  (other.value)
		);
		
		return *this;
	}

	// ------------------------------------------------------------------------
	template<>
	Saturated<uint8_t>&
	Saturated<uint8_t>::operator-=(const Saturated<UnsignedType>& other) {
		asm (
			"sub  %[x], %[y]"   "\n\t"
			"brcc 0f"           "\n\t"	// Falls es einen unsigned Unterlauf 
										// gab (C=1) Minimalwert laden
			"clr  %[x]"   "\n\t"
			"0:"
			: [x] "+r" (this->value)
			: [y] "r"  (other.value)
		);
		
		return *this;
	}

	// ------------------------------------------------------------------------
	template<>
	Saturated<uint8_t>&
	Saturated<uint8_t>::operator-=(const Saturated<SignedType>& other) {
		asm (
			"subi %[x], 0x80"	"\n\t"	// Transformation
										//   [0x00, 0xff] -> [0x80, 0x7f]
			"sub  %[x], %[y]"   "\n\t"
			"brvc 0f"           "\n\t"	// Falls es einen signed Überlauf gab 
										// (V=1) Minimalwert laden
			"ldi  %[x], 0x80"   "\n\t"
			"sbrc %[y], 7"		"\n\t"	// R0 ist negativ, daher muss der 
										// Maximalwert geladen werden
			"ldi  %[x], 0x7f"	"\n"
			"0:"				"\n\t"
			"subi %[x], 0x80"	"\n\t"	// Rücktransformation 
										//   [0x80, 0x7f] -> [0x00, 0xff]
			: [x] "+d" (this->value)
			: [y] "r"  (other.value)
		);
		
		return *this;
	}
	
	// TODO operator-
}
