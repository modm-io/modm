// coding: utf-8
/* Copyright (c) 2011, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_HMC58X3_HPP
#	error "Don't include this file directly, use 'hmc58x3.hpp' instead!"
#endif

// ----------------------------------------------------------------------------
template < typename I2cMaster >
xpcc::Hmc58x3<I2cMaster>::Hmc58x3(Data &data, uint8_t address)
:	data(data), i2cTask(I2cTask::Idle), i2cSuccess(0),
	adapter(address, i2cTask, i2cSuccess),
	rawBuffer{0x10, 0x20, 0x01, 0,0,0,0,0,0, 0x00}
{
}

// MARK: - i2cTasks
// MARK: ping
template < class I2cMaster >
xpcc::co::Result<bool>
xpcc::Hmc58x3<I2cMaster>::ping()
{
	CO_BEGIN();

	CO_WAIT_UNTIL(adapter.configurePing() &&
			(i2cTask = I2cTask::Ping, this->startTransaction(&adapter)));

	CO_WAIT_WHILE(i2cTask == I2cTask::Ping);

	CO_END_RETURN(i2cSuccess == I2cTask::Ping);
}

template < typename I2cMaster >
xpcc::co::Result<bool>
xpcc::Hmc58x3<I2cMaster>::readMagneticField()
{
	CO_BEGIN();

	if (CO_CALL(read(Register::DataX_Lsb, rawBuffer+3, 7)))
	{
		std::memcpy(data.data, rawBuffer+3, 6);
		CO_RETURN(true);
	}

	CO_END_RETURN(false);
}

// ----------------------------------------------------------------------------
// MARK: - base methods
template < typename I2cMaster >
xpcc::co::Result<bool>
xpcc::Hmc58x3<I2cMaster>::configureRaw(uint8_t rate, uint8_t gain, const uint8_t* gainValues, uint8_t average)
{
	CO_BEGIN();

	rawBuffer[0] = rate | average;
	rawBuffer[1] = gain;
	rawBuffer[2] = uint8_t(OperationMode::ContinousConversion);

	if (CO_CALL(write(Register::ConfigA, rawBuffer, 3)))
	{
		// look-up the gain for the data object
		data.gain = gainValues[((gain >> 5) & 0x07)];
		CO_RETURN(true);
	}

	CO_END_RETURN(false);
}

template < typename I2cMaster >
xpcc::co::Result<bool>
xpcc::Hmc58x3<I2cMaster>::setGainRaw(uint8_t gain, const uint8_t* gainValues)
{
	CO_BEGIN();

	rawBuffer[1] = (rawBuffer[1] & ~uint8_t(ConfigB::GN_Mask)) | gain;

	if (CO_CALL(write(Register::ConfigB, rawBuffer[1])))
	{
		// look-up the gain for the data object
		data.gain = gainValues[((gain >> 5) & 0x07)];
		CO_RETURN(true);
	}

	CO_END_RETURN(false);
}

// ----------------------------------------------------------------------------
// MARK: - register access
template < typename I2cMaster >
xpcc::co::Result<bool>
xpcc::Hmc58x3<I2cMaster>::readStatus()
{
	return read(Register::Status, rawBuffer[9]);
}

// MARK: update register
template < typename I2cMaster >
xpcc::co::Result<bool>
xpcc::Hmc58x3<I2cMaster>::updateRegister(uint8_t index, uint8_t setMask, uint8_t clearMask)
{
	CO_BEGIN();

	rawBuffer[index] = (rawBuffer[index] & ~clearMask) | setMask;

	CO_END_RETURN_CALL(write(Register(index), rawBuffer[index]));
}

// MARK: write multilength register
template < class I2cMaster >
xpcc::co::Result<bool>
xpcc::Hmc58x3<I2cMaster>::write(Register reg, uint8_t *buffer, uint8_t length)
{
	CO_BEGIN();

	if (length > 5)
		CO_RETURN(false);

	rawBuffer[3] = uint8_t(reg);
	std::memcpy(rawBuffer+4, buffer, length);

	CO_WAIT_UNTIL(
			adapter.configureWrite(rawBuffer+3, length+1) and
					(i2cTask = I2cTask::WriteRegister, this->startTransaction(&adapter))
	);

	CO_WAIT_WHILE(i2cTask == I2cTask::WriteRegister);

	CO_END_RETURN(i2cSuccess == I2cTask::WriteRegister);
}

// MARK: read multilength register
template < class I2cMaster >
xpcc::co::Result<bool>
xpcc::Hmc58x3<I2cMaster>::read(Register reg, uint8_t *buffer, uint8_t length)
{
	CO_BEGIN();

	rawBuffer[3] = uint8_t(reg);

	CO_WAIT_UNTIL(
			adapter.configureWriteRead(rawBuffer+3, 1, buffer, length) and
					(i2cTask = I2cTask::ReadRegister, this->startTransaction(&adapter))
	);

	CO_WAIT_WHILE(i2cTask == I2cTask::ReadRegister);

	CO_END_RETURN(i2cSuccess == I2cTask::ReadRegister);
}
