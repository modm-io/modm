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
	: I2cDevice<I2cMaster, 2>(address), data(data)
{}

// ----------------------------------------------------------------------------
template<typename I2cMaster>
modm::ResumableResult<bool>
modm::Tcs3472<I2cMaster>::setInterruptLowThreshold(uint16_t threshold)
{
	RF_BEGIN();

	if (RF_CALL(writeRegister(RegisterAddress::LOW_THRESH_LOW_BYTE, threshold)))
	{
		modm::delay(20us);
		if (RF_CALL(writeRegister(RegisterAddress::LOW_THRESH_HIGH_BYTE, threshold >> 8)))
		{
			RF_RETURN(true);
		}
	}

	RF_END_RETURN(false);
}

template<typename I2cMaster>
modm::ResumableResult<bool>
modm::Tcs3472<I2cMaster>::setInterruptHighThreshold(uint16_t threshold)
{
	RF_BEGIN();

	if (RF_CALL(writeRegister(RegisterAddress::HIGH_THRESH_LOW_BYTE, threshold)))
	{
		if (RF_CALL(writeRegister(RegisterAddress::HIGH_THRESH_HIGH_BYTE, threshold >> 8)))
		{
			RF_RETURN(true);
		}
	}

	RF_END_RETURN(false);
}

template<typename I2cMaster>
modm::ResumableResult<bool>
modm::Tcs3472<I2cMaster>::setWaitTime(WaitTime wait_time, bool wait_long)
{
	RF_BEGIN();

	if (RF_CALL(writeRegister(RegisterAddress::CONFIGURATION, wait_long ? 1 << 1 : 0)))
	{
		if (RF_CALL(writeRegister(RegisterAddress::WAIT_TIME, uint8_t(wait_time))))
		{
			RF_RETURN(true);
		}
	}

	RF_END_RETURN(false);
}

template<typename I2cMaster>
modm::ResumableResult<bool>
modm::Tcs3472<I2cMaster>::configure(Gain gain, IntegrationTime int_time)
{
	RF_BEGIN();

	if (RF_CALL(setGain(gain)))
	{
		if (RF_CALL(setIntegrationTime(int_time)))
		{
			RF_RETURN(true);
		}
	}

	RF_END_RETURN(false);
}

// ----------------------------------------------------------------------------
// MARK: - Tasks
template<typename I2cMaster>
modm::ResumableResult<bool>
modm::Tcs3472<I2cMaster>::reloadInterrupt()
{
	RF_BEGIN();

	// Only send command, don't append data! otherwise the reload is not working!
	buffer[0] = 0x80 | uint8_t(RegisterAddress::RELOAD_INTERRUPT);

	this->transaction.configureWrite(buffer, 1);

	RF_END_RETURN_CALL(this->runTransaction());
}

// ----------------------------------------------------------------------------
template<typename I2cMaster>
modm::ResumableResult<bool>
modm::Tcs3472<I2cMaster>::writeRegister(RegisterAddress address, uint8_t value)
{
	RF_BEGIN();

	buffer[0] = 0x80 | uint8_t(address);
	buffer[1] = value;

	this->transaction.configureWrite(buffer, 2);

	RF_END_RETURN_CALL(this->runTransaction());
}

template<typename I2cMaster>
modm::ResumableResult<bool>
modm::Tcs3472<I2cMaster>::readRegisters(RegisterAddress address, uint8_t *const values,
										uint8_t count)
{
	RF_BEGIN();

	buffer[0] = 0x80 | 0x20 |		// read command auto increment
				uint8_t(address);	// at this address

	this->transaction.configureWriteRead(buffer, 1, values, count);

	RF_END_RETURN_CALL(this->runTransaction());
}
