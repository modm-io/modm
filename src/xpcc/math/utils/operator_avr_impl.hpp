/*
 * Copyright (c) 2009-2011, Fabian Greif
 * Copyright (c) 2012, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef	MODM_MATH_OPERATOR_HPP
	#error	"Don't include this file directly, use 'operator.hpp' instead!"
#endif

inline uint32_t
modm::math::mul(uint16_t a, uint16_t b)
{
	uint32_t result;
	asm(
		"mul	%B1, %B2"			"\n\t"	// a.high x b.high
		"movw	%C0, r0"			"\n\t"
		"mul	%A1, %A2"			"\n\t"	// a.low x b.low
		"movw	%A0, r0"			"\n\t"
		"mul	%B1, %A2"			"\n\t"	// a.high x b.low
		"add	%B0, r0"			"\n\t"
		"adc	%C0, r1"			"\n\t"
		"clr	__zero_reg__"		"\n\t"	// reset r1
		"adc	%D0, __zero_reg__"	"\n\t"	// add carry
		"mul	%A1, %B2"			"\n\t"	// a.low x b.high
		"add	%B0, r0"			"\n\t"
		"adc	%C0, r1" 			"\n\t"
		"clr	__zero_reg__"		"\n\t"	// reset r1
		"adc	%D0, __zero_reg__"	"\n\t"	// add carry
		: "=&r" (result)
		: "r" (a), "r" (b)
	);
	
	return result;
}

inline int32_t
modm::math::mul(int16_t a, int16_t b)
{
	int32_t result;
	int8_t help_reg;
	
	// %1 is used like as __zero_reg__ because the original one (r1) contains
	// the result of the mul-operations
	asm(
		"clr	%1"					"\n\t"
		
		"muls	%B2, %B3"			"\n\t"	// (signed) a.high x (signed) b.high  
		"movw	%C0, r0"			"\n\t"
		
		"mul	%A2, %A3"			"\n\t"	// a.low x b.low
		"movw	%A0, r0"			"\n\t"
		
		"mulsu	%B2, %A3"			"\n\t"	// (signed) a.high x b.low
		"sbc	%D0, %1"			"\n\t"	// correct sign
		"add	%B0, r0"			"\n\t"
		"adc	%C0, r1"			"\n\t"
		"adc	%D0, %1"			"\n\t"	// add carry
		
		"mulsu	%B3, %A2"			"\n\t"	// (signed) b.high x a.low
		"sbc	%D0, %1"			"\n\t"	// correct sign
		"add	%B0, r0"			"\n\t"
		"adc	%C0, r1"			"\n\t"
		"adc	%D0, %1"			"\n\t"	// add carry
		
		"clr	__zero_reg__"   	"\n\t"	// clear r1
		: "=&r" (result), "=&r" (help_reg)
		: "a" (a), "a" (b)
	);
	
	return result;
}

inline int32_t
modm::math::mac(int32_t result, int16_t a, int16_t b)
{
	int8_t help_reg;
	
	// %1 is used like as __zero_reg__ because the original one (r1) contains
	// the result of the mul-operations
	asm(
		"clr	%1"					"\n\t"
		
		"muls	%B2, %B3"			"\n\t"	// (signed) a.high x (signed) b.high  
		"add	%C0, r0"			"\n\t"
		"adc	%D0, r1"			"\n\t"
		
		"mul	%A2, %A3"			"\n\t"	// a.low x b.low
		"add	%A0, r0"			"\n\t"
		"adc	%B0, r1"			"\n\t"
		"adc	%C0, %1"			"\n\t"
		"adc	%D0, %1"			"\n\t"
		
		"mulsu	%B2, %A3"			"\n\t"	// (signed) a.high x b.low
		"sbc	%D0, %1"			"\n\t"	// correct sign
		"add	%B0, r0"			"\n\t"
		"adc	%C0, r1"			"\n\t"
		"adc	%D0, %1"			"\n\t"	// add carry
		
		"mulsu	%B3, %A2"			"\n\t"	// (signed) b.high x a.low
		"sbc	%D0, %1"			"\n\t"	// correct sign
		"add	%B0, r0"			"\n\t"
		"adc	%C0, r1"			"\n\t"
		"adc	%D0, %1"			"\n\t"	// add carry
		
		"clr __zero_reg__"	"\n\t"	// clear r1
		: "+r" (result), "=&r" (help_reg)
		: "a" (a), "a" (b)
	);
	
	return result;
}
