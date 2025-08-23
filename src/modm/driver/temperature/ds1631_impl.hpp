/*
 * Copyright (c) 2009, Martin Rosekeit
 * Copyright (c) 2009-2011, Fabian Greif
 * Copyright (c) 2011, Georgi Grinshpun
 * Copyright (c) 2012-2015, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_DS1631_HPP
#	error	"Don't include this file directly, use 'ds1631.hpp' instead!"
#endif

// ----------------------------------------------------------------------------
template <typename I2cMaster>
modm::Ds1631<I2cMaster>::Ds1631(Data &data, uint8_t address) :
	I2cDevice<I2cMaster,2>(address), data(data), config(0),
	updateTime(250), conversionTime(232),
	periodTimeout(updateTime), conversionTimeout(conversionTime)
{
	this->stop();
}

template < typename I2cMaster >
bool
modm::Ds1631<I2cMaster>::initialize()
{
	buffer[0] = uint8_t(Command::Configuration);

	this->transaction.configureWriteRead(buffer, 1, &config, 1);

	return this->runTransaction();
}

template < typename I2cMaster >
bool
modm::Ds1631<I2cMaster>::update()
{
	PT_BEGIN();

	while(true)
	{
		PT_WAIT_UNTIL(periodTimeout.isExpired());
		periodTimeout.restart(updateTime);

		if (config.none(Config::OneShot))
		{
			PT_CALL(startConversion());

			conversionTimeout.restart(conversionTime);
			PT_WAIT_UNTIL(conversionTimeout.isExpired());
		}

		PT_CALL(readTemperature());
	}

	PT_END();
}

template < typename I2cMaster >
bool
modm::Ds1631<I2cMaster>::setUpdateRate(uint8_t rate)
{
	// clamp conversion rate to max 33Hz (=~30ms)
	if (rate == 0) rate = 1;
	if (rate > 33) rate = 33;

	if (config.any(Config::OneShot))
	{
		if (not startConversion())
			return false;
	}

	updateTime = modm::ShortDuration(1000/rate - 29);
	periodTimeout.restart(updateTime);
	this->restart();

	return true;
}

template < typename I2cMaster >
bool
modm::Ds1631<I2cMaster>::setResolution(Resolution resolution)
{
	Resolution_t::set(config, resolution);

	return writeConfiguration();
}

template < typename I2cMaster >
bool
modm::Ds1631<I2cMaster>::setAlertPolarity(AlertPolarity polarity)
{
	config.update(Config::Polarity, bool(polarity));

	return writeConfiguration();
}

template < typename I2cMaster >
bool
modm::Ds1631<I2cMaster>::setConversionMode(ConversionMode mode)
{
	config.update(Config::OneShot, bool(mode));

	return writeConfiguration();
}

// MARK: read temperature
template < typename I2cMaster >
bool
modm::Ds1631<I2cMaster>::readTemperature()
{
	buffer[0] = uint8_t(Command::Temperature);
	this->transaction.configureWriteRead(buffer, 1, data.data, 2);

	return this->runTransaction();
}

// MARK: configuration
template < typename I2cMaster >
bool
modm::Ds1631<I2cMaster>::writeConfiguration()
{
	buffer[0] = uint8_t(Command::Configuration);
	buffer[1] = config.value;

	this->transaction.configureWrite(buffer, 2);

	return this->runTransaction();
}

template < typename I2cMaster >
bool
modm::Ds1631<I2cMaster>::writeCommand(Command cmd)
{
	buffer[0] = uint8_t(cmd);
	this->transaction.configureWrite(buffer, 1);

	return this->runTransaction();
}

template < typename I2cMaster >
bool
modm::Ds1631<I2cMaster>::setLimitRegister(Command cmd, float temperature)
{
	{
		uint8_t res = uint8_t(Resolution_t::get(config));

		int16_t temp = temperature * (2 << res);
		temp <<= 4 + res;

		buffer[0] = uint8_t(cmd);
		buffer[1] = (temp >> 8);
		buffer[2] = temp;
	}

	this->transaction.configureWrite(buffer, 3);

	return this->runTransaction();
}
