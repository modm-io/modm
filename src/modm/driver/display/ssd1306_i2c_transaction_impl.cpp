// coding: utf-8
/*
 * Copyright (c) 2015, Niklas Hauser
 * Copyright (c) 2017, Sascha Schade
 * Copyright (c) 2021, Thomas Sommer
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
modm::ssd1306::Ssd1306_I2cWriteTransaction::Ssd1306_I2cWriteTransaction(uint8_t address) :
	I2cWriteTransaction(address), transfer_type(ssd1306::Transfer::COMMAND_BURST), transfer_active(false)
{}

bool
modm::ssd1306::Ssd1306_I2cWriteTransaction::configureDisplayWrite(const uint8_t *buffer, std::size_t size)
{
	if (I2cWriteTransaction::configureWrite(buffer, size))
	{
		transfer_type = Transfer::DATA_BURST;
		return true;
	}
	return false;
}

modm::I2cTransaction::Writing
modm::ssd1306::Ssd1306_I2cWriteTransaction::writing()
{
	if (!transfer_active)
	{
		transfer_active = true;
		return Writing(&transfer_type, 1, OperationAfterWrite::Write);
	}
	return I2cWriteTransaction::writing();
}

void
modm::ssd1306::Ssd1306_I2cWriteTransaction::detaching(modm::I2c::DetachCause cause)
{
	I2cWriteTransaction::detaching(cause);
	if (transfer_active or (cause != modm::I2c::DetachCause::NormalStop))
	{
		transfer_type = Transfer::COMMAND_BURST;
		transfer_active = false;
	}
}
