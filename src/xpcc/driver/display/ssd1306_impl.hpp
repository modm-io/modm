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
:	I2cDevice<I2cMaster, 2, ssd1306::DataTransmissionAdapter>(address),
	commandBuffer{0x80, 0, 0x80, 0, 0x80, 0, 0x80, 0, 0x80, 0, 0x80, 0, 0x80, 0}
{
	this->adapter.setCommandBuffer(commandBuffer);
}

// ----------------------------------------------------------------------------
// MARK: - Tasks
template < class I2cMaster >
xpcc::co::Result<bool>
xpcc::Ssd1306<I2cMaster>::initialize()
{
	CO_BEGIN();

	commandBuffer[11] = true;
	commandBuffer[11] &= CO_CALL(writeCommand(Command::SetDisplayOff));
	commandBuffer[11] &= CO_CALL(writeCommand(Command::SetDisplayClockDivideRatio, 0xF0));
	commandBuffer[11] &= CO_CALL(writeCommand(Command::SetMultiplexRatio, 0x3F));
	commandBuffer[11] &= CO_CALL(writeCommand(Command::SetDisplayOffset, 0x00));
	commandBuffer[11] &= CO_CALL(writeCommand(Command::SetDisplayStartLine | 0x00));
	commandBuffer[11] &= CO_CALL(writeCommand(Command::SetChargePump, 0x14));
	commandBuffer[11] &= CO_CALL(writeCommand(Command::SetMemoryMode, 0x01));
	commandBuffer[11] &= CO_CALL(writeCommand(Command::SetSegmentRemap127));
	commandBuffer[11] &= CO_CALL(writeCommand(Command::SetComOutputScanDirectionDecrement));
	commandBuffer[11] &= CO_CALL(writeCommand(Command::SetComPins, 0x12));
	commandBuffer[11] &= CO_CALL(writeCommand(Command::SetContrastControl, 0xCE));
	commandBuffer[11] &= CO_CALL(writeCommand(Command::SetPreChargePeriod, 0xF1));
	commandBuffer[11] &= CO_CALL(writeCommand(Command::SetV_DeselectLevel, 0x40));
	commandBuffer[11] &= CO_CALL(writeCommand(Command::SetEntireDisplayResumeToRam));
	commandBuffer[11] &= CO_CALL(writeCommand(Command::SetNormalDisplay));
//	commandBuffer[11] &= CO_CALL(writeCommand(Command::SetColumnAddress, 0, 127));
//	commandBuffer[11] &= CO_CALL(writeCommand(Command::SetPageAddress, 0, 7));
	commandBuffer[11] &= CO_CALL(writeCommand(Command::SetDisplayOn));

	CO_END_RETURN(commandBuffer[11]);
}

// ----------------------------------------------------------------------------
template < class I2cMaster >
xpcc::co::Result<void>
xpcc::Ssd1306<I2cMaster>::startWriteDisplay()
{
	CO_BEGIN();

	CO_WAIT_UNTIL( this->adapter.configureDisplayWrite(buffer, 1024) and this->startTransaction() );

	CO_END();
}

template < class I2cMaster >
xpcc::co::Result<bool>
xpcc::Ssd1306<I2cMaster>::writeDisplay()
{
	CO_BEGIN();

	CO_CALL(startWriteDisplay());

	CO_WAIT_WHILE(this->isTransactionRunning());

	CO_END_RETURN(this->wasTransactionSuccessful());
}

template < class I2cMaster >
xpcc::co::Result<bool>
xpcc::Ssd1306<I2cMaster>::setRotation(Rotation rotation)
{
	CO_BEGIN();

	if ( CO_CALL(writeCommand((rotation == Rotation::Normal) ?
			Command::SetSegmentRemap127 : Command::SetSegmentRemap0)) )
	{
		CO_RETURN_CALL(writeCommand((rotation == Rotation::Normal) ?
				Command::SetComOutputScanDirectionDecrement : Command::SetComOutputScanDirectionIncrement));
	}

	CO_END_RETURN(false);
}

template < class I2cMaster >
xpcc::co::Result<bool>
xpcc::Ssd1306<I2cMaster>::configureScroll(uint8_t origin, uint8_t size,
		ScrollDirection direction, ScrollStep steps)
{
	CO_BEGIN();

	if (!CO_CALL(disableScroll()))
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

	CO_WAIT_WHILE(this->isTransactionRunning());

	CO_END_RETURN(this->wasTransactionSuccessful());
}

// ----------------------------------------------------------------------------
// MARK: write command
template < class I2cMaster >
xpcc::co::Result<bool>
xpcc::Ssd1306<I2cMaster>::writeCommand(uint8_t command)
{
	CO_BEGIN();

	commandBuffer[1] = command;
	CO_WAIT_UNTIL( startTransactionWithLength(2) );

	CO_WAIT_WHILE(this->isTransactionRunning());

	CO_END_RETURN(this->wasTransactionSuccessful());
}

template < class I2cMaster >
xpcc::co::Result<bool>
xpcc::Ssd1306<I2cMaster>::writeCommand(uint8_t command, uint8_t data)
{
	CO_BEGIN();

	commandBuffer[1] = command;
	commandBuffer[3] = data;
	CO_WAIT_UNTIL( startTransactionWithLength(4) );

	CO_WAIT_WHILE(this->isTransactionRunning());

	CO_END_RETURN(this->wasTransactionSuccessful());
}

template < class I2cMaster >
xpcc::co::Result<bool>
xpcc::Ssd1306<I2cMaster>::writeCommand(uint8_t command, uint8_t data1, uint8_t data2)
{
	CO_BEGIN();

	commandBuffer[1] = command;
	commandBuffer[3] = data1;
	commandBuffer[5] = data2;
	CO_WAIT_UNTIL( startTransactionWithLength(6) );

	CO_WAIT_WHILE(this->isTransactionRunning());

	CO_END_RETURN(this->wasTransactionSuccessful());
}

// ----------------------------------------------------------------------------
template < class I2cMaster >
bool
xpcc::Ssd1306<I2cMaster>::startTransactionWithLength(uint8_t length)
{
	return this->startWrite(commandBuffer, length);
}


// ----------------------------------------------------------------------------
xpcc::ssd1306::DataTransmissionAdapter::DataTransmissionAdapter(uint8_t address) :
	I2cWriteAdapter(address), writeable(true)
{}

bool
xpcc::ssd1306::DataTransmissionAdapter::configureDisplayWrite(uint8_t (*buffer)[8], std::size_t size)
{
	if (I2cWriteAdapter::configureWrite(&buffer[0][0], size))
	{
		commands[13] = 0xfe;
		writeable = false;
		return true;
	}
	return false;
}

xpcc::I2cTransaction::Writing
xpcc::ssd1306::DataTransmissionAdapter::writing()
{
	// we first tell the display the column address again
	if (commands[13] == 0xfe)
	{
		commands[1] = Command::SetColumnAddress;
		commands[3] = 0;
		commands[5] = 127;
		commands[13] = 0xfd;
		return Writing(commands, 6, OperationAfterWrite::Restart);
	}
	// then the page address. again.
	if (commands[13] == 0xfd)
	{
		commands[1] = Command::SetPageAddress;
		commands[3] = 0;
		commands[5] = 7;
		commands[13] = 0xfc;
		return Writing(commands, 6, OperationAfterWrite::Restart);
	}
	// then we set the D/C bit to tell it data is coming
	if (commands[13] == 0xfc)
	{
		commands[13] = 0x40;
		return Writing(&commands[13], 1, OperationAfterWrite::Write);
	}

	// now we write the entire frame buffer into it.
	return Writing(buffer, size, OperationAfterWrite::Stop);
}

void
xpcc::ssd1306::DataTransmissionAdapter::detaching(xpcc::I2c::DetachCause cause)
{
	I2cWriteAdapter::detaching(cause);
	if (commands[13] == 0x40 or cause != xpcc::I2c::DetachCause::NormalStop)
	{
		commands[13] = 0;
		writeable = true;
	}
}
