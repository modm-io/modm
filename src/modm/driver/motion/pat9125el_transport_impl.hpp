/*
 * Copyright (c) 2018, Christopher Durand
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_PAT9125EL_HPP
#	error  "Don't include this file directly, use 'pat9125el_transport.hpp' instead!"
#endif

namespace modm
{

template<class I2cMaster>
Pat9125elI2cTransport<I2cMaster>::Pat9125elI2cTransport(uint8_t address)
	: I2cDevice<I2cMaster, 4>{address}
{
}

template<class I2cMaster>
modm::ResumableResult<bool>
Pat9125elI2cTransport<I2cMaster>::write(uint8_t reg, uint8_t value)
{
	RF_BEGIN();

	buffer[0] = reg;
	buffer[1] = value;

	this->transaction.configureWrite(&buffer[0], 2);

	RF_END_RETURN_CALL(this->runTransaction());
}

template<class I2cMaster>
modm::ResumableResult<bool>
Pat9125elI2cTransport<I2cMaster>::read(uint8_t reg, uint8_t& value)
{
	return read(reg, &value, 1);
}

template<class I2cMaster>
modm::ResumableResult<bool>
Pat9125elI2cTransport<I2cMaster>::read(uint8_t reg, uint8_t* buffer, uint8_t length)
{
	RF_BEGIN();

	buffer[0] = reg;
	this->transaction.configureWriteRead(&buffer[0], 1, buffer, length);

	RF_END_RETURN_CALL(this->runTransaction());
}

}
