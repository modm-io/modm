// coding: utf-8
/* Copyright (c) 2011, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_ANALOG_SENSORS_HPP
#define XPCC_ANALOG_SENSORS_HPP

#include "adc_interrupt.hpp"
#include "../../../device.hpp"

namespace xpcc
{

/**
 * Generic ADC wrapper for analog sensors.
 *
 * This template class stores 16bit ADC values in a buffer and
 * provides an easy interface for managing that data.
 *
 * The code will read the ADC channels you specify in an array.
 * This means you can remap the ADC channels in the order that they
 * should appear in the buffer.
 * The array must obviously have as many elements as channels!
 * For example, if channels 0,2,7 are connected, but the sensors
 * are in the wrong order, just remap them with the array {7,0,2}.
 *
 * To make use of oversampling and averaging, set SAMPLES > 0.
 * This will loop 2^SAMPLES times through the ADC channels, adding
 * the average of each result to the buffer.
 * Be aware that averaging more than 64 values (SAMPLES>6) could cause
 * a 16bit overflow depending on your ADC values (10bit * 64 = 16bit).
 *
 * @ingroup adc
 * @author	Niklas Hauser
 *
 * @tparam CHANNELS	number of ADC channels connected to sensor(s)
 * @tparam SAMPLES	2^SAMPLES number of samples to average for each channel
 * @tparam ADConv	AdcInterrupt class
 */
template < uint8_t CHANNELS, uint8_t SAMPLES, class AdcInterrupt >
class AnalogSensors
{
public:
	/**
	 * @param sensorMapping
	 * 			first element of a remap array
	 * @param sensorData
	 */
	static void
	initialize(const uint8_t* sensorMapping, uint16_t* sensorData);

	/**
	 * Starts the ADC readout routine and buffers the results,
	 * sets `isNewDataAvailable()` to `true`.
	 * @return `false` when a readout is in progress, `true` otherwise
	 */
	static bool
	readSensors();

	/**
	 * @return pointer to first element of 16bit result array
	 * This method resets `isNewDataAvailable()` to `false`.
	 */
	static uint16_t*
	getData();

	/**
	 * @return pointer to first element of 16bit result array, without
	 * changing `isNewDataAvailable()`
	 */
	static uint16_t*
	readData();

	/**
	 * @return `true` if new data is available and has not yet been read.
	 */
	static bool
	isNewDataAvailable();

private:
	static void
	sampleAdc();

	static bool newData;
	static const uint8_t* map;
	static uint16_t *data;
	static uint16_t numberOfSamples;
};

}	// namespace xpcc

#include "analog_sensors_impl.hpp"

#endif // XPCC_ANALOG_SENSORS_HPP
