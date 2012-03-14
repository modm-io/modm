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
#include <xpcc/driver/connectivity/i2c/device.hpp>

namespace xpcc
{
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
	 * \tparam I2C Asynchronous Interface
	 */
	template < typename I2C >
	class Tmp102 : public i2c::Device< I2C >
	{
	public:
		enum Register
		{
			REGISTER_TEMPERATURE,
			REGISTER_CONFIGURATION,
			REGISTER_T_LOW,
			REGISTER_T_HIGH
		};
		
		enum Temperature
		{
			TEMPERATURE_EXTENDED_MODE_bm = 0x01
		};
		
		enum Configuration
		{
			// first byte
			CONFIGURATION_SHUTDOWN_MODE_bm = 0x01,
			CONFIGURATION_THERMOSTAT_MODE_bm = 0x02,
			CONFIGURATION_POLARITY_bm = 0x04,
			CONFIGURATION_FAULT_QUEUE_gm = 0x18,
			CONFIGURATION_FAULT_QUEUE_1_gc = 0x00,
			CONFIGURATION_FAULT_QUEUE_2_gc = 0x08,
			CONFIGURATION_FAULT_QUEUE_4_gc = 0x10,
			CONFIGURATION_FAULT_QUEUE_6_gc = 0x18,
			CONFIGURATION_CONVERTER_RESOLUTION_gm = 0x60,
			CONFIGURATION_CONVERTER_RESOLUTION_12_gc = 0x60,
			CONFIGURATION_ONE_SHOT_bm = 0x80,
			
			// second byte
			CONFIGURATION_EXTENDED_MODE_bm = 0x10,
			CONFIGURATION_ALERT_bm = 0x20,
			CONFIGURATION_CONVERSION_RATE_gm = 0xc0,
			CONFIGURATION_CONVERSION_RATE_025_gc = 0x00,
			CONFIGURATION_CONVERSION_RATE_1_gc = 0x40,
			CONFIGURATION_CONVERSION_RATE_4_gc = 0x80,
			CONFIGURATION_CONVERSION_RATE_8_gc = 0xc0
		};
		
		/**
		 * Constructor, Default address is 0x90 (alternatives are 0x91, 0x92 and 0x93)
		 */
		Tmp102(uint8_t address=0x90);
		
		void
		configure(uint8_t msb=CONFIGURATION_CONVERTER_RESOLUTION_12_gc,
				  uint8_t lsb=CONFIGURATION_CONVERSION_RATE_4_gc);
		
		/// starts a temperature conversion right now
		void
		startConversion();
		
		/// read the Temperature registers and buffer the results
		void
		readTemperature();
		
		/// \return pointer to 8bit array containing temperature
		uint8_t*
		getData();
		
		/// \return the temperature as a signed float in Celcius
		float
		getTemperature();
		
	private:
		void
		writeRegister(Register reg, uint8_t msb, uint8_t lsb);
		
		void
		readRegister(Register reg, uint8_t *buffer);
		
		uint8_t config;
		uint8_t data[2];
	};
	
}

#include "tmp102_impl.hpp"

#endif // XPCC__TMP102_HPP
