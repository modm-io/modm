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

#include <xpcc/math/utils/endianess.hpp>

// ----------------------------------------------------------------------------
template < typename I2cMaster >
xpcc::Tmp102<I2cMaster>::Tmp102(Data &data, uint8_t address)
:	data(data), config_msb(), config_lsb(Config2::ConversionRate4Hz), updateTime(250),
	i2cTask(I2cTask::Idle), i2cSuccess(0), adapter(address, i2cTask, i2cSuccess)
{
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
			PT_CALL(startConversion(this));
			timeout.restart(29);
		}

		PT_WAIT_UNTIL(timeout.isExpired());
		timeout.restart(updateTime & ~(1 << 15));

		PT_CALL(readTemperature(this));
	}

	PT_END();
}

// ----------------------------------------------------------------------------
// MARK: ping
template < typename I2cMaster >
xpcc::co::Result<bool>
xpcc::Tmp102<I2cMaster>::ping(void *ctx)
{
	CO_BEGIN(ctx);

	CO_WAIT_UNTIL(adapter.configurePing() &&
			(i2cTask = I2cTask::Ping, this->startTransaction(&adapter)));

	CO_WAIT_WHILE(i2cTask == I2cTask::Ping);

	CO_END_RETURN(i2cSuccess == I2cTask::Ping);
}

// MARK: - tasks
template < typename I2cMaster >
xpcc::co::Result<bool>
xpcc::Tmp102<I2cMaster>::setUpdateRate(void *ctx, uint8_t rate)
{
	CO_BEGIN(ctx);

	this->restart();
	// clamp conversion rate to max 33Hz (=~30ms)
	if (rate > 33) rate = 33;

	if (rate == 0 || rate == 1 || rate == 4 || rate == 8)
	{
		// clear shutdown mode
		config_msb &= ~Config1::ShutdownMode;
		// clear conversion rate bits
		config_lsb &= ~Config2::ConversionRateMask;
		// maps:
		// 0 to 0b00
		// 1 to 0b01
		// 4 to 0b10
		// 8 to 0b11
		if (rate & 0b1001) config_lsb |= Config2::ConversionRate1Hz;
		if (rate & 0b1100) config_lsb |= Config2::ConversionRate4Hz;
		if ( CO_CALL(writeConfiguration(ctx, 3)) )
		{
			if (rate == 0) updateTime = 4000;
			else updateTime = 1000/rate;
			CO_RETURN(true);
		}
	}
	else
	{
		updateTime = (1000/rate - 29) | (1 << 15);
		CO_RETURN(true);
	}

	this->stop();

	CO_END_RETURN(false);
}

// MARK: Extended mode
template < typename I2cMaster >
xpcc::co::Result<bool>
xpcc::Tmp102<I2cMaster>::enableExtendedMode(void *ctx, bool enable)
{
	CO_BEGIN(ctx);

	if (enable)	config_lsb |=  Config2::ExtendedMode;
	else		config_lsb &= ~Config2::ExtendedMode;

	CO_END_RETURN_CALL(writeConfiguration(ctx, 3));
}

// MARK: Alert mode
template < typename I2cMaster >
xpcc::co::Result<bool>
xpcc::Tmp102<I2cMaster>::configureAlertMode(void *ctx, ThermostatMode mode, AlertPolarity polarity, FaultQueue faults)
{
	CO_BEGIN(ctx);

	if (static_cast<uint8_t>(mode))
			config_msb |=  Config1::ThermostatMode;
	else	config_msb &= ~Config1::ThermostatMode;

	if (static_cast<uint8_t>(polarity))
			config_msb |=  Config1::Polarity;
	else	config_msb &= ~Config1::Polarity;

	config_msb &= ~Config1::FaultQueueMask;
	config_msb |= static_cast<Config1>(faults);

	CO_END_RETURN_CALL(writeConfiguration(ctx, 2));
}

// MARK: conversion
template < typename I2cMaster >
xpcc::co::Result<bool>
xpcc::Tmp102<I2cMaster>::startConversion(void *ctx)
{
	CO_BEGIN(ctx);

	config_msb |= Config1::OneShot;

	if ( CO_CALL(writeConfiguration(ctx, 2)) )
	{
		config_msb &= ~Config1::OneShot;
		CO_RETURN(true);
	}

	CO_END_RETURN(false);
}

// MARK: read temperature
template < typename I2cMaster >
xpcc::co::Result<bool>
xpcc::Tmp102<I2cMaster>::readTemperature(void *ctx)
{
	CO_BEGIN(ctx);

	buffer[0] = static_cast<uint8_t>(Register::Temperature);
	CO_WAIT_UNTIL(adapter.configureWriteRead(buffer, 1, data.getPointer(), 2) &&
			(i2cTask = I2cTask::ReadTemperature, this->startTransaction(&adapter)));

	CO_WAIT_WHILE(i2cTask == I2cTask::ReadTemperature);

	CO_END_RETURN(i2cSuccess == I2cTask::ReadTemperature);
}

// MARK: read temperature
template < typename I2cMaster >
xpcc::co::Result<bool>
xpcc::Tmp102<I2cMaster>::readComparatorMode(void *ctx, bool &result)
{
	CO_BEGIN(ctx);

	buffer[0] = i(Register::Configuration);
	CO_WAIT_UNTIL(adapter.configureWriteRead(buffer, 1, buffer, 2) &&
			(i2cTask = I2cTask::ReadAlert, this->startTransaction(&adapter)));

	CO_WAIT_WHILE(i2cTask == I2cTask::ReadAlert);

	if (i2cSuccess == I2cTask::ReadAlert)
	{
		config_msb = static_cast<Config1_t>(buffer[0]) & ~Config1::ResolutionMask;
		result =     static_cast<Config2_t>(buffer[1]) &  Config2::Alert;
		config_lsb = static_cast<Config2_t>(buffer[1]) & ~Config2::Alert;
		CO_RETURN(true);
	}

	CO_END_RETURN(false);
}

// MARK: configuration
template < typename I2cMaster >
xpcc::co::Result<bool>
xpcc::Tmp102<I2cMaster>::writeConfiguration(void *ctx, uint8_t length)
{
	CO_BEGIN(ctx);

	buffer[0] = i(Register::Configuration);
	buffer[1] = config_msb.value;
	buffer[2] = config_lsb.value;

	CO_WAIT_UNTIL(adapter.configureWrite(buffer, length) &&
			(i2cTask = I2cTask::Configuration, this->startTransaction(&adapter)));

	CO_WAIT_WHILE(i2cTask == I2cTask::Configuration);

	CO_END_RETURN(i2cSuccess == I2cTask::Configuration);
}

// MARK: configuration
template < typename I2cMaster >
xpcc::co::Result<bool>
xpcc::Tmp102<I2cMaster>::writeLimitRegister(void *ctx, Register reg, float temperature)
{
	CO_BEGIN(ctx);

	{
		int16_t temp = temperature * 16.f;
		temp <<= (config_lsb & Config2::ExtendedMode) ? 3 : 4;
		temp = xpcc::math::bigEndianToHost(static_cast<uint16_t>(temp));

		buffer[0] = i(reg);
		buffer[1] = temp;
		buffer[2] = (temp >> 8);
	}

	CO_WAIT_UNTIL(adapter.configureWrite(buffer, 3) &&
			(i2cTask = I2cTask::LimitRegister, this->startTransaction(&adapter)));

	CO_WAIT_WHILE(i2cTask == I2cTask::LimitRegister);

	CO_END_RETURN(i2cSuccess == I2cTask::LimitRegister);
}
