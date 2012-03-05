// coding: utf-8
// ----------------------------------------------------------------------------
/* Copyright (c) 2012, Roboterclub Aachen e.V.
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
 * $Id: bmp085.hpp 738 2012-02-25 17:54:01Z salkinium $
 */
// ----------------------------------------------------------------------------

#ifndef XPCC__BMP085_HPP
#define XPCC__BMP085_HPP

#include <xpcc/driver/connectivity/i2c/master.hpp>

namespace xpcc
{
	namespace bmp085
	{
		/// The addresses of the Configuration and Data Registers
		enum Register
		{
			REGISTER_CHIP_ID = 0xD0,
			REGISTER_VERSION = 0xD1,
			REGISTER_CAL_AC1 = 0xAA,
			REGISTER_CAL_AC2 = 0xAC,
			REGISTER_CAL_AC3 = 0xAE,
			REGISTER_CAL_AC4 = 0xB0,
			REGISTER_CAL_AC5 = 0xB2,
			REGISTER_CAL_AC6 = 0xB4,
			REGISTER_CAL_B1 = 0xB6,
			REGISTER_CAL_B2 = 0xB8,
			REGISTER_CAL_MB = 0xBA,
			REGISTER_CAL_MC = 0xBC,
			REGISTER_CAL_MD = 0xBE,
			REGISTER_CONTROL = 0xF4,
			REGISTER_CONVERSION_MSB = 0xF6,
			REGISTER_CONVERSION_LSB = 0xF7,
			REGISTER_CONVERSION_XLSB = 0xF8,
		};
		
		/// The options of REGISTER_CHIP_ID
		enum ChipId {
			CHIP_ID_gc = 0x55
		};
		/// The options of REGISTER_CONTROL
		enum Control {
			CONVERSION_gm = 0x3F,
			CONVERSION_TEMPERATURE_gc = 0x2E,
			CONVERSION_PRESSURE_gc = 0x34
		};
		enum Mode {
			MODE_gm = (0x03 << 6),
			MODE_ULTRA_LOW_POWER_gc = (0x00 << 6),
			MODE_STANDARD_gc = (0x01 << 6),
			MODE_HIGH_RESOLUTION_gc = (0x02 << 6),
			MODE_ULTRA_HIGH_RESOLUTION_gc = (0x03 << 6),
		};
	}
	
	/**
	 * \brief Basic BMA180 digital accelerometer sensor driver
	 *
	 * The BMA180 is a 3-axis accelerometer with high resolution (14-bit)
	 * measurement at up to +-16 g. Digital output data is formatted as 16-bit
	 * twos complement and is accessible through I2C digital interface.
	 *
	 * The following LSB-values are valid:
	 * 1 LSB{ADC} = 0.25mg in 2g range; it scales with range
	 * 1 LSB{TEMP} = 0.5 degrees Celcius
	 *
	 * For further information on the special sensing functions, consult the
	 * <a href="http://www.bosch-sensortec.com/content/language1/downloads/BST-BMA180-FL000-03.pdf">
	 * datasheet</a>.
	 *
	 * \author	Niklas Hauser
	 * \ingroup inertial
	 *
	 * \tparam TwiMaster Asynchronous Two Wire interface
	 */
	template < typename TwiMaster >
	class BMP085 : public xpcc::i2c::Delegate
	{
	public:
		/**
		 * \brief	Constructor
		 * \param	address		address is 0x77
		 */
		BMP085(uint8_t address=0x77);
		
		/// Configures the sensor and reads out and stores the calibration bytes
		bool
		initialize(bmp085::Mode mode=bmp085::MODE_STANDARD_gc);
		
		/// starts a temperature conversion which lasts 4.5ms
		bool
		startTemperatureMeasurement();
		
		/// read the result registers and buffers the results
		bool
		readTemperature();
		
		/// starts a pressure conversion which lasts from 4.5ms to 25.5ms
		bool
		startPressureMeasurement();
		
		/// read the result registers and buffers the results
		bool
		readPressure();
		
		/// \return pointer to 8bit array containing tp temperature and pressure
		uint8_t*
		getData();
		
		/// \return pointer to 8bit array containing calibration data
		uint8_t*
		getCalibrationData();
		
		/**
		 * \c true, when new pressure data has been from the sensor and buffered,
		 * \c false, when the data has been accessed, or data is being 
		 * copied into the buffer.
		 */
		bool
		isNewDataAvailable();
		
		int16_t*
		getCalibratedTemperature();
		
		int32_t*
		getCalibratedPressure();
		
	private:
		/**
		 * this delegate function gets called after calling readPressure()
		 * \return always \c false, since we do not want to continue using the bus
		 */
		void
		twiCompletion(const uint8_t *data, std::size_t index, bool reading);
		
		bmp085::Mode config;
		bool newData;
		uint8_t calibrationData[22];
		uint8_t data[5];
		uint8_t deviceAddress;
		
		bool calTemperature, calPressure;
		int16_t calibratedTemperature;
		int16_t ac1, ac2, ac3, b1, b2, mb, mc, md;
		uint16_t ac4, ac5, ac6;
		int32_t calibratedPressure, b5;
	};
	
}

#include "bmp085_impl.hpp"

#endif // XPCC__BMP085_HPP
