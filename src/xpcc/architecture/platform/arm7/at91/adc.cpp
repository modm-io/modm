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
 * $Id$
 */
// ----------------------------------------------------------------------------

#include "adc.hpp"
#include "device.h"

// ----------------------------------------------------------------------------
void
xpcc::at91::Adc::enableChannel(uint8_t channel)
{
	AT91C_BASE_ADC->ADC_CHER = (1 << channel);
}

void
xpcc::at91::Adc::disableChannel(uint8_t channel)
{
	AT91C_BASE_ADC->ADC_CHDR = (1 << channel);
}

// ----------------------------------------------------------------------------
void
xpcc::at91::Adc::startConversion()
{
	AT91C_BASE_ADC->ADC_CR = AT91C_ADC_START;
}

// ----------------------------------------------------------------------------
bool
xpcc::at91::Adc::isFinished(uint8_t channel)
{
	return (AT91C_BASE_ADC->ADC_SR & (1 << channel));
}

bool
xpcc::at91::Adc::isFinished()
{
	// Check all enabled channels
	return ((AT91C_BASE_ADC->ADC_SR & AT91C_BASE_ADC->ADC_CHSR) == AT91C_BASE_ADC->ADC_CHSR);
}

// ----------------------------------------------------------------------------
uint32_t
xpcc::at91::Adc::getValue(uint8_t channel)
{
	// ADC_CDR0..7 all located right after each other in the memory
	return *(&AT91C_BASE_ADC->ADC_CDR0 + channel);
}

uint32_t
xpcc::at91::Adc::getLastConvertedValue()
{
	return AT91C_BASE_ADC->ADC_LCDR;
}

// ----------------------------------------------------------------------------
void
xpcc::at91::Adc::enableInterrupt(uint8_t channel)
{
	AT91C_BASE_ADC->ADC_IER = (1 << channel);
}

void
xpcc::at91::Adc::disableInterrupt(uint8_t channel)
{
	AT91C_BASE_ADC->ADC_IDR = (1 << channel);
}

// ----------------------------------------------------------------------------
void
xpcc::at91::Adc::registerInterruptHandler(Aic::Handler function)
{
	Aic::registerHandler(AT91C_ID_ADC, function);
	Aic::enableInterrupt(AT91C_ID_ADC);
}

// ----------------------------------------------------------------------------
void
xpcc::at91::Adc::removeInterruptHandler()
{
	Aic::disableInterrupt(AT91C_ID_ADC);
}
