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
 * $Id: pwm.cpp 607 2011-09-13 19:51:03Z dergraaf $
 */
// ----------------------------------------------------------------------------

#include "pwm.hpp"
#include "device.h"

// ----------------------------------------------------------------------------
void
xpcc::at91::Pwm::enable()
{
	AT91C_BASE_PMC->PMC_PCER = (1 << AT91C_ID_PWMC);
}

void
xpcc::at91::Pwm::disable()
{
	AT91C_BASE_PMC->PMC_PCDR = (1 << AT91C_ID_PWMC);
}

// ----------------------------------------------------------------------------
void
xpcc::at91::Pwm::configureClockGenerator(Clock clock, Prescaler prescaler,
		uint8_t divideFactor)
{
	if (clock == CLOCK_A)
	{
		uint32_t value = AT91C_BASE_PWMC->PWMC_MR & ~(AT91C_PWMC_PREA | AT91C_PWMC_DIVA);
		AT91C_BASE_PWMC->PWMC_MR = value |
				(prescaler << 8) |
				(divideFactor << 0);
	}
	else if (clock == CLOCK_B)
	{
		uint32_t value = AT91C_BASE_PWMC->PWMC_MR & ~(AT91C_PWMC_PREB | AT91C_PWMC_DIVB);
		AT91C_BASE_PWMC->PWMC_MR = value |
				(prescaler << 24) |
				(divideFactor << 16);
	}
}

// ----------------------------------------------------------------------------
void
xpcc::at91::Pwm::enableChannels(uint32_t channel)
{
	AT91C_BASE_PWMC->PWMC_ENA = channel;
}

void
xpcc::at91::Pwm::disableChannels(uint32_t channel)
{
	AT91C_BASE_PWMC->PWMC_DIS = channel;
}

// ----------------------------------------------------------------------------
void
xpcc::at91::Pwm::registerInterruptHandler(Aic::Handler function)
{
	Aic::registerHandler(AT91C_ID_PWMC, function);
	Aic::enableInterrupt(AT91C_ID_PWMC);
}

// ----------------------------------------------------------------------------
// CHANNEL
// ----------------------------------------------------------------------------

xpcc::at91::PwmChannel::PwmChannel(uint32_t channel) :
		base(&AT91C_BASE_PWMC->PWMC_CH[channel])
{
}

// ----------------------------------------------------------------------------
void
xpcc::at91::PwmChannel::configure(Clock clock, uint32_t period, uint32_t configurationMask)
{
	base->PWMC_CMR = clock | configurationMask;
	base->PWMC_CPRDR = period;
}
