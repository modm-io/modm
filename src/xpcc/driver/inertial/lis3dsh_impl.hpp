// coding: utf-8
/* Copyright (c) 2013, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_LIS3DSH_HPP
#	error  "Don't include this file directly, use 'lis3dsh.hpp' instead!"
#endif

#include <cstring>

// ----------------------------------------------------------------------------
// MARK: LIS302 DRIVER
template < class Transport >
xpcc::Lis3dsh<Transport>::Lis3dsh(Data &data, uint8_t address)
:	Transport(address), data(data), rawBuffer{7,0,0,0,0,0, 0, 0,0,0,0,0,0, 0,0}
{
}

template < class Transport >
xpcc::ResumableResult<bool>
xpcc::Lis3dsh<Transport>::configure(Scale scale, MeasurementRate rate)
{
	CO_BEGIN();

	// MeasurementRate must be set in Control4
	rawBuffer[0] = i(rate) | 0x07;
	// Scale must be set in Control5
	rawBuffer[4] = i(scale);
	{
		// see updateControlRegister
		uint8_t s = rawBuffer[4] >> 2;
		data.meta = (s == 8) ? s + 8 : s + 2;
	}

	rawBuffer[5] = uint8_t(Control6::ADD_INC);

	if ( CO_CALL(this->write(i(Register::CTRL_REG4), rawBuffer[0])) )
	{
		if ( CO_CALL(this->write(i(Register::CTRL_REG5), rawBuffer[4])) )
		{
			CO_RETURN_CALL(this->write(i(Register::CTRL_REG6), rawBuffer[5]));
		}
	}
	CO_END_RETURN(false);
}

template < class Transport >
xpcc::ResumableResult<bool>
xpcc::Lis3dsh<Transport>::updateControlRegister(uint8_t index, Control_t setMask, Control_t clearMask)
{
	CO_BEGIN();

	rawBuffer[index] = (rawBuffer[index] & ~clearMask.value) | setMask.value;
	// update the scale in the data object, if we update CTRL_REG5 (index 4)
	if (index == 4)
	{
		// only shift by 2, not 3
		uint8_t scale = (Control5_t(rawBuffer[4]) & Control5::FSCALE_Mask).value >> 2;
		// for G2, G6, G8 we can simply add 2 to the result.
		// this does not work for G16, where we have to add 8
		data.meta = (scale == 8) ? scale + 8 : scale + 2;
	}

	CO_END_RETURN_CALL(this->write(0x20 + index, rawBuffer[index]));
}

template < class Transport >
xpcc::ResumableResult<bool>
xpcc::Lis3dsh<Transport>::readAcceleration()
{
	CO_BEGIN();

	if (CO_CALL(this->read(i(Register::STATUS), rawBuffer + 6, 9)))
	{
		// copy the memory
		std::memcpy(data.data, rawBuffer + 7, 6);
		CO_RETURN(true);
	}

	CO_END_RETURN(false);
}

// ----------------------------------------------------------------------------
template < class Transport >
xpcc::ResumableResult<bool>
xpcc::Lis3dsh<Transport>::updateRegister(uint8_t reg, uint8_t setMask, uint8_t clearMask)
{
	CO_BEGIN();

	if (CO_CALL(this->read(reg, rawBuffer[7])))
	{
		rawBuffer[7] = (rawBuffer[7] & ~clearMask) | setMask;
		CO_RETURN_CALL(this->write(reg, rawBuffer[7]));
	}

	CO_END_RETURN(false);
}
