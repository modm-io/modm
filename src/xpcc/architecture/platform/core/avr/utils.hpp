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
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_ATXMEGA__UTILS_HPP
#define	XPCC_ATXMEGA__UTILS_HPP

#include <stddef.h>		// for offsetof()
#include <xpcc/architecture/utils.hpp>
#include <xpcc/architecture/platform/avr/xmega.hpp>

namespace xpcc
{
	namespace xmega
	{
		/**
		 * \brief	CCP write helper function
		 * 
		 * This function is written in assembly because of the time critical
		 * operation of writing to the registers.
		 * 
		 * \param	address	A pointer to the address to write to.
		 * \param	value	The value to put in to the register.
		 * 
		 * \ingroup	xmega
		 */
		ALWAYS_INLINE static void
		changeProtectedRegister(volatile uint8_t * address, uint8_t value)
		{
		#ifdef RAMPZ
			RAMPZ = 0;
		#endif
			
			asm volatile (
				"movw r30, %0"		"\n\t"
				"ldi  r16, 0xD8"	"\n\t"
				"out  0x34, r16"	"\n\t"
				"st   Z,  %1"
					:
					: "r" (address), "r" (value)
					: "r16", "r30", "r31"
			);
		}
		
		/**
		 * \brief	Read a calibration byte
		 * 
		 * Example:
		 * \code
		 * ADCA.CALL = readCalibrationByte(offsetof(NVM_PROD_SIGNATURES_t, ADCACAL0));
		 * \endcode
		 * 
		 * \ingroup	xmega
		 */
		uint8_t
		readCalibrationByte(uint8_t index);		
		
		/**
		 * Reset the Xmega via software.
		 *
		 * @ingroup	xmega
		 */
		static inline void
		softwareReset()
		{
			changeProtectedRegister(&RST_CTRL, RST_SWRST_bm);
		}
	}
}

#endif	// XPCC_ATXMEGA__UTILS_HPP
