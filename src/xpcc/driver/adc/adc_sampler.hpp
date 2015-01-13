// coding: utf-8
/* Copyright (c) 2011, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_ADC_SAMPLER_HPP
#define XPCC_ADC_SAMPLER_HPP

#include <xpcc/architecture/interface/adc_interrupt.hpp>
#include <xpcc/utils/template_metaprogramming.hpp>
#include <xpcc/math/utils/misc.hpp>


namespace xpcc
{

/**
 * Generic ADC wrapper for (over-)sampling analog inputs.
 *
 * This template class stores 8, 16 or 32bit ADC values in a buffer and
 * provides an easy interface for managing that data.
 *
 * The code will read the ADC channels you specify in an array.
 * This means you can remap the ADC channels in the order that they
 * should appear in the buffer.
 * The array must obviously have as many elements as channels!
 * For example, if channels 0,2,7 are connected, but the sensors
 * are in the wrong order, just remap them with the array {7,0,2}.
 *
 * To make use of oversampling and averaging, set `Oversamples > 0`.
 * This will loop `2^Oversamples` times through the ADC channels, adding
 * the average of each result to the buffer.
 *
 * This class will choose the smallest data width for you.
 * It decides this by adding the ADC resolution and the oversample size
 * and choosing the next smallest size (not larger than 32bit though!).
 *
 * @warning	The averaging algorithm only works for unsigned ADC data!
 *
 * @tparam AdcInterrupt	a class implementing the AdcInterrupt interface
 * @tparam Channels		number of ADC channels connected to sensor(s) >= 1
 * @tparam Oversamples	`2^Oversamples` samples to average for each channel
 *
 * @ingroup driver_adc
 * @author	Niklas Hauser
 */
template < class AdcInterrupt, uint8_t Channels, uint8_t Oversamples=0 >
class AdcSampler
{
// doxygen gets confused by template metaprogramming
#ifdef __DOXYGEN__
public:
	/// this type is chosen automatically, it may be uint8_t, uint16_t or uint32_t
	typedef uint16_t DataType;
private:
#else
	static_assert(Channels > 0, "There must be at least one Channel to be sampled!");

	typedef typename AdcInterrupt::Channel Channel;

	static constexpr uint32_t totalSamples = xpcc::pow(2, Oversamples) * Channels;
	typedef typename xpcc::tmp::Select<
			totalSamples <= xpcc::pow(2, 16),
			uint16_t,
			uint32_t >::Result SampleType;

public:
	typedef typename xpcc::tmp::Select<
			(AdcInterrupt::Resolution + Oversamples) <= 16,
			typename xpcc::tmp::Select<
					(AdcInterrupt::Resolution + Oversamples) <= 8,
					uint8_t,
					uint16_t >::Result,
			uint32_t >::Result DataType;
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

}	// namespace xpcc

#include "adc_sampler_impl.hpp"

#endif // XPCC_ADC_SAMPLER_HPP
