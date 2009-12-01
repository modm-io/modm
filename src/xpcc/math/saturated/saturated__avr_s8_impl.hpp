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
 * $Id: saturated_impl_avr_s8.hpp 75 2009-10-14 22:48:12Z dergraaf $
 */
// ----------------------------------------------------------------------------

#ifndef XPCC__SATURATED_HPP
	#error	"Don't include this file directly use 'math/saturated.hpp' instead!"
#endif

namespace xpcc
{
	// ------------------------------------------------------------------------
	template<>
	Saturated<int8_t>&
	Saturated<int8_t>::operator+=(const Saturated& other) {
		asm (
			"add  %[x], %[y]"	"\n\t"
			"brvc 0f"			"\n\t"	// Falls es einen signed Überlauf gab
										// (V=1) Maximalwert laden
			"ldi  %[x], 0x7f"	"\n\t"
			"sbrc %[y], 7"		"\n\t"	// y ist negativ, daher muss der
										// Minimalwert geladen werden
			"ldi  %[x], 0x80"	"\n\t"
			"0:"
			: [x] "+d" (this->value)
			: [y] "r"  (other.value)
		);
		
		return *this;
	}

	// ------------------------------------------------------------------------
	// TODO testen
	/*template<>
	Saturated<int8_t>&
	Saturated<int8_t>::operator+=(const Saturated<UnsignedType>& other) {
		asm (
			"add  %[x], %[y]"	"\n\t"
			"brvc 0f"			"\n\t"	// Falls es einen signed Überlauf gab
										// (V=1) Maximalwert laden
			"ldi  %[x], 0xff"	"\n\t"
			"0:"
			: [x] "+d" (this->value)
			: [y] "r"  (other.value)
		);
		
		return *this;
	}*/

	// ------------------------------------------------------------------------
	template<>
	Saturated<int8_t>&
	Saturated<int8_t>::operator-=(const Saturated& other) {
		asm (
			"sub  %[x], %[y]"	"\n\t"
			"brvc 0f"			"\n\t"	// Falls es einen signed Überlauf gab
										// (V=1) Minimalwert laden
			"ldi  %[x], 0x80"	"\n\t"
			"sbrc %[y], 7"		"\n\t"	// y ist negativ, daher muss der
										// Maximalwert geladen werden
			"ldi  %[x], 0x7f"		"\n\t"
			"0:"
			: [x] "+d" (this->value)
			: [y] "r"  (other.value)
		);
		
		return *this;
	}

	// ------------------------------------------------------------------------
	// TODO testen
	/*template<>
	Saturated<int8_t>&
	Saturated<int8_t>::operator-=(const Saturated<UnsignedType>& other) {
		asm (
			"sub  %[x], %[y]"	"\n\t"
			"brvc 0f"			"\n\t"	// Falls es einen signed Überlauf gab
										// (V=1) Minimalwert laden
			"ldi  %[x], 0x80"	"\n\t"
			"0:"
			: [x] "+d" (this->value)
			: [y] "r"  (other.value)
		);
		
		return *this;
	}*/

	// ------------------------------------------------------------------------
	// FIXME warum funktioniert das nicht???
	template<>
	Saturated<int8_t>
	operator - (const Saturated<int8_t>& a) {
		Saturated<int8_t> temp(a);
		asm (
			"neg %[x]"		"\n\t"
			"brvc 0f"		"\n\t"	// Signed Überlauf (V=1): Das Ergebnis
									// ist 0x80 und wird verändert zu 0x7f
			"dec  %[x]"		"\n\t"
			"0:"
			: [x] "+d" (temp.value)
		);
		return temp;
	}

	// ------------------------------------------------------------------------
	template<>
	Saturated<int8_t>
	abs(const Saturated<int8_t>& a) {
		Saturated<int8_t> temp(a);
		asm (
			"sbrc %[x], 7"	"\n\t"
			"neg  %[x]"		"\n\t"	// x < 0: negieren
			"sbrc %[x], 7"	"\n\t"
			"dec  %[x]"		"\n\t"	// R0 ist immer noch < 0 (also 0x80), 
									// lade 0x7f
			: [x] "+d" (temp.value)
		);
		return temp;
	}
}
