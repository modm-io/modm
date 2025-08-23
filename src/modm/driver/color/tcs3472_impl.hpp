// coding: utf-8
/*
 * Copyright (c) 2013, David Hebbeker
 * Copyright (c) 2013-2014, Sascha Schade
 * Copyright (c) 2013-2015, 2021, Niklas Hauser
 * Copyright (c) 2017, Arjun Sarin
 * Copyright (c) 2021, Thomas Sommer
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_TCS3472_HPP
#error "Don't include this file directly, use 'tcs3472.hpp' instead!"
#endif

template<typename I2cMaster>
modm::Tcs3472<I2cMaster>::Tcs3472(Data &data, uint8_t address)
	: I2cDevice<I2cMaster>(address), data(data)
{}

// ----------------------------------------------------------------------------
template<typename I2cMaster>
bool
modm::Tcs3472<I2cMaster>::setInterruptLowThreshold(uint16_t threshold)
{
	if (writeRegister(RegisterAddress::LOW_THRESH_LOW_BYTE, threshold))
	{
		modm::this_fiber::sleep_for(20us);
		if (writeRegister(RegisterAddress::LOW_THRESH_HIGH_BYTE, threshold >> 8))
		{
			return true;
		}
	}

	return false;
}

template<typename I2cMaster>
bool
modm::Tcs3472<I2cMaster>::setInterruptHighThreshold(uint16_t threshold)
{
	if (writeRegister(RegisterAddress::HIGH_THRESH_LOW_BYTE, threshold))
	{
		if (writeRegister(RegisterAddress::HIGH_THRESH_HIGH_BYTE, threshold >> 8))
		{
			return true;
		}
	}

	return false;
}

template<typename I2cMaster>
bool
modm::Tcs3472<I2cMaster>::setWaitTime(WaitTime wait_time, bool wait_long)
{
	if (writeRegister(RegisterAddress::CONFIGURATION, wait_long ? 1 << 1 : 0))
	{
		if (writeRegister(RegisterAddress::WAIT_TIME, uint8_t(wait_time)))
		{
			return true;
		}
	}

	return false;
}

template<typename I2cMaster>
bool
modm::Tcs3472<I2cMaster>::configure(Gain gain, IntegrationTime int_time)
{
	if (setGain(gain))
	{
		if (setIntegrationTime(int_time))
		{
			return true;
		}
	}

	return false;
}

// ----------------------------------------------------------------------------
// MARK: - Tasks
template<typename I2cMaster>
bool
modm::Tcs3472<I2cMaster>::reloadInterrupt()
{
	// Only send command, don't append data! otherwise the reload is not working!
	buffer[0] = 0x80 | uint8_t(RegisterAddress::RELOAD_INTERRUPT);
	return I2cDevice<I2cMaster>::write(buffer, 1);
}

// ----------------------------------------------------------------------------
template<typename I2cMaster>
bool
modm::Tcs3472<I2cMaster>::writeRegister(RegisterAddress address, uint8_t value)
{
	buffer[0] = 0x80 | uint8_t(address);
	buffer[1] = value;
	return I2cDevice<I2cMaster>::write(buffer, 2);
}

template<typename I2cMaster>
bool
modm::Tcs3472<I2cMaster>::readRegisters(RegisterAddress address, uint8_t *const values,
										uint8_t count)
{
	buffer[0] = 0x80 | 0x20 |		// read command auto increment
				uint8_t(address);	// at this address
	return I2cDevice<I2cMaster>::writeRead(buffer, 1, values, count);
}
