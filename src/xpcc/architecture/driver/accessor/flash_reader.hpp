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

#ifndef	XPCC__FLASH_READER_HPP
#define	XPCC__FLASH_READER_HPP

#include <xpcc/architecture/utils.hpp>

#ifdef	__DOXYGEN__

	/** 
	 * \brief	Define a flash variable
	 * \ingroup	accessor
	 */
	#define	FLASH_STORAGE(var)

	/**
	 * \brief	Define a flash string
	 * \ingroup	accessor
	 */
	#define	FLASH_STORAGE_STRING(s)

	/**
	 * \brief	Declare a flash string
	 * \ingroup	accessor
	 */
	#define	EXTERN_FLASH_STORAGE_STRING(s)

#else // !__DOXYGEN__

	#ifdef XPCC__CPU_AVR
	
		#include "flash_reader_avr_impl.hpp"
	
	#else
	
		#include <cstring>
		
		// A simple implementation for all targets that don't have a
		// strict separation between Flash and RAM like the AVRs does.
		
		// 'const' implies internal linkage in C++ if not explicit declared
		// as 'extern'.
		#define	FLASH_STORAGE(var)				extern const var; const var
		#define	EXTERN_FLASH_STORAGE(var)		extern const var

		#define	FLASH_STORAGE_STRING(s)			extern const char s[]; const char s[]
		#define	EXTERN_FLASH_STORAGE_STRING(s)	extern const char s[]

		#define	INLINE_FLASH_STORAGE_STRING(s)	((const char *)(s))
		#define PSTR(s) ((const char *)(s))

		namespace xpcc
		{
			template<typename T, std::size_t size>
			struct FlashReader
			{
				xpcc_always_inline
				static T
				read(const T* p)
				{
					return *p;
				}
			};
		}
		
	#endif

#endif

#endif	// XPCC__FLASH_READER_HPP
