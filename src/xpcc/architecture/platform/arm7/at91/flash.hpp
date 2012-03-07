// coding: utf-8
// ----------------------------------------------------------------------------
/* Copyright (c) 2011, Roboterclub Aachen e.V.
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
 * $Id: flash.hpp 607 2011-09-13 19:51:03Z dergraaf $
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_AT91__FLASH_HPP
#define XPCC_AT91__FLASH_HPP

#include <stdint.h>
#include <stddef.h>

namespace xpcc
{
	namespace at91
	{
		/**
		 * \brief	Embedded Flash Controller (EFC)
		 * 
		 * The waitstates for the Flash Controller are set during startup 
		 * based on the value of F_CPU.
		 * 
		 * @warning	Not tested yet!
		 * 
		 * \ingroup	at91
		 */
		class Flash
		{
			/**
			 * @brief	Write Flash
			 * 
			 * @param address	Flash address (has to be 32-bit aligned => lower two bits must be zero)
			 * @param buffer	Source
			 * @param len		Number of bytes to write
			 */
			static void
			write(uint32_t address, uint8_t* buffer, size_t len);
			
			/**
			 * @brief	Write Flashpage
			 * 
			 * Address should be aligned to a flash page, otherwise only a partial
			 * page write is done. Nonetheless it must be 32-bit aligned.
			 * 
			 * Pagesize:
			 * - 256 Byte for AT91SAM7S512/256/128
			 * - 128 Byte for AT91SAM7S64/32/321
			 * 
			 * @param address	Flash address
			 * @param buffer	Source
			 * @param len		Number of bytes to write
			 */
			static void
			writePage(uint32_t address, uint8_t* buffer, size_t len);
			
			/**
			 * \brief	Erase the complete Flash
			 * 
			 * Single pages are erased before writing them (see write() and
			 * writePage()).
			 * 
			 * Erase All operation is allowed only if there are no lock bits set.
			 */
			static void
			erase();
			
			static bool
			isLocked(uint32_t address);
			
			/// set the lock state of a flash address/page
			static void
			setLock(uint32_t address, bool lock);
		};
	}
}

#endif // XPCC_AT91__FLASH_HPP
