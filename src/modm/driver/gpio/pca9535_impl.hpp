/*
 * Copyright (c) 2015, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_PCA9535_HPP
#	error "Don't include this file directly, use 'pca9535.hpp' instead!"
#endif

// ----------------------------------------------------------------------------
template < typename I2cMaster >
modm::Pca9535<I2cMaster>::Pca9535(uint8_t address)
:	I2cDevice<I2cMaster,2>(address), memory()
{
}

// MARK: - Tasks
template < typename I2cMaster >
modm::ResumableResult<bool>
modm::Pca9535<I2cMaster>::setOutput(Pins pins)
{
	RF_BEGIN();

	// output is 0, input is 1
	memory.configuration.reset(pins);

	RF_END_RETURN_CALL( writeMemory(Index::Configuration) );
}

template < typename I2cMaster >
modm::ResumableResult<bool>
modm::Pca9535<I2cMaster>::set(Pins pins)
{
	RF_BEGIN();

	// high is 1, low is 0
	memory.output.set(pins);

	RF_END_RETURN_CALL( writeMemory(Index::Output) );
}

template < typename I2cMaster >
modm::ResumableResult<bool>
modm::Pca9535<I2cMaster>::reset(Pins pins)
{
	RF_BEGIN();

	// high is 1, low is 0
	memory.output.reset(pins);

	RF_END_RETURN_CALL( writeMemory(Index::Output) );
}

template < typename I2cMaster >
modm::ResumableResult<bool>
modm::Pca9535<I2cMaster>::toggle(Pins pins)
{
	RF_BEGIN();

	// high is 1, low is 0
	memory.output.toggle(pins);

	RF_END_RETURN_CALL( writeMemory(Index::Output) );
}

template < typename I2cMaster >
modm::ResumableResult<bool>
modm::Pca9535<I2cMaster>::set(Pins pins, bool value)
{
	RF_BEGIN();

	// high is 1, low is 0
	memory.output.update(pins, value);

	RF_END_RETURN_CALL( writeMemory(Index::Output) );
}

template < typename I2cMaster >
modm::ResumableResult<bool>
modm::Pca9535<I2cMaster>::setInput(Pins pins)
{
	RF_BEGIN();

	// output is 0, input is 1
	memory.configuration.set(pins);

	RF_END_RETURN_CALL( writeMemory(Index::Configuration) );
}

template < typename I2cMaster >
modm::ResumableResult<bool>
modm::Pca9535<I2cMaster>::setInvertInput(Pins pins)
{
	RF_BEGIN();

	// inverted is 1, normal is 0
	memory.polarity.set(pins);

	RF_END_RETURN_CALL( writeMemory(Index::Polarity) );
}

template < typename I2cMaster >
modm::ResumableResult<bool>
modm::Pca9535<I2cMaster>::resetInvertInput(Pins pins)
{
	RF_BEGIN();

	// inverted is 1, normal is 0
	memory.polarity.reset(pins);

	RF_END_RETURN_CALL( writeMemory(Index::Polarity) );
}

template < typename I2cMaster >
modm::ResumableResult<bool>
modm::Pca9535<I2cMaster>::writePort(uint16_t data)
{
	RF_BEGIN();

	// high is 1, low is 0
	memory.output.value = data;

	RF_END_RETURN_CALL( writeMemory(Index::Output) );
}

template < typename I2cMaster >
modm::ResumableResult<bool>
modm::Pca9535<I2cMaster>::readPort(uint16_t &data)
{
	RF_BEGIN();

	if ( RF_CALL(readInput()) )
	{
		// high is 1, low is 0
		data = memory.input.value;
		RF_RETURN( true );
	}

	RF_END_RETURN( false );
}

// MARK: write multilength register
template < class I2cMaster >
modm::ResumableResult<bool>
modm::Pca9535<I2cMaster>::writeMemory(Index index)
{
	RF_BEGIN();

	this->transaction.configureWrite(buffer + uint8_t(index), 3);

	RF_END_RETURN_CALL( this->runTransaction() );
}

// MARK: read multilength register
template < class I2cMaster >
modm::ResumableResult<bool>
modm::Pca9535<I2cMaster>::readMemory(Index index)
{
	RF_BEGIN();

	this->transaction.configureWriteRead(
			buffer + uint8_t(index)    , 1,
			buffer + uint8_t(index) + 1, 2);

	RF_END_RETURN_CALL( this->runTransaction() );
}
