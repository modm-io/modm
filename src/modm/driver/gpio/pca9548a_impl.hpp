// coding: utf-8
/*
 * Copyright (c) 2018, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_PCA9548A_HPP
#	error "Don't include this file directly, use 'pca9548a.hpp' instead!"
#endif

#include <modm/debug/logger.hpp>

template < typename I2cMaster >
modm::Pca9548a<I2cMaster>::Pca9548a(uint8_t address)
:	I2cDevice<I2cMaster, 2>{address}
{
}

template < typename I2cMaster >
modm::ResumableResult<bool>
modm::Pca9548a<I2cMaster>::setActiveChannel(uint8_t channel)
{
	RF_BEGIN();

	if (channel >= CHANNELS) {
		RF_RETURN(false);
	}

	// Channel to mask
	RF_END_RETURN_CALL( writeCommandRegister(1 << channel) );
}

template < typename I2cMaster >
modm::ResumableResult<bool>
modm::Pca9548a<I2cMaster>::readCommandRegister(uint8_t &command_register)
{
	RF_BEGIN();

	this->transaction.configureRead(buffer, 1);
	command_register = buffer[0];
	current_command_register = command_register;

	RF_END_RETURN_CALL( this->runTransaction() );
}

template < typename I2cMaster >
modm::ResumableResult<bool>
modm::Pca9548a<I2cMaster>::writeCommandRegister(uint8_t command_register)
{
	RF_BEGIN();

	if (current_command_register == command_register) {
		// No change, return without bus operation
		RF_RETURN(true);
	}
	current_command_register = command_register;

	buffer[0] = command_register;
	this->transaction.configureWrite(buffer, 1);

	RF_END_RETURN_CALL( this->runTransaction() );
}
