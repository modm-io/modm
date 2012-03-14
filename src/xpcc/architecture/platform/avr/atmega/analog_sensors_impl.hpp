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

#ifndef XPCC_ATMEGA__ANALOG_SENSORS_HPP
	#error  "Don't include this file directly, use 'analog_sensors.hpp' instead!"
#endif

// ----------------------------------------------------------------------------
template < typename ADConv, uint8_t CHANNELS, uint8_t SAMPLES >
ADConv xpcc::atmega::AnalogSensors<ADConv,CHANNELS,SAMPLES>::adc;

template < typename ADConv, uint8_t CHANNELS, uint8_t SAMPLES >
bool xpcc::atmega::AnalogSensors<ADConv,CHANNELS,SAMPLES>::newData;

template < typename ADConv, uint8_t CHANNELS, uint8_t SAMPLES >
uint8_t* xpcc::atmega::AnalogSensors<ADConv,CHANNELS,SAMPLES>::map;

template < typename ADConv, uint8_t CHANNELS, uint8_t SAMPLES >
uint16_t xpcc::atmega::AnalogSensors<ADConv,CHANNELS,SAMPLES>::data[CHANNELS];

template < typename ADConv, uint8_t CHANNELS, uint8_t SAMPLES >
uint16_t xpcc::atmega::AnalogSensors<ADConv,CHANNELS,SAMPLES>::numberOfSamples(0);

// ----------------------------------------------------------------------------
template < typename ADConv, uint8_t CHANNELS, uint8_t SAMPLES >
void
xpcc::atmega::AnalogSensors<ADConv,CHANNELS,SAMPLES>::initialize(uint8_t &sensorMapping)
{
	map = &sensorMapping;
	adc.attachConversionCompleteInterrupt(sampleAdc);
	adc.setInterruptEnable(true);
}

template < typename ADConv, uint8_t CHANNELS, uint8_t SAMPLES >
void
xpcc::atmega::AnalogSensors<ADConv,CHANNELS,SAMPLES>::sampleAdc()
{
	if (SAMPLES) {
		
		// do oversample and average
		static uint8_t indexOfChannel(0);
		static uint16_t sample[CHANNELS];
		// reset array to zero at the beginning of sampling
		if (numberOfSamples <= CHANNELS) sample[indexOfChannel] = 0;
		
		sample[indexOfChannel] += adc.getValue();
		
		if (++numberOfSamples <= xpcc::Pow<2,SAMPLES>::value * CHANNELS) {
			// continue sampling on next channel
			if (++indexOfChannel >= CHANNELS) indexOfChannel = 0;
			adc.startConversion(map[indexOfChannel]);
		}
		else {
			// stop getting values and calculate the average of the 2^N samples
			for (uint_fast8_t i=0; i<CHANNELS; ++i) {
				// lazy divide: (value / 2^n) => (value >> n)
				data[i] = (sample[i] >> SAMPLES);
			}
			numberOfSamples = 0;
			indexOfChannel = 0;
			newData = true;
		}
		
	} else {
		
		// just get the raw data
		data[numberOfSamples] = adc.getValue();
		
		if (++numberOfSamples < CHANNELS) {
			adc.startConversion(map[numberOfSamples]);
		} else {
			numberOfSamples = 0;
			newData = true;
		}
		
	}
}

template < typename ADConv, uint8_t CHANNELS, uint8_t SAMPLES >
bool
xpcc::atmega::AnalogSensors<ADConv,CHANNELS,SAMPLES>::readSensors()
{
	if (numberOfSamples > 0) return false;
	adc.startConversion(map[0]);
	return true;
}

template < typename ADConv, uint8_t CHANNELS, uint8_t SAMPLES >
bool
xpcc::atmega::AnalogSensors<ADConv,CHANNELS,SAMPLES>::isNewDataAvailable()
{
	return newData;
}

template < typename ADConv, uint8_t CHANNELS, uint8_t SAMPLES >
uint16_t*
xpcc::atmega::AnalogSensors<ADConv,CHANNELS,SAMPLES>::readData()
{
	return &data[0];
}

template < typename ADConv, uint8_t CHANNELS, uint8_t SAMPLES >
uint16_t*
xpcc::atmega::AnalogSensors<ADConv,CHANNELS,SAMPLES>::getData()
{
	newData = false;
	return &data[0];
}


