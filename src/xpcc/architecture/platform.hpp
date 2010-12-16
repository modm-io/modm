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
/**
 * \ingroup 	architecture
 * \defgroup	platform	Platform
 * 
 * Select built-in functions and macros for the active platform
 */

#include "platform/detect.hpp"

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

#define	STRINGIFY(s)	STRINGIFY2(s)
#define	STRINGIFY2(s)	STRINGIFY3(s)
#define	STRINGIFY3(s)	#s

#define	CONCAT(a,b)		CONCAT2(a,b)
#define	CONCAT2(a,b)	CONCAT3(a,b)
#define	CONCAT3(a,b)	a ## b

#ifdef XPCC__COMPILER_GCC
#	define ALWAYS_INLINE  		inline __attribute__((always_inline))
#	define ATTRIBUTE_UNUSED		__attribute__((unused))
#else
#	define ALWAYS_INLINE  inline
#	define ATTRIBUTE_UNUSED
#endif

#ifdef XPCC__CPU_AVR
#	define	MAIN_FUNCTION	int main(void) __attribute__((OS_main)); \
							int main(void)
#else
#	define	MAIN_FUNCTION	int main(void)
#endif

#ifdef XPCC__CPU_AVR
#	include <avr/io.h>
#	include <avr/interrupt.h>

#	include "platform/avr.hpp"
#endif

#include <stdint.h>

#ifdef XPCC__OS_WIN32
#	include "platform/windows.hpp"
#endif

#endif	// !__DOXYGEN__
