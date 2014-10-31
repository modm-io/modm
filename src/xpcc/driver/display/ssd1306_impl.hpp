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
:	I2cDevice<I2cMaster>(),
	commandBuffer{0x80, 0, 0x80, 0, 0x80, 0, 0x80, 0, 0x80, 0, 0x80, 0, 0x80, 0},
	i2cTask(I2cTask::Idle), i2cSuccess(0),
	adapter(address, i2cTask, i2cSuccess), adapterData(address, i2cTask, i2cSuccess)
{
	adapterData.setCommandBuffer(commandBuffer);
}

// ----------------------------------------------------------------------------
// MARK: - Tasks
template < class I2cMaster >
xpcc::co::Result<bool>
xpcc::Ssd1306<I2cMaster>::ping(void *ctx)
{
	CO_BEGIN(ctx);

	CO_WAIT_UNTIL(adapter.configurePing() &&
			(i2cTask = I2cTask::Ping, this->startTransaction(&adapter)));

	CO_WAIT_WHILE(i2cTask == I2cTask::Ping);

	CO_END_RETURN(i2cSuccess == I2cTask::Ping);
}

// ----------------------------------------------------------------------------
template < class I2cMaster >
xpcc::co::Result<bool>
xpcc::Ssd1306<I2cMaster>::initialize(void *ctx)
{
	CO_BEGIN(ctx);

	initSuccessful = true;
	initSuccessful &= CO_CALL(writeCommand(ctx, Command::SetDisplayOff));
	initSuccessful &= CO_CALL(writeCommand(ctx, Command::SetDisplayClockDivideRatio, 0xF0));
	initSuccessful &= CO_CALL(writeCommand(ctx, Command::SetMultiplexRatio, 0x3F));
	initSuccessful &= CO_CALL(writeCommand(ctx, Command::SetDisplayOffset, 0x00));
	initSuccessful &= CO_CALL(writeCommand(ctx, Command::SetDisplayStartLine | 0x00));
	initSuccessful &= CO_CALL(writeCommand(ctx, Command::SetChargePump, 0x14));
	initSuccessful &= CO_CALL(writeCommand(ctx, Command::SetMemoryMode, 0x01));
	initSuccessful &= CO_CALL(writeCommand(ctx, Command::SetSegmentRemap127));
	initSuccessful &= CO_CALL(writeCommand(ctx, Command::SetComOutputScanDirectionDecrement));
	initSuccessful &= CO_CALL(writeCommand(ctx, Command::SetComPins, 0x12));
	initSuccessful &= CO_CALL(writeCommand(ctx, Command::SetContrastControl, 0xCE));
	initSuccessful &= CO_CALL(writeCommand(ctx, Command::SetPreChargePeriod, 0xF1));
	initSuccessful &= CO_CALL(writeCommand(ctx, Command::SetV_DeselectLevel, 0x40));
	initSuccessful &= CO_CALL(writeCommand(ctx, Command::SetEntireDisplayResumeToRam));
	initSuccessful &= CO_CALL(writeCommand(ctx, Command::SetNormalDisplay));
//	initSuccessful &= CO_CALL(writeCommand(ctx, Command::SetColumnAddress, 0, 127));
//	initSuccessful &= CO_CALL(writeCommand(ctx, Command::SetPageAddress, 0, 7));
	initSuccessful &= CO_CALL(writeCommand(ctx, Command::SetDisplayOn));

	CO_END_RETURN(initSuccessful);
}

// ----------------------------------------------------------------------------
template < class I2cMaster >
xpcc::co::Result<void>
xpcc::Ssd1306<I2cMaster>::startWriteDisplay(void *ctx)
{
	CO_BEGIN(ctx);

	CO_WAIT_UNTIL(adapterData.configureWrite(buffer, 1024) &&
			(i2cTask = I2cTask::WriteDisplay, this->startTransaction(&adapterData)));

	CO_END();
}

template < class I2cMaster >
xpcc::co::Result<bool>
xpcc::Ssd1306<I2cMaster>::writeDisplay(void *ctx)
{
	CO_BEGIN(ctx);

	CO_CALL(startWriteDisplay(ctx));
	CO_WAIT_WHILE(i2cTask == I2cTask::WriteDisplay);

	CO_END_RETURN(i2cSuccess == I2cTask::WriteDisplay);
}

template < class I2cMaster >
xpcc::co::Result<bool>
xpcc::Ssd1306<I2cMaster>::setRotation(void *ctx, Rotation rotation)
{
	CO_BEGIN(ctx);

	if ( CO_CALL(writeCommand(ctx, (rotation == Rotation::Normal) ?
			Command::SetSegmentRemap127 : Command::SetSegmentRemap0)) )
	{
		CO_RETURN_CALL(writeCommand(ctx, (rotation == Rotation::Normal) ?
				Command::SetComOutputScanDirectionDecrement : Command::SetComOutputScanDirectionIncrement));
	}

	CO_END_RETURN(false);
}

template < class I2cMaster >
xpcc::co::Result<bool>
xpcc::Ssd1306<I2cMaster>::configureScroll(void *ctx, uint8_t origin, uint8_t size,
		ScrollDirection direction, ScrollStep steps)
{
	CO_BEGIN(ctx);

	if (!CO_CALL(disableScroll(ctx)))
		CO_RETURN(false);

	{
		uint8_t beginY = (origin > 7) ? 7 : origin;

		uint8_t endY = ((origin + size) > 7) ? 7 : (origin + size);
		if (endY < beginY) endY = beginY;

		commandBuffer[1] = static_cast<uint8_t>(direction);
		commandBuffer[3] = 0x00;
		commandBuffer[5] = beginY;
		commandBuffer[7] = i(steps);
		commandBuffer[9] = endY;
		commandBuffer[11] = 0x00;
		commandBuffer[13] = 0xFF;
	}

	CO_WAIT_UNTIL( startTransactionWithLength(14) );

	CO_WAIT_WHILE(i2cTask == commandBuffer[1]);

	CO_END_RETURN(i2cSuccess == commandBuffer[1]);
}

// ----------------------------------------------------------------------------
// MARK: write command
template < class I2cMaster >
xpcc::co::Result<bool>
xpcc::Ssd1306<I2cMaster>::writeCommand(void *ctx, uint8_t command)
{
	CO_BEGIN(ctx);

	commandBuffer[1] = command;
	CO_WAIT_UNTIL( startTransactionWithLength(2) );

	CO_WAIT_WHILE(i2cTask == command);

	CO_END_RETURN(i2cSuccess == command);
}

template < class I2cMaster >
xpcc::co::Result<bool>
xpcc::Ssd1306<I2cMaster>::writeCommand(void *ctx, uint8_t command, uint8_t data)
{
	CO_BEGIN(ctx);

	commandBuffer[1] = command;
	commandBuffer[3] = data;
	CO_WAIT_UNTIL( startTransactionWithLength(4) );

	CO_WAIT_WHILE(i2cTask == command);

	CO_END_RETURN(i2cSuccess == command);
}

template < class I2cMaster >
xpcc::co::Result<bool>
xpcc::Ssd1306<I2cMaster>::writeCommand(void *ctx, uint8_t command, uint8_t data1, uint8_t data2)
{
	CO_BEGIN(ctx);

	commandBuffer[1] = command;
	commandBuffer[3] = data1;
	commandBuffer[5] = data2;
	CO_WAIT_UNTIL( startTransactionWithLength(6) );

	CO_WAIT_WHILE(i2cTask == command);

	CO_END_RETURN(i2cSuccess == command);
}

// ----------------------------------------------------------------------------
// MARK: start transaction
template < class I2cMaster >
bool
xpcc::Ssd1306<I2cMaster>::startTransactionWithLength(uint8_t length)
{
	return (adapter.configureWrite(commandBuffer, length) &&
			(i2cTask = commandBuffer[1], this->startTransaction(&adapter)));
}

// ----------------------------------------------------------------------------
// MARK: DataTransmissionAdapter
template < class I2cMaster >
bool
xpcc::Ssd1306<I2cMaster>::DataTransmissionAdapter::configureWrite(uint8_t (*buffer)[8], std::size_t size)
{
	if (I2cWriteAdapter::configureWrite(&buffer[0][0], size))
	{
		commands[13] = 0xff;
		return true;
	}
	return false;
}

template < class I2cMaster >
xpcc::I2cTransaction::Writing
xpcc::Ssd1306<I2cMaster>::DataTransmissionAdapter::writing()
{
	// we first tell the display the column address again
	if (commands[13] == 0xff)
	{
		commands[1] = Command::SetColumnAddress;
		commands[3] = 0;
		commands[5] = 127;
		commands[13] = 0xfe;
		return Writing(commands, 6, OperationAfterWrite::Restart);
	}
	// then the page address. again.
	if (commands[13] == 0xfe)
	{
		commands[1] = Command::SetPageAddress;
		commands[3] = 0;
		commands[5] = 7;
		commands[13] = 0xfd;
		return Writing(commands, 6, OperationAfterWrite::Restart);
	}
	// then we set the D/C bit to tell it data is coming
	if (commands[13] == 0xfd)
	{
		commands[13] = 0x40;
		return Writing(&commands[13], 1, OperationAfterWrite::Write);
	}

	// now we write the entire frame buffer into it.
	return Writing(buffer, size, OperationAfterWrite::Stop);
}
