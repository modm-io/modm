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

#ifndef XPCC__HMC58_HPP
#define XPCC__HMC58_HPP

#include <xpcc/architecture/peripheral/i2c_adapter.hpp>

namespace xpcc
{
	namespace hmc58
	{
		/// The addresses of the Configuration and Data Registers
		enum Register
		{
			REGISTER_CONFIG_A,
			REGISTER_CONFIG_B,
			REGISTER_MODE,
			REGISTER_DATA_X0,	//!< x-axis MSB
			REGISTER_DATA_X1,	//!< x-axis LSB
			REGISTER_DATA_Y0,	//!< y-axis MSB
			REGISTER_DATA_Y1,	//!< y-axis LSB
			REGISTER_DATA_Z0,	//!< z-axis MSB
			REGISTER_DATA_Z1,	//!< z-axis LSB
			REGISTER_STATUS,
			REGISTER_ID_A,
			REGISTER_ID_B,
			REGISTER_ID_C
		};

		/// Averaging modes of REGISTER_CONFIG_A
		enum MeasurementAverage {
			MEASUREMENT_AVERAGE_gm = 0x60,
			MEASUREMENT_AVERAGE_8 = 0x60,
			MEASUREMENT_AVERAGE_4 = 0x20,
			MEASUREMENT_AVERAGE_2 = 0x10,
			MEASUREMENT_AVERAGE_1 = 0x00
		};

		enum _DataOutputRate {
			DATA_OUTPUT_RATE_gm = 0x1c
			// customize for each part
		};

		/// measurement mode option of REGISTER_CONFIG_A
		enum MeasurementMode {
			MEASUREMENT_MODE_NORMAL,
			MEASUREMENT_MODE_POSITIVE,
			MEASUREMENT_MODE_NEGATIVE,
			MEASUREMENT_MODE_gm
		};

		// configuration register B
		enum _Gain {
			GAIN_gm = 0xe0
			// customize for each part
		};

		/// operating mode options of REGISTER_MODE
		enum OperationMode {
			OPERATION_MODE_CONTINUOUS,
			OPERATION_MODE_SINGLE,
			OPERATION_MODE_IDLE,
			OPERATION_MODE_SLEEP,
			OPERATION_MODE_gm = 0x03
		};

		/// REGISTER_STATUS bit masks
		enum Status {
			STATUS_REGULATOR_ENABLED = 0x04,
			STATUS_DATA_REGISTER_LOCK = 0x02,
			STATUS_DATA_READY = 0x01
		};

		/// Content of the identification registers
		enum Identification {
			IDENTIFICATION_A = 'H',
			IDENTIFICATION_B = '4',
			IDENTIFICATION_C = '3'
		};
	}

	/**
	 * \brief	HMC58* 3-axis digital magnetometer family driver.
	 *
	 * The HMC58* is a surface-mount, multi-chip module designed for low-field
	 * magnetic sensing with a digital interface for applications such as
	 * low-cost compassing and magnetometry. The HMC58* includes high-resolution
	 * magneto-resistive sensors plus an ASIC containing amplification,
	 * automatic degaussing strap drivers, offset cancellation, and a 12-bit
	 * ADC that enables 1-2 degrees compass heading accuracy.
	 *
	 * \ingroup driver_inertial
	 * \author	Niklas Hauser
	 *
	 * \tparam I2cMaster Asynchronous Two Wire interface
	 */
	template < typename I2cMaster >
	class Hmc58 : protected xpcc::I2cWriteReadAdapter
	{
	public:
		/// \brief	Constructor, sets address to default of 0x1e
		Hmc58(uint8_t* data, uint8_t address=0x1e);

		/**
		 * Configures the sensor to normal measurement mode with default gain of
		 * ~1Gs and 8 sample averaging in continous updates at the specified
		 * data output rate.
		 */
		bool
		configure(uint8_t dataOutputRate=0x10);

		/**
		 * read the X-ZDATA0-1 registers and buffer the results
		 * sets isNewDataAvailable() to \c true
		 */
		ALWAYS_INLINE void
		readMagnetometer();

		/**
		 * \return pointer to 8bit array containing xyz Gauss.
		 * Be aware that the array is in BIG ENDIAN format, so you cannot
		 * simply reinterpret the result as int16_t!!
		 */
		inline uint8_t*
		getData();

		/**
		 * \c true, when new data has been from the sensor and buffered,
		 * \c false, when the data has already been read
		 */
		ALWAYS_INLINE bool
		isNewDataAvailable();

		void
		update();

		/// Reads the sensor register if new results have been computed.
		bool
		isDataReady();

		/// Sets the specified measurement mode
		bool
		setMeasurementMode(hmc58::MeasurementMode mode=hmc58::MEASUREMENT_MODE_NORMAL);

		/// Sets the specified data output mode
		bool
		setDataOutputRate(uint8_t dataOutputRate=0x10);

		/// Sets the specified gain
		bool
		setGain(uint8_t gain=0x20);

	private:
		/**
		 * writes 8bit data to a register, blocking!
		 * \param reg register address
		 * \param data 8bit data to write
		 */
		bool
		writeRegister(hmc58::Register reg, uint8_t value);

		/**
		 * reads a 8bit register, blocking!
		 * \param reg the 8bit register to read
		 * \return 8bit content
		 */
		uint8_t
		readRegister(hmc58::Register reg);

		enum Status {
			READ_MAGNETOMETER_PENDING = 0x01,
			READ_MAGNETOMETER_RUNNING = 0x02,
			NEW_MAGNETOMETER_DATA = 0x04,
		};

		uint8_t status;
		uint8_t* data;
		uint8_t buffer[4];
	};

}

#include "hmc58_impl.hpp"

#endif // XPCC__HMC58_HPP
