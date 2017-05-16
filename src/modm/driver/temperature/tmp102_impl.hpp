/*
 * Copyright (c) 2009-2011, Fabian Greif
 * Copyright (c) 2009-2010, Martin Rosekeit
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
:	Lm75<I2cMaster>(reinterpret_cast<lm75::Data&>(data), address),
	timeout(250), updateTime(250),
	config_lsb(ConversionRate_t(ConversionRate::Hz4))
{
	this->stop();
}

template < typename I2cMaster >
bool
modm::Tmp102<I2cMaster>::run()
{
	PT_BEGIN();

	while(true)
	{
		PT_WAIT_UNTIL(timeout.isExpired());

		if (updateTime & (1 << 15))
		{
			PT_CALL(startConversion());
			timeout.restart(29);
		}

		PT_WAIT_UNTIL(timeout.isExpired());
		timeout.restart(updateTime & ~(1 << 15));

		PT_CALL(this->readTemperature());
	}

	PT_END();
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
modm::ResumableResult<bool>
modm::Tmp102<I2cMaster>::setUpdateRate(uint8_t rate)
{
	RF_BEGIN();

	this->restart();
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
		if ( RF_CALL(writeConfiguration(3)) )
		{
			if (rate == 0) updateTime = 4000;
			else updateTime = 1000/rate;
			timeout.restart(updateTime & ~(1 << 15));
			RF_RETURN(true);
		}
	}
	else
	{
		updateTime = (1000/rate - 29) | (1 << 15);
		timeout.restart(updateTime & ~(1 << 15));
		RF_RETURN(true);
	}

	this->stop();

	RF_END_RETURN(false);
}

// MARK: Extended mode
template < typename I2cMaster >
modm::ResumableResult<bool>
modm::Tmp102<I2cMaster>::enableExtendedMode(bool enable)
{
	RF_BEGIN();

	config_lsb.update(Config2::ExtendedMode, enable);

	RF_END_RETURN_CALL(writeConfiguration(3));
}

// MARK: conversion
template < typename I2cMaster >
modm::ResumableResult<bool>
modm::Tmp102<I2cMaster>::startConversion()
{
	RF_BEGIN();

	reinterpret_cast<Config1_t&>(this->config_msb).set(Config1::OneShot);

	if ( RF_CALL(writeConfiguration(2)) )
	{
		reinterpret_cast<Config1_t&>(this->config_msb).reset(Config1::OneShot);
		RF_RETURN(true);
	}

	RF_END_RETURN(false);
}

// MARK: read temperature
template < typename I2cMaster >
modm::ResumableResult<bool>
modm::Tmp102<I2cMaster>::readComparatorMode(bool &result)
{
	RF_BEGIN();

	this->buffer[0] = uint8_t(Register::Configuration);
	this->transaction.configureWriteRead(this->buffer, 1, this->buffer, 2);

	if (RF_CALL( this->runTransaction() ))
	{
		reinterpret_cast<Config1_t&>(this->config_msb) = Config1_t(this->buffer[0]) & ~Resolution_t::mask();
		result = static_cast<bool>(Config2_t(this->buffer[1]) & Config2::Alert);
		config_lsb = Config2_t(this->buffer[1]) & ~Config2::Alert;
		RF_RETURN(true);
	}

	RF_END_RETURN(false);
}

// MARK: configuration
template < typename I2cMaster >
modm::ResumableResult<bool>
modm::Tmp102<I2cMaster>::writeConfiguration(uint8_t length)
{
	RF_BEGIN();

	this->buffer[0] = uint8_t(Register::Configuration);
	this->buffer[1] = reinterpret_cast<Config1_t&>(this->config_msb).value;
	this->buffer[2] = config_lsb.value;

	this->transaction.configureWrite(this->buffer, length);

	RF_END_RETURN_CALL( this->runTransaction() );
}

template < typename I2cMaster >
modm::ResumableResult<bool>
modm::Tmp102<I2cMaster>::setLimitRegister(Register reg, float temperature)
{
	RF_BEGIN();

	{
		int16_t temp = temperature * 16.f;
		temp <<= (config_lsb & Config2::ExtendedMode) ? 3 : 4;

		this->buffer[0] = uint8_t(reg);
		this->buffer[1] = (temp >> 8);
		this->buffer[2] = temp;
	}

	this->transaction.configureWrite(this->buffer, 3);

	RF_END_RETURN_CALL( this->runTransaction() );
}
