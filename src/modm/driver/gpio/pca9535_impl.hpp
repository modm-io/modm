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
bool
modm::Pca9535<I2cMaster>::setOutput(Pins pins)
{
	// output is 0, input is 1
	memory.configuration.reset(pins);

	return writeMemory(Index::Configuration);
}

template < typename I2cMaster >
bool
modm::Pca9535<I2cMaster>::set(Pins pins)
{
	// high is 1, low is 0
	memory.output.set(pins);

	return writeMemory(Index::Output);
}

template < typename I2cMaster >
bool
modm::Pca9535<I2cMaster>::reset(Pins pins)
{
	// high is 1, low is 0
	memory.output.reset(pins);

	return writeMemory(Index::Output);
}

template < typename I2cMaster >
bool
modm::Pca9535<I2cMaster>::toggle(Pins pins)
{
	// high is 1, low is 0
	memory.output.toggle(pins);

	return writeMemory(Index::Output);
}

template < typename I2cMaster >
bool
modm::Pca9535<I2cMaster>::set(Pins pins, bool value)
{
	// high is 1, low is 0
	memory.output.update(pins, value);

	return writeMemory(Index::Output);
}

template < typename I2cMaster >
bool
modm::Pca9535<I2cMaster>::setInput(Pins pins)
{
	// output is 0, input is 1
	memory.configuration.set(pins);

	return writeMemory(Index::Configuration);
}

template < typename I2cMaster >
bool
modm::Pca9535<I2cMaster>::setInvertInput(Pins pins)
{
	// inverted is 1, normal is 0
	memory.polarity.set(pins);

	return writeMemory(Index::Polarity);
}

template < typename I2cMaster >
bool
modm::Pca9535<I2cMaster>::resetInvertInput(Pins pins)
{
	// inverted is 1, normal is 0
	memory.polarity.reset(pins);

	return writeMemory(Index::Polarity);
}

template < typename I2cMaster >
bool
modm::Pca9535<I2cMaster>::writePort(uint16_t data)
{
	// high is 1, low is 0
	memory.output.value = data;

	return writeMemory(Index::Output);
}

template < typename I2cMaster >
bool
modm::Pca9535<I2cMaster>::readPort(uint16_t &data)
{
	if ( readInput() )
	{
		// high is 1, low is 0
		data = memory.input.value;
		return true;
	}

	return false;
}

// MARK: write multilength register
template < class I2cMaster >
bool
modm::Pca9535<I2cMaster>::writeMemory(Index index)
{
	this->transaction.configureWrite(buffer + uint8_t(index), 3);

	return this->runTransaction();
}

// MARK: read multilength register
template < class I2cMaster >
bool
modm::Pca9535<I2cMaster>::readMemory(Index index)
{
	this->transaction.configureWriteRead(
			buffer + uint8_t(index)    , 1,
			buffer + uint8_t(index) + 1, 2);

	return this->runTransaction();
}
