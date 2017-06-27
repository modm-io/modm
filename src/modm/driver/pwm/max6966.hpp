/*
 * Copyright (c) 2012, Fabian Greif
 * Copyright (c) 2012-2013, Sascha Schade
 * Copyright (c) 2012-2014, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_MAX6966_HPP
#define MODM_MAX6966_HPP

#include <stdint.h>
#include <modm/architecture/interface/gpio.hpp>
#include <modm/architecture/interface/delay.hpp>

namespace modm
{
	namespace max6966
	{
		static const uint8_t WRITE = 0x00;
		static const uint8_t READ = 0x80;

		enum Register
		{
			REGISTER_PORT0 = 0x00,
			REGISTER_PORT1 = 0x01,
			REGISTER_PORT2 = 0x02,
			REGISTER_PORT3 = 0x03,
			REGISTER_PORT4 = 0x04,
			REGISTER_PORT5 = 0x05,
			REGISTER_PORT6 = 0x06,
			REGISTER_PORT7 = 0x07,
			REGISTER_PORT8 = 0x08,
			REGISTER_PORT9 = 0x09,

			REGISTER_PORT0_9 = 0x0A,
			REGISTER_PORT0_3 = 0x0B,
			REGISTER_PORT4_7 = 0x0C,
			REGISTER_PORT8_9 = 0x0D,
			// no write, just read
			REGISTER_READ_PORT7_0 = 0x0E,
			REGISTER_READ_PORT9_8 = 0x0F,

			REGISTER_CONFIGURATION = 0x10,
			REGISTER_RAMP_UP = 0x11,
			REGISTER_RAMP_DOWN = 0x12,
			REGISTER_CURRENT7_0 = 0x13,
			REGISTER_CURRENT9_8 = 0x14,
			REGISTER_GLOBAL_CURRENT = 0x15,

			REGISTER_NO_OP = 0x20,
//			REGISTER_RESERVED = 0x7D,
		};

		enum Config
		{
			CONFIG_RUN_MODE = 0x01,
			CONFIG_CS_RUN = 0x02,
			CONFIG_RAMP_UP = 0x04,
			CONFIG_FADE_OFF = 0x08,
			CONFIG_HOLD_OFF = 0x10,
			CONFIG_PWM_STAGGER = 0x20,
//			CONFIG_RESERVED = 0x40,
			CONFIG_DOUT_OSC = 0x80
		};

		enum Time
		{
			TIME_OFF = 0x00,
			TIME_1_16s = 0x01,
			TIME_1_8s = 0x02,
			TIME_1_4s = 0x03,
			TIME_1_2s = 0x04,
			TIME_1s = 0x05,
			TIME_2s = 0x06,
			TIME_4s = 0x07,
		};

		enum Current
		{
			CURRENT_2_5mA = 0x00,
			CURRENT_5mA = 0x01,
			CURRENT_7_5mA = 0x02,
			CURRENT_10mA = 0x03,
			CURRENT_12_5mA = 0x04,
			CURRENT_15mA = 0x05,
			CURRENT_17_5mA = 0x06,
			CURRENT_20mA = 0x07,
		};

		/**
		 * \brief	None-intuitive correspondence between PWM value and brightness.
		 */
		enum Brightness
		{
			ON  = 0xfe,
			OFF = 0xff,
			MIN = 0x03
		};
	}

	/**
	 * \brief	MAX6966 10-channel, daisy-chainable, constant-current sink, 8bit PWM LED driver.
	 *
	 * \tparam	Spi		Spi interface
	 * \tparam	Cs		Chip Select Pin
	 * \tparam	DRIVERS	Number of daisy-chained chips
	 *
	 * \author	Niklas Hauser
	 * \ingroup	driver_pwm
	 */
	template<
		typename Spi,
		typename Cs,
		uint8_t DRIVERS=1 >
	class MAX6966
	{
	public:
		/**
		 * \param current	Set the global current
		 * \param config	Enable Data Output, disable external Clock Input
		 */
		static void
		initialize(max6966::Current current, uint8_t config=0);


		/// configure the Chip
		static inline void
		setConfiguration(uint8_t config, uint8_t driver=0)
		{
			writeToDriver(driver, max6966::REGISTER_CONFIGURATION, config);
		}

		/// configure the Chip
		static void
		setAllConfiguration(uint8_t config);

		/// get Chip configuration
		static inline uint8_t
		getConfiguration(uint8_t driver=0)
		{
			return readFromDriver(driver, max6966::REGISTER_CONFIGURATION);
		}

		/// configure the ramp up time
		static inline void
		setRampUpTime(max6966::Time time=max6966::TIME_1s, uint8_t driver=0)
		{
			writeToDriverMasked(driver, max6966::REGISTER_RAMP_UP, time, 0x07);
		}

		/// configure the hold time
		static inline void
		setHoldTime(max6966::Time time=max6966::TIME_1s, uint8_t driver=0)
		{
			writeToDriverMasked(driver, max6966::REGISTER_RAMP_UP, (time << 3), (0x07 << 3));
		}

		/// configure the ramp down time
		static inline void
		setRampDownTime(max6966::Time time=max6966::TIME_1s, uint8_t driver=0)
		{
			writeToDriver(driver, max6966::REGISTER_RAMP_DOWN, time);
		}


		/// set the 8bit value of a single channel
		static void
		setChannel(uint16_t channel, uint8_t value);

		/// set the intensity of a single channel
		static void
		setChannelIntensity(uint16_t channel, uint8_t intensity)
		{
			setChannel(channel, intensityToPwm(intensity));
		}

		/// More efficient version to set all channels differently
		/// Avoids writing NO_OPS. Sets the same channels of all drivers
		/// at the same time.
		/// \param values	array with all pwm values for all channels and drivers.
		static void
		setChannels(uint8_t * values);

		static void
		setChannelsIntensity(uint8_t * intensity)
		{
			// for all channels and drivers
			for (uint_fast8_t ii = 0; ii < 10 * DRIVERS; ++ii)
			{
				intensity[ii] = intensityToPwm(intensity[ii]);
			}
			setChannels(intensity);
		}

		/// \param value	the 8bit value of all channels (same value for all channels)
		static void
		setAllChannels(uint8_t value);

		static void
		setAllChannelsIntensity(uint8_t value)
		{
			setAllChannels(intensityToPwm(value));
		}

		/// get the 8bit value of a channel from the chip
		static uint8_t
		getChannel(uint16_t channel);

		static void
		setHalfCurrent(uint16_t channel, bool full=true);

		/// set peak current of one driver
		static inline void
		setCurrent(max6966::Current current, uint8_t driver=0)
		{
			writeToDriver(driver, max6966::REGISTER_GLOBAL_CURRENT, current);
		}

		/// set peak current of all drivers
		static void
		setAllCurrent(max6966::Current current);

	protected:
		static void
		writeToDriver(uint8_t driver, max6966::Register reg, uint8_t data);

		static uint8_t
		readFromDriver(uint8_t driver, max6966::Register reg);

		static inline void
		writeToDriverMasked(uint8_t driver, max6966::Register reg, uint8_t data, uint8_t mask)
		{
			uint8_t readout = readFromDriver(driver, reg);
			writeToDriver(driver, reg, (data & mask) | (readout & ~mask));
		}

		/** Fix the stupid PWM values of MAX6966 */
		static inline uint8_t
		intensityToPwm(uint8_t intensity)
		{
			if (intensity == 0xff) {
				return max6966::ON;
			}
			if (intensity == 0x00) {
				return max6966::OFF;
			}
			if (intensity < 3) {
				return max6966::MIN;
			}
			return intensity;
		}
	};
}

#include "max6966_impl.hpp"

#endif // MODM_MAX6966_HPP
