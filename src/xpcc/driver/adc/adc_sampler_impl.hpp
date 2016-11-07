/*
 * Copyright (c) 2015, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_ADC_SAMPLER_HPP
#	error 	"Don't include this file directly, use 'adc_sampler.hpp' instead!"
#endif

// ----------------------------------------------------------------------------
template < class AdcInterrupt, uint8_t Channels, uint32_t Oversamples >
const typename xpcc::AdcSampler<AdcInterrupt,Channels,Oversamples>::Channel*
xpcc::AdcSampler<AdcInterrupt,Channels,Oversamples>::map(nullptr);

template < class AdcInterrupt, uint8_t Channels, uint32_t Oversamples >
typename xpcc::AdcSampler<AdcInterrupt,Channels,Oversamples>::DataType*
xpcc::AdcSampler<AdcInterrupt,Channels,Oversamples>::data(nullptr);

template < class AdcInterrupt, uint8_t Channels, uint32_t Oversamples >
typename xpcc::AdcSampler<AdcInterrupt,Channels,Oversamples>::SampleType
xpcc::AdcSampler<AdcInterrupt,Channels,Oversamples>::samples(0);

template < class AdcInterrupt, uint8_t Channels, uint32_t Oversamples >
uint8_t
xpcc::AdcSampler<AdcInterrupt,Channels,Oversamples>::index(0);

template < class AdcInterrupt, uint8_t Channels, uint32_t Oversamples >
bool
xpcc::AdcSampler<AdcInterrupt,Channels,Oversamples>::newData(false);

// ----------------------------------------------------------------------------
template < class AdcInterrupt, uint8_t Channels, uint32_t Oversamples >
void
xpcc::AdcSampler<AdcInterrupt,Channels,Oversamples>::initialize(const Channel* mapping, DataType* data)
{
	map = mapping;
	AdcSampler::data = data;
	samples = 0;
	index = 0;
	newData = false;

	AdcInterrupt::attachInterruptHandler(sampleAdc);
}

template < class AdcInterrupt, uint8_t Channels, uint32_t Oversamples >
void
xpcc::AdcSampler<AdcInterrupt,Channels,Oversamples>::sampleAdc()
{
	if (Oversamples > 1) {

		// reset array to zero at the beginning of sampling
		if (samples <= Channels) data[index] = 0;

		data[index] += AdcInterrupt::getValue();

		if (++samples <= totalSamples)
		{
			// continue sampling on next index
			if (++index >= Channels) index = 0;

			AdcInterrupt::setChannel(map[index]);
			AdcInterrupt::startConversion();
		}
		else
		{
			// stop getting values and calculate the average of the n samples
			for (uint_fast8_t ii=0; ii < Channels; ++ii)
				data[ii] /= Oversamples;

			samples = 0;
			index = 0;
			newData = true;
		}

	} else {

		// just get the raw data
		data[index] = AdcInterrupt::getValue();

		if (++index < Channels) {
			AdcInterrupt::setChannel(map[index]);
			AdcInterrupt::startConversion();
		} else {
			index = 0;
			newData = true;
		}

	}
}

template < class AdcInterrupt, uint8_t Channels, uint32_t Oversamples >
bool
xpcc::AdcSampler<AdcInterrupt,Channels,Oversamples>::startReadout()
{
	if (samples > 0) return false;
	newData = false;
	index = 0;

	AdcInterrupt::setChannel(map[0]);
	AdcInterrupt::startConversion();

	return true;
}

template < class AdcInterrupt, uint8_t Channels, uint32_t Oversamples >
bool
xpcc::AdcSampler<AdcInterrupt,Channels,Oversamples>::isReadoutFinished()
{
	return newData;
}

template < class AdcInterrupt, uint8_t Channels, uint32_t Oversamples >
typename xpcc::AdcSampler<AdcInterrupt,Channels,Oversamples>::DataType*
xpcc::AdcSampler<AdcInterrupt,Channels,Oversamples>::getData()
{
	return data;
}


