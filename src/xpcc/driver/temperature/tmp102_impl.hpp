// coding: utf-8
/* Copyright (c) 2011, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_TMP102_HPP
#	error  "Don't include this file directly, use 'tmp102.hpp' instead!"
#endif

// ----------------------------------------------------------------------------
template < typename I2cMaster >
xpcc::Tmp102<I2cMaster>::Tmp102(Data &data, uint8_t address)
:	I2cDevice<I2cMaster, 2>(address),
	data(data), timeout(250), updateTime(250),
	config_msb(0), config_lsb(ConversionRate_t(ConversionRate::Hz4))
{
	this->stop();
}

template < typename I2cMaster >
bool
xpcc::Tmp102<I2cMaster>::run()
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

		PT_CALL(readTemperature());
	}

	PT_END();
}

// ----------------------------------------------------------------------------
// MARK: - tasks
template < typename I2cMaster >
xpcc::co::Result<bool>
xpcc::Tmp102<I2cMaster>::setUpdateRate(uint8_t rate)
{
	CO_BEGIN();

	this->restart();
	// clamp conversion rate to max 33Hz (=~30ms)
	if (rate > 33) rate = 33;

	if (rate == 0 || rate == 1 || rate == 4 || rate == 8)
	{
		// clear shutdown mode
		config_msb.reset(Config1::ShutdownMode);
		// clear conversion rate bits
		ConversionRate_t::reset(config_lsb);
		// maps:
		// 0 to 0b00
		// 1 to 0b01
		// 4 to 0b10
		// 8 to 0b11
		if (rate & 0b1001) ConversionRate_t::set(config_lsb, ConversionRate::Hz1);
		if (rate & 0b1100) ConversionRate_t::set(config_lsb, ConversionRate::Hz4);
		if ( CO_CALL(writeConfiguration(3)) )
		{
			if (rate == 0) updateTime = 4000;
			else updateTime = 1000/rate;
			timeout.restart(updateTime & ~(1 << 15));
			CO_RETURN(true);
		}
	}
	else
	{
		updateTime = (1000/rate - 29) | (1 << 15);
		timeout.restart(updateTime & ~(1 << 15));
		CO_RETURN(true);
	}

	this->stop();

	CO_END_RETURN(false);
}

// MARK: Extended mode
template < typename I2cMaster >
xpcc::co::Result<bool>
xpcc::Tmp102<I2cMaster>::enableExtendedMode(bool enable)
{
	CO_BEGIN();

	config_lsb.update(Config2::ExtendedMode, enable);

	CO_END_RETURN_CALL(writeConfiguration(3));
}

// MARK: Alert mode
template < typename I2cMaster >
xpcc::co::Result<bool>
xpcc::Tmp102<I2cMaster>::configureAlertMode(ThermostatMode mode, AlertPolarity polarity, FaultQueue faults)
{
	CO_BEGIN();

	config_msb.update(Config1::ThermostatMode, bool(mode));
	config_msb.update(Config1::Polarity, bool(polarity));
	FaultQueue_t::set(config_msb, faults);

	CO_END_RETURN_CALL(writeConfiguration(2));
}

// MARK: conversion
template < typename I2cMaster >
xpcc::co::Result<bool>
xpcc::Tmp102<I2cMaster>::startConversion()
{
	CO_BEGIN();

	config_msb.set(Config1::OneShot);

	if ( CO_CALL(writeConfiguration(2)) )
	{
		config_msb.reset(Config1::OneShot);
		CO_RETURN(true);
	}

	CO_END_RETURN(false);
}

// MARK: read temperature
template < typename I2cMaster >
xpcc::co::Result<bool>
xpcc::Tmp102<I2cMaster>::readTemperature()
{
	CO_BEGIN();

	buffer[0] = uint8_t(Register::Temperature);
	CO_WAIT_UNTIL( this->startWriteRead(buffer, 1, data.data, 2) );

	CO_WAIT_WHILE( this->isTransactionRunning() );

	CO_END_RETURN( this->wasTransactionSuccessful() );
}

// MARK: read temperature
template < typename I2cMaster >
xpcc::co::Result<bool>
xpcc::Tmp102<I2cMaster>::readComparatorMode(bool &result)
{
	CO_BEGIN();

	buffer[0] = i(Register::Configuration);
	CO_WAIT_UNTIL( this->startWriteRead(buffer, 1, buffer, 2) );

	CO_WAIT_WHILE( this->isTransactionRunning() );

	if (this->wasTransactionSuccessful())
	{
		config_msb = Config1_t(buffer[0]) & ~Resolution_t::mask();
		result =     Config2_t(buffer[1]) &  Config2::Alert;
		config_lsb = Config2_t(buffer[1]) & ~Config2::Alert;
		CO_RETURN(true);
	}

	CO_END_RETURN(false);
}

// MARK: configuration
template < typename I2cMaster >
xpcc::co::Result<bool>
xpcc::Tmp102<I2cMaster>::writeConfiguration(uint8_t length)
{
	CO_BEGIN();

	buffer[0] = i(Register::Configuration);
	buffer[1] = config_msb.value;
	buffer[2] = config_lsb.value;

	CO_WAIT_UNTIL( this->startWrite(buffer, length) );

	CO_WAIT_WHILE( this->isTransactionRunning() );

	CO_END_RETURN( this->wasTransactionSuccessful() );
}

// MARK: configuration
template < typename I2cMaster >
xpcc::co::Result<bool>
xpcc::Tmp102<I2cMaster>::writeLimitRegister(Register reg, float temperature)
{
	CO_BEGIN();

	{
		int16_t temp = temperature * 16.f;
		temp <<= (config_lsb & Config2::ExtendedMode) ? 3 : 4;

		buffer[0] = i(reg);
		buffer[1] = (temp >> 8);
		buffer[2] = temp;
	}

	CO_WAIT_UNTIL( this->startWrite(buffer, 3) );

	CO_WAIT_WHILE( this->isTransactionRunning() );

	CO_END_RETURN( this->wasTransactionSuccessful() );
}
