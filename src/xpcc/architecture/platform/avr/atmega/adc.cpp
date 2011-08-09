// coding: utf-8
// ----------------------------------------------------------------------------
/* Copyright (c) 2009, Roboterclub Aachen e.V.
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

#include <avr/io.h>

#include "adc.hpp"

// ----------------------------------------------------------------------------
void
xpcc::atmega::Adc::initialize(Reference referenceVoltage, Prescaler prescaler)
{
	ADMUX = referenceVoltage & 0xc0;
	ADCSRA = (1 << ADEN) | (prescaler & 0x07);
}

uint8_t
xpcc::atmega::Adc::getChannel()
{	
	return ADMUX & 0x0f;
}

// ----------------------------------------------------------------------------
uint16_t
xpcc::atmega::Adc::readChannel(uint8_t channel)
{
	if (channel > 8)
		return 0;
	
	startConversion(channel);
	
	while (!isFinished()) {
		// wait until the conversion is finished
	}
	
	return getValue();
}

// ----------------------------------------------------------------------------
void
xpcc::atmega::Adc::startConversion(uint8_t channel)
{
	if (channel > 8)
		return;
	
	// select channel
	ADMUX = (ADMUX & 0xe0) | channel;
	
	// clear interrupt flag
	ADCSRA |= (1 << ADIF);
	
	// start conversion
	ADCSRA |= (1 << ADSC);
}

bool
xpcc::atmega::Adc::isFinished()
{
	return (ADCSRA & (1 << ADIF));
}

uint16_t
xpcc::atmega::Adc::getValue()
{
	return ADC;
}
