/*
 * Copyright (c) 2013, David Hebbeker
 * Copyright (c) 2013-2014, Sascha Schade
 * Copyright (c) 2013-2015, 2021, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_TCS3414_HPP
#	error	"Don't include this file directly, use 'tcs3414.hpp' instead!"
#endif


template < typename I2cMaster >
modm::Tcs3414<I2cMaster>::Tcs3414(Data &data, uint8_t address)
	: I2cDevice<I2cMaster,2>(address), data(data)
{}

// ----------------------------------------------------------------------------
template<typename I2cMaster>
modm::ResumableResult<bool>
modm::Tcs3414<I2cMaster>::writeRegister(RegisterAddress address, uint8_t value)
{
	RF_BEGIN();

	buffer[0] =	0x80 |				// write command
				0x40 |				// with SMB read/write block protocol
				uint8_t(address);	// at this address
	// buffer[1] contains ignored byte count
	buffer[2] = value;

	this->transaction.configureWrite(buffer, 2);

	RF_END_RETURN_CALL(this->runTransaction());
}

template<typename I2cMaster>
modm::ResumableResult<bool>
modm::Tcs3414<I2cMaster>::readRegisters(RegisterAddress address, uint8_t* values,
                                        uint8_t count)
{
	RF_BEGIN();

	buffer[0] =	0x80 |				// write command
				0x40 |				// with SMB read/write block protocol
				uint8_t(address);	// at this address

	this->transaction.configureWriteRead(buffer, 1, values, count);

	RF_END_RETURN_CALL(this->runTransaction());
}
