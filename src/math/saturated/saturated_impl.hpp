
#include "../saturated.h"

// ----------------------------------------------------------------------------
// 								uint8_t
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
template<>
Saturated<uint8_t>&
Saturated<uint8_t>::operator+=(const Saturated<unsigned_type>& other) {
	asm (
		"add  %[x], %[y]"   "\n\t"
		"brcc 0f"           "\n\t"	// Falls es einen unsigned Überlauf gab
									// (C=1) Maximalwert laden
		"ldi  %[x], 0xff"   "\n"
		"0:"				"\n\t"
		: [x] "+d" (this->m_value)
		: [y] "r"  (other.m_value)
	);
	
	return *this;
}

// ----------------------------------------------------------------------------
// Diese Kombination ist am aufwändigsten in der Behandlung und geschieht am
// einfachsten durch Umskalierung auf zwei signed-Werte, signed-saturierter
// Operation und nachfolgender Rückskalierung. Zu beachten ist, daß dieser
// Operator nicht kommutativ ist, d.h. a+b liefert i.A. ein anderes Ergebnis
// als b+a. 
template<>
Saturated<uint8_t>&
Saturated<uint8_t>::operator+=(const Saturated<signed_type>& other) {
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
		: [x] "+d" (this->m_value)
		: [y] "r"  (other.m_value)
	);
	
	return *this;
}

// ----------------------------------------------------------------------------
template<>
Saturated<uint8_t>&
Saturated<uint8_t>::operator-=(const Saturated<unsigned_type>& other) {
	asm (
		"sub  %[x], %[y]"   "\n\t"
		"brcc 0f"           "\n\t"	// Falls es einen unsigned Unterlauf gab 
									// (C=1) Minimalwert laden
		"clr  %[x]"   "\n\t"
		"0:"
		: [x] "+r" (this->m_value)
		: [y] "r"  (other.m_value)
	);
	
	return *this;
}

// ----------------------------------------------------------------------------
template<>
Saturated<uint8_t>&
Saturated<uint8_t>::operator-=(const Saturated<signed_type>& other) {
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
		: [x] "+d" (this->m_value)
		: [y] "r"  (other.m_value)
	);
	
	return *this;
}

// ----------------------------------------------------------------------------
// 								int8_t
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
template<>
Saturated<int8_t>&
Saturated<int8_t>::operator+=(const Saturated<signed_type>& other) {
	asm (
		"add  %[x], %[y]"	"\n\t"
		"brvc 0f"			"\n\t"	// Falls es einen signed Überlauf gab (V=1)
									// Maximalwert laden
		"ldi  %[x], 0x7f"	"\n\t"
		"sbrc %[y], 7"		"\n\t"	// y ist negativ, daher muss der
									// Minimalwert geladen werden
		"ldi  %[x], 0x80"	"\n\t"
		"0:"
		: [x] "+d" (this->m_value)
		: [y] "r"  (other.m_value)
	);
	
	return *this;
}

// ----------------------------------------------------------------------------
// TODO testen
template<>
Saturated<int8_t>&
Saturated<int8_t>::operator+=(const Saturated<unsigned_type>& other) {
	asm (
		"add  %[x], %[y]"	"\n\t"
		"brvc 0f"			"\n\t"	// Falls es einen signed Überlauf gab (V=1)
									// Maximalwert laden
		"ldi  %[x], 0xff"	"\n\t"
		"0:"
		: [x] "+d" (this->m_value)
		: [y] "r"  (other.m_value)
	);
	
	return *this;
}

// ----------------------------------------------------------------------------
template<>
Saturated<int8_t>&
Saturated<int8_t>::operator-=(const Saturated<signed_type>& other) {
	asm (
		"sub  %[x], %[y]"	"\n\t"
		"brvc 0f"			"\n\t"	// Falls es einen signed Überlauf gab (V=1)
									// Minimalwert laden
		"ldi  %[x], 0x80"	"\n\t"
		"sbrc %[y], 7"		"\n\t"	// y ist negativ, daher muss der
									// Maximalwert geladen werden
		"ldi  %[x], 0x7f"		"\n\t"
		"0:"
		: [x] "+d" (this->m_value)
		: [y] "r"  (other.m_value)
	);
	
	return *this;
}

// ----------------------------------------------------------------------------
// TODO testen
template<>
Saturated<int8_t>&
Saturated<int8_t>::operator-=(const Saturated<unsigned_type>& other) {
	asm (
		"sub  %[x], %[y]"	"\n\t"
		"brvc 0f"			"\n\t"	// Falls es einen signed Überlauf gab (V=1)
									// Minimalwert laden
		"ldi  %[x], 0x80"	"\n\t"
		"0:"
		: [x] "+d" (this->m_value)
		: [y] "r"  (other.m_value)
	);
	
	return *this;
}

// ----------------------------------------------------------------------------
template<>
Saturated<int8_t>
operator-(const Saturated<int8_t>& a) {
	Saturated<int8_t> temp(a);
	asm (
		"neg %[x]"		"\n\t"
		"brvc 0f"		"\n\t"	// Signed Überlauf (V=1): Das Ergebnis
								// ist 0x80 und wird verändert zu 0x7f
		"dec  %[x]"		"\n\t"
		"0:"
		: [x] "+d" (temp.m_value)
	);
	return temp;
}

// ----------------------------------------------------------------------------
template<typename T>
Saturated<int8_t>
abs(const Saturated<int8_t>& a) {
	Saturated<int8_t> temp(a);
	asm (
		"sbrc %[x], 7"	"\n\t"
		"neg  %[x]"		"\n\t"	// x < 0: negieren
		"sbrc %[x], 7"	"\n\t"
		"dec  %[x]"		"\n\t"	// R0 ist immer noch < 0 (also 0x80), 
								// lade 0x7f
		: [x] "+d" (temp.m_value)
	);
	return temp;
}
