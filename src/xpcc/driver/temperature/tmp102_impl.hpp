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
xpcc::Tmp102<I2cMaster>::Tmp102(uint8_t* data, uint8_t address)
:	i2cTask(I2cTask::Idle), i2cSuccess(0), data(data), config_msb(0),
	config_lsb(0x80), updateTime(250), adapter(address, i2cTask, i2cSuccess)
{
}

template < typename I2cMaster >
uint8_t*
xpcc::Tmp102<I2cMaster>::getData()
{
	return data;
}

template < typename I2cMaster >
bool
xpcc::Tmp102<I2cMaster>::update()
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
			(i2cTask = I2cTask::Ping, this->startTransaction(&adapter))
	);

	CO_WAIT_WHILE(i2cTask == I2cTask::Ping);

	if (i2cSuccess == I2cTask::Ping)
		CO_RETURN(true);

	CO_END();
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
		config_msb &= ~CONFIGURATION_SHUTDOWN_MODE;
		// clear conversion rate bits
		config_lsb &= ~CONFIGURATION_CONVERSION_RATE;
		// maps:
		// 0b0000 to 0b00
		// 0b0001 to 0b01
		// 0b0100 to 0b10
		// 0b1000 to 0b11
		if (rate & 0b1001) config_lsb |= CONFIGURATION_CONVERSION_RATE_1HZ;
		if (rate & 0b1100) config_lsb |= CONFIGURATION_CONVERSION_RATE_4HZ;
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

	CO_END();
}

// MARK: Extended mode
template < typename I2cMaster >
xpcc::co::Result<bool>
xpcc::Tmp102<I2cMaster>::enableExtendedMode(void *ctx, bool enable)
{
	CO_BEGIN(ctx);

	if (enable)	config_lsb |=  CONFIGURATION_EXTENDED_MODE;
	else		config_lsb &= ~CONFIGURATION_EXTENDED_MODE;

	if ( CO_CALL(writeConfiguration(ctx, 3)) )
		CO_RETURN(true);

	CO_END();
}

// MARK: Alert mode
template < typename I2cMaster >
xpcc::co::Result<bool>
xpcc::Tmp102<I2cMaster>::configureAlertMode(void *ctx, ThermostatMode mode, AlertPolarity polarity, FaultQueue faults)
{
	CO_BEGIN(ctx);

	if (static_cast<uint8_t>(mode))
		config_msb |= CONFIGURATION_THERMOSTAT_MODE;
	else	config_msb &= ~CONFIGURATION_THERMOSTAT_MODE;

	if (static_cast<uint8_t>(polarity))
		config_msb |= CONFIGURATION_POLARITY;
	else	config_msb &= ~CONFIGURATION_POLARITY;

	config_msb &= ~CONFIGURATION_FAULT_QUEUE;
	config_msb |= static_cast<uint8_t>(faults);

	if ( CO_CALL(writeConfiguration(ctx, 2)) )
		CO_RETURN(true);

	CO_END();
}

// MARK: conversion
template < typename I2cMaster >
xpcc::co::Result<bool>
xpcc::Tmp102<I2cMaster>::startConversion(void *ctx)
{
	CO_BEGIN(ctx);

	config_msb |= CONFIGURATION_ONE_SHOT;

	if ( CO_CALL(writeConfiguration(ctx, 2)) )
	{
		config_msb &= ~CONFIGURATION_ONE_SHOT;
		CO_RETURN(true);
	}

	CO_END();
}

// MARK: read temperature
template < typename I2cMaster >
xpcc::co::Result<bool>
xpcc::Tmp102<I2cMaster>::readTemperature(void *ctx)
{
	CO_BEGIN(ctx);

	CO_WAIT_UNTIL(
			!adapter.isBusy() && (
					buffer[0] = REGISTER_TEMPERATURE,
					adapter.configureWriteRead(buffer, 1, data, 2) &&
							(i2cTask = I2cTask::ReadTemperature, this->startTransaction(&adapter)) )
	);

	CO_WAIT_WHILE(i2cTask == I2cTask::ReadTemperature);

	if (i2cSuccess == I2cTask::ReadTemperature)
		CO_RETURN(true);

	CO_END();
}

// MARK: read temperature
template < typename I2cMaster >
xpcc::co::Result<bool>
xpcc::Tmp102<I2cMaster>::readComparatorMode(void *ctx, bool &result)
{
	CO_BEGIN(ctx);

	CO_WAIT_UNTIL(
			!adapter.isBusy() && (
					buffer[0] = REGISTER_CONFIGURATION,
					adapter.configureWriteRead(buffer, 1, buffer, 2) &&
							(i2cTask = I2cTask::ReadAlert, this->startTransaction(&adapter)) )
	);

	CO_WAIT_WHILE(i2cTask == I2cTask::ReadAlert);

	if (i2cSuccess == I2cTask::ReadAlert)
	{
		config_msb = buffer[0] & ~CONFIGURATION_CONVERTER_RESOLUTION;
		result = buffer[1] & CONFIGURATION_ALERT;
		config_lsb = buffer[1] & ~CONFIGURATION_ALERT;
		CO_RETURN(true);
	}

	result = false;

	CO_END();
}

// MARK: configuration
template < typename I2cMaster >
xpcc::co::Result<bool>
xpcc::Tmp102<I2cMaster>::writeConfiguration(void *ctx, uint8_t length)
{
	CO_BEGIN(ctx);

	CO_WAIT_UNTIL(
			!adapter.isBusy() && (
					buffer[0] = REGISTER_CONFIGURATION,
					buffer[1] = config_msb,
					buffer[2] = config_lsb,
					adapter.configureWrite(buffer, length) &&
							(i2cTask = I2cTask::Configuration, this->startTransaction(&adapter)) )
	);

	CO_WAIT_WHILE(i2cTask == I2cTask::Configuration);

	if (i2cSuccess == I2cTask::Configuration)
		CO_RETURN(true);

	CO_END();
}

// MARK: configuration
template < typename I2cMaster >
xpcc::co::Result<bool>
xpcc::Tmp102<I2cMaster>::writeLimitRegister(void *ctx, Register reg, float temperature)
{
	CO_BEGIN(ctx);

	{
		int16_t temp = temperature * 16.f;
		temp <<= (config_lsb & CONFIGURATION_EXTENDED_MODE) ? 3 : 4;
		temp = xpcc::math::bigEndianToHost(static_cast<uint16_t>(temp));

		buffer[2] = (temp >> 8);
		buffer[1] = temp;
	}

	CO_WAIT_UNTIL(
			!adapter.isBusy() && (
					buffer[0] = reg,
					adapter.configureWrite(buffer, 3) &&
							(i2cTask = I2cTask::LimitRegister, this->startTransaction(&adapter)) )
	);

	CO_WAIT_WHILE(i2cTask == I2cTask::LimitRegister);

	if (i2cSuccess == I2cTask::LimitRegister)
		CO_RETURN(true);

	CO_END();
}

// MARK: - utility
template < typename I2cMaster >
float
xpcc::Tmp102<I2cMaster>::getTemperature()
{
	int16_t temp = static_cast<int16_t>(xpcc::math::bigEndianToHost(*reinterpret_cast<uint16_t*>(data)));
	if (data[1] & 0x01)
	{
		// temperature extended mode
		return (temp >> 3) / 16.f;
	}
	return (temp >> 4) / 16.f;
}
