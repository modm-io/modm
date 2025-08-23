/*
 * Copyright (c) 2015, Christian Menard
 * Copyright (c) 2015, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_PCA9685_HPP
#	error "Don't include this file directly, use 'pca9685.hpp' instead!"
#endif

// ----------------------------------------------------------------------------
template<typename I2cMaster>
modm::Pca9685<I2cMaster>::Pca9685(uint8_t address) :
	I2cDevice<I2cMaster>(address)
{}

template<typename I2cMaster>
bool
modm::Pca9685<I2cMaster>::initialize(uint8_t mode1, uint8_t mode2)
{
	// set the first mode register
	buffer[0] = REG_MODE1;
	buffer[1] = mode1 | MODE1_AI;  // ensure that auto increment is enabled
	if (not I2cDevice<I2cMaster>::write(buffer, 2))
		return false;

	// set the second mode register

	buffer[0] = REG_MODE2;
	buffer[1] = mode2;
	if (not I2cDevice<I2cMaster>::write(buffer, 2))
		return false;

	// Always turn on all LEDs at tick 0 and switch them of later according
	// to the current value

	buffer[0] = REG_ALL_LED_ON_L;
	buffer[1] = 0x00;
	buffer[2] = 0x00;
	return I2cDevice<I2cMaster>::write(buffer, 3);
}

template<typename I2cMaster>
bool
modm::Pca9685<I2cMaster>::setChannel(uint8_t channel, uint16_t value)
{
	// there are only 16 channels
	if (channel >= 16)
		return false;

	buffer[0] = REG_LED0_OFF_L + 4 * channel;
	// The Controller turns all LEDs on at tick 0
	// and turns this LED of at value
	buffer[1] = uint8_t(value);
	buffer[2] = uint8_t(value >> 8) & 0x0f;
	return I2cDevice<I2cMaster>::write(buffer, 3);
}

template<typename I2cMaster>
bool
modm::Pca9685<I2cMaster>::setAllChannels(uint16_t value)
{
	buffer[0] = REG_ALL_LED_OFF_L;
	// The Controller turns all LEDs on at tick 0
	// and turns this LED of at tick $value
	buffer[1] = uint8_t(value);
	buffer[2] = uint8_t(value >> 8) & 0x0f;
	return I2cDevice<I2cMaster>::write(buffer, 3);
}
