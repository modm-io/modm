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

#ifndef MODM_SSD1306_HPP
#error "Don't include this file directly, use 'ssd1306.hpp' instead!"
#endif

template<class I2cMaster, uint8_t Height>
modm::Ssd1306<I2cMaster, Height>::Ssd1306(uint8_t address)
	: I2cDevice<I2cMaster, 3, ssd1306::Ssd1306_I2cWriteTransaction>(address)
{}

// ----------------------------------------------------------------------------
// MARK: - Tasks
template<class I2cMaster, uint8_t Height>
modm::ResumableResult<bool>
modm::Ssd1306<I2cMaster, Height>::initialize()
{
	RF_BEGIN();
	transaction_success = true;

	commandBuffer[0] = FundamentalCommands::DisplayOff;
	commandBuffer[1] = TimingAndDrivingCommands::DisplayClockDivideRatio;
	commandBuffer[2] = 8 << 4;	// Frequency (influences scrolling speed too)
	commandBuffer[2] |= 0;		// Prescaler
	commandBuffer[3] = HardwareConfigCommands::MultiplexRatio;
	commandBuffer[4] = 63;		// Range 0-63
	commandBuffer[5] = HardwareConfigCommands::DisplayOffset;
	commandBuffer[6] = 0;		// Range 0-63
	transaction_success &= RF_CALL(writeCommands(7));

	RF_CALL(initializeMemoryMode());

	commandBuffer[0] = TimingAndDrivingCommands::ChargePump;
	commandBuffer[1] = ChargePump::V7_5;
	commandBuffer[2] = HardwareConfigCommands::SegmentRemap127;
	commandBuffer[3] = HardwareConfigCommands::ComOutputScanDirectionDecrement;
	commandBuffer[4] = HardwareConfigCommands::DisplayStartLine;
	commandBuffer[4] |= 0;		// Range 0-63
	transaction_success &= RF_CALL(writeCommands(5));

	commandBuffer[0] = HardwareConfigCommands::ComPinsOrder;
	commandBuffer[1] = Height == 64 ? 0x12 : 0x02;
	commandBuffer[2] = FundamentalCommands::ContrastControl;
	commandBuffer[3] = 0xCF;	// Strange non-linear beahaviour
	commandBuffer[4] = TimingAndDrivingCommands::PreChargePeriod;
	commandBuffer[5] = 1;		// [3:0] Phase 1 period
	commandBuffer[5] |= 15 << 4;// [7:4] Phase 2 period
	transaction_success &= RF_CALL(writeCommands(6));

	commandBuffer[0] = TimingAndDrivingCommands::V_DeselectLevel;
	commandBuffer[1] = 4 << 4;	// [7:4] See Datasheet
	commandBuffer[2] = ScrollingCommands::DisableScroll;
	commandBuffer[3] = FundamentalCommands::EntireDisplayResumeToRam;
	commandBuffer[4] = FundamentalCommands::NormalDisplay;
	transaction_success &= RF_CALL(writeCommands(5));

	commandBuffer[0] = FundamentalCommands::DisplayOn;
	transaction_success &= RF_CALL(writeCommands(1));

	RF_END_RETURN(transaction_success);
}

/**
 * @brief	MemoryMode::HORIZONTAL and MemoryMode::VERTICAL
 * 			have the best performance cause the whole buffer
 * 			is send in one transaction.
 */
template<class I2cMaster, uint8_t Height>
modm::ResumableResult<void>
modm::Ssd1306<I2cMaster, Height>::initializeMemoryMode()
{
	RF_BEGIN();
	commandBuffer[0] = AdressingCommands::MemoryMode;
	commandBuffer[1] = MemoryMode::HORIZONTAL;
	transaction_success &= RF_CALL(writeCommands(2));

	// Default on Power-up - can be omitted
	commandBuffer[0] = AdressingCommands::ColumnAddress;
	commandBuffer[1] = 0;
	commandBuffer[2] = 127;
	commandBuffer[3] = AdressingCommands::PageAddress;
	commandBuffer[4] = 0;
	commandBuffer[5] = 7;
	transaction_success &= RF_CALL(writeCommands(6));

	RF_END();
}

// ----------------------------------------------------------------------------
template<class I2cMaster, uint8_t Height>
modm::ResumableResult<void>
modm::Ssd1306<I2cMaster, Height>::startWriteDisplay()
{
	RF_BEGIN();

	RF_WAIT_UNTIL(
		this->transaction.configureDisplayWrite((uint8_t*)(&this->buffer), sizeof(this->buffer)) and
		this->startTransaction());

	RF_END();
}

template<class I2cMaster, uint8_t Height>
modm::ResumableResult<bool>
modm::Ssd1306<I2cMaster, Height>::writeDisplay()
{
	RF_BEGIN();

	RF_CALL(startWriteDisplay());

	RF_WAIT_WHILE(this->isTransactionRunning());

	RF_END_RETURN(this->wasTransactionSuccessful());
}

template<class I2cMaster, uint8_t Height>
modm::ResumableResult<bool>
modm::Ssd1306<I2cMaster, Height>::setOrientation(glcd::Orientation orientation)
{
	RF_BEGIN();

	if (orientation == glcd::Orientation::Landscape0)
	{
		commandBuffer[0] = HardwareConfigCommands::SegmentRemap127;
		commandBuffer[1] = HardwareConfigCommands::ComOutputScanDirectionDecrement;
	}
	else if (orientation == glcd::Orientation::Landscape180)
	{
		commandBuffer[0] = HardwareConfigCommands::SegmentRemap0;
		commandBuffer[1] = HardwareConfigCommands::ComOutputScanDirectionIncrement;
	}

	RF_END_RETURN_CALL(writeCommands(2));
}

template<class I2cMaster, uint8_t Height>
modm::ResumableResult<bool>
modm::Ssd1306<I2cMaster, Height>::configureScroll(uint8_t origin, uint8_t size,
												  ScrollDirection direction, ScrollStep steps)
{
	RF_BEGIN();

	if (!RF_CALL(disableScroll()))
		RF_RETURN(false);

	{
		uint8_t beginY = (origin > 7) ? 7 : origin;

		uint8_t endY = ((origin + size) > 7) ? 7 : (origin + size);
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

// ----------------------------------------------------------------------------
// MARK: write command
template<class I2cMaster, uint8_t Height>
modm::ResumableResult<bool>
modm::Ssd1306<I2cMaster, Height>::writeCommands(std::size_t length)
{
	RF_BEGIN();

	RF_WAIT_UNTIL(this->startWrite(commandBuffer, length));

	RF_WAIT_WHILE(this->isTransactionRunning());

	RF_END_RETURN(this->wasTransactionSuccessful());
}
