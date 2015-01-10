// coding: utf-8
/* Copyright (c) 2011, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_ANALOG_SENSORS_HPP
#	error  "Don't include this file directly, use 'analog_sensors.hpp' instead!"
#endif

#include <xpcc/math/utils/misc.hpp>

// ----------------------------------------------------------------------------
template < uint8_t CHANNELS, uint8_t SAMPLES, class AdcInterrupt >
bool xpcc::AnalogSensors<CHANNELS,SAMPLES,AdcInterrupt>::newData;

template < uint8_t CHANNELS, uint8_t SAMPLES, class AdcInterrupt >
const uint8_t* xpcc::AnalogSensors<CHANNELS,SAMPLES,AdcInterrupt>::map;

template < uint8_t CHANNELS, uint8_t SAMPLES, class AdcInterrupt >
uint16_t* xpcc::AnalogSensors<CHANNELS,SAMPLES,AdcInterrupt>::data;

template < uint8_t CHANNELS, uint8_t SAMPLES, class AdcInterrupt >
uint16_t xpcc::AnalogSensors<CHANNELS,SAMPLES,AdcInterrupt>::numberOfSamples(0);

// ----------------------------------------------------------------------------
template < uint8_t CHANNELS, uint8_t SAMPLES, class AdcInterrupt >
void inline
xpcc::AnalogSensors<CHANNELS,SAMPLES,AdcInterrupt>::initialize(const uint8_t* sensorMapping, uint16_t* sensorData)
{
	map = sensorMapping;
	data = sensorData;
	AdcInterrupt::attachConversionCompleteInterrupt(sampleAdc);
	AdcInterrupt::enableInterrupt();
}

template < uint8_t CHANNELS, uint8_t SAMPLES, class AdcInterrupt >
void
xpcc::AnalogSensors<CHANNELS,SAMPLES,AdcInterrupt>::sampleAdc()
{
	if (SAMPLES) {

		// do oversample and average
		static uint8_t indexOfChannel(0);
		static uint16_t sample[CHANNELS];
		// reset array to zero at the beginning of sampling
		if (numberOfSamples <= CHANNELS) sample[indexOfChannel] = 0;

		sample[indexOfChannel] += AdcInterrupt::getValue();

		if (++numberOfSamples <= xpcc::pow(2,SAMPLES) * CHANNELS) {
			// continue sampling on next channel
			if (++indexOfChannel >= CHANNELS) indexOfChannel = 0;
			AdcInterrupt::startConversion(map[indexOfChannel]);
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
		data[numberOfSamples] = AdcInterrupt::getValue();

		if (++numberOfSamples < CHANNELS) {
			AdcInterrupt::startConversion(map[numberOfSamples]);
		} else {
			numberOfSamples = 0;
			newData = true;
		}

	}
}

template < uint8_t CHANNELS, uint8_t SAMPLES, class AdcInterrupt >
bool inline
xpcc::AnalogSensors<CHANNELS,SAMPLES,AdcInterrupt>::readSensors()
{
	if (numberOfSamples > 0) return false;
	AdcInterrupt::startConversion(map[0]);
	return true;
}

template < uint8_t CHANNELS, uint8_t SAMPLES, class AdcInterrupt >
bool
xpcc::AnalogSensors<CHANNELS,SAMPLES,AdcInterrupt>::isNewDataAvailable()
{
	return newData;
}

template < uint8_t CHANNELS, uint8_t SAMPLES, class AdcInterrupt >
uint16_t*
xpcc::AnalogSensors<CHANNELS,SAMPLES,AdcInterrupt>::readData()
{
	return data;
}

template < uint8_t CHANNELS, uint8_t SAMPLES, class AdcInterrupt >
uint16_t*
xpcc::AnalogSensors<CHANNELS,SAMPLES,AdcInterrupt>::getData()
{
	newData = false;
	return data;
}


