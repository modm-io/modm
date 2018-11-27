// coding: utf-8
/*
 * Copyright (c) 2018 Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_PCA9548A_HPP
#define MODM_PCA9548A_HPP

#include <modm/architecture/interface/i2c_device.hpp>

namespace modm
{

/**
 * PCA9548A/TCA9548A: 8-channel I2C switch with reset
 *
 * This bidirectional 8-channel switch can be used to multiplex I2C busses.
 *
 * @ingroup driver_gpio
 *
 * @author  Sascha Schade
 */
template < typename I2cMaster >
class Pca9548a : public modm::I2cDevice< I2cMaster, 2 >
{
public:
	Pca9548a(uint8_t address=0b1110000);

	modm::ResumableResult<bool>
	setActiveChannel(uint8_t channel);

	modm::ResumableResult<bool>
	readCommandRegister(uint8_t &command_register);

	modm::ResumableResult<bool>
	writeCommandRegister(uint8_t command_register);

private:
	uint8_t buffer[1];
	uint8_t current_command_register = 0;

	static constexpr uint8_t CHANNELS = 8;
};

} // modm namespace

#include "pca9548a_impl.hpp"

#endif // MODM_PCA9548A_HPP
