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
#include "sh1106.hpp"

template<class I2cMaster, uint16_t H>
modm::ResumableResult<bool>
modm::Sh1106<I2cMaster, H>::initializeMemoryMode()
{
	RF_BEGIN();
	// Default on Power-up - can be omitted
	this->commandBuffer[0] = ssd1306::AdressingCommands::MemoryMode;
	this->commandBuffer[1] = ssd1306::MemoryMode::PAGE;
	RF_END_RETURN_CALL(this->writeCommands(2));
}

template<class I2cMaster, uint16_t H>
modm::ResumableResult<bool>
modm::Sh1106<I2cMaster, H>::updateClipping()
{
	RF_BEGIN();

	// SH1106-Screen wiring has 2 pixels offset on left
	this->commandBuffer[0] =
		ssd1306::AdressingCommands::HigherColumnStartAddress | ((this->clipping.topLeft.x + 2) >> 4);
	this->commandBuffer[1] =
		ssd1306::AdressingCommands::LowerColumnStartAddress | ((this->clipping.topLeft.x + 2) & 0x0F);

	yd_start = this->clipping.topLeft.y / 8;
	yd_end = (this->clipping.bottomRight.y / 8) - 1;

	RF_END_RETURN(true);
}

template<class I2cMaster, uint16_t H>
template<template<typename> class Accessor>
modm::ResumableResult<bool>
modm::Sh1106<I2cMaster, H>::writeImage(graphic::ImageAccessor<ColorType, Accessor> accessor) {
	RF_BEGIN();
	this->clipping = this->getIntersection(accessor.getSection());

	if (this->pointInCanvas(this->clipping.bottomRight - Point(1, 1)))
	{
		RF_CALL(updateClipping());

		this->transaction_success = true;
		yb = 0;

		for (yd = yd_start; yd <= yd_end; yd++)
		{
			this->commandBuffer[2] = ssd1306::AdressingCommands::PageStartAddress | yd;
			this->transaction_success &= RF_CALL(this->writeCommands(3));

			RF_WAIT_UNTIL(this->transaction.configureDisplayWrite(accessor.getPointer() + (this->clipping.getWidth() * yb++), this->clipping.getWidth()));
			RF_WAIT_UNTIL(this->startTransaction());
			RF_WAIT_WHILE(this->isTransactionRunning());
			this->transaction_success &= this->wasTransactionSuccessful();
		};
	} else {
		MODM_LOG_ERROR << "buffer exceeds display border in " << __FUNCTION__ << modm::endl;
		this->transaction_success = false;
	}
	RF_END_RETURN(this->transaction_success);
}