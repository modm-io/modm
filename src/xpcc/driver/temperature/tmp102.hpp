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

#ifndef XPCC__TMP102_HPP
#define XPCC__TMP102_HPP

#include <stdint.h>
#include <xpcc/driver/connectivity/i2c/write_read_adapter.hpp>

namespace xpcc
{
	/**
	 * \see		TMP102
	 * \ingroup	temperature
	 */
	namespace tmp102
	{
		enum Register
		{
			REGISTER_TEMPERATURE = 0x00,
			REGISTER_CONFIGURATION = 0x01,
			REGISTER_T_LOW = 0x02,
			REGISTER_T_HIGH = 0x03
		};
		
		enum Temperature
		{
			TEMPERATURE_EXTENDED_MODE = 0x01
		};
		
		enum Config1
		{// first byte
			CONFIGURATION_SHUTDOWN_MODE = 0x01,
			CONFIGURATION_THERMOSTAT_MODE = 0x02,
			CONFIGURATION_POLARITY = 0x04,
			CONFIGURATION_FAULT_QUEUE = 0x18,
			CONFIGURATION_FAULT_QUEUE_1 = 0x00,
			CONFIGURATION_FAULT_QUEUE_2 = 0x08,
			CONFIGURATION_FAULT_QUEUE_4 = 0x10,
			CONFIGURATION_FAULT_QUEUE_6 = 0x18,
			CONFIGURATION_CONVERTER_RESOLUTION = 0x60,
			CONFIGURATION_CONVERTER_RESOLUTION_12BIT = 0x60,
			CONFIGURATION_ONE_SHOT = 0x80
		};
		
		enum Config2
		{// second byte
			CONFIGURATION_EXTENDED_MODE = 0x10,
			CONFIGURATION_ALERT = 0x20,
			CONFIGURATION_CONVERSION_RATE = 0xc0,
			CONFIGURATION_CONVERSION_RATE_0_25HZ = 0x00,
			CONFIGURATION_CONVERSION_RATE_1HZ = 0x40,
			CONFIGURATION_CONVERSION_RATE_4HZ = 0x80,
			CONFIGURATION_CONVERSION_RATE_8HZ = 0xc0
		};
	}
	
	/**
	 * \brief	TMP102 digital temperature sensor driver
	 *
	 * The TMP102 is a digital temperature sensor with a two-wire interface
	 * and measures temperature over a range of -40 to +125 deg Celsius with a
	 * resolution of 1/16 (0.0625) deg C and an accuracy of up to 0.5 deg C.
	 *
	 * The sensor has a default refresh rate of 4Hz but can be raised up to
	 * 30Hz by repeatedly manually starting a conversion (with 
	 * startConversion()), which lasts 26ms.
	 *
	 * To convert the raw data into degrees Celsius, cast the MSB and LSB into
	 * a signed 16bit integer, shift it right by 4 (or 3 in extended mode) and 
	 * devide by 16 (or use the getTemperature() method).
	 *
	 * If you are only interested in the integer value of the temperature,
	 * simply only use the MSB (getData()[0]) when not in extended mode.
	 *
	 * \see <a href="http://www.ti.com/lit/ds/symlink/tmp102.pdf">Datasheet</a>
	 *
	 * \ingroup temperature
	 * \author	Niklas Hauser
	 *
	 * \tparam I2cMaster Asynchronous Interface
	 */
	template < typename I2cMaster >
	class Tmp102
	{
	public:
		/**
		 * \param	data		pointer to a 2 uint8_t buffer
		 * \param	address		Default address is 0x48 (alternatives are 0x49, 0x4A and 0x4B)
		 */
		Tmp102(uint8_t* data, uint8_t address=0x48);
		
		bool
		configure(tmp102::Config2 lsb=tmp102::CONFIGURATION_CONVERSION_RATE_4HZ,
				  tmp102::Config1 msb=tmp102::CONFIGURATION_CONVERTER_RESOLUTION_12BIT);
		
		/// starts a temperature conversion right now
		ALWAYS_INLINE void
		startConversion();
		
		/**
		 * read the Temperature registers and buffer the results
		 * sets isNewDataAvailable() to \c true
		 */
		ALWAYS_INLINE void
		readTemperature();
		
		/**
		 * \c true, when new data has been read from the sensor and is buffered,
		 * \c false, when the data has been accessed
		 */
		ALWAYS_INLINE bool
		isNewDataAvailable();
		
		/// \return pointer to 8bit array containing temperature as big endian int16_t
		uint8_t*
		getData();
		
		/// \return the temperature as a signed float in Celcius
		float
		getTemperature();
		
		void
		update();
		
	private:
		xpcc::i2c::WriteReadAdapter adapter;
		
		enum Running {
			NOTHING_RUNNING,
			READ_TEMPERATURE_RUNNING,
			START_CONVERSION_RUNNING,
		};
		
		enum Status {
			START_CONVERSION_PENDING = 0x01,
			READ_TEMPERATURE_PENDING = 0x02,
			NEW_TEMPERATURE_DATA = 0x04,
		};
		
		Running running;
		uint8_t status;
		uint8_t config;
		uint8_t* data;
		uint8_t buffer[3];
	};
	
}

#include "tmp102_impl.hpp"

#endif // XPCC__TMP102_HPP
