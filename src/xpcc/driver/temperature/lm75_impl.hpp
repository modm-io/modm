/*
 * Copyright (c) 2009-2011, Fabian Greif
 * Copyright (c) 2010, Martin Rosekeit
 * Copyright (c) 2012, 2015, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_LM75_HPP
#	error  "Don't include this file directly, use 'lm75.hpp' instead!"
#endif

// ----------------------------------------------------------------------------
template < typename I2cMaster >
modm::Lm75<I2cMaster>::Lm75(Data &data, uint8_t address) :
	I2cDevice<I2cMaster, 2>(address),
	data(data), config_msb(0)
{
}

// ----------------------------------------------------------------------------
// MARK: - tasks
// MARK: Alert mode
template < typename I2cMaster >
modm::ResumableResult<bool>
modm::Lm75<I2cMaster>::configureAlertMode(ThermostatMode mode, AlertPolarity polarity, FaultQueue faults)
{
	RF_BEGIN();

	config_msb.update(Config1::ThermostatMode, bool(mode));
	config_msb.update(Config1::Polarity, bool(polarity));
	FaultQueue_t::set(config_msb, faults);

	buffer[0] = uint8_t(Register::Configuration);
	buffer[1] = config_msb.value;

	this->transaction.configureWrite(buffer, 2);

	RF_END_RETURN_CALL( this->runTransaction() );
}

// MARK: read temperature
template < typename I2cMaster >
modm::ResumableResult<bool>
modm::Lm75<I2cMaster>::readTemperature()
{
	RF_BEGIN();

	buffer[0] = uint8_t(Register::Temperature);
	this->transaction.configureWriteRead(buffer, 1, data.data, 2);

	RF_END_RETURN_CALL( this->runTransaction() );
}

// MARK: configuration
template < typename I2cMaster >
modm::ResumableResult<bool>
modm::Lm75<I2cMaster>::setLimitRegister(Register reg, float temperature)
{
	RF_BEGIN();

	{
		int16_t temp = temperature * 2.f;
		temp <<= 7;

		buffer[0] = uint8_t(reg);
		buffer[1] = (temp >> 8);
		buffer[2] = temp;
	}

	this->transaction.configureWrite(buffer, 3);

	RF_END_RETURN_CALL( this->runTransaction() );
}
