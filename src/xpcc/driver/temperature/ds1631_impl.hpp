/*
 * Copyright (c) 2009-2011, Fabian Greif
 * Copyright (c) 2009, Martin Rosekeit
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

#ifndef XPCC_DS1631_HPP
#	error	"Don't include this file directly, use 'ds1631.hpp' instead!"
#endif

// ----------------------------------------------------------------------------
template <typename I2cMaster>
xpcc::Ds1631<I2cMaster>::Ds1631(Data &data, uint8_t address) :
	I2cDevice<I2cMaster,2>(address), data(data), config(0),
	periodTimeout(250), conversionTimeout(250), updateTime(250), conversionTime(232)
{
	this->stop();
}

template < typename I2cMaster >
xpcc::ResumableResult<bool>
xpcc::Ds1631<I2cMaster>::initialize()
{
	RF_BEGIN();

	buffer[0] = uint8_t(Command::Configuration);

	this->transaction.configureWriteRead(buffer, 1, &config, 1);

	RF_END_RETURN_CALL( this->runTransaction() );
}

template < typename I2cMaster >
bool
xpcc::Ds1631<I2cMaster>::run()
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
xpcc::ResumableResult<bool>
xpcc::Ds1631<I2cMaster>::setUpdateRate(uint8_t rate)
{
	RF_BEGIN();

	// clamp conversion rate to max 33Hz (=~30ms)
	if (rate == 0) rate = 1;
	if (rate > 33) rate = 33;

	if (config.any(Config::OneShot))
	{
		if (not RF_CALL(startConversion()))
			RF_RETURN(false);
	}

	updateTime = (1000/rate - 29);
	periodTimeout.restart(updateTime);
	this->restart();

	RF_END_RETURN(true);
}

template < typename I2cMaster >
xpcc::ResumableResult<bool>
xpcc::Ds1631<I2cMaster>::setResolution(Resolution resolution)
{
	RF_BEGIN();

	Resolution_t::set(config, resolution);

	RF_END_RETURN_CALL( writeConfiguration() );
}

template < typename I2cMaster >
xpcc::ResumableResult<bool>
xpcc::Ds1631<I2cMaster>::setAlertPolarity(AlertPolarity polarity)
{
	RF_BEGIN();

	config.update(Config::Polarity, bool(polarity));

	RF_END_RETURN_CALL( writeConfiguration() );
}

template < typename I2cMaster >
xpcc::ResumableResult<bool>
xpcc::Ds1631<I2cMaster>::setConversionMode(ConversionMode mode)
{
	RF_BEGIN();

	config.update(Config::OneShot, bool(mode));

	RF_END_RETURN_CALL( writeConfiguration() );
}

// MARK: read temperature
template < typename I2cMaster >
xpcc::ResumableResult<bool>
xpcc::Ds1631<I2cMaster>::readTemperature()
{
	RF_BEGIN();

	buffer[0] = uint8_t(Command::Temperature);
	this->transaction.configureWriteRead(buffer, 1, data.data, 2);

	RF_END_RETURN_CALL( this->runTransaction() );
}

// MARK: configuration
template < typename I2cMaster >
xpcc::ResumableResult<bool>
xpcc::Ds1631<I2cMaster>::writeConfiguration()
{
	RF_BEGIN();

	buffer[0] = uint8_t(Command::Configuration);
	buffer[1] = config.value;

	this->transaction.configureWrite(buffer, 2);

	RF_END_RETURN_CALL( this->runTransaction() );
}

template < typename I2cMaster >
xpcc::ResumableResult<bool>
xpcc::Ds1631<I2cMaster>::writeCommand(Command cmd)
{
	RF_BEGIN();

	buffer[0] = uint8_t(cmd);
	this->transaction.configureWrite(buffer, 1);

	RF_END_RETURN_CALL( this->runTransaction() );
}

template < typename I2cMaster >
xpcc::ResumableResult<bool>
xpcc::Ds1631<I2cMaster>::setLimitRegister(Command cmd, float temperature)
{
	RF_BEGIN();

	{
		uint8_t res = uint8_t(Resolution_t::get(config));

		int16_t temp = temperature * (2 << res);
		temp <<= 4 + res;

		buffer[0] = uint8_t(cmd);
		buffer[1] = (temp >> 8);
		buffer[2] = temp;
	}

	this->transaction.configureWrite(buffer, 3);

	RF_END_RETURN_CALL( this->runTransaction() );
}
