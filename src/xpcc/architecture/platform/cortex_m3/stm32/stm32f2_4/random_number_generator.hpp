// coding: utf-8
// ----------------------------------------------------------------------------
/* Copyright (c) 2012, Roboterclub Aachen e.V.
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

#ifndef XPCC_STM32F2_4__RANDOM_NUMBER_GENERATOR_HPP
#define XPCC_STM32F2_4__RANDOM_NUMBER_GENERATOR_HPP

#include <stdint.h>
#include "../../device.h"

namespace xpcc
{
	namespace stm32
	{
		/**
		 * Random Number Generator
		 * 
		 * TODO check error status
		 * 
		 * \author	Fabian Greif
		 * \ingroup	stm32
		 */
		class RandomNumberGenerator
		{
		public:
			static void
			enable()
			{
				RNG->CR = RNG_CR_RNGEN;
			}
			
			/**
			 * Check if a new random number is ready
			 * 
			 * \return	\c true if a new random number has been generated
			 * 			and can be read via getValue()
			 */
			static bool
			isReady()
			{
				return (RNG->SR & RNG_SR_DRDY);
			}
			
			/**
			 * Read the generated 32-bit random number
			 * 
			 * Once read isReady() will return \c false until a new number
			 * has be generated.
			 */
			static uint32_t
			getValue()
			{
				return RNG->DR;
			}
		};
	}
}

#endif	//  XPCC_STM32F2_4__RANDOM_NUMBER_GENERATOR_HPP
