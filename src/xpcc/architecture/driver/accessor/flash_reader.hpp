/*
 * Copyright (c) 2009-2011, Fabian Greif
 * Copyright (c) 2009, Martin Rosekeit
 * Copyright (c) 2012, 2015-2016, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
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
