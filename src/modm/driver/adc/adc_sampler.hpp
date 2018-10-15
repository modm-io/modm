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

#ifndef MODM_ADC_SAMPLER_HPP
#define MODM_ADC_SAMPLER_HPP

#include <modm/architecture/interface/adc_interrupt.hpp>
#include <modm/math/utils/misc.hpp>
#include <type_traits>

namespace modm
{

/**
 * @tparam AdcInterrupt	a class implementing the AdcInterrupt interface
 * @tparam Channels		number of ADC channels connected to sensor(s) >= 1
 * @tparam Oversamples	# of samples to average for each channel
 *
 * @ingroup modm_driver_adc_sampler
 * @author	Niklas Hauser
 */
template < class AdcInterrupt, uint8_t Channels, uint32_t Oversamples=1 >
class AdcSampler
{
// doxygen gets confused by template metaprogramming
#ifdef __DOXYGEN__
public:
	/// this type is chosen automatically, it may be uint8_t, uint16_t or uint32_t
	typedef uint16_t DataType;
#else
	static_assert(Channels > 0, "There must be at least one Channel to be sampled!");
	static_assert(Oversamples > 0, "Must sample each channel at least once (Oversamples must be > 0)!");

	using Channel = typename AdcInterrupt::Channel;

	static constexpr uint32_t totalSamples = Oversamples * Channels;

	using SampleType = std::conditional_t<
			totalSamples < modm::pow(2, 16),
			std::conditional_t<
					totalSamples < 256,
					uint8_t,
					uint16_t >,
			uint32_t >;

public:
	using DataType = std::conditional_t<
			(modm::pow(2, AdcInterrupt::Resolution) * Oversamples) < modm::pow(2, 16),
			std::conditional_t<
					(modm::pow(2, AdcInterrupt::Resolution) * Oversamples) < 256,
					uint8_t,
					uint16_t >,
			uint32_t >;
#endif

public:
	/**
	 * @param mapping
	 * 			array of length `Channels` containing the channel-to-data mapping
	 * @param data
	 * 			array of length `Channels` containing the results
	 */
	static void
	initialize(const Channel* mapping, DataType* data);

	/// Starts the ADC readout routine and buffers the results
	/// @return `false` when a readout is in progress, `true` otherwise
	static bool
	startReadout();

	/**
	 * @return `true` if sampling finshed,
	 *			`false` if sampling is currently on-going
	 * @warning	The class adds and divides the oversamples in-place, so
 	 * 			the data is invalid while `isReadoutFinished()` returns false!
 	 */
	static bool
	isReadoutFinished();

	/// @return pointer to first element of 16bit result array
	static DataType*
	getData();

private:
	static void
	sampleAdc();

	static const Channel* map;
	static DataType *data;

	static SampleType samples;
	static uint8_t index;
	static bool newData;
};

}	// namespace modm

#include "adc_sampler_impl.hpp"

#endif // MODM_ADC_SAMPLER_HPP
