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
 * $Id: flash_reader.hpp 71 2009-10-12 21:43:00Z dergraaf $
 */
// ----------------------------------------------------------------------------

#ifndef	XPCC__FLASH_READER_HPP
#define	XPCC__FLASH_READER_HPP

#ifdef	__DOXYGEN__
/// \ingroup	hal
/// \brief		Define a flash variable
#define	FLASH(var)

/// \ingroup	hal
/// \brief		Define a flash string
#define	FLASH_STRING(s)

/// \ingroup	hal
/// \brief		Declare a flash string
#define	EXTERN_FLASH_STRING(s)

#else

#ifdef	__AVR__

	#include "flash_reader_avr.hpp"

#else // !__DOXYGEN__

#include <string.h>
#include <xpcc/utils/macros.hpp>

#define	FLASH(var)				const var
#define	EXTERN_FLASH(var)		extern const var

#define	FLASH_STRING(s)			const char s[]
#define	EXTERN_FLASH_STRING(s)	extern const char s[]

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
			memcpy(reinterpret_cast<void *>(&retval), p, size);
			return retval;
		}
	};
}

#endif	// __AVR__

#endif	// __DOXYGEN__

#endif	// XPCC__FLASH_READER_HPP
