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
 * $Id: core.hpp 686 2012-01-10 15:31:26Z dergraaf $
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_STM32__CORE_HPP
#define XPCC_STM32__CORE_HPP

#include <stdint.h>

namespace xpcc
{
	namespace stm32
	{
		typedef void (*InterruptHandler)(void);
		
		class Core
		{
		public:
			/**
			 * Get unique device id (96-bits)
			 * 
			 * @param offset	0..2
			 * @return	32-bit of the unique id
			 */
			uint32_t
			getUniqueId(uint8_t offset)
			{
#if defined(STM32F2XX) || defined(STM32F4XX)
				uint32_t *baseaddress = (uint32_t*) 0x1FFF7A10;
#else
				uint32_t *baseaddress = (uint32_t*) 0x1FFFF7E8;
#endif
				return *(baseaddress + offset);
			}
		};
	}
}

#endif	// XPCC_STM32__CORE_HPP
