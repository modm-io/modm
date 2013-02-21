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

#include <xpcc/architecture/platform/cortex_m3/stm32.hpp>
#include "clock.hpp"

// ----------------------------------------------------------------------------
bool
xpcc::stm32::Clock::enableHse(HseConfig config, uint32_t waitCycles)
{
	if (config == HseConfig::HSE_BYPASS) {
		RCC->CR |= RCC_CR_HSEBYP | RCC_CR_HSEON;
	}
	else {
		RCC->CR |= RCC_CR_HSEON;
	}

	uint32_t t = waitCycles;
	while (!(RCC->CR & RCC_CR_HSERDY) && --t) {
	}

	return (RCC->CR & RCC_CR_HSERDY);
}

// ----------------------------------------------------------------------------
void
xpcc::stm32::Clock::enablePll(PllSource source, PllMul pllM)
{
#if defined(STM32F3XX)
#	warning "this is not tested yet"

	uint32_t tmp = 0;
	
	// Read reserved and read-only values and clear all other values
	tmp |= RCC->CFGR & (
			(1 << 31) | (1 << 30) | (1 << 29) | (1 << 28) | (1 << 27) |
			(1 << 15) | (1 << 14) |
			(1 <<  3) | (1 <<  2));
	
	// PLLSRC source for pll
	tmp |= (source == PllSource::PLL_HSI) ? RCC_CFGR_PLLSRC_HSI_Div2
			: RCC_CFGR_PLLSRC_PREDIV1;
	
	// PLLMUL = factor is user defined
	tmp |=  static_cast<uint32_t>(pllM);
	
	RCC->CFGR = tmp;
	
	// enable pll
	RCC->CR |= RCC_CR_PLLON;
#else
#	error "This file is not supposed to be used with the given CPU (only STM32F3xx)"
#endif
}

// ----------------------------------------------------------------------------
bool
xpcc::stm32::Clock::switchToPll(uint32_t waitCycles)
{
#if defined(STM32F3XX)
#	warning "this is not tested yet"
	uint32_t t = waitCycles;
	while (!(RCC->CR & RCC_CR_PLLRDY)) {
		if (!(--t)) {
			return false;
		}
	}

	// AHB  = 72 / 1 = 72 MHz,
	//
	RCC->CFGR =
			(RCC->CFGR & 0xffff0000) | // Try to generate a halfword write
			((
					RCC_CFGR_PPRE1_DIV2 | /* APB1   = 72 / 2 = 36 MHz */
					RCC_CFGR_PPRE2_DIV1 | /* APB2   = 72 / 1 = 72 MHz */
					RCC_CFGR_HPRE_DIV1  | /* SYSCLK = 72 / 1 = 72 MHz */
					RCC_CFGR_SW_PLL)      /* select PLL as source */
					& 0x0000ffff);
	
	// Wait till the main PLL is used as system clock source
	while ((RCC->CFGR & (uint32_t) RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL);
	{
	}

	return true;
#else
#	error "This file is not supposed to be used with the given CPU (only STM32F3xx)"
#endif
}
