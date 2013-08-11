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
 */
// ----------------------------------------------------------------------------

#ifndef XPCC__UTILS_HPP
#define XPCC__UTILS_HPP

#include "detect.hpp"

#ifdef __DOXYGEN__

	/**
	 * \brief	Main function definition for microcontroller projects
	 * 
	 * Inhibits some stack operations at the beginning of main for avr-gcc. May
	 * save up a few bytes of stack memory.
	 * 
	 * Typical structure of an microcontroller program:
	 * \code
	 * #include <xpcc/architecture/platform.hpp>
	 * 
	 * MAIN_FUNCTION
	 * {
	 *    ...
	 *    
	 *    while (1)
	 *    {
	 *        ...
	 *    }
	 * }
	 * \endcode
	 * 
	 * \ingroup	platform
	 */
	#define	MAIN_FUNCTION
	
	/**
	 * \brief	Force inlining
	 * 
	 * Macro to force inlining on functions if needed. Compiling with -Os  does not
	 * always inline them when declared only \c inline.
	 * 
	 * \ingroup	platform
	 */
	#define	ALWAYS_INLINE
	
	/**
	 * \brief	Convert the argument into a C-String
	 * \ingroup	platform
	 */
	#define	STRINGIFY(s)	#s
	
	/**
	 * \brief	Concatenate the two arguments
	 * \ingroup	platform
	 */
	#define	CONCAT(a,b)		a ## b

#else // !__DOXYGEN__

	#define	STRINGIFY(s)	STRINGIFY_(s)
	#define	STRINGIFY_(s)	STRINGIFY__(s)
	#define	STRINGIFY__(s)	#s
	
	#define	CONCAT(a,b)		CONCAT_(a,b)
	#define	CONCAT_(a,b)	CONCAT__(a,b)
	#define	CONCAT__(a,b)	a ## b
	
	#define	CONCAT3(a,b,c)		CONCAT3_(a,b,c)
	#define	CONCAT3_(a,b,c)		CONCAT3__(a,b,c)
	#define	CONCAT3__(a,b,c)	a ## b ## c
	
	#define	CONCAT4(a,b,c,d)	CONCAT4_(a,b,c,d)
	#define	CONCAT4_(a,b,c,d)	CONCAT4__(a,b,c,d)
	#define	CONCAT4__(a,b,c,d)	a ## b ## c ## d

	#define	CONCAT5(a,b,c,d,e)		CONCAT5_(a,b,c,d,e)
	#define	CONCAT5_(a,b,c,d,e)		CONCAT5__(a,b,c,d,e)
	#define	CONCAT5__(a,b,c,d,e)	a ## b ## c ## d ## e

	#ifdef XPCC__COMPILER_GCC
	#	define ALWAYS_INLINE  		inline __attribute__((always_inline))
	#	define ATTRIBUTE_UNUSED		__attribute__((unused))
	#	define ATTRIBUTE_WEAK		__attribute__ ((weak))
	#	define ATTRIBUTE_ALIGNED(n)	__attribute__((aligned(n)))
	
	// see http://dbp-consulting.com/tutorials/StrictAliasing.html
	#	define ATTRIBUTE_MAY_ALIAS	__attribute__((__may_alias__))	
	#else
	#	define ALWAYS_INLINE  		inline
	#	define ATTRIBUTE_UNUSED
	#	define ATTRIBUTE_WEAK
	#	define ATTRIBUTE_ALIGNED(n)
	#	define ATTRIBUTE_MAY_ALIAS
	#endif
	
	#ifdef XPCC__CPU_AVR
	#	define	MAIN_FUNCTION	int main(void) __attribute__((OS_main)); \
								int main(void)
	#elif defined XPCC__CPU_HOSTED
	#	define 	MAIN_FUNCTION	int main( int argc, char* argv[] )
	#else
	#	define	MAIN_FUNCTION	int main(void)
	#endif

	#define XPCC__ARRAY_SIZE(x)	(sizeof(x) / sizeof(x[0]))

	/**
	 * Makes it possible to use enum classes as bit flags.
	 * This disables some typesafety introduced by the enum class feature
	 * but it should still be more typesafe than using the traditional
	 * enum type.
	 * Use (for public enum classes after class definition) like this:
	 * ENUM_CLASS_FLAG(Adc{{ id }}::InterruptFlag)
	 */
	#define ENUM_CLASS_FLAG(name) \
	inline name operator|(name a, name b) \
	{return static_cast<name>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));} \
	inline uint32_t operator&(name a, name b) \
	{return (static_cast<uint32_t>(a) & static_cast<uint32_t>(b));} \
	inline uint32_t operator&(uint32_t a, name b) \
	{return ((a) & static_cast<uint32_t>(b));} \
	inline uint32_t operator&(name a, uint32_t b) \
	{return (static_cast<uint32_t>(a) & (b));}


#endif	// !__DOXYGEN__

#endif	// XPCC__UTILS_HPP
