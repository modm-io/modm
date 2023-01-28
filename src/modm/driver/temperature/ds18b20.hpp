/*
 * Copyright (c) 2010-2011, Fabian Greif
 * Copyright (c) 2012-2014, Niklas Hauser
 * Copyright (c) 2022, Christopher Durand
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_DS18B20_HPP
#define MODM_DS18B20_HPP

#include <modm/architecture/interface/one_wire.hpp>

namespace modm
{
	/**
	 * \brief Measurement resolution setting
	 *
	 * The maximum conversion time doubles for every additional bit:
	 * 9 bits:	93.75 ms
	 * 10 bits:	187.5 ms
	 * 11 bits:	375 ms
	 * 12 bits:	750 ms
	 *
	 * \ingroup	modm_driver_ds18b20
	 */
	struct ds18b20
	{
		enum class Resolution : uint8_t
		{
			Bits9  = 0b0'00'11111,
			Bits10 = 0b0'01'11111,
			Bits11 = 0b0'10'11111,
			Bits12 = 0b0'11'11111
		};
	};

	/**
	 * \todo	Implement temperature alarm functionality
	 *
	 * \author	Fabian Greif
	 * \ingroup	modm_driver_ds18b20
	 */
	template <typename OneWire>
	class Ds18b20 : public ds18b20
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

		/**
		 * \brief	Configure measurement resolution
		 *
		 * Set the measurement resolution. Every additional bit doubles the
		 * measurement time.
		 *
		 * \return	\c true if a device is found, \c false if no
		 * 			device is available on the bus.
		 */
		bool
		configure(Resolution resolution);

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

#endif // MODM_DS18B20_HPP
