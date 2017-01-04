/*
 * Copyright (c) 2010-2011, Fabian Greif
 * Copyright (c) 2012-2014, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_DS18B20_HPP
#define XPCC_DS18B20_HPP

#include <modm/architecture/interface/one_wire.hpp>

namespace xpcc
{
	/**
	 * \brief	Programmable resolution 1-wire digital thermometer
	 *
	 * The DS18B20 digital thermometer provides 9-bit to 12-bit Celsius
	 * temperature measurements and has an alarm function with non-volatile
	 * user-programmable upper and lower trigger points. The DS18B20
	 * communicates over a 1-Wire bus that by definition requires only one
	 * data line (and ground) for communication with a central microprocessor.
	 *
	 * It has an operating temperature range of -55°C to +125°C and is
	 * accurate to ±0.5°C over the range of -10°C to +85°C.
	 *
	 * In addition, the DS18B20 can derive power directly from the data line
	 * ("parasite power"), eliminating the need for an external power supply.
	 *
	 * Features:
	 * - Power Supply Range is 3.0V to 5.5V
	 * - Each device has a unique 64-Bit Serial Code Stored in an On-Board ROM
	 * - Thermometer Resolution is User Selectable from 9 to 12 Bits
	 * - Converts Temperature to 12-Bit Digital Word in 750ms (Max)
	 *
	 * \todo	Implement the configure() method to set the resolution
	 * 			and the temperature alarm functionality
	 *
	 * \author	Fabian Greif
	 * \ingroup	driver_temperature
	 */
	template <typename OneWire>
	class Ds18b20
	{
	public:
		/**
		 * \brief	Constructor
		 *
		 * Default resolution is 12-bit.
		 *
		 * \param 	rom		8-byte unique ROM number of the device
		 */
		Ds18b20(const uint8_t *rom);

		// TODO
		//void
		//configure();

		/**
		 * \brief	Check if the device is present
		 *
		 * \return	\c true if the device is found, \c false if the
		 * 			ROM number is not available on the bus.
		 */
		bool
		isAvailable();

		/**
		 * \brief	Start the conversion of this device
		 */
		void
		startConversion();

		/**
		 * \brief	Start the conversion for all connected devices
		 *
		 * Uses the SKIP_ROM command to start the conversion on all
		 * connected DS18B20 devices.
		 *
		 * \warning	Use this function with caution. If you have devices other
		 * 			than the DS18B20 connected to your 1-wire bus check if
		 * 			they tolerate the SKIP_ROM + CONVERT_T command.
		 */
		void
		startConversions();

		/**
		 * \brief	Check if the last conversion is complete
		 *
		 * \return	\c true conversion complete, \n
		 * 			\c false conversion in progress.
		 */
		bool
		isConversionDone();

		/**
		 * \brief	Read the current temperature in centi-degree
		 *
		 * \todo	Needs a better output format
		 * \return	temperature in centi-degree
		 */
		int16_t
		readTemperature();

	protected:
		/**
		 * \brief	Select the current device
		 */
		bool
		selectDevice();

		uint8_t identifier[8];

		static const uint8_t CONVERT_T = 0x44;
		static const uint8_t WRITE_SCRATCHPAD = 0x4e;
		static const uint8_t READ_SCRATCHPAD = 0xbe;
		static const uint8_t COPY_SCRATCHPAD = 0x48;
		static const uint8_t RECALL_E2 = 0xb8;
		static const uint8_t READ_POWER_SUPPLY = 0xb4;

		static OneWire ow;
	};
}

#include "ds18b20_impl.hpp"

#endif // XPCC_DS18B20_HPP
