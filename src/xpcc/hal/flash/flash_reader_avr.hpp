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
 * $Id: flash_reader_avr.hpp 71 2009-10-12 21:43:00Z dergraaf $
 */
// ----------------------------------------------------------------------------

#ifndef	XPCC__FLASH_READER_HPP

#error	"Don't include this file directly, use \"flash_reader.hpp\" instead!"

#else

#include <string.h>
#include <avr/pgmspace.h>

#define	FLASH(var)				extern const var PROGMEM; const var
#define	EXTERN_FLASH(var)		extern const var PROGMEM
#define	FLASH_STRING(s)			extern const char s[] PROGMEM; const char s[]
#define	EXTERN_FLASH_STRING(s)	extern const char s[] PROGMEM

namespace xpcc
{
	template<typename T, size_t size>
	struct FlashReader
	{
		ALWAYS_INLINE
		static T
		read(const void* p)
		{
			T retval;
			memcpy_P(reinterpret_cast<void *>(&retval), p, size);
			return retval;
		}
	};
	
	template<typename T>
	struct FlashReader<T, 1>
	{
		ALWAYS_INLINE
		static T
		read(const void* p)
		{
			union {
				uint8_t in;
				T out;
			} convert;
			
			convert.in = pgm_read_byte(p);
			return convert.out;
		}
	};
	
	template<typename T>
	struct FlashReader<T, 2>
	{
		ALWAYS_INLINE
		static T
		read(const void* p)
		{
			union {
				uint16_t in;
				T out;
			} convert;
			
			convert.in = pgm_read_word(p);
			return convert.out;
		}
	};
	
#if __AVR_HAVE_LPMX__ && 0
	// FIXME find a working version for this case
	template<typename T>
	struct FlashReader<T, 3>
	{
		ALWAYS_INLINE
		static T
		read(const void* p)
		{
			/*uint16_t addr16 = (uint16_t)(p);
			union {
				uint32_t in;
				T out;
			} convert;
			
			__asm__ (
				"lpm %A0, Z+"   "\n\t"
				"lpm %B0, Z+"   "\n\t"
				"lpm %C0, Z"    "\n\t"
				: "=r" (convert.in), "=z" (addr16)
				: "1" (addr16)
			);
			
			return convert.out;*/
			
			/*uint16_t addr16 = (uint16_t)(p);
			uint32_t result;
			__asm__ (
				"lpm %A0, Z+"   "\n\t"
				"lpm %B0, Z+"   "\n\t"
				"lpm %C0, Z"    "\n\t"
				: "=r" (result), "=z" (addr16)
				: "1" (addr16)
			);
			
			return reinterpret_cast<T&>(result);*/
			
			/*uint16_t addr = (uint16_t)(p);
			struct foo {
				uint8_t a;
				uint16_t b;
			} result;
			
			__asm__ (
				"lpm %A0, Z+"   "\n\t"
				"lpm %B0, Z+"   "\n\t"
				"lpm %C0, Z"    "\n\t"
				: "=r" (result), "=z" (addr)
				: "1" (addr)
			);
			
			return reinterpret_cast<T&>(result);*/
			
			/*uint16_t addr = (uint16_t) p;
			
			char retval[3];
			retval[0] = pgm_read_byte(addr++);
			retval[1] = pgm_read_byte(addr++);
			retval[2] = pgm_read_byte(addr);
			
			return reinterpret_cast<T&>(retval);*/
			
			uint16_t addr16 = (uint16_t)(p);
			union {
				char in[3];
				T out;
			} convert;
			
			__asm__ (
				"lpm %0, Z+"   "\n\t"
				"lpm %1, Z+"   "\n\t"
				"lpm %2, Z"    "\n\t"
				: "=r" (convert.in[0]),
				  "=r" (convert.in[1]),
				  "=r" (convert.in[2]),
				  "=z" (addr16)
				: "3" (addr16)
			);
			
			return convert.out;
		}
	};
#endif
	
	template<typename T>
	struct FlashReader<T, 4>
	{
		ALWAYS_INLINE
		static T
		read(const void* p)
		{
			// we have to work with this union here because the return
			// type of pgm_read_dword() is always uint32_t and the compiler
			// refuses to accept a simple cast to T if T is no build-in type.
			union {
				uint32_t in;
				T out;
			} convert;
			
			convert.in = pgm_read_dword(p);
			return convert.out;
		}
	};
}

#endif	// XPCC__FLASH_READER_HPP
