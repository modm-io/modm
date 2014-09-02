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
xpcc::Tmp102<I2cMaster>::Tmp102(uint8_t* data, uint8_t address)
:	I2cDevice<I2cMaster>(), i2cTask(I2cTask::Idle), i2cSuccess(0), config(0),
	data(data), adapter(address, i2cTask, i2cSuccess)
{
}

template < typename I2cMaster >
uint8_t*
xpcc::Tmp102<I2cMaster>::getData()
{
	return data;
}

// MARK: - tasks
// MARK: ping
template < typename I2cMaster >
xpcc::pt::Result
xpcc::Tmp102<I2cMaster>::ping(void *ctx)
{
	NPT_BEGIN(ctx);

	NPT_WAIT_UNTIL(adapter.configurePing() && this->startTransaction(&adapter));

	i2cTask = I2cTask::Ping;

	NPT_WAIT_WHILE(i2cTask == I2cTask::Ping);

	if (i2cSuccess == I2cTask::Ping)
		NPT_EXIT_SUCCESS();

	NPT_END();
}

// MARK: configuration
template < typename I2cMaster >
xpcc::pt::Result
xpcc::Tmp102<I2cMaster>::configure(void *ctx, uint8_t lsb, uint8_t msb)
{
	NPT_BEGIN(ctx);

	config = msb;

	NPT_WAIT_UNTIL(
			!adapter.isBusy() && (
					buffer[0] = static_cast<uint8_t>(Register::Configuration),
					buffer[1] = msb,
					buffer[2] = lsb,
					adapter.configureWrite(buffer, 3) && this->startTransaction(&adapter))
	);

	i2cTask = I2cTask::Configuration;

	NPT_WAIT_WHILE(i2cTask == I2cTask::Configuration);

	if (i2cSuccess == I2cTask::Configuration)
		NPT_EXIT_SUCCESS();

	NPT_END();
}

// MARK: conversion
template < typename I2cMaster >
xpcc::pt::Result
xpcc::Tmp102<I2cMaster>::startConversion(void *ctx)
{
	NPT_BEGIN(ctx);

	NPT_WAIT_UNTIL(
			!adapter.isBusy() && (
					buffer[0] = static_cast<uint8_t>(Register::Configuration),
					buffer[1] = config | CONFIGURATION_ONE_SHOT,
					adapter.configureWrite(buffer, 2) && this->startTransaction(&adapter) )
	);

	i2cTask = I2cTask::StartConversion;

	NPT_WAIT_WHILE(i2cTask == I2cTask::StartConversion);

	if (i2cSuccess == I2cTask::StartConversion)
		NPT_EXIT_SUCCESS();

	NPT_END();
}

// MARK: read temperature
template < typename I2cMaster >
xpcc::pt::Result
xpcc::Tmp102<I2cMaster>::readTemperature(void *ctx)
{
	NPT_BEGIN(ctx);

	NPT_WAIT_UNTIL(
			!adapter.isBusy() && (
					buffer[0] = static_cast<uint8_t>(Register::Temperature),
					buffer[1] = config | CONFIGURATION_ONE_SHOT,
					adapter.configureWriteRead(buffer, 1, data, 2) && this->startTransaction(&adapter) )
	);

	i2cTask = I2cTask::ReadTemperature;

	NPT_WAIT_WHILE(i2cTask == I2cTask::ReadTemperature);

	if (i2cSuccess == I2cTask::ReadTemperature)
		NPT_EXIT_SUCCESS();

	NPT_END();
}

// MARK: - utility
template < typename I2cMaster >
float
xpcc::Tmp102<I2cMaster>::getTemperature()
{
	int16_t temp = static_cast<int16_t>((data[0] << 8) | data[1]);
	if (data[1] & 0x01)
	{
		// temperature extended mode
		return (temp >> 3) / 16.f;
	}
	return (temp >> 4) / 16.f;
}
