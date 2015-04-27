// coding: utf-8
/* Copyright (c) 2014, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#include "ssd1306.hpp"

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
