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
xpcc::co::Result<bool>
xpcc::Lis302dl<Transport>::initialize(void *ctx, Scale scale, MeasurementRate rate)
{
	return updateControlRegister(ctx, r(scale) | r(rate) | static_cast<Control1>(0x47));
}

template < class Transport >
xpcc::co::Result<bool>
xpcc::Lis302dl<Transport>::updateControlRegister(void *ctx, uint8_t index, Control_t setMask, Control_t clearMask)
{
	CO_BEGIN(ctx);

	rawBuffer[index] = (rawBuffer[index] & ~clearMask.value) | setMask.value;
	if (index == 0)
		data.getPointer()[3] = rawBuffer[0];

	CO_END_RETURN_CALL(this->write(ctx, i(Register::CtrlReg1) + index, rawBuffer[index]));
}

template < class Transport >
xpcc::co::Result<bool>
xpcc::Lis302dl<Transport>::writeClickThreshold(void *ctx, Axis axis, uint8_t threshold)
{
	switch(axis)
	{
		case Axis::X:
			return this->updateRegister(ctx, i(Register::ClickThsYX), threshold & 0x0F, 0x0F);

		case Axis::Y:
			return this->updateRegister(ctx, i(Register::ClickThsYX), threshold << 4, 0xF0);

		default:
//		case Axis::Z:
			return this->write(ctx, i(Register::ClickThsZ), threshold & 0x0F);
	}
}

template < class Transport >
xpcc::co::Result<bool>
xpcc::Lis302dl<Transport>::readAcceleration(void *ctx)
{
	CO_BEGIN(ctx);

	if (CO_CALL(this->read(ctx, i(Register::Status) | Transport::AddressIncrement, rawBuffer + 3, 7)))
	{
		data.getPointer()[0] = rawBuffer[5];
		data.getPointer()[1] = rawBuffer[7];
		data.getPointer()[2] = rawBuffer[9];
		CO_RETURN(true);
	}

	CO_END_RETURN(false);
}

// ----------------------------------------------------------------------------
template < class Transport >
xpcc::co::Result<bool>
xpcc::Lis302dl<Transport>::updateRegister(void *ctx, uint8_t reg, uint8_t setMask, uint8_t clearMask)
{
	CO_BEGIN(ctx);

	if (CO_CALL(this->read(ctx, reg, rawBuffer[4])))
	{
		rawBuffer[4] = (rawBuffer[4] & ~clearMask) | setMask;
		CO_RETURN_CALL(this->write(ctx, reg, rawBuffer[4]));
	}

	CO_END_RETURN(false);
}
