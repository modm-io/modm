// coding: utf-8
/* Copyright (c) 2013, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_HMC6343_HPP
#	error  "Don't include this file directly, use 'hmc6343.hpp' instead!"
#endif
#include <xpcc/math/utils/operator.hpp>

// ----------------------------------------------------------------------------
template < class I2cMaster >
xpcc::Hmc6343<I2cMaster>::Hmc6343(Data &data, uint8_t address)
:	data(data), timeout(500), i2cTask(I2cTask::Idle), i2cSuccess(0),
	adapter(address, i2cTask, i2cSuccess)
{
}

// MARK: - i2cTasks
// MARK: ping
template < class I2cMaster >
xpcc::co::Result<bool>
xpcc::Hmc6343<I2cMaster>::ping(void *ctx)
{
	CO_BEGIN(ctx);

	CO_WAIT_UNTIL(adapter.configurePing() && this->startTransaction(&adapter));

	i2cTask = I2cTask::Ping;
	CO_WAIT_WHILE(i2cTask == I2cTask::Ping);

	CO_END_RETURN(i2cSuccess == I2cTask::Ping);
}

// ----------------------------------------------------------------------------
// MARK: - register access
// MARK: write command
template < class I2cMaster >
xpcc::co::Result<bool>
xpcc::Hmc6343<I2cMaster>::writeCommand(void *ctx, Command command, uint16_t timeout)
{
	CO_BEGIN(ctx);

	buffer[0] = i(command);
	CO_WAIT_UNTIL(
			this->timeout.isExpired() &&
			adapter.configureWrite(buffer, 1) && this->startTransaction(&adapter)
	);

	i2cTask = i(command) + I2cTask::PostCommandBase;
	this->timeout.restart(timeout);
	CO_WAIT_WHILE(i2cTask == (i(command) + I2cTask::PostCommandBase));

	CO_END_RETURN(i2cSuccess == (i(command) + I2cTask::PostCommandBase));
}

// MARK: write register
template < class I2cMaster >
xpcc::co::Result<bool>
xpcc::Hmc6343<I2cMaster>::writeRegister(void *ctx, Register reg, uint8_t value)
{
	CO_BEGIN(ctx);

	buffer[0] = i(Command::WriteEeprom);
	buffer[1] = i(reg);
	buffer[2] = value;

	CO_WAIT_UNTIL(
			timeout.isExpired() &&
			adapter.configureWrite(buffer, 3) && this->startTransaction(&adapter)
	);

	i2cTask = i(reg) + I2cTask::WriteEepromBase;
	timeout.restart(10);
	CO_WAIT_WHILE(i2cTask == (i(reg) + I2cTask::WriteEepromBase));

	CO_END_RETURN(i2cSuccess == (i(reg) + I2cTask::WriteEepromBase));
}

// MARK: write 16bit register
template < class I2cMaster >
xpcc::co::Result<bool>
xpcc::Hmc6343<I2cMaster>::writeRegister(void *ctx, Register16 reg, uint16_t value)
{
	CO_BEGIN(ctx);

	// LSB
	if ( CO_CALL( writeRegister(ctx, static_cast<Register>(reg), value) ) )
	{
		// MSB
		if ( CO_CALL( writeRegister(ctx, static_cast<Register>(i(reg)+1), (value >> 8)) ) )
			CO_RETURN(true);
	}

	CO_END_RETURN(false);
}

// MARK: read register
template < class I2cMaster >
xpcc::co::Result<bool>
xpcc::Hmc6343<I2cMaster>::readRegister(void *ctx, Register reg, uint8_t &value)
{
	CO_BEGIN(ctx);

	buffer[0] = i(Command::ReadEeprom);
	buffer[1] = i(reg);
	CO_WAIT_UNTIL(
			timeout.isExpired() &&
			adapter.configureWrite(buffer, 2) && this->startTransaction(&adapter)
	);

	i2cTask = i(reg) + I2cTask::PostEepromBase;
	timeout.restart(10);
	CO_WAIT_WHILE(i2cTask == (i(reg) + I2cTask::PostEepromBase));

	if(i2cSuccess == (i(reg) + I2cTask::PostEepromBase))
	{
		CO_WAIT_UNTIL(
				timeout.isExpired() &&
				adapter.configureRead(&value, 1) && this->startTransaction(&adapter)
		);

		i2cTask = i(reg) + I2cTask::ReadEepromBase;
		CO_WAIT_WHILE(i2cTask == (i(reg) + I2cTask::ReadEepromBase));

		if (i2cSuccess == (i(reg) + I2cTask::ReadEepromBase))
			CO_RETURN(true);
	}

	CO_END_RETURN(false);
}

// MARK: read 16bit register
template < class I2cMaster >
xpcc::co::Result<bool>
xpcc::Hmc6343<I2cMaster>::readRegister(void *ctx, Register16 reg, uint16_t &value)
{
	CO_BEGIN(ctx);

	// LSB
	if ( CO_CALL( readRegister(ctx, static_cast<Register>(reg), buffer[2]) ) )
	{
		// MSB
		value = buffer[2];
		if ( CO_CALL( readRegister(ctx, static_cast<Register>(i(reg)+1), buffer[2]) ) )
		{
			// an optimization would be to take the uint8_t addresses of value
			// but then we would have to deal with endianess, and that headache is annoying.
			value |= (buffer[2] << 8);
			CO_RETURN(true);
		}
	}

	CO_END_RETURN(false);
}

// MARK: read 6 or 1 bytes of data
template < class I2cMaster >
xpcc::co::Result<bool>
xpcc::Hmc6343<I2cMaster>::readPostData(void *ctx, Command command, uint8_t offset, uint8_t readSize)
{
	CO_BEGIN(ctx);

	if (CO_CALL(writeCommand(ctx, command, 1)))
	{
		CO_WAIT_UNTIL(
				timeout.isExpired() &&
				adapter.configureRead(data.getPointer() + offset, readSize) && this->startTransaction(&adapter)
		);

		i2cTask = i(command) + I2cTask::ReadCommandBase;
		CO_WAIT_WHILE(i2cTask == (i(command) + I2cTask::ReadCommandBase));

		if (i2cSuccess == (i(command) + I2cTask::ReadCommandBase))
			CO_RETURN(true);
	}

	CO_END_RETURN(false);
}
