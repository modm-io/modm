// coding: utf-8
/* Copyright (c) 2013, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_LIS302DL_HPP
#	error  "Don't include this file directly, use 'lis302dl.hpp' instead!"
#endif

// ----------------------------------------------------------------------------
// MARK: LIS302 DRIVER
template < class Transport >
xpcc::Lis302dl<Transport>::Lis302dl(Data &data, uint8_t address)
:	Transport(address), data(data), rawBuffer{0,0,0,0,0,0,0,0,0,0}
{
}

template < class Transport >
xpcc::co::ResumableResult<bool>
xpcc::Lis302dl<Transport>::configure(Scale scale, MeasurementRate rate)
{
	return updateControlRegister(r(scale) | r(rate) | Control1_t(0x47));
}

template < class Transport >
xpcc::co::ResumableResult<bool>
xpcc::Lis302dl<Transport>::updateControlRegister(uint8_t index, Control_t setMask, Control_t clearMask)
{
	CO_BEGIN();

	rawBuffer[index] = (rawBuffer[index] & ~clearMask.value) | setMask.value;
	if (index == 0)
		data.meta = bool(Control1_t(rawBuffer[0]) & Control1::FS);

	CO_END_RETURN_CALL(this->write(i(Register::CtrlReg1) + index, rawBuffer[index]));
}

template < class Transport >
xpcc::co::ResumableResult<bool>
xpcc::Lis302dl<Transport>::writeClickThreshold(Axis axis, uint8_t threshold)
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
xpcc::co::ResumableResult<bool>
xpcc::Lis302dl<Transport>::readAcceleration()
{
	CO_BEGIN();

	if (CO_CALL(this->read(i(Register::Status) | Transport::AddressIncrement, rawBuffer + 3, 7)))
	{
		data.data[0] = rawBuffer[5];
		data.data[1] = rawBuffer[7];
		data.data[2] = rawBuffer[9];
		CO_RETURN(true);
	}

	CO_END_RETURN(false);
}

// ----------------------------------------------------------------------------
template < class Transport >
xpcc::co::ResumableResult<bool>
xpcc::Lis302dl<Transport>::updateRegister(uint8_t reg, uint8_t setMask, uint8_t clearMask)
{
	CO_BEGIN();

	if (CO_CALL(this->read(reg, rawBuffer[4])))
	{
		rawBuffer[4] = (rawBuffer[4] & ~clearMask) | setMask;
		CO_RETURN_CALL(this->write(reg, rawBuffer[4]));
	}

	CO_END_RETURN(false);
}
