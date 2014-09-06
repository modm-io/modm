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

template < class I2cMaster >
xpcc::Ssd1306<I2cMaster>::Ssd1306(uint8_t address)
:	I2cDevice<I2cMaster>(), i2cTask(I2cTask::Idle), i2cSuccess(0), inverted(false),
	adapter(address, i2cTask, i2cSuccess), adapterData(address, i2cTask, i2cSuccess)
{
}

// ----------------------------------------------------------------------------
// MARK: - Tasks
template < class I2cMaster >
xpcc::co::Result<bool>
xpcc::Ssd1306<I2cMaster>::ping(void *ctx)
{
	CO_BEGIN(ctx);

	CO_WAIT_UNTIL(adapter.configurePing() && this->startTransaction(&adapter));

	i2cTask = I2cTask::Ping;

	CO_WAIT_WHILE(i2cTask == I2cTask::Ping);

	if (i2cSuccess == I2cTask::Ping)
		CO_RETURN(true);

	CO_END();
}

// ----------------------------------------------------------------------------
template < class I2cMaster >
xpcc::co::Result<bool>
xpcc::Ssd1306<I2cMaster>::initialize(void *ctx)
{
	CO_BEGIN(ctx);

	initSuccessful = true;
	initSuccessful &= CO_CALL(writeCommand(ctx, Command::SetDisplayOff));
	initSuccessful &= CO_CALL(writeCommand(ctx, Command::SetDisplayClockDivideRatio, 0x80));
	initSuccessful &= CO_CALL(writeCommand(ctx, Command::SetMultiplexRatio, 0x3F));
	initSuccessful &= CO_CALL(writeCommand(ctx, Command::SetDisplayOffset, 0x00));
	initSuccessful &= CO_CALL(writeCommand(ctx, Command::SetDisplayStartLine | 0x00));
	initSuccessful &= CO_CALL(writeCommand(ctx, Command::SetChargePump, 0x14));
	initSuccessful &= CO_CALL(writeCommand(ctx, Command::SetMemoryMode, 0x00));
	initSuccessful &= CO_CALL(writeCommand(ctx, Command::SetSegmentRemap127));
	initSuccessful &= CO_CALL(writeCommand(ctx, Command::SetComOutputScanDirectionDecrement));
	initSuccessful &= CO_CALL(writeCommand(ctx, Command::SetComPins, 0x12));
	initSuccessful &= CO_CALL(writeCommand(ctx, Command::SetContrastControl, 0xCE));
	initSuccessful &= CO_CALL(writeCommand(ctx, Command::SetPreChargePeriod, 0xF1));
	initSuccessful &= CO_CALL(writeCommand(ctx, Command::SetV_DeselectLevel, 0x40));
	initSuccessful &= CO_CALL(writeCommand(ctx, Command::SetEntireDisplayResumeToRam));
	initSuccessful &= CO_CALL(writeCommand(ctx, Command::SetNormalDisplay));
	initSuccessful &= CO_CALL(writeCommand(ctx, Command::SetColumnAddress, 0, 127));
	initSuccessful &= CO_CALL(writeCommand(ctx, Command::SetPageAddress, 0, 7));
	initSuccessful &= CO_CALL(writeCommand(ctx, Command::SetDisplayOn));

	if (initSuccessful)
		CO_RETURN(true);

	CO_END();
}

// ----------------------------------------------------------------------------
template < class I2cMaster >
xpcc::co::Result<bool>
xpcc::Ssd1306<I2cMaster>::startWriteDisplay(void *ctx)
{
	CO_BEGIN(ctx);

	if (inverted)
	{
		CO_CALL(writeCommand(ctx, Command::SetColumnAddress, 0, 127));
		CO_CALL(writeCommand(ctx, Command::SetPageAddress, 0, 7));
	}

	CO_WAIT_UNTIL(adapterData.configureWrite(buffer, 1024) && this->startTransaction(&adapterData));

	i2cTask = I2cTask::WriteDisplay;

	CO_END();
}


template < class I2cMaster >
xpcc::co::Result<bool>
xpcc::Ssd1306<I2cMaster>::writeDisplay(void *ctx)
{
	CO_BEGIN(ctx);

	CO_CALL(startWriteDisplay(ctx));

	CO_WAIT_WHILE(i2cTask == I2cTask::WriteDisplay);

	if (i2cSuccess == I2cTask::WriteDisplay)
		CO_RETURN(true);

	CO_END();
}

template < class I2cMaster >
xpcc::co::Result<bool>
xpcc::Ssd1306<I2cMaster>::invertDisplay(void *ctx, bool invert)
{
	CO_BEGIN(ctx);

	CO_CALL(writeCommand(ctx, invert ? Command::SetInvertedDisplay : Command::SetNormalDisplay));

	inverted = invert;

	if (!inverted)
	{
		CO_CALL(writeCommand(ctx, Command::SetColumnAddress, 0, 127));
		CO_CALL(writeCommand(ctx, Command::SetPageAddress, 0, 7));
	}

	CO_END();
}

// ----------------------------------------------------------------------------
// MARK: write command
template < class I2cMaster >
xpcc::co::Result<bool>
xpcc::Ssd1306<I2cMaster>::writeCommand(void *ctx, uint8_t command)
{
	CO_BEGIN(ctx);

	CO_WAIT_UNTIL(
			!adapter.isBusy() && (
					commandBuffer[0] = 0x80,
					commandBuffer[1] = command,
					startTransactionWithLength(2) )
	);

	CO_WAIT_WHILE(i2cTask == command);

	if (i2cSuccess == command)
		CO_RETURN(true);

	CO_END();
}

template < class I2cMaster >
xpcc::co::Result<bool>
xpcc::Ssd1306<I2cMaster>::writeCommand(void *ctx, uint8_t command, uint8_t data)
{
	CO_BEGIN(ctx);

	CO_WAIT_UNTIL(
			!adapter.isBusy() && (
					commandBuffer[0] = 0x80,
					commandBuffer[1] = command,
					commandBuffer[2] = 0x80,
					commandBuffer[3] = data,
					startTransactionWithLength(4) )
	);

	CO_WAIT_WHILE(i2cTask == command);

	if (i2cSuccess == command)
		CO_RETURN(true);

	CO_END();
}

template < class I2cMaster >
xpcc::co::Result<bool>
xpcc::Ssd1306<I2cMaster>::writeCommand(void *ctx, uint8_t command, uint8_t data1, uint8_t data2)
{
	CO_BEGIN(ctx);

	CO_WAIT_UNTIL(
			!adapter.isBusy() && (
					commandBuffer[0] = 0x80,
					commandBuffer[1] = command,
					commandBuffer[2] = 0x80,
					commandBuffer[3] = data1,
					commandBuffer[4] = 0x80,
					commandBuffer[5] = data2,
					startTransactionWithLength(6) )
	);

	CO_WAIT_WHILE(i2cTask == command);

	if (i2cSuccess == command)
		CO_RETURN(true);

	CO_END();
}

template < class I2cMaster >
xpcc::co::Result<bool>
xpcc::Ssd1306<I2cMaster>::writeCommand(void *ctx, uint8_t command,
		uint8_t data1, uint8_t data2, uint8_t data3, uint8_t data4, uint8_t data5)
{
	CO_BEGIN(ctx);

	CO_WAIT_UNTIL(
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

	CO_WAIT_WHILE(i2cTask == command);

	if (i2cSuccess == command)
		CO_RETURN(true);

	CO_END();
}

template < class I2cMaster >
xpcc::co::Result<bool>
xpcc::Ssd1306<I2cMaster>::writeCommand(void *ctx, uint8_t command,
		uint8_t data1, uint8_t data2, uint8_t data3, uint8_t data4, uint8_t data5, uint8_t data6)
{
	CO_BEGIN(ctx);

	CO_WAIT_UNTIL(
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

	CO_WAIT_WHILE(i2cTask == command);

	if (i2cSuccess == command)
		CO_RETURN(true);

	CO_END();
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
