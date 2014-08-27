// coding: utf-8
/* Copyright (c) 2014, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_SSD1306_HPP
#	error	"Don't include this file directly, use 'ssd1306.hpp' instead!"
#endif

#include <xpcc/io/iostream.hpp>
extern xpcc::IOStream stream;

template < class I2cMaster >
xpcc::Ssd1306<I2cMaster>::Ssd1306(uint8_t address)
:	I2cDevice<I2cMaster>(), i2cTask(I2cTask::Idle), i2cSuccess(0), timer(100),
	adapter(address, i2cTask, i2cSuccess), adapterData(address, i2cTask, i2cSuccess)
{
}

template < class I2cMaster >
void
xpcc::Ssd1306<I2cMaster>::update()
{
	if (timer.isExpired())
	{
		startWriteDisplay();
	}

	runWriteDisplay();
}

// ----------------------------------------------------------------------------
// MARK: - Tasks
// MARK: ping
template < class I2cMaster >
bool
xpcc::Ssd1306<I2cMaster>::startPing()
{
	if (i2cTask == I2cTask::Idle && adapter.configurePing() && this->startTransaction(&adapter))
	{
		i2cTask = I2cTask::Ping;
		return true;
	}
	return false;
}

template < class I2cMaster >
bool
xpcc::Ssd1306<I2cMaster>::runPing()
{
	return (i2cTask == I2cTask::Ping);
}

template < class I2cMaster >
bool
xpcc::Ssd1306<I2cMaster>::isPingSuccessful()
{
	return (i2cSuccess == I2cTask::Ping);
}

// ----------------------------------------------------------------------------
// MARK: inititalize
template < class I2cMaster >
bool
xpcc::Ssd1306<I2cMaster>::startInitialize()
{
	return this->startTask(Task::Initialize);
}

template < class I2cMaster >
bool
xpcc::Ssd1306<I2cMaster>::runInitialize()
{
	NPT_BEGIN_TASK(Task::Initialize);

	initSuccessful = true;
	initSuccessful &= NPT_SPAWN_SUCCESS(WriteCommand, (Command::SetDisplayOff), (Command::SetDisplayOff));
	initSuccessful &= NPT_SPAWN_SUCCESS(WriteCommand, (Command::SetDisplayClockDivideRatio, 0x80), (Command::SetDisplayClockDivideRatio));
	initSuccessful &= NPT_SPAWN_SUCCESS(WriteCommand, (Command::SetMultiplexRatio, 0x3F), (Command::SetMultiplexRatio));
	initSuccessful &= NPT_SPAWN_SUCCESS(WriteCommand, (Command::SetDisplayOffset, 0x00), (Command::SetDisplayOffset));
	initSuccessful &= NPT_SPAWN_SUCCESS(WriteCommand, (Command::SetDisplayStartLine | 0x00), (Command::SetDisplayStartLine | 0x00));
	initSuccessful &= NPT_SPAWN_SUCCESS(WriteCommand, (Command::SetChargePump, 0x14), (Command::SetChargePump));
	initSuccessful &= NPT_SPAWN_SUCCESS(WriteCommand, (Command::SetMemoryMode, 0x00), (Command::SetMemoryMode));
	initSuccessful &= NPT_SPAWN_SUCCESS(WriteCommand, (Command::SetSegmentRemap127), (Command::SetSegmentRemap127));
	initSuccessful &= NPT_SPAWN_SUCCESS(WriteCommand, (Command::SetComOutputScanDirectionDecrement), (Command::SetComOutputScanDirectionDecrement));
	initSuccessful &= NPT_SPAWN_SUCCESS(WriteCommand, (Command::SetComPins, 0x12), (Command::SetComPins));
	initSuccessful &= NPT_SPAWN_SUCCESS(WriteCommand, (Command::SetContrastControl, 0xCE), (Command::SetContrastControl));
	initSuccessful &= NPT_SPAWN_SUCCESS(WriteCommand, (Command::SetPreChargePeriod, 0xF1), (Command::SetPreChargePeriod));
	initSuccessful &= NPT_SPAWN_SUCCESS(WriteCommand, (Command::SetV_DeselectLevel, 0x40), (Command::SetV_DeselectLevel));
	initSuccessful &= NPT_SPAWN_SUCCESS(WriteCommand, (Command::SetEntireDisplayResumeToRam), (Command::SetEntireDisplayResumeToRam));
	initSuccessful &= NPT_SPAWN_SUCCESS(WriteCommand, (Command::SetNormalDisplay), (Command::SetNormalDisplay));
	initSuccessful &= NPT_SPAWN_SUCCESS(WriteCommand, (Command::SetColumnAddress, 0, 127), (Command::SetColumnAddress));
	initSuccessful &= NPT_SPAWN_SUCCESS(WriteCommand, (Command::SetPageAddress, 0, 7), (Command::SetPageAddress));
	initSuccessful &= NPT_SPAWN_SUCCESS(WriteCommand, (Command::SetDisplayOn), (Command::SetDisplayOn));

	NPT_END();
}

template < class I2cMaster >
bool
xpcc::Ssd1306<I2cMaster>::isInitializeSuccessful()
{
	return initSuccessful;
}

// ----------------------------------------------------------------------------
// MARK: write display
template < class I2cMaster >
bool
xpcc::Ssd1306<I2cMaster>::startWriteDisplay()
{
	return this->startTask(Task::WriteDisplay);
}

template < class I2cMaster >
bool
xpcc::Ssd1306<I2cMaster>::runWriteDisplay()
{
	NPT_BEGIN_TASK(Task::WriteDisplay);

	NPT_WAIT_UNTIL(adapterData.configureWrite(buffer, 1024) && this->startTransaction(&adapterData));

	i2cTask = I2cTask::WriteDisplay;

	NPT_WAIT_WHILE(i2cTask == I2cTask::WriteDisplay);

	NPT_END();
}

template < class I2cMaster >
bool
xpcc::Ssd1306<I2cMaster>::isWriteDisplaySuccessful()
{
	return (i2cSuccess == I2cTask::WriteDisplay);
}

// ----------------------------------------------------------------------------
// MARK: write command
template < class I2cMaster >
xpcc::pt::State
xpcc::Ssd1306<I2cMaster>::startRunWriteCommand(uint8_t command)
{
	NPT_BEGIN();

	NPT_WAIT_UNTIL_START(
			!adapter.isBusy() && (
					commandBuffer[0] = 0x80,
					commandBuffer[1] = command,
					startTransactionWithLength(2) )
	);

	NPT_WAIT_WHILE(i2cTask == command);

	NPT_END();
}

template < class I2cMaster >
xpcc::pt::State
xpcc::Ssd1306<I2cMaster>::startRunWriteCommand(uint8_t command, uint8_t data)
{
	NPT_BEGIN();

	NPT_WAIT_UNTIL_START(
			!adapter.isBusy() && (
					commandBuffer[0] = 0x80,
					commandBuffer[1] = command,
					commandBuffer[2] = 0x80,
					commandBuffer[3] = data,
					startTransactionWithLength(4) )
	);

	NPT_WAIT_WHILE(i2cTask == command);

	NPT_END();
}

template < class I2cMaster >
xpcc::pt::State
xpcc::Ssd1306<I2cMaster>::startRunWriteCommand(uint8_t command, uint8_t data1, uint8_t data2)
{
	NPT_BEGIN();

	NPT_WAIT_UNTIL_START(
			!adapter.isBusy() && (
					commandBuffer[0] = 0x80,
					commandBuffer[1] = command,
					commandBuffer[2] = 0x80,
					commandBuffer[3] = data1,
					commandBuffer[4] = 0x80,
					commandBuffer[5] = data2,
					startTransactionWithLength(6) )
	);

	NPT_WAIT_WHILE(i2cTask == command);

	NPT_END();
}

template < class I2cMaster >
xpcc::pt::State
xpcc::Ssd1306<I2cMaster>::startRunWriteCommand(uint8_t command,
		uint8_t data1, uint8_t data2, uint8_t data3, uint8_t data4, uint8_t data5)
{
	NPT_BEGIN();

	NPT_WAIT_UNTIL_START(
			!adapter.isBusy() && (
					commandBuffer[0] = 0x80,
					commandBuffer[1] = command,
					commandBuffer[2] = 0x00,
					commandBuffer[3] = data1,
					commandBuffer[4] = 0x00,
					commandBuffer[5] = data2,
					commandBuffer[6] = 0x00,
					commandBuffer[7] = data3,
					commandBuffer[8] = 0x00,
					commandBuffer[9] = data4,
					commandBuffer[10] = 0x00,
					commandBuffer[11] = data5,
					startTransactionWithLength(12) )
	);

	NPT_WAIT_WHILE(i2cTask == command);

	NPT_END();
}

template < class I2cMaster >
xpcc::pt::State
xpcc::Ssd1306<I2cMaster>::startRunWriteCommand(uint8_t command,
		uint8_t data1, uint8_t data2, uint8_t data3, uint8_t data4, uint8_t data5, uint8_t data6)
{
	NPT_BEGIN();

	NPT_WAIT_UNTIL_START(
			!adapter.isBusy() && (
					commandBuffer[0] = 0x80,
					commandBuffer[1] = command,
					commandBuffer[2] = 0x80,
					commandBuffer[3] = data1,
					commandBuffer[4] = 0x80,
					commandBuffer[5] = data2,
					commandBuffer[6] = 0x80,
					commandBuffer[7] = data3,
					commandBuffer[8] = 0x80,
					commandBuffer[9] = data4,
					commandBuffer[10] = 0x80,
					commandBuffer[11] = data5,
					commandBuffer[12] = 0x80,
					commandBuffer[13] = data6,
					startTransactionWithLength(14) )
	);

	NPT_WAIT_WHILE(i2cTask == command);

	NPT_END();
}

template < class I2cMaster >
bool
xpcc::Ssd1306<I2cMaster>::isWriteCommandSuccessful(uint8_t command)
{
	return (i2cSuccess == command);
}

// ----------------------------------------------------------------------------
// MARK: start transaction
template < class I2cMaster >
bool
xpcc::Ssd1306<I2cMaster>::startTransactionWithLength(uint8_t length)
{
	if (adapter.configureWrite(commandBuffer, length) && this->startTransaction(&adapter))
	{
		i2cTask = commandBuffer[1];
		return true;
	}
	return false;
}
