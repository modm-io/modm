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
xpcc::Tmp175<I2cMaster>::Tmp175(uint8_t* data, uint8_t address)
:	I2cWriteReadTransaction(address), running(Running::Nothing), config(0), data(data)
{
	configureWriteRead(buffer, 0, data, 0);
}

template < typename I2cMaster >
bool
xpcc::Tmp175<I2cMaster>::configure(tmp175::Config1 msb)
{
	config = msb;
	buffer[0] = tmp175::REGISTER_CONFIGURATION;
	buffer[1] = msb;
	configureWriteRead(buffer, 2, data, 0);

	return I2cMaster::startBlocking(this);
}

template < typename I2cMaster >
void
xpcc::Tmp175<I2cMaster>::startConversion()
{
	status.startConversionPending = true;
}

template < typename I2cMaster >
void
xpcc::Tmp175<I2cMaster>::readTemperature()
{
	status.readTemperaturePending = true;
}

template < typename I2cMaster >
float
xpcc::Tmp175<I2cMaster>::getTemperature()
{
	int16_t temp = static_cast<int16_t>((data[0]<<8)|data[1]);
	if (data[1] & tmp175::TEMPERATURE_EXTENDED_MODE) {
		return (temp>>3)/16.f;
	}
	else {
		return (temp>>4)/16.f;
	}
}

template < typename I2cMaster >
bool
xpcc::Tmp175<I2cMaster>::isNewDataAvailable()
{
	return status.newTemperatureData;
}

template < typename I2cMaster >
uint8_t*
xpcc::Tmp175<I2cMaster>::getData()
{
	status.newTemperatureData = false;
	return data;
}

template < typename I2cMaster >
void
xpcc::Tmp175<I2cMaster>::update()
{
	if (running != Running::Nothing)
	{
		switch (getAdapterState())
		{
			case xpcc::I2c::AdapterState::Idle:
				if (running == Running::ReadTemperature) {
					status.newTemperatureData = true;
				}

			case xpcc::I2c::AdapterState::Error:
				running = Running::Nothing;

			default:
				break;
		}
	}
	else  {
		if (status.startConversionPending)
		{
			buffer[0] = tmp175::REGISTER_CONFIGURATION;
			buffer[1] = config & tmp175::CONFIGURATION_ONE_SHOT;
			configureWriteRead(buffer, 2, data, 0);

			if (I2cMaster::start(this)) {
				status.startConversionPending = false;
				running = Running::StartConversion;
			}
		}
		else if (status.readTemperaturePending)
		{
			buffer[0] = tmp175::REGISTER_TEMPERATURE;
			configureWriteRead(buffer, 1, data, 2);

			if (I2cMaster::start(this)) {
				status.readTemperaturePending = false;
				running = Running::ReadTemperature;
			}
		}
	}
}


