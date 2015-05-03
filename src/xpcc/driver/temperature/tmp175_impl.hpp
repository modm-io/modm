// coding: utf-8
/* Copyright (c) 2011, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_TMP175_HPP
#	error  "Don't include this file directly, use 'tmp175.hpp' instead!"
#endif

// ----------------------------------------------------------------------------
template < typename I2cMaster >
xpcc::Tmp175<I2cMaster>::Tmp175(Data &data, uint8_t address)
:	Lm75<I2cMaster>(reinterpret_cast<lm75::Data&>(data), address),
	periodTimeout(250), conversionTimeout(250), updateTime(250), conversionTime(232)
{
	this->stop();
}

template < typename I2cMaster >
bool
xpcc::Tmp175<I2cMaster>::run()
{
	PT_BEGIN();

	while(true)
	{
		PT_WAIT_UNTIL(periodTimeout.isExpired());
		periodTimeout.restart(updateTime);

		PT_CALL(startConversion());

		conversionTimeout.restart(conversionTime);
		PT_WAIT_UNTIL(conversionTimeout.isExpired());

		PT_CALL(this->readTemperature());
	}

	PT_END();
}

template < typename I2cMaster >
xpcc::tmp175::Data&
xpcc::Tmp175<I2cMaster>::getData()
{
	return reinterpret_cast<Data&>(this->data);
}

// MARK: - tasks
template < typename I2cMaster >
void
xpcc::Tmp175<I2cMaster>::setUpdateRate(uint8_t rate)
{
	// clamp conversion rate to max 33Hz (=~30ms)
	if (rate == 0) rate = 1;
	if (rate > 33) rate = 33;

	updateTime = (1000/rate - 29);
	periodTimeout.restart(updateTime);

	this->restart();
}

template < typename I2cMaster >
xpcc::ResumableResult<bool>
xpcc::Tmp175<I2cMaster>::setResolution(Resolution resolution)
{
	RF_BEGIN();

	Resolution_t::set(reinterpret_cast<Config1_t&>(this->config_msb), resolution);

	conversionTime = (uint8_t(resolution) + 1) * 29;

	RF_END_RETURN_CALL( writeConfiguration() );
}

// MARK: conversion
template < typename I2cMaster >
xpcc::ResumableResult<bool>
xpcc::Tmp175<I2cMaster>::startConversion()
{
	RF_BEGIN();

	reinterpret_cast<Config1_t&>(this->config_msb).set(Config1::OneShot);

	if ( RF_CALL(writeConfiguration()) )
	{
		reinterpret_cast<Config1_t&>(this->config_msb).reset(Config1::OneShot);
		RF_RETURN(true);
	}

	RF_END_RETURN(false);
}

// MARK: configuration
template < typename I2cMaster >
xpcc::ResumableResult<bool>
xpcc::Tmp175<I2cMaster>::writeConfiguration()
{
	RF_BEGIN();

	this->buffer[0] = uint8_t(Register::Configuration);
	this->buffer[1] = reinterpret_cast<Config1_t&>(this->config_msb).value;

	this->transaction.configureWrite(this->buffer, 2);

	RF_END_RETURN_CALL( this->runTransaction() );
}

template < typename I2cMaster >
xpcc::ResumableResult<bool>
xpcc::Tmp175<I2cMaster>::setLimitRegister(Register reg, float temperature)
{
	RF_BEGIN();

	{
		uint8_t res = uint8_t(Resolution_t::get(reinterpret_cast<Config1_t&>(this->config_msb)));

		int16_t temp = temperature * (2 << res);
		temp <<= 4 + res;

		this->buffer[0] = uint8_t(reg);
		this->buffer[1] = (temp >> 8);
		this->buffer[2] = temp;
	}

	this->transaction.configureWrite(this->buffer, 3);

	RF_END_RETURN_CALL( this->runTransaction() );
}

