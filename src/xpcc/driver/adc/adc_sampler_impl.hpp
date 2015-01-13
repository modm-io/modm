// coding: utf-8
/* Copyright (c) 2011, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_ADC_SAMPLER_HPP
#	error 	"Don't include this file directly, use 'adc_sampler.hpp' instead!"
#endif

// ----------------------------------------------------------------------------
template < class AdcInterrupt, uint8_t Channels, uint8_t Oversamples >
const typename xpcc::AdcSampler<AdcInterrupt,Channels,Oversamples>::Channel*
xpcc::AdcSampler<AdcInterrupt,Channels,Oversamples>::map(nullptr);

template < class AdcInterrupt, uint8_t Channels, uint8_t Oversamples >
typename xpcc::AdcSampler<AdcInterrupt,Channels,Oversamples>::DataType*
xpcc::AdcSampler<AdcInterrupt,Channels,Oversamples>::data(nullptr);

template < class AdcInterrupt, uint8_t Channels, uint8_t Oversamples >
typename xpcc::AdcSampler<AdcInterrupt,Channels,Oversamples>::SampleType
xpcc::AdcSampler<AdcInterrupt,Channels,Oversamples>::samples(0);

template < class AdcInterrupt, uint8_t Channels, uint8_t Oversamples >
uint8_t
xpcc::AdcSampler<AdcInterrupt,Channels,Oversamples>::index(0);

template < class AdcInterrupt, uint8_t Channels, uint8_t Oversamples >
bool
xpcc::AdcSampler<AdcInterrupt,Channels,Oversamples>::newData(false);

// ----------------------------------------------------------------------------
template < class AdcInterrupt, uint8_t Channels, uint8_t Oversamples >
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

template < class AdcInterrupt, uint8_t Channels, uint8_t Oversamples >
void
xpcc::AdcSampler<AdcInterrupt,Channels,Oversamples>::sampleAdc()
{
	if (Oversamples) {

		// reset array to zero at the beginning of sampling
		if (samples <= Channels) data[index] = 0;

		data[index] += AdcInterrupt::getValue();

		if (++samples <= totalSamples) {
			// continue sampling on next index
			if (++index >= Channels) index = 0;
			AdcInterrupt::setChannel(map[index]);
			AdcInterrupt::startConversion();
		}
		else {
			// stop getting values and calculate the average of the 2^N samples
			for (uint_fast8_t ii=0; ii < Channels; ++ii) {
				// lazy divide: (value / 2^n) => (value >> n)
				data[ii] >>= Oversamples;
			}
			samples = index = 0;
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

template < class AdcInterrupt, uint8_t Channels, uint8_t Oversamples >
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

template < class AdcInterrupt, uint8_t Channels, uint8_t Oversamples >
bool
xpcc::AdcSampler<AdcInterrupt,Channels,Oversamples>::isReadoutFinished()
{
	return newData;
}

template < class AdcInterrupt, uint8_t Channels, uint8_t Oversamples >
typename xpcc::AdcSampler<AdcInterrupt,Channels,Oversamples>::DataType*
xpcc::AdcSampler<AdcInterrupt,Channels,Oversamples>::getData()
{
	return data;
}


