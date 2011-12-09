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
 * $Id: core.hpp 662 2011-12-06 18:35:25Z georgi-g $
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_STM32F2__CORE_HPP
#define XPCC_STM32F2__CORE_HPP

#include <stdint.h>

namespace xpcc
{
	namespace stm32
	{
		typedef void (*InterruptHandler)(void);

		/**
		 * @brief		TODO
		 * @ingroup		stm32
		 */
		class Core
		{
		public:

			/**
			 * Usage:
			 *
			 * For using internal clock (which is 16MHz) call
			 * 		enablePll(PLL_HSI, 8);
			 * 		switchToPll();
			 *
			 *
			 * For using external crystal or oscillator with frequency f call
			 * 		if (enableHSE(HSE_CRYSTAL)){
			 * 			enablePll(PLL_HSE, f/2MHz);
			 * 			switchToPll();
			 * 		}
			 *
			 * For using external oscillator with frequency f call
			 * 		if (enableHSE(HSE_BYPASS)){
			 * 			enablePll(PLL_HSE, f/2MHz);
			 * 			switchToPll();
			 * 		}
			 */
			class Clock{
			public:
				enum HSEConfig{
					HSE_CRYSTAL,
					HSE_BYPASS,
				};

				static bool
				enableHSE(HSEConfig config, uint32_t wait_cycles = 1500);

				enum PLLSource{
					PLL_HSI,
					PLL_HSE,
				};


				/**
				 * PLLSRC 			source select for pll and for plli2s. If you are using HSE you must
				 * 					enable it first.
				 *
				 * PLLM				Division factor for the main PLL (PLL) and audio PLL (PLLI2S) input clock.
				 *
				 * 					The software has to set these bits correctly to ensure that frequency of selected
				 * 					source divided by PLLM is in ranges from 1 to 2 MHz.
				 *
				 * 					VCO input frequency = PLL input clock frequency / PLLM with 2 ≤ PLLM ≤ 63
				 *
				 * PLLN				Main PLL (PLL) multiplication factor for VCO
				 * 					The software has to set these bits correctly to ensure that the VCO output
				 * 					frequency is
				 * 						336 MHz for ST32F4. Core will run at 168 MHz.
				 *						240 MHz for ST32F2. Core will run at 120 MHz.
				 *
				 * 					VCO output frequency = VCO input frequency × PLLN with 64 ≤ PLLN ≤ 432
				 *
				 *
				 *
				 */
				static void
				enablePll(PLLSource source, uint8_t pllM, uint16_t pllN);

				static bool
				switchToPll(uint32_t wait_cycles = 1500);

			};

		};
	}
}

#endif	//  XPCC_STM32F2__CORE_HPP
