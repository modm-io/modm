// coding: utf-8
/* Copyright (c) 2009, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_LM75_HPP
#	error  "Don't include this file directly, use 'lm75.hpp' instead!"
#endif

// ----------------------------------------------------------------------------
template < typename I2cMaster >
xpcc::Lm75<I2cMaster>::Lm75(Data &data, uint8_t address) :
	I2cDevice<I2cMaster, 2>(address),
	data(data), config_msb(0)
{
}

// ----------------------------------------------------------------------------
// MARK: - tasks
// MARK: Alert mode
template < typename I2cMaster >
xpcc::co::ResumableResult<bool>
xpcc::Lm75<I2cMaster>::configureAlertMode(ThermostatMode mode, AlertPolarity polarity, FaultQueue faults)
{
	CO_BEGIN();

	config_msb.update(Config1::ThermostatMode, bool(mode));
	config_msb.update(Config1::Polarity, bool(polarity));
	FaultQueue_t::set(config_msb, faults);

	buffer[0] = uint8_t(Register::Configuration);
	buffer[1] = config_msb.value;

	this->transaction.configureWrite(buffer, 2);

	CO_END_RETURN_CALL( this->runTransaction() );
}

// MARK: read temperature
template < typename I2cMaster >
xpcc::co::ResumableResult<bool>
xpcc::Lm75<I2cMaster>::readTemperature()
{
	CO_BEGIN();

	buffer[0] = uint8_t(Register::Temperature);
	this->transaction.configureWriteRead(buffer, 1, data.data, 2);

	CO_END_RETURN_CALL( this->runTransaction() );
}

// MARK: configuration
template < typename I2cMaster >
xpcc::co::ResumableResult<bool>
xpcc::Lm75<I2cMaster>::setLimitRegister(Register reg, float temperature)
{
	CO_BEGIN();

	{
		int16_t temp = temperature * 2.f;
		temp <<= 7;

		buffer[0] = uint8_t(reg);
		buffer[1] = (temp >> 8);
		buffer[2] = temp;
	}

	this->transaction.configureWrite(buffer, 3);

	CO_END_RETURN_CALL( this->runTransaction() );
}
