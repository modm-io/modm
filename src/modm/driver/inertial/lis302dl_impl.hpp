/*
 * Copyright (c) 2014-2015, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_LIS302DL_HPP
#	error  "Don't include this file directly, use 'lis302dl.hpp' instead!"
#endif

// ----------------------------------------------------------------------------
// MARK: LIS302 DRIVER
template < class Transport >
modm::Lis302dl<Transport>::Lis302dl(Data &data, uint8_t address)
:	Transport(address), data(data), rawBuffer{0,0,0,0,0,0,0,0,0,0}
{
}

template < class Transport >
modm::ResumableResult<bool>
modm::Lis302dl<Transport>::configure(Scale scale, MeasurementRate rate)
{
	return updateControlRegister(r(scale) | r(rate) | Control1_t(0x47));
}

template < class Transport >
modm::ResumableResult<bool>
modm::Lis302dl<Transport>::updateControlRegister(uint8_t index, Control_t setMask, Control_t clearMask)
{
	RF_BEGIN();

	rawBuffer[index] = (rawBuffer[index] & ~clearMask.value) | setMask.value;
	if (index == 0)
		data.meta = bool(Control1_t(rawBuffer[0]) & Control1::FS);

	RF_END_RETURN_CALL(this->write(i(Register::CtrlReg1) + index, rawBuffer[index]));
}

template < class Transport >
modm::ResumableResult<bool>
modm::Lis302dl<Transport>::setClickThreshold(Axis axis, uint8_t threshold)
{
	switch(axis)
	{
		case Axis::X:
			return this->updateRegister(i(Register::ClickThsYX), threshold & 0x0F, 0x0F);

		case Axis::Y:
			return this->updateRegister(i(Register::ClickThsYX), threshold << 4, 0xF0);

		default:
//		case Axis::Z:
			return this->write(i(Register::ClickThsZ), threshold & 0x0F);
	}
}

template < class Transport >
modm::ResumableResult<bool>
modm::Lis302dl<Transport>::readAcceleration()
{
	RF_BEGIN();

	if (RF_CALL(this->read(i(Register::Status) | Transport::AddressIncrement, rawBuffer + 3, 7)))
	{
		data.data[0] = rawBuffer[5];
		data.data[1] = rawBuffer[7];
		data.data[2] = rawBuffer[9];
		RF_RETURN(true);
	}

	RF_END_RETURN(false);
}

// ----------------------------------------------------------------------------
template < class Transport >
modm::ResumableResult<bool>
modm::Lis302dl<Transport>::updateRegister(uint8_t reg, uint8_t setMask, uint8_t clearMask)
{
	RF_BEGIN();

	if (RF_CALL(this->read(reg, rawBuffer[4])))
	{
		rawBuffer[4] = (rawBuffer[4] & ~clearMask) | setMask;
		RF_RETURN_CALL(this->write(reg, rawBuffer[4]));
	}

	RF_END_RETURN(false);
}
