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

#include "../gpio.hpp"
#include "../device.h"

#include "adc_4.hpp"

#include <xpcc_config.hpp>
#include <xpcc/architecture/driver/delay.hpp>	// xpcc::delay_us

using namespace xpcc::stm32;

#if defined(STM32F3XX)
/* id < 4 */


void
Adc4::initialize(const ClockMode clk, const Prescaler pre,
		const CalibrationMode cal, const bool blocking)
{
	uint32_t tmp = 0;

	// enable clock
	RCC->AHBENR |= RCC_AHBENR_ADC34EN;

	// reset ADC
	// FIXME: not a good idea since you can only reset both
	// ADC1/ADC2 or ADC3/ADC4 at once ....
	// RCC->APB2RSTR |=  RCC_APB2RSTR_ADC4RST;
	// RCC->APB2RSTR &= ~RCC_APB2RSTR_ADC4RST;

	// set ADC "analog" clock source
	if (clk != ClockMode::DoNotChange) {
		if (clk == ClockMode::Asynchronous) {
			setPrescaler(pre);
		}
		tmp  =  ADC3_4->CCR;
		tmp &= ~ADC12_CCR_CKMODE;
		tmp |=  static_cast<uint32_t>(clk);
		ADC3_4->CCR = tmp;
	}

	// enable regulator
	ADC4->CR &= ~ADC_CR_ADVREGEN;
	ADC4->CR |= static_cast<uint32_t>(VoltageRegulatorState::Enabled);
	xpcc::delay_us(10);	// FIXME: this is ugly -> find better solution

	calibrate(cal, true);	// blocking calibration

	ADC4->CR |= ADC_CR_ADEN;
	if (blocking) {
		while(!isReady());
		resetInterruptFlags(InterruptFlag::Ready);
	}
}

#endif