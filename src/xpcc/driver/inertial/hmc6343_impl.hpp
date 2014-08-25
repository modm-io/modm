// coding: utf-8
/* Copyright (c) 2013, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC__HMC6343_HPP
#	error  "Don't include this file directly, use 'hmc6343.hpp' instead!"
#endif
#include <xpcc/math/utils/operator.hpp>
#include <xpcc/math/utils/endianess.hpp>
#import "hmc6343.hpp"
#import "macros.hpp"
#import "i2c.hpp"

// ----------------------------------------------------------------------------
template < typename I2cMaster >
xpcc::Hmc6343<I2cMaster>::Hmc6343(uint8_t* data, uint8_t address)
:	I2cDevice<I2cMaster>(), Protoface(), timeout(500), task(Task::Idle), success(0),
	data(data), adapter(address, task, success)
{
}

// MARK: - tasks
// MARK: ping
template < typename I2cMaster >
bool
xpcc::Hmc6343<I2cMaster>::startPing()
{
	if (task == Task::Idle && timeout.isExpired() &&
		adapter.configurePing() && this->startTransaction(&adapter))
	{
		task = Task::Ping;
		return true;
	}
	return false;
}

template < typename I2cMaster >
bool
xpcc::Hmc6343<I2cMaster>::runPing()
{
	return (task == Task::Ping);
}

template < typename I2cMaster >
bool
xpcc::Hmc6343<I2cMaster>::isPingSuccessful()
{
	return (success == Task::Ping);
}

// COMMANDS ----------------------------------------------------------------------------
// MARK: set orientation
template < typename I2cMaster >
bool
xpcc::Hmc6343<I2cMaster>::startSetOrientation(Orientation orientation)
{ return startWriteCommand(static_cast<Command>(orientation));
}

template < typename I2cMaster >
bool
xpcc::Hmc6343<I2cMaster>::runSetOrientation()
{
	// we don't want to remember the orientation that the user picked, so we check for all
	return (runWriteCommand(Command::LevelOrientation) ||
			runWriteCommand(Command::UprightSidewaysOrientation) ||
			runWriteCommand(Command::UprightFlatFrontOrientation));
}

template < typename I2cMaster >
bool
xpcc::Hmc6343<I2cMaster>::isSetOrientationSuccessful()
{
	// we don't want to remember the orientation that the user picked, so we check for all
	return (isWriteCommandSuccessful(Command::LevelOrientation) ||
			isWriteCommandSuccessful(Command::UprightSidewaysOrientation) ||
			isWriteCommandSuccessful(Command::UprightFlatFrontOrientation));
}

// ----------------------------------------------------------------------------
// MARK: - data access
template < typename I2cMaster >
int16_t
xpcc::Hmc6343<I2cMaster>::swapData(uint8_t index)
{
	uint16_t* rawData = reinterpret_cast<uint16_t*>(data);
	return static_cast<int16_t>(xpcc::bigEndianToHost(rawData[index]));
}

// ----------------------------------------------------------------------------
// MARK: - register access
// MARK: write command
template < typename I2cMaster >
bool
xpcc::Hmc6343<I2cMaster>::startWriteCommand(Command command, uint16_t timeout)
{
	if(task == Task::Idle && this->timeout.isExpired())
	{
		buffer[0] = static_cast<uint8_t>(command);

		if (adapter.configureWrite(buffer, 1) && this->startTransaction(&adapter))
		{
			task = static_cast<uint8_t>(command) + Task::PostCommandBase;
			// 10ms timing delay when writing to the chips EEPROM
			this->timeout.restart(timeout);
			return true;
		}
	}
	return false;
}

template < typename I2cMaster >
bool
xpcc::Hmc6343<I2cMaster>::runWriteCommand(Command command)
{
	return (task == (static_cast<uint8_t>(command) + Task::PostCommandBase));
}

template < typename I2cMaster >
bool
xpcc::Hmc6343<I2cMaster>::isWriteCommandSuccessful(Command command)
{
	return (success == (static_cast<uint8_t>(command) + Task::PostCommandBase));
}

// MARK: write register
template < typename I2cMaster >
bool
xpcc::Hmc6343<I2cMaster>::startWriteRegister(Register reg, uint8_t value)
{
	if(task == Task::Idle)
	{
		return startWriteRegisterIgnoreTaskCheck(reg, value);
	}
	return false;
}

template < typename I2cMaster >
bool
xpcc::Hmc6343<I2cMaster>::startWriteRegisterIgnoreTaskCheck(Register reg, uint8_t value)
{
	if(timeout.isExpired())
	{
		buffer[0] = static_cast<uint8_t>(Command::WriteEeprom);
		buffer[1] = static_cast<uint8_t>(reg);
		buffer[2] = value;

		if (adapter.configureWrite(buffer, 3) && this->startTransaction(&adapter))
		{
			task = static_cast<uint8_t>(reg) + Task::WriteEepromBase;
			// 10ms timing delay when writing to the chips EEPROM
			timeout.restart(10);
			return true;
		}
	}
	return false;
}

template < typename I2cMaster >
bool
xpcc::Hmc6343<I2cMaster>::runWriteRegister(Register reg)
{
	return (task == (static_cast<uint8_t>(reg) + Task::WriteEepromBase));
}

template < typename I2cMaster >
bool
xpcc::Hmc6343<I2cMaster>::isWriteRegisterSuccessful(Register reg)
{
	return (success == (static_cast<uint8_t>(reg) + Task::WriteEepromBase));
}

// MARK: write 16bit register
template < typename I2cMaster >
bool
xpcc::Hmc6343<I2cMaster>::startWrite16BitRegister(Register16 reg, uint16_t value)
{
	// write MSB first, then LSB
	if (task == Task::Idle && adapter.setPreserveTag() && startWriteRegisterIgnoreTaskCheck(reg, value >> 8))
	{
		registerBufferLSB = value;
		this->startTask(NPtTask::Write16BitRegister);
		return true;
	}
	return false;
}

template < typename I2cMaster >
bool
xpcc::Hmc6343<I2cMaster>::runWrite16BitRegister(Register16 reg)
{
	NPT_BEGIN(NPtTask::Write16BitRegister);

	NPT_WAIT_UNTIL(adapter.getState() != xpcc::I2c::AdapterState::Busy);
	if (isWriteRegisterSuccessful(static_cast<Register>(static_cast<uint8_t>(reg)+1)))
	{
		NPT_WAIT_UNTIL(startWriteRegisterIgnoreTaskCheck(static_cast<Register>(reg),registerBufferLSB));
		NPT_WAIT_WHILE(runWriteRegister(static_cast<Register>(reg));
	}

	NPT_END();
}

template < typename I2cMaster >
bool
xpcc::Hmc6343<I2cMaster>::isWrite16BitRegisterSuccessful(Register16 reg)
{ return isWriteRegisterSuccessful(static_cast<Register>(reg));
}

// MARK: read register
template < typename I2cMaster >
bool
xpcc::Hmc6343<I2cMaster>::startReadRegister(Register reg)
{
	if(task == Task::Idle)
	{
		return startReadRegisterIgnoreTaskCheck(reg);
	}
	return false;
}

template < typename I2cMaster >
bool
xpcc::Hmc6343<I2cMaster>::startReadRegisterIgnoreTaskCheck(Register reg)
{
	if(timeout.isExpired())
	{
		buffer[0] = static_cast<uint8_t>(Command::ReadEeprom);
		buffer[1] = static_cast<uint8_t>(reg);

		if (adapter.configureWrite(buffer, 2) && adapter.setPreserveTag() && this->startTransaction(&adapter))
		{
			task = static_cast<uint8_t>(reg) + Task::PostEepromBase;
			// 10ms timing delay when writing to the chips EEPROM
			timeout.restart(10);
			this->startTask(NPtTask::ReadEeprom);
			return true;
		}
	}
	return false;
}

template < typename I2cMaster >
bool
xpcc::Hmc6343<I2cMaster>::runReadRegister(Register reg)
{
	NPT_BEGIN(NPtTask::ReadEeprom);

	NPT_WAIT_UNTIL(adapter.getState() != xpcc::I2c::AdapterState::Busy);

	if (success == (static_cast<uint8_t>(reg) + Task::PostEepromBase))
	{
		NPT_WAIT_UNTIL(timeout.isExpired());
		NPT_WAIT_UNTIL(adapter.configureRead(&registerBufferLSB, 1) &&
				this->startTransaction(&adapter));
		task = static_cast<uint8_t>(reg) + Task::ReadEepromBase;

		NPT_WAIT_WHILE(task == (static_cast<uint8_t>(reg) + Task::ReadEepromBase));
	}

	NPT_END();
}

template < typename I2cMaster >
bool
xpcc::Hmc6343<I2cMaster>::isReadRegisterSuccessful(Register reg)
{
	return (success == (static_cast<uint8_t>(reg) + Task::ReadEepromBase));
}

template < typename I2cMaster >
uint8_t
xpcc::Hmc6343<I2cMaster>::getReadRegister()
{
	return registerBufferLSB;
}

// MARK: read 16bit register
template < typename I2cMaster >
bool
xpcc::Hmc6343<I2cMaster>::startRead16BitRegister(Register16 reg, uint16_t value)
{
	// first read MSB, then read LSB
	if (task == Task::Idle && adapter.setPreserveTag() &&
			startReadRegisterIgnoreTaskCheck(static_cast<Register>(static_cast<uint8_t>(reg)+1)))
	{
		this->startTask(Task::Read16BitRegister);
		return true;
	}
	return false;
}

template < typename I2cMaster >
bool
xpcc::Hmc6343<I2cMaster>::runRead16BitRegister(Register16 reg)
{
	NPT_BEGIN(NPtTask::Read16BitRegister);

	// wait for the adapter to finish
	NPT_WAIT_UNTIL(adapter.getState() != xpcc::I2c::AdapterState::Busy);
	// check
	if (isReadRegisterSuccessful(static_cast<Register>(static_cast<uint8_t>(reg)+1)))
	{
		registerBufferMSB = getReadRegister();
		NPT_WAIT_UNTIL(startReadRegisterIgnoreTaskCheck(static_cast<Register>(reg)));
		NPT_WAIT_WHILE(runReadRegister(static_cast<Register>(reg)));
	}

	NPT_END();
}

template < typename I2cMaster >
bool
xpcc::Hmc6343<I2cMaster>::isRead16BitRegisterSuccessful(Register16 reg)
{ return isReadRegisterSuccessful(static_cast<Register>(reg));
}

template < typename I2cMaster >
uint16_t
xpcc::Hmc6343<I2cMaster>::getRead16BitRegister()
{
	return (registerBufferMSB << 8) | registerBufferLSB;
}

// MARK: read 6 bytes data
template < typename I2cMaster >
bool
xpcc::Hmc6343<I2cMaster>::startReadPostData(Command command)
{
	// sanity check
	if (task == Task::Idle && static_cast<uint8_t>(command) <= 0x65 &&
			adapter.setPreserveTag() && startWriteCommand(command, 1))
	{
		this->startTask(NPtTask::ReadPostData);
		return true;
	}
	return false;
}

template < typename I2cMaster >
bool
xpcc::Hmc6343<I2cMaster>::runReadPostData(Command command, uint8_t offset, uint8_t readSize)
{
	// sanity check
	if (static_cast<uint8_t>(command) > 0x65 || offset > 4*6 + 1 || readSize > 6)
		return false;

	NPT_BEGIN(NPtTask::ReadPostData);

	NPT_WAIT_UNTIL(adapter.getState() != xpcc::I2c::AdapterState::Busy);

	if (isWriteCommandSuccessful(command))
	{
		NPT_WAIT_UNTIL(timeout.isExpired());

		NPT_WAIT_UNTIL(adapter.configureRead(buffer + offset, readSize) && this->startTransaction(&adapter));

		task = static_cast<uint8_t>(command) + Task::ReadCommandBase;

		NPT_WAIT_WHILE(task == (static_cast<uint8_t>(command) + Task::ReadCommandBase));
	}

	NPT_END();
}

template < typename I2cMaster >
bool
xpcc::Hmc6343<I2cMaster>::isReadPostDataSuccessful(Command command)
{
	// sanity check
	return (static_cast<uint8_t>(command) <= 0x65 &&
			success == (static_cast<uint8_t>(command) + Task::ReadCommandBase));
}

