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
 * $Id: core.cpp 662 2011-12-06 18:35:25Z georgi-g $
 */
// ----------------------------------------------------------------------------

#include "core.hpp"
#include <xpcc/architecture/platform/cortex_m3.hpp>

bool
xpcc::stm32::Core::Clock::enableHSE(HSEConfig config, uint32_t wait_cycles){
	if (config == HSE_BYPASS){
		RCC->CR |= RCC_CR_HSEBYP|RCC_CR_HSEON;
	}
	else{
		RCC->CR |= RCC_CR_HSEON;
	}

	volatile uint32_t t = wait_cycles;
	while (!(RCC->CR & RCC_CR_HSERDY) && --t){
	}

	return RCC->CR & RCC_CR_HSERDY;
}

#ifdef STM32F10X_CL
void
xpcc::stm32::Core::Clock::enablePll(PLLSource source, PLLMul pllMul, uint8_t preDiv1){
	uint32_t tmp = 0;

	tmp |= pllMul;
	tmp |= (source == PLL_HSI_DIV_2)?RCC_CFGR_PLLSRC_HSI_Div2:RCC_CFGR_PLLSRC_PREDIV1;


	RCC->CFGR = tmp; // be careful modifying bit 17
	RCC->CFGR2 |= (preDiv1-1) & RCC_CFGR2_PREDIV1;


	// enable pll
	RCC->CR |= RCC_CR_PLLON;
}

#else

void
xpcc::stm32::Core::Clock::enablePll(PLLSource source, PLLMul pllMul){
	uint32_t tmp = 0;

	tmp |= pllMul;
	tmp |= (source == PLL_HSI_DIV_2)?RCC_CFGR_PLLSRC_HSI_Div2:RCC_CFGR_PLLSRC_HSE;


	RCC->CFGR = tmp;

	// enable pll
	RCC->CR |= RCC_CR_PLLON;
}

#endif // Connectivity Line

bool
xpcc::stm32::Core::Clock::switchToPll(uint32_t wait_cycles){
	volatile uint32_t t = wait_cycles;
	while (!(RCC->CR & RCC_CR_PLLRDY)){
		if (!(--t))
			return false;
	}

	// APB2 72MHz, APB1 36MHz, AHB 72MHz, select pll as source
	RCC->CFGR =
			(RCC->CFGR&0xffff0000) | //try to generate a halfword write
			((RCC_CFGR_PPRE1_DIV2 | RCC_CFGR_PPRE2_DIV1 | RCC_CFGR_HPRE_DIV1 | RCC_CFGR_SW_PLL)&0x0000ffff);

	/* Wait till the main PLL is used as system clock source */
	while ((RCC->CFGR & (uint32_t)RCC_CFGR_SWS ) != RCC_CFGR_SWS_PLL);
	{
	}

	return true;
}

