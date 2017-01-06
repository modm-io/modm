/*
 * Copyright (c) 2015, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include "ssd1306.hpp"

// ----------------------------------------------------------------------------
modm::ssd1306::DataTransmissionAdapter::DataTransmissionAdapter(uint8_t address) :
	I2cWriteTransaction(address), writeable(true)
{}

bool
modm::ssd1306::DataTransmissionAdapter::configureDisplayWrite(uint8_t (*buffer)[8], std::size_t size)
{
	if (I2cWriteTransaction::configureWrite(&buffer[0][0], size))
	{
		commands[13] = 0xfe;
		writeable = false;
		return true;
	}
	return false;
}

modm::I2cTransaction::Writing
modm::ssd1306::DataTransmissionAdapter::writing()
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
modm::ssd1306::DataTransmissionAdapter::detaching(modm::I2c::DetachCause cause)
{
	I2cWriteTransaction::detaching(cause);
	if (commands[13] == 0x40 or cause != modm::I2c::DetachCause::NormalStop)
	{
		commands[13] = 0;
		writeable = true;
	}
}
