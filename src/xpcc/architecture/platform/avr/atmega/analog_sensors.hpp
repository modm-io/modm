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
 * $Id: analog_sensors.hpp 607 2011-09-13 19:51:03Z dergraaf $
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_ATMEGA__ANALOG_SENSORS_HPP
#define XPCC_ATMEGA__ANALOG_SENSORS_HPP

#include <stdint.h>
#include <xpcc/math/utils/misc.hpp>

namespace xpcc
{
	namespace atmega
	{
		/**
		 * \brief	Generic ADC wrapper for analog sensors.
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
		 * \ingroup atmega
		 * \author	Niklas Hauser
		 *
		 * \tparam ADConv	AdcInterrupt class
		 * \tparam CHANNELS	number of ADC channels connected to sensor(s)
		 * \tparam SAMPLES	2^SAMPLES number of samples to average for each channel
		 */
		template < typename ADConv, uint8_t CHANNELS, uint8_t SAMPLES=0 >
		class AnalogSensors
		{
		public:
			/**
			 * \param sensorMapping first element of a remap array
			 */
			static inline void
			initialize(uint8_t &sensorMapping);
			
			/**
			 * Starts the ADC readout routine and buffers the results,
			 * sets isNewDataAvailable() to \c true.
			 * \return \c false when a readout is in progress, \c true otherwise
			 */
			static inline bool
			readSensors();
			
			/**
			 * \return pointer to first element of 16bit result array
			 * This method resets isNewDataAvailable() to \c false.
			 */
			static inline uint16_t*
			getData();
			
			/**
			 * \return pointer to first element of 16bit result array, without
			 * changing isNewDataAvailable()
			 */
			static inline uint16_t*
			readData();
			
			/**
			 * \return \c true if new data is available and has not yet been read.
			 */
			static inline bool
			isNewDataAvailable();
			
		private:
			static inline void
			sampleAdc();
			
			static ADConv adc;
			
			static bool newData;
			static uint8_t* map;
			static uint16_t data[CHANNELS];
			static uint16_t numberOfSamples;
		};
	}	
}

#include "analog_sensors_impl.hpp"

#endif // XPCC_ATMEGA__ANALOG_SENSORS_HPP
