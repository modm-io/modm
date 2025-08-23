/*
 * Copyright (c) 2015, Niklas Hauser
 * Copyright (c) 2015, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_PCA8574_HPP
#	error "Don't include this file directly, use 'pca8574.hpp' instead!"
#endif

template < class I2cMaster >
modm::Pca8574<I2cMaster>::Pca8574(uint8_t address):
	I2cDevice<I2cMaster>(address),
	direction(Pins(0xff)), output(Pins(0xff)), input(Pins(0xff))
{
}

template < class I2cMaster >
bool
modm::Pca8574<I2cMaster>::set(Pins pins)
{
	// high is 1, low is 0
	// we can _always_ set the pins high
	output.set(pins);

	return writePort(output.value);
}

template < class I2cMaster >
bool
modm::Pca8574<I2cMaster>::reset(Pins pins)
{
	// high is 1, low is 0
	// only reset those that are actually configured as output
	output.reset(pins & direction);

	return writePort(output.value);
}

template < class I2cMaster >
bool
modm::Pca8574<I2cMaster>::toggle(Pins pins)
{
	// high is 1, low is 0
	// only toggle those that are actually configured as output
	output.toggle(pins & direction);

	return writePort(output.value);
}

template < class I2cMaster >
bool
modm::Pca8574<I2cMaster>::set(Pins pins, bool value)
{
	// high is 1, low is 0
	// only update those that are actually configured as output
	output.update(pins & direction, value);

	return writePort(output.value);
}

template < class I2cMaster >
bool
modm::Pca8574<I2cMaster>::setInput(Pins pins)
{
	// reset direction bits
	direction.reset(pins);
	// set pins high, which means input (open-drain, remember?)
	output.set(pins);

	return writePort(output.value);
}

template < class I2cMaster >
bool
modm::Pca8574<I2cMaster>::writePort(uint8_t value)
{
	output.value = value;
	return I2cDevice<I2cMaster>::write(&output.value, 1);
};

template < class I2cMaster >
bool
modm::Pca8574<I2cMaster>::readPort(uint8_t &value)
{

	if (I2cDevice<I2cMaster>::read(&input.value, 1))
	{
		value = input.value;
		return true;
	}

	return false;
};
