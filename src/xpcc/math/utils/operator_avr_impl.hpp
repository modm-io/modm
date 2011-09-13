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
	#error	"Don't include this file directly, use 'operator.hpp' instead!"
#endif

inline uint32_t
xpcc::math::mul(uint16_t a, uint16_t b)
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
xpcc::math::mul(int16_t a, int16_t b)
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
xpcc::math::mac(int32_t result, int16_t a, int16_t b)
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
