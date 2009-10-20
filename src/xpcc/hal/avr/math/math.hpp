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

#ifndef	AVR__MATH_HPP
#define	AVR__MATH_HPP

#include <stdint.h>

namespace avr
{

/**
 * \brief	Fast and short 32 bits sqrt routine
 * 
 * Quadratwurzel basierend auf einer Implementierung von Ruud v Gessel,
 * die zusammen mit avr-gcc verwendet werden kann. Je nach Algorithmus wird
 * das Ergebnis zum Nächsten gerundet oder abgerundet. Abrunden ist dann
 * angesagt, wenn die Wurzel aus einer großen Eingabe wie 0xffffffff zu
 * ziehen ist, da bei Aufrunden hier das Ergebnis zu 0 überläuft.
 * 
 * Die Ausführungszeit ist maximal 310 Ticks (inclusive CALL+RET)
 * 
 * \see		http://www.mikrocontroller.net/articles/AVR_Arithmetik#avr-gcc_Implementierung_.2832_Bit.29
 * \see		http://members.chello.nl/j.beentjes3/Ruud/sqrt32avr.htm
 */
extern "C" uint16_t
sqrt32_round(uint32_t);

/**
 * 
 */
extern "C" uint16_t
sqrt32_floor(uint32_t);

/**
 * \brief	unsigned 16bit x 16bit = 32bit multiplication
 * 
 * \see		AVR201
 */
inline uint32_t
mul32(uint16_t a, uint16_t b)
{
	uint32_t result;
	asm(
		"mul %B1,%B2"      "\n\t"		// a.high x b.high
		"movw %C0,r0"      "\n\t"
		"mul %A1,%A2"      "\n\t"		// a.low x b.low
		"movw %A0,r0"      "\n\t"
		"mul %B1,%A2"      "\n\t"		// a.high x b.low
		"add %B0,r0"      "\n\t"
		"adc %C0,r1"      "\n\t"
		"clr __zero_reg__"    "\n\t"	// reset r1
		"adc %D0,__zero_reg__"  "\n\t"	// add carry
		"mul %A1,%B2"      "\n\t"		// a.low x b.high
		"add %B0,r0"      "\n\t"
		"adc %C0,r1"      "\n\t"
		"clr __zero_reg__"    "\n\t"	// reset r1
		"adc %D0,__zero_reg__"  "\n\t"	// add carry
		: "=&r" (result)
		: "r" (a), "r" (b)
	);
	
	return result;
}

/**
 * \brief	signed 16bit x 16bit = 32bit multiplication
 * 
 * \see		AVR201
 */
inline int32_t
mul32(int16_t a, int16_t b)
{
	int32_t result;
	int8_t help_reg;
	
	// %1 is used like as __zero_reg__ because the orignal one (r1) contains
	// the result of the mul-operations
	asm(
		"clr %1"		"\n\t"
		
		"muls %B2,%B3"   "\n\t"			// (signed) a.high x (signed) b.high  
		"movw %C0,r0"   "\n\t"
		
		"mul %A2,%A3"  "\n\t"			// a.low x b.low
		"movw %A0,r0"   "\n\t"
		
		"mulsu %B2,%A3""\n\t"			// (signed) a.high x b.low
		"sbc %D0,%1"  "\n\t"			// correct sign
		"add %B0,r0"  "\n\t"
		"adc %C0,r1"  "\n\t"
		"adc %D0,%1"  "\n\t"			// add carry
		
		"mulsu %B3,%A2" "\n\t"			// (signed) b.high x a.low
		"sbc %D0,%1"  "\n\t"			// correct sign
		"add %B0,r0"    "\n\t"
		"adc %C0,r1"  "\n\t"
		"adc %D0,%1"  "\n\t"			// add carry
		
		"clr __zero_reg__"    "\n\t"	// clear r1
		: "=&r" (result), "=&r" (help_reg)
		: "a" (a), "a" (b)
	);
	
	return result;
}

/**
 * \brief	Signed multiply accumulate of two 16bits numbers with
 * 			a 32bits result
 * 
 * \verbatim
 * result += a * b;
 * \endverbatim
 * 
 * \see		AVR201
 */
inline int32_t
mac32(int32_t result, int16_t a, int16_t b)
{
	int8_t help_reg;
	
	// %1 is used like as __zero_reg__ because the orignal one (r1) contains
	// the result of the mul-operations
	asm(
		"clr %1"		"\n\t"
		
		"muls %B2,%B3"   "\n\t"			// (signed) a.high x (signed) b.high  
		"add %C0,r0"		"\n\t"
		"adc %D0,r1"		"\n\t"
		
		"mul %A2,%A3"  "\n\t"			// a.low x b.low
		"add %A0,r0"		"\n\t"
		"adc %B0,r1"		"\n\t"
		"adc %C0,%1"		"\n\t"
		"adc %D0,%1"		"\n\t"
		
		"mulsu %B2,%A3""\n\t"			// (signed) a.high x b.low
		"sbc %D0,%1"  "\n\t"			// correct sign
		"add %B0,r0"  "\n\t"
		"adc %C0,r1"  "\n\t"
		"adc %D0,%1"  "\n\t"			// add carry
		
		"mulsu %B3,%A2" "\n\t"			// (signed) b.high x a.low
		"sbc %D0,%1"  "\n\t"			// correct sign
		"add %B0,r0"    "\n\t"
		"adc %C0,r1"  "\n\t"
		"adc %D0,%1"  "\n\t"			// add carry
		
		"clr __zero_reg__"    "\n\t"	// clear r1
		: "+r" (result), "=&r" (help_reg)
		: "a" (a), "a" (b)
	);
	
	return result;
}

}

#endif	// AVR__MATH_HPP
