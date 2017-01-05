/*
 * Copyright (c) 2011-2012, 2014, Niklas Hauser
 * Copyright (c) 2011, Fabian Greif
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_SCP1000_HPP
#define MODM_SCP1000_HPP

#include <stdint.h>
#include <modm/architecture/driver/delay.hpp>

namespace modm
{
	namespace scp1000
	{
		/// The addresses of the Direct Access Registers.
		enum Register
		{
			REGISTER_REVID = 0x00,
			REGISTER_DATAWR = 0x01,
			REGISTER_ADDPTR = 0x02,
			REGISTER_OPERATION = 0x03,
			REGISTER_OPSTATUS = 0x04,
			REGISTER_RSTR = 0x06,
			REGISTER_STATUS = 0x07,
			REGISTER_DATARD8 = 0x1F,
			REGISTER_DATARD16 = 0x20,
			REGISTER_TEMPOUT = 0x21
		};

		/// The operation options of REGISTER_OPERATION.
		enum Operation
		{
			OPERATION_NO_OPERATION = 0x00,
			OPERATION_READ_REGISTER = 0x01,
			OPERATION_WRITE_REGISTER = 0x02,
			OPERATION_READ_EEPROM = 0x05,
			OPERATION_WRITE_EEPROM = 0x06,
			OPERATION_INIT_SEQUENCE = 0x07,
			OPERATION_HIGH_RESOLUTION_MODE_START = 0x0A,
			OPERATION_HIGH_SPEED_MODE_START = 0x09,
			OPERATION_ULTRA_LOW_POWER_MODE_START = 0x0B,
			OPERATION_LOW_POWER_AQUISITION = 0x0C,
			OPERATION_ASIC_TEST = 0x0F
		};

		/// The status bitmasks of REGISTER_STATUS.
		enum Status
		{
			STATUS_EXT_TRIGGERED_AQUISITION_RUNNING = 0x40,
			STATUS_DATA_READY = 0x20,
			STATUS_REAL_TIME_ERROR = 0x10,
			STATUS_STARTUP_RUNNING = 0x01
		};

		/// The status of REGISTER_OPSTATUS.
		enum OperationStatus
		{
			OPERATION_STATUS_FINISHED = 0x00,
			OPERATION_STATUS_RUNNING = 0x01
		};

		enum Reset
		{
			NO_RESET = 0x00,
			RESET = 0x01
		};
	}

	/**
	 * \brief SCP1000-D01 absolute pressure sensor driver
	 *
	 * The SCP1000 sensor measures both absolute pressure in Pa as well as
	 * temperature in degrees Celsius.
	 *
	 * The sensor has 4 measurement modes plus standby and power down mode.
	 * In all measurement modes, the pressure output word-length is 19 bits
	 * and the temperature output word-length is 14 bits.
	 *
\verbatim
Measurement Mode: refresh | pressure | temperature
High Resolution :  1.8Hz  |  17bit   |   14bit
High Speed      :   ~9Hz  |  15bit   |   14bit
Ultra Low Power :   ~1Hz  |  15bit   |   14bit
\endverbatim
	 *
	 * DRDY pin pulled high, when new data is available.
	 * DRDY pin pulled low, when DATARD16 is being read.
	 *
	 * You do not have to read out the temperature as it is only used by the
	 * sensor for internal calculations, but it is important to read the
	 * 16 bit pressure register as the last one in the cycle.
	 * A typical reading sequence: TEMPOUT => DATARD8 => DATARD16
	 *
	 * For further information on how to process the sensors data, consult the
	 * <a href="http://www.sparkfun.com/datasheets/Components/SCP1000-D01.pdf">
	 * datasheet</a>.
	 *
	 * \author	Niklas Hauser
	 * \ingroup driver_pressure
	 *
	 * \tparam Spi	software or hardware Spi interface
	 * \tparam Cs	Chip Select pin
	 * \tparam Int	Interrupt pin
	 */
	template < typename Spi, typename Cs, typename Int >
	class Scp1000
	{
	public:
		/**
		 * Resets and initializes the chip to the new operation mode. This
		 * takes at least 152ms, since the sensor has a long boot.
		 * \param opMode Operation Mode.
		 */
		static bool
		initialize(scp1000::Operation opMode=scp1000::OPERATION_HIGH_RESOLUTION_MODE_START);

		/**
		 * Reads the temperature register and buffers the result
		 * sets isTemperatureAvailable() to \c true
		 */
		static void
		readTemperature();

		/**
		 * Reads the pressure MSB and LSB registers and buffers the results
		 * sets isPressureAvailable() to \c true
		 */
		static void
		readPressure();

		//// \return pointer to the temperature buffer array
		static uint8_t*
		getTemperature();

		/// \return pointer to the pressure buffer array
		static uint8_t*
		getPressure();

		/**
		 * Writes the operation mode to REGISTER_OPERATION and waits
		 * until the operation completed (up to 16ms).
		 *
		 * \param	opMode Operation Mode.
		 * \return	\c true if the operation completed correctly,
		 * 			\c false if it took longer then 16ms to complete.
		 */
		static bool
		setOperation(scp1000::Operation opMode);

		/**
		 * \param opStatus set to true to read operation status.
		 * \return the (operation) status.
		 */
		static uint8_t
		readStatus(bool opStatus=false);

		/**
		 * Resets the chip, waits 151ms, then starts checking the
		 * status register if the startup completed.
		 *
		 * \param timeout time until the startup is declared as failed
		 * \return false if a timeout occurred, true if reset correctly
		 */
		static bool
		reset(uint8_t timeout);

		/// \return true, if temperature buffer has been refreshed, but not read
		static bool
		isNewTemperatureAvailable();

		/// \return true, if pressure buffer has been refreshed, but not read
		static bool
		isNewPressureAvailable();

		/// \return state of the INT pin, not buffered!
		static bool
		isNewDataReady();

	private:
		/**
		 * Writes 8bit data to a Direct Access Register.
		 *
		 * \param reg the Register address
		 * \param data the data to write
		 */
		static void
		writeRegister(scp1000::Register reg, uint8_t data);

		/**
		 * Reads a 8bit Direct Access Register in three SPI cycles.
		 *
		 * \param reg the 8bit Register to read.
		 * \return the 8bit content of the Register
		 */
		static uint8_t
		read8BitRegister(scp1000::Register reg);

		/**
		 * Reads a 16bit Direct Access Register in three SPI cycles.
		 *
		 * \param reg the 16bit Register to read.
		 * \param buffer pointer to the receiver array
		 * \return the 16bit content of the Register
		 */
		static void
		read16BitRegister(scp1000::Register reg, uint8_t *buffer);

		static Spi spi;
		static Cs chipSelect;
		static Int interruptPin;

		static bool newTemperature;
		static bool newPressure;
		static uint8_t temperature[2];
		static uint8_t pressure[3];
	};
}

#include "scp1000_impl.hpp"

#endif // MODM_SCP1000_HPP
