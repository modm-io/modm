// coding: utf-8
// ----------------------------------------------------------------------------
/* Copyright (c) 2013, Roboterclub Aachen e.V.
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

#ifndef XPCC__HMC6343_HPP
#define XPCC__HMC6343_HPP

#include <xpcc/communication/i2c/write_read_adapter.hpp>

namespace xpcc
{
	namespace hmc6343
	{
		enum Register
		{
			EEPROM_SLAVE_ADDRESS,		//!< I2C Slave Address
			EEPROM_RESERVED1,
			EEPROM_SOFTWARE_VERSION,	//!< Software Version Number
			EEPROM_RESERVED2,
			EEPROM_OPERATION_MODE_1,	//!< Operation Mode Register 1
			EEPROM_OPERATION_MODE_2,	//!< Operation Mode Register 2
			EEPROM_DEVICE_SERIAL_LSB,	//!< Device Serial Number
			EEPROM_DEVICE_SERIAL_MSB,	//!< Device Serial Number
			EEPROM_DATE_CODE_YEAR,		//!< Package Date Code: Last two digits of the year
			EEPROM_DATE_CODE_WEEK,		//!< Package Date Code: Fiscal Week
			EEPROM_DEVIATION_ANGLE_LSB,	//!< Deviation Angle (+-1800) in tenth of a degree
			EEPROM_DEVIATION_ANGLE_MSB,	//!< Deviation Angle (+-1800) in tenth of a degree
			EEPROM_VARIATION_ANGLE_LSB,	//!< Variation Angle (+-1800) in tenth of a degree
			EEPROM_VARIATION_ANGLE_MSB,	//!< Variation Angle (+-1800) in tenth of a degree
			EEPROM_X_OFFSET_LSB,		//!< Hard-Iron Calibration Offset for the X-axis
			EEPROM_X_OFFSET_MSB,		//!< Hard-Iron Calibration Offset for the X-axis
			EEPROM_Y_OFFSET_LSB,		//!< Hard-Iron Calibration Offset for the Y-axis
			EEPROM_Y_OFFSET_MSB,		//!< Hard-Iron Calibration Offset for the Y-axis
			EEPROM_Z_OFFSET_LSB,		//!< Hard-Iron Calibration Offset for the Z-axis
			EEPROM_Z_OFFSET_MSB,		//!< Hard-Iron Calibration Offset for the Z-axis
			EEPROM_FILTER_LSB,			//!< Heading IIR Filter (0x00 to 0x0F typical)
			EEPROM_FILTER_MSB,			//!< Heading IIR Filter (set at zero)
		};

		enum Command
		{
			COMMAND_POST_ACCEL_DATA = 0x40,			//!< Post Accel Data. AxMSB, AxLSB, AyMSB, AyLSB, AzMSB, AzLSB
			COMMAND_POST_MAG_DATA = 0x45,			//!< Post Mag Data. MxMSB, MxLSB, MyMSB, MyLSB, MzMSB, MzLSB
			COMMAND_POST_HEADING_DATA = 0x50,		//!< Post Heading Data. HeadMSB, HeadLSB, PitchMSB, PitchLSB, RollMSB, RollLSB
			COMMAND_POST_TILT_DATA = 0x55,			//!< Post Tilt Data. PitchMSB, PitchLSB, RollMSB, RollLSB, TempMSB, TempLSB
			COMMAND_READ_OPERATION_MODE_1 = 0x65,
			COMMAND_ENTER_USER_CALIBRATION = 0x71,
			COMMAND_LEVEL_ORIENTATION = 0x72,
			COMMAND_UPRIGHT_SIDEWAYS_ORIENTATION = 0x73,
			COMMAND_UPRIGHT_FLAT_FRONT_ORIENTATION = 0x74,
			COMMAND_ENTER_RUN_MODE = 0x75,
			COMMAND_ENTER_STANDBY_MODE = 0x76,
			COMMAND_EXIT_USER_CALIBRATION_MODE = 0x7E,
			COMMAND_RESET_PROCESSOR = 0x82,
			COMMAND_ENTER_SLEEP_MODE = 0x83,
			COMMAND_EXIT_SLEEP_MODE = 0x84,
			COMMAND_READ_EEPROM = 0xE1,
			COMMAND_WRITE_EEPROM = 0xF1,
		};

		enum Orientation
		{
			ORIENTATION_LEVEL = COMMAND_LEVEL_ORIENTATION,
			ORIENTATION_UPRIGHT_SIDEWAYS = COMMAND_UPRIGHT_SIDEWAYS_ORIENTATION,
			ORIENTATION_UPRIGHT_FLAT_FRONT = COMMAND_UPRIGHT_FLAT_FRONT_ORIENTATION,
		};

		enum OperationMode1
		{
			OPERATION_MODE_COMP = 0x80,					//!< Calculating compass data if set. (read only)
			OPERATION_MODE_CAL = 0x40,					//!< Calculating calibration offsets if set. (read only)
			OPERATION_MODE_FILTER = 0x20,				//!< IIR Heading Filter used if set.
			OPERATION_MODE_RUN = 0x10,					//!< Run Mode if set.
			OPERATION_MODE_STANDBY = 0x08,				//!< Standby Mode if set.
			OPERATION_MODE_UF_ORIENTATION = 0x04,		//!< Upright Front Orientation if set.
			OPERATION_MODE_UE_ORIENTATION = 0x02,		//!< Upright Edge Orientation if set.
			OPERATION_MODE_LEVEL_ORIENTATION = 0x01,	//!< Level Orientation if set
		};

		enum MeasurementRate
		{
			MEASUREMENT_RATE_1HZ,
			MEASUREMENT_RATE_5HZ,
			MEASUREMENT_RATE_10HZ,
		};
	}

	/**
	 * \brief	HMC6343 3-Axis Compass with algorithms driver.
	 *
	 * The Honeywell HMC6343 is a fully integrated compass module that
	 * includes firmware for heading computation and calibration for
	 * magnetic distortions. The module combines 3-axis magneto-resistive
	 * sensors and 3-axis MEMS accelerometers, analog and digital
	 * support circuits, microprocessor and algorithms required for
	 * heading computation.
	 *
	 * \ingroup inertial
	 * \author	Niklas Hauser
	 *
	 * \tparam I2cMaster Asynchronous Two Wire interface
	 */
	template < typename I2cMaster >
	class Hmc6343
	{
	public:
		/// \brief	Constructor, requires pointer to 20 byte array, sets address to default of 0x19
		Hmc6343(uint8_t* data, uint8_t address=0x19);

		/**
		 * Configures the sensor to normal orientation mode with 10Hz data rate.
		 */
		bool
		configure(hmc6343::MeasurementRate measurementRate=hmc6343::MEASUREMENT_RATE_10HZ);

		/// Sets the specified orientation
		bool
		setOrientation(hmc6343::Orientation orientation=hmc6343::ORIENTATION_LEVEL);

		bool
		setDeviationAngle(int16_t angle);

		bool
		setVariationAngle(int16_t angle);

		bool
		setIIRFilter(uint8_t filter);

		// User modes
		ALWAYS_INLINE bool
		enterRunMode();

		ALWAYS_INLINE bool
		enterStandByMode();

		ALWAYS_INLINE bool
		enterSleepMode();

		inline bool
		exitSleepMode();

		ALWAYS_INLINE bool
		enterUserCalibrationMode();

		inline bool
		exitUserCalibrationMode();

		/// resets the processor, any new command is delayed by 500ms
		inline bool
		resetProcessor();

		// data management
		/**
		 * reads the Acceleration registers and buffer the results
		 * sets isNewAccelerationDataAvailable() to \c true
		 */
		ALWAYS_INLINE void
		readAcceleration();

		/**
		 * reads the Magnetometer registers and buffer the results
		 * sets isNewMagnetometerDataAvailable() to \c true
		 */
		ALWAYS_INLINE void
		readMagnetometer();

		/**
		 * reads the Attitude registers and buffer the results
		 * sets isNewAttitudeDataAvailable() to \c true
		 */
		ALWAYS_INLINE void
		readAttitude();

		/**
		 * reads the Temperature registers and buffer the results
		 * sets isNewTemperatureDataAvailable() to \c true
		 */
		ALWAYS_INLINE void
		readTemperature();

		ALWAYS_INLINE bool
		isNewAccelerationDataAvailable();

		ALWAYS_INLINE bool
		isNewMagnetometerDataAvailable();

		ALWAYS_INLINE bool
		isNewAttitudeDataAvailable();

		ALWAYS_INLINE bool
		isNewTemperatureDataAvailable();

		// access to data
		uint16_t
		getDeviceID();

		/**
		 * \return pointer to 8bit array containing 3 16bit Axyz values.
		 * Be aware that the array is in BIG ENDIAN format, so you cannot
		 * simply reinterpret the result as int16_t!!
		 */
		inline uint8_t*
		getAcceleration();

		/**
		 * \return pointer to 8bit array containing 3 16bit Mxyz values.
		 * Be aware that the array is in BIG ENDIAN format, so you cannot
		 * simply reinterpret the result as int16_t!!
		 */
		inline uint8_t*
		getMagnetometer();

		/// returns the heading in tenth of degrees (0 -> 3600)
		inline uint16_t
		getHeading();

		/// returns the Pitch in tenth of degrees (-900 -> 0 -> 900)
		inline int16_t
		getPitch();

		/// returns the Roll in tenth of degrees (-900 -> 0 -> 900)
		inline int16_t
		getRoll();

		/// returns the temperature in unknown format (was not specified in datasheet)
		inline int16_t
		getTemperature();

		/**
		 * \return pointer to 8bit array containing Axyz Mxyz Heading Pitch Roll Temperature.
		 * Be aware that the array is in BIG ENDIAN format, so you cannot
		 * simply reinterpret the result as int16_t!!
		 */
		ALWAYS_INLINE uint8_t*
		getData();


		void
		update();

	public:
		bool
		writeCommand(hmc6343::Command command);

		/**
		 * writes 8bit data to a register, blocking!
		 * \param reg register address
		 * \param data 8bit data to write
		 */
		bool
		writeRegister(hmc6343::Register reg, uint8_t value);

		/**
		 * reads a 8bit register, blocking!
		 * \param reg the 8bit register to read
		 * \return 8bit content
		 */
		uint8_t
		readRegister(hmc6343::Register reg);

	private:
		xpcc::i2c::WriteReadAdapter adapter;
		xpcc::Timeout<> timeout;

		enum Running
		{
			NOTHING_RUNNING,
			READ_ACCELERATION_RUNNING,
			READ_MAGNETOMETER_RUNNING,
			READ_ATTITUDE_RUNNING,
			READ_TEMPERATURE_RUNNING,

			EXIT_USER_CALIBRATION_MODE_RUNNING,
			EXIT_SLEEP_MODE_RUNNING,
			RESET_PROCESSOR_RUNNING,
		};
		enum Status
		{
			NEW_ACCELERATION_DATA = 0x10,
			NEW_MAGNETOMETER_DATA = 0x20,
			NEW_ATTITUDE_DATA = 0x40,
			NEW_TEMPERATURE_DATA = 0x80,
		};
		enum Pending
		{
			START_ACCELERATION_PENDING = 0x01,
			START_MAGNETOMETER_PENDING = 0x02,
			START_ATTITUDE_PENDING = 0x04,
			START_TEMPERATURE_PENDING = 0x08,

			READ_ACCELERATION_PENDING = 0x10,
			READ_MAGNETOMETER_PENDING = 0x20,
			READ_ATTITUDE_PENDING = 0x40,
			READ_TEMPERATURE_PENDING = 0x80,
		};

		Running running;

		uint8_t status;
		uint8_t pending;
		uint8_t* data;
		uint8_t buffer[3];
	};
}

#include "hmc6343_impl.hpp"

#endif	// XPCC__HMC6343_HPP
