/*
 * Copyright (c) 2009-2010, Martin Rosekeit
 * Copyright (c) 2009-2011, Fabian Greif
 * Copyright (c) 2011-2015, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_TMP102_HPP
#	error  "Don't include this file directly, use 'tmp102.hpp' instead!"
#endif

// ----------------------------------------------------------------------------
template < typename I2cMaster >
modm::Tmp102<I2cMaster>::Tmp102(Data &data, uint8_t address)
:	Lm75<I2cMaster>(reinterpret_cast<lm75::Data&>(data), address)
{
}

template < typename I2cMaster >
bool
modm::Tmp102<I2cMaster>::update()
{
	if (timer.execute())
	{
		if (withConversion)
		{
			startConversion();
			modm::this_fiber::sleep_for(29ms);
		}
		this->readTemperature();
		return true;
	}
	return false;
}

template < typename I2cMaster >
modm::tmp102::Data&
modm::Tmp102<I2cMaster>::getData()
{
	return reinterpret_cast<Data&>(this->data);
}

// ----------------------------------------------------------------------------
// MARK: - tasks
template < typename I2cMaster >
bool
modm::Tmp102<I2cMaster>::setUpdateRate(uint8_t rate)
{
	// clamp conversion rate to max 33Hz (=~30ms)
	if (rate > 33) rate = 33;

	if (rate == 0 || rate == 1 || rate == 4 || rate == 8)
	{
		// clear shutdown mode
		reinterpret_cast<Config1_t&>(this->config_msb).reset(Config1::ShutdownMode);
		// clear conversion rate bits
		ConversionRate_t::reset(config_lsb);
		// maps:
		// 0 to 0b00
		// 1 to 0b01
		// 4 to 0b10
		// 8 to 0b11
		if (rate & 0b1001) ConversionRate_t::set(config_lsb, ConversionRate::Hz1);
		if (rate & 0b1100) ConversionRate_t::set(config_lsb, ConversionRate::Hz4);
		if ( writeConfiguration(3) )
		{
			if (rate == 0) timer.restart(4s);
			else timer.restart(std::chrono::milliseconds(1000/rate));
			withConversion = false;
			return true;
		}
	}
	else
	{
		timer.restart(std::chrono::milliseconds(1000/rate - 29));
		withConversion = true;
		return true;
	}

	return false;
}

// MARK: Extended mode
template < typename I2cMaster >
bool
modm::Tmp102<I2cMaster>::enableExtendedMode(bool enable)
{
	config_lsb.update(Config2::ExtendedMode, enable);

	return writeConfiguration(3);
}

// MARK: conversion
template < typename I2cMaster >
bool
modm::Tmp102<I2cMaster>::startConversion()
{
	reinterpret_cast<Config1_t&>(this->config_msb).set(Config1::OneShot);

	if ( writeConfiguration(2) )
	{
		reinterpret_cast<Config1_t&>(this->config_msb).reset(Config1::OneShot);
		return true;
	}

	return false;
}

// MARK: read temperature
template < typename I2cMaster >
bool
modm::Tmp102<I2cMaster>::readComparatorMode(bool &result)
{
	this->buffer[0] = uint8_t(Register::Configuration);
	if (I2cDevice<I2cMaster>::writeRead(this->buffer, 1, this->buffer, 2))
	{
		reinterpret_cast<Config1_t&>(this->config_msb) = Config1_t(this->buffer[0]) & ~Resolution_t::mask();
		result = static_cast<bool>(Config2_t(this->buffer[1]) & Config2::Alert);
		config_lsb = Config2_t(this->buffer[1]) & ~Config2::Alert;
		return true;
	}

	return false;
}

// MARK: configuration
template < typename I2cMaster >
bool
modm::Tmp102<I2cMaster>::writeConfiguration(uint8_t length)
{
	this->buffer[0] = uint8_t(Register::Configuration);
	this->buffer[1] = reinterpret_cast<Config1_t&>(this->config_msb).value;
	this->buffer[2] = config_lsb.value;
	return I2cDevice<I2cMaster>::write(this->buffer, length);
}

template < typename I2cMaster >
bool
modm::Tmp102<I2cMaster>::setLimitRegister(Register reg, float temperature)
{
	{
		int16_t temp = temperature * 16.f;
		temp <<= (config_lsb & Config2::ExtendedMode) ? 3 : 4;

		this->buffer[0] = uint8_t(reg);
		this->buffer[1] = (temp >> 8);
		this->buffer[2] = temp;
	}
	return I2cDevice<I2cMaster>::write(this->buffer, 3);
}
