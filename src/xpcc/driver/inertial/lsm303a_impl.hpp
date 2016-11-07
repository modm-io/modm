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

#ifndef XPCC_LSM303A_HPP
#	error  "Don't include this file directly, use 'lsm303a.hpp' instead!"
#endif

#include <cstring>

// ----------------------------------------------------------------------------
// MARK: LIS302 DRIVER
template < class I2cMaster >
xpcc::Lsm303a<I2cMaster>::Lsm303a(Data &data, uint8_t address)
:	Lis3TransportI2c<I2cMaster>(address), data(data), rawBuffer{7,0,0,0,0,0, 0,0, 0,0,0,0,0,0, 0,0}
{
}

template < class I2cMaster >
xpcc::ResumableResult<bool>
xpcc::Lsm303a<I2cMaster>::configure(Scale scale, MeasurementRate rate)
{
	RF_BEGIN();

	// MeasurementRate must be set in Control1
	rawBuffer[0] = i(rate) | 0x07;
	// Scale must be set in Control4
	rawBuffer[3] = (r(scale) | Control4::HR | Control4::BLE).value;

	if (scale == Scale::G2) data.scale = 1;
	else if (scale == Scale::G4) data.scale = 2;
	else if (scale == Scale::G8) data.scale = 4;
	else data.scale = 12;

	if ( RF_CALL(this->write(i(Register::CTRL1), rawBuffer[0])) )
	{
		RF_RETURN_CALL( this->write(i(Register::CTRL4), rawBuffer[3]) );
	}
	RF_END_RETURN(false);
}

template < class I2cMaster >
xpcc::ResumableResult<bool>
xpcc::Lsm303a<I2cMaster>::updateControlRegister(uint8_t index, Control_t setMask, Control_t clearMask)
{
	RF_BEGIN();

	rawBuffer[index] = (rawBuffer[index] & ~clearMask.value) | setMask.value;
	// update the scale in the data object, if we update CTRL_REG4 (index 3)
	if (index == 3)
	{
		Scale scale = Scale((Control4_t(rawBuffer[3]) & (Control4::FS1 | Control4::FS0)).value);
		if (scale == Scale::G2) data.scale = 1;
		else if (scale == Scale::G4) data.scale = 2;
		else if (scale == Scale::G8) data.scale = 4;
		else data.scale = 12;
	}

	RF_END_RETURN_CALL(this->write(0x20 + index, rawBuffer[index]));
}

template < class I2cMaster >
xpcc::ResumableResult<bool>
xpcc::Lsm303a<I2cMaster>::readAcceleration()
{
	RF_BEGIN();

	if (RF_CALL(this->read(i(Register::STATUS) | 0x80, rawBuffer + 7, 9)))
	{
		// copy the memory
		std::memcpy(data.data, rawBuffer + 7, 6);
		RF_RETURN(true);
	}

	RF_END_RETURN(false);
}

// ----------------------------------------------------------------------------
template < class I2cMaster >
xpcc::ResumableResult<bool>
xpcc::Lsm303a<I2cMaster>::updateRegister(uint8_t reg, uint8_t setMask, uint8_t clearMask)
{
	RF_BEGIN();

	if (RF_CALL(this->read(reg, rawBuffer[8])))
	{
		rawBuffer[8] = (rawBuffer[8] & ~clearMask) | setMask;
		RF_RETURN_CALL(this->write(reg, rawBuffer[8]));
	}

	RF_END_RETURN(false);
}
