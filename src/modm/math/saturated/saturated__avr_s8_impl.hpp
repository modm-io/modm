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
