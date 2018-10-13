/*
 * Copyright (c) 2009, Fabian Greif
 * Copyright (c) 2010, Martin Rosekeit
 * Copyright (c) 2012, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_SATURATED_HPP
	#error	"Don't include this file directly use 'math/saturated.hpp' instead!"
#endif

namespace modm
{
	// ------------------------------------------------------------------------
	template<>
	Saturated<uint8_t>&
	Saturated<uint8_t>::operator += (const Saturated& other) {
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
	/*template<>
	Saturated<uint8_t>&
	Saturated<uint8_t>::operator += (const Saturated<SignedType>& other) {
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
	}*/

	// ------------------------------------------------------------------------
	template<>
	Saturated<uint8_t>&
	Saturated<uint8_t>::operator -= (const Saturated& other) {
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
	/*template<>
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
	}*/
}
