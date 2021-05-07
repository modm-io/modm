/*
 * Copyright (c) 2021, Thomas Sommer
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#pragma once

#include "ssd1306.hpp"

namespace modm
{

/**
 * SH1106 is said to be 'compatible' with SSD1306. However there's a relevant
 * difference: SH1106 does only support MemoryMode::PAGE. This requires a little
 * more extensive writeDisplay() routine. We have to alternate between setting
 * Page-address and sending page-data instead of sending the whole buffer at
 * once like is for SSD1306 in MemoryMode::HORIZONTAL / MemoryMode::VERTICAL
 *
 * @ingroup modm_driver_sh1106
 */
template<class I2cMaster, uint8_t Height = 64>
class Sh1106 : public Ssd1306<I2cMaster, Height>
{
public:
	Sh1106(uint8_t address = 0x3C) : Ssd1306<I2cMaster, Height>(address) {}

protected:
	modm::ResumableResult<void>
	startWriteDisplay() override
	{
		RF_BEGIN();

		this->transaction_success = true;

		this->commandBuffer[0] = ssd1306::AdressingCommands::HigherColumnStartAddress;
		this->commandBuffer[1] = 0x02;

		for (page = 0; page < Height / 8; page++)
		{
			this->commandBuffer[2] = 0xB0 | page;
			this->transaction_success &= RF_CALL(this->writeCommands(3));

			RF_WAIT_UNTIL(
				this->transaction.configureDisplayWrite((uint8_t*)&this->buffer[page], 128));
			RF_WAIT_UNTIL(this->startTransaction());
			RF_WAIT_WHILE(this->isTransactionRunning());
			this->transaction_success &= this->wasTransactionSuccessful();
		};

		RF_END_RETURN(this->transaction_success);
	}

	modm::ResumableResult<void>
	initializeMemoryMode() override
	{
		RF_BEGIN();
		// Default on Power-up - can be omitted
		this->commandBuffer[0] = ssd1306::AdressingCommands::MemoryMode;
		this->commandBuffer[1] = ssd1306::MemoryMode::PAGE;
		this->transaction_success &= RF_CALL(this->writeCommands(2));
		RF_END();
	}

private:
	size_t page;
};

}  // namespace modm
