/*
 * Copyright (c) 2014-2015, Niklas Hauser
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

template<class I2cMaster, uint16_t H>
modm::ResumableResult<bool>
modm::Ssd1306<I2cMaster, H>::initialize()
{
	RF_BEGIN();
	transaction_success = true;

	transaction_success &= RF_CALL(set(Toggle::Enable, false));

	commandBuffer[0] = TimingAndDrivingCommands::DisplayClockDivideRatio;
	commandBuffer[1] = 8 << 4;  // Frequency (influences scrolling speed too)
	commandBuffer[1] |= 0;      // Prescaler
	commandBuffer[2] = HardwareConfigCommands::MultiplexRatio;
	commandBuffer[3] = 63;      // Range 0-63
	commandBuffer[4] = HardwareConfigCommands::DisplayOffset;
	commandBuffer[5] = 0;       // Range 0-63
	transaction_success &= RF_CALL(writeCommands(6));

	transaction_success &= RF_CALL(initializeMemoryMode());

	commandBuffer[0] = TimingAndDrivingCommands::ChargePump;
	commandBuffer[1] = ChargePump::V7_5;
	commandBuffer[2] = HardwareConfigCommands::SegmentRemap127;
	commandBuffer[3] = HardwareConfigCommands::ComOutputScanDirectionDecrement;
	commandBuffer[4] = HardwareConfigCommands::DisplayStartLine;
	commandBuffer[4] |= 0;        // Range 0-63
	transaction_success &= RF_CALL(writeCommands(5));

	commandBuffer[0] = HardwareConfigCommands::ComPinsOrder;
	commandBuffer[1] = H == 64 ? 0x12 : 0x02;
	commandBuffer[2] = FundamentalCommands::ContrastControl;
	commandBuffer[3] = 0xCF;      // Strange non-linear beahaviour
	commandBuffer[4] = TimingAndDrivingCommands::PreChargePeriod;
	commandBuffer[5] = 1;         // [3:0] Phase 1 period
	commandBuffer[5] |= 15 << 4;  // [7:4] Phase 2 period
	transaction_success &= RF_CALL(writeCommands(6));

	commandBuffer[0] = TimingAndDrivingCommands::V_DeselectLevel;
	commandBuffer[1] = 4 << 4;    // [7:4] See Datasheet
	commandBuffer[2] = ScrollingCommands::DisableScroll;
	commandBuffer[3] = FundamentalCommands::EntireDisplayResumeToRam;
	transaction_success &= RF_CALL(writeCommands(4));

	transaction_success &= RF_CALL(set(Toggle::Inversion, false));
	transaction_success &= RF_CALL(set(Toggle::Enable, true));

	RF_END_RETURN(transaction_success);
}

template<class I2cMaster, uint16_t H>
modm::ResumableResult<bool>
modm::Ssd1306<I2cMaster, H>::initializeMemoryMode()
{
	RF_BEGIN();
	commandBuffer[0] = AdressingCommands::MemoryMode;
	commandBuffer[1] = MemoryMode::HORIZONTAL;
	RF_END_RETURN_CALL(writeCommands(2));
}

template<class I2cMaster, uint16_t H>
modm::ResumableResult<bool>
modm::Ssd1306<I2cMaster, H>::setOrientation(graphic::Orientation orientation)
{
	RF_BEGIN();

	// No support for Portrait
	if(orientation & graphic::OrientationFlags::Portrait)
		RF_RETURN(false);

	switch (orientation)
	{
		case graphic::Orientation::Landscape0:
			commandBuffer[0] = HardwareConfigCommands::SegmentRemap127;
			commandBuffer[1] = HardwareConfigCommands::ComOutputScanDirectionDecrement;
			break;
		case graphic::Orientation::Landscape180:
			commandBuffer[0] = HardwareConfigCommands::SegmentRemap0;
			commandBuffer[1] = HardwareConfigCommands::ComOutputScanDirectionIncrement;
			break;
	}

	RF_CALL(writeCommands(2));
	RF_END_RETURN_CALL(clear());
}

template<class I2cMaster, uint16_t H>
modm::ResumableResult<bool>
modm::Ssd1306<I2cMaster, H>::set(Toggle toggle, bool state) {
	RF_BEGIN();

	commandBuffer[0] = uint8_t(toggle) | uint8_t(state);

	RF_END_RETURN_CALL(writeCommands(1));
}

template<class I2cMaster, uint16_t H>
modm::ResumableResult<bool>
modm::Ssd1306<I2cMaster, H>::configureScroll(uint8_t placement, uint8_t size,
												  ScrollDirection direction, ScrollStep steps)
{
	RF_BEGIN();

	if (!RF_CALL(disableScroll())) RF_RETURN(false);

	{
		uint8_t beginY = (placement > 7) ? 7 : placement;

		uint8_t endY = ((placement + size) > 7) ? 7 : (placement + size);
		if (endY < beginY) endY = beginY;

		commandBuffer[0] = uint8_t(direction);
		commandBuffer[1] = 0x00;
		commandBuffer[2] = beginY;
		commandBuffer[3] = uint8_t(steps);
		commandBuffer[4] = endY;
		commandBuffer[5] = 0x00;
		commandBuffer[6] = 0xFF;
	}

	RF_END_RETURN_CALL(writeCommands(7));
}

template<class I2cMaster, uint16_t H>
modm::ResumableResult<bool>
modm::Ssd1306<I2cMaster, H>::writeCommands(std::size_t length)
{
	RF_BEGIN();

	RF_WAIT_UNTIL(this->startWrite(commandBuffer, length));

	RF_WAIT_WHILE(this->isTransactionRunning());

	RF_END_RETURN(this->wasTransactionSuccessful());
}

template<class I2cMaster, uint16_t H>
modm::ResumableResult<bool>
modm::Ssd1306<I2cMaster, H>::updateClipping()
{
	RF_BEGIN();

	commandBuffer[0] = AdressingCommands::ColumnAddress;
	commandBuffer[1] = this->clipping.topLeft.x;
	commandBuffer[2] = this->clipping.bottomRight.x - 1;
	commandBuffer[3] = AdressingCommands::PageAddress;
	commandBuffer[4] = this->clipping.topLeft.y / 8;
	commandBuffer[5] = (this->clipping.bottomRight.y / 8) - 1;

	RF_END_RETURN_CALL(writeCommands(6));
}

#include <modm/debug/logger.hpp>

template<class I2cMaster, uint16_t H>
template<template<typename> class Accessor>
modm::ResumableResult<bool>
modm::Ssd1306<I2cMaster, H>::writeImage(graphic::ImageAccessor<ColorType, Accessor> accessor) {
	RF_BEGIN();

	this->clipping = this->getIntersection(accessor.getSection());

	if(!this->pointInCanvas(this->clipping.bottomRight - Point(1, 1))) {
		MODM_LOG_ERROR << "buffer exceeds display border in " << __FUNCTION__ << modm::endl;
		RF_RETURN(false);
	}

	RF_CALL(updateClipping());

	// FIXME Integrate this->clipping results
	RF_WAIT_UNTIL(this->transaction.configureDisplayWrite(accessor.getPointer(), this->clipping.getPixels() / 8));

	this->startTransaction();
	RF_WAIT_WHILE(this->isTransactionRunning());

	RF_END_RETURN(this->wasTransactionSuccessful());
}

// TODO Test in Hardware
template<class I2cMaster, uint16_t H>
modm::ResumableResult<bool>
modm::Ssd1306<I2cMaster, H>::clear(ColorType color)
{
	// OPTIMIZE Make this impossible fast through use of DMA
	// See https://github.com/modm-io/modm/issues/666
	RF_BEGIN();

	// FIXME Gray8(color).value is not very accurate
	std::fill(commandBuffer[0], commandBuffer[7], Gray8(color).value);
	transaction_success = true;

	this->clipping = Section({0, 0}, {128, H});
	RF_CALL(updateClipping());

	while(true) {
		RF_WAIT_UNTIL(this->transaction.configureDisplayWrite(commandBuffer, 7));
		this->startTransaction();
		RF_WAIT_WHILE(this->isTransactionRunning());
		transaction_success &= this->wasTransactionSuccessful();

		// TODO Needs end condition
	}

	RF_END_RETURN(true);
}