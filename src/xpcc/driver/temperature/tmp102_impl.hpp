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
:	I2cDevice<I2cMaster>(), task(Task::Idle), success(0), config(0),
	data(data), adapter(address, task, success)
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
bool
xpcc::Tmp102<I2cMaster>::startPing()
{
	if (task == Task::Idle && adapter.configurePing() && this->startTransaction(&adapter))
	{
		task = Task::Ping;
		return true;
	}
	return false;
}

template < typename I2cMaster >
bool
xpcc::Tmp102<I2cMaster>::runPing()
{
	return (task == Task::Ping);
}

template < typename I2cMaster >
bool
xpcc::Tmp102<I2cMaster>::isPingSuccessful()
{
	return (success == Task::Ping);
}

// MARK: configuration
template < typename I2cMaster >
bool
xpcc::Tmp102<I2cMaster>::startConfiguration(uint8_t lsb, uint8_t msb)
{
	if (task == Task::Idle)
	{
		config = msb;
		buffer[0] = static_cast<uint8_t>(Register::Configuration);
		buffer[1] = msb;
		buffer[2] = lsb;

		if (adapter.configureWrite(buffer, 3) && this->startTransaction(&adapter))
		{
			task = Task::Configuration;
			return true;
		}
	}
	return false;
}

template < typename I2cMaster >
bool
xpcc::Tmp102<I2cMaster>::runConfiguration()
{
	return (task == Task::Configuration);
}

template < typename I2cMaster >
bool
xpcc::Tmp102<I2cMaster>::isConfigurationSuccessful()
{
	return (success == Task::Configuration);
}

// MARK: conversion
template < typename I2cMaster >
bool inline
xpcc::Tmp102<I2cMaster>::startConversion()
{
	if (task == Task::Idle)
	{
		buffer[0] = static_cast<uint8_t>(Register::Configuration);
		buffer[1] = config | CONFIGURATION_ONE_SHOT;

		if (adapter.configureWrite(buffer, 2) && this->startTransaction(&adapter))
		{
			task = Task::StartConversion;
			return true;
		}
	}
	return false;
}

template < typename I2cMaster >
bool
xpcc::Tmp102<I2cMaster>::runConversion()
{
	return (task == Task::StartConversion);
}

template < typename I2cMaster >
bool
xpcc::Tmp102<I2cMaster>::isConversionSuccessful()
{
	return (success == Task::StartConversion);
}

// MARK: read temperature
template < typename I2cMaster >
bool
xpcc::Tmp102<I2cMaster>::startReadTemperature()
{
	if (task == Task::Idle)
	{
		buffer[0] = static_cast<uint8_t>(Register::Temperature);

		if (adapter.configureWriteRead(buffer, 1, data, 2) && this->startTransaction(&adapter))
		{
			task = Task::ReadTemperature;
			return true;
		}
	}
	return false;
}

template < typename I2cMaster >
bool
xpcc::Tmp102<I2cMaster>::runReadTemperature()
{
	return (task == Task::ReadTemperature);
}

template < typename I2cMaster >
bool
xpcc::Tmp102<I2cMaster>::isReadTemperatureSuccessful()
{
	return (success == Task::ReadTemperature);
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
