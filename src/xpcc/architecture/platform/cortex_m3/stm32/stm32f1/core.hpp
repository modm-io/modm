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

#ifndef XPCC_STM32F1__CORE_HPP
#define XPCC_STM32F1__CORE_HPP

#include <stdint.h>
#include "../device.h"

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

#ifdef STM32F10X_CL
				enum PLLSource{
					PLL_HSI_DIV_2,
					PLL_PREDIV1,
				};

				enum PLLMul{
					PLL_MUL_4 = RCC_CFGR_PLLMULL4,
					PLL_MUL_5 = RCC_CFGR_PLLMULL5,
					PLL_MUL_6 = RCC_CFGR_PLLMULL6,
					PLL_MUL_7 = RCC_CFGR_PLLMULL7,
					PLL_MUL_8 = RCC_CFGR_PLLMULL8,
					PLL_MUL_9 = RCC_CFGR_PLLMULL9,
					PLL_MUL_6_5 = RCC_CFGR_PLLMULL6_5,
				};

				/*
				 * PLLSRC 			source select for pll and for plli2s. If you are using HSE you must
				 * 					enable it first.
				 *
				 * PLLMul			PLL multiplication factor
				 *
				 * 					Set multiplication factor the way, that output of Pll is 72MHz
				 * 					If PREDIV1 is used as source:
				 * 					PLL output = pllMul * frequency of Pll source / preDiv1
				 *
				 * 					If HSI is used as source
				 * 					PLL output = pllMul * frequency of Pll source
				 * 					You will see, it is not possible to run the core on full frequency if
				 * 					HSI is used, but Api does not handle this case yet. Device will be configured
				 * 					as running on 72MHz.
				 *
				 * preDiv1			Must be within in range 1 to 16.
				 */
				static void
				enablePll(PLLSource source, PLLMul pllMul, uint8_t preDiv1);

#else
				enum PLLSource{
					PLL_HSI_DIV_2,
					PLL_HSE,
				};

				enum PLLMul{
					PLL_MUL_2 = RCC_CFGR_PLLMULL2,
					PLL_MUL_3 = RCC_CFGR_PLLMULL3,
					PLL_MUL_4 = RCC_CFGR_PLLMULL4,
					PLL_MUL_5 = RCC_CFGR_PLLMULL5,
					PLL_MUL_6 = RCC_CFGR_PLLMULL6,
					PLL_MUL_7 = RCC_CFGR_PLLMULL7,
					PLL_MUL_8 = RCC_CFGR_PLLMULL8,
					PLL_MUL_9 = RCC_CFGR_PLLMULL9,
					PLL_MUL_10 = RCC_CFGR_PLLMULL10,
					PLL_MUL_11 = RCC_CFGR_PLLMULL11,
					PLL_MUL_12 = RCC_CFGR_PLLMULL12,
					PLL_MUL_13 = RCC_CFGR_PLLMULL13,
					PLL_MUL_14 = RCC_CFGR_PLLMULL14,
					PLL_MUL_15 = RCC_CFGR_PLLMULL15,
					PLL_MUL_16 = RCC_CFGR_PLLMULL16,
				};

				/*
				 * PLLSRC 			source select for pll and for plli2s. If you are using HSE you must
				 * 					enable it first.
				 *
				 * pllMul			PLL multiplication factor.
				 *
				 * 					Set multiplication factor the way, that output of Pll is 72MHz
				 * 					PLL output = pllMul * frequency of Pll source
				 *
				 * 					You will see, it is not possible to run the core on full frequency if
				 * 					HSI is used, but Api does not handle this case yet. Device will be configured
				 * 					as running on 72MHz.
				 */
				static void
				enablePll(PLLSource source, PLLMul pllMul);
#endif

				static bool
				switchToPll(uint32_t wait_cycles = 1500);

			};

		};
	}
}

#endif	//  XPCC_STM32F1__CORE_HPP
