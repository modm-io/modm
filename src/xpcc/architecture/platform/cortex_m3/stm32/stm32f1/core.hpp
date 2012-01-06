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
			 * Clock management.
			 * 
			 * For using an external crystal with 8 MHz use:
			 * \code 
			 * if (enableHSE(HSE_CRYSTAL))
			 * {
			 *     enablePll(PLL_HSE, PLL_MUL_9);
			 *     switchToPll();
			 * }
			 * \endcode
			 *
			 * For using an external oscillator with 8 MHz use:
			 * \code
			 * if (enableHSE(HSE_BYPASS))
			 * {
			 *     enablePll(PLL_HSE, PLL_MUL_9);
			 *     switchToPll();
			 * }
			 * \endcode
			 * 
			 * For using a 25 MHz crystal on a Connectivity Line device:
			 * if (enableHSE(HSE_CRYSTAL))
			 * {
			 *     enablePll2(5, PLL2_MUL_8);	// 40 MHz = (25 MHz / 5) * 8
			 *     enablePll(PLL_PREDIV1, PLL_MUL_9, PREDIV1_PLL2, 5);	// 72 MHz = (40 MHz / 5) * 9
			 *     switchToPll();
			 * }
			 * \endcode
			 */
			class Clock
			{
			public:
				enum HseConfig
				{
					HSE_CRYSTAL,
					HSE_BYPASS,
				};

				static bool
				enableHse(HseConfig config, uint32_t waitCycles = 1500);

#ifdef STM32F10X_CL
				enum PllSource
				{
					PLL_HSI_DIV_2,		///< Can't be used yet!
					PLL_PREDIV1,
				};
				
				enum PreDiv1Source
				{
					PREDIV1_HSE,
					PREDIV1_PLL2,
				};
				
				enum PllMul
				{
					PLL_MUL_4 = RCC_CFGR_PLLMULL4,
					PLL_MUL_5 = RCC_CFGR_PLLMULL5,
					PLL_MUL_6 = RCC_CFGR_PLLMULL6,
					PLL_MUL_7 = RCC_CFGR_PLLMULL7,
					PLL_MUL_8 = RCC_CFGR_PLLMULL8,
					PLL_MUL_9 = RCC_CFGR_PLLMULL9,
					PLL_MUL_6_5 = RCC_CFGR_PLLMULL6_5,		///< Frequency * 6.5
				};
				
				enum Pll2Mul
				{
					PLL2_MUL_8 = RCC_CFGR2_PLL2MUL8,
					PLL2_MUL_9 = RCC_CFGR2_PLL2MUL9,
					PLL2_MUL_10 = RCC_CFGR2_PLL2MUL10,
					PLL2_MUL_11 = RCC_CFGR2_PLL2MUL11,
					PLL2_MUL_12 = RCC_CFGR2_PLL2MUL12,
					PLL2_MUL_13 = RCC_CFGR2_PLL2MUL13,
					PLL2_MUL_14 = RCC_CFGR2_PLL2MUL14,
					PLL2_MUL_16 = RCC_CFGR2_PLL2MUL16,
					PLL2_MUL_20 = RCC_CFGR2_PLL2MUL20,
				};
				
				/**
				 * Enable PLL2.
				 * 
				 * \param	div
				 * 		Divisior for PLL2 and PLL3. Must be in
				 * 		range 1 <= div <= 16.
				 * 
				 * \param	mul
				 * 		PLL2 multiplication factor.
				 */
				static void
				enablePll2(uint32_t div, Pll2Mul mul);
				
				/**
				 * Enable PLL.
				 * 
				 * \param	source
				 * 		Source select for pll and for plli2s. If you are using
				 * 		HSE you must enable it first (see enableHse()).
				 *
				 * \param	pllMull
				 * 		PLL multiplication factor.
				 *		Set multiplication factor so that output of the PLL
				 *		is 72MHz.
				 * 		If PREDIV1 is used as source:
				 * 		"PLL output = pllMul * frequency of PLL source / preDiv1"
				 * 
				 * \param	preDivFactor
				 * 		Must be within in range 1 to 16.
				 * 
				 * \warning	You will see, it is not possible to run the core on
				 *			full frequency if HSI is used, but the API does not
				 *			handle this case yet. Device will be configured
				 * 			as running on 72MHz.
				 */
				static void
				enablePll(PllSource source, PllMul pllMul,
						PreDiv1Source preDivSource = PREDIV1_HSE, uint32_t preDivFactor = 1);

#else
				enum PllSource
				{
					PLL_HSI_DIV_2,		///< Can't be used yet!
					PLL_HSE,//!< PLL_HSE
				};

				enum PllMul
				{
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

				/**
				 * Enable PLL.
				 * 
				 * \param	source
				 * 		Source select for PLL and for plli2s. If you are using
				 * 		HSE you must enable it first (see enableHse()).
				 *
				 * \param	pllMull
				 * 		PLL multiplication factor.
				 *		Set multiplication factor so that the output of the
				 *		PLL is 72MHz
				 * 		PLL output = pllMul * frequency of PLL source
				 * 
				 * \warning	You will see, it is not possible to run the core on
				 *			full frequency if HSI is used, but the API does not
				 *			handle this case yet. Device will be configured
				 * 			as running on 72MHz.
				 */
				static void
				enablePll(PllSource source, PllMul pllMul);
#endif
				
				static bool
				switchToPll(uint32_t waitCycles = 1500);

			};

		};
	}
}

#endif	//  XPCC_STM32F1__CORE_HPP
