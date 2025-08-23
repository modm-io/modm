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
bool
modm::Ssd1306<I2cMaster, Height>::initialize()
{
	transaction_success = true;

	commandBuffer[0] = FundamentalCommands::DisplayOff;
	commandBuffer[1] = TimingAndDrivingCommands::DisplayClockDivideRatio;
	commandBuffer[2] = 8 << 4;	// Frequency (influences scrolling speed too)
	commandBuffer[2] |= 0;		// Prescaler
	commandBuffer[3] = HardwareConfigCommands::MultiplexRatio;
	commandBuffer[4] = 63;		// Range 0-63
	commandBuffer[5] = HardwareConfigCommands::DisplayOffset;
	commandBuffer[6] = 0;		// Range 0-63
	transaction_success &= writeCommands(7);

	initializeMemoryMode();

	commandBuffer[0] = TimingAndDrivingCommands::ChargePump;
	commandBuffer[1] = ChargePump::V7_5;
	commandBuffer[2] = HardwareConfigCommands::SegmentRemap127;
	commandBuffer[3] = HardwareConfigCommands::ComOutputScanDirectionDecrement;
	commandBuffer[4] = HardwareConfigCommands::DisplayStartLine;
	commandBuffer[4] |= 0;		// Range 0-63
	transaction_success &= writeCommands(5);

	commandBuffer[0] = HardwareConfigCommands::ComPinsOrder;
	commandBuffer[1] = Height == 64 ? 0x12 : 0x02;
	commandBuffer[2] = FundamentalCommands::ContrastControl;
	commandBuffer[3] = 0xCF;	// Strange non-linear beahaviour
	commandBuffer[4] = TimingAndDrivingCommands::PreChargePeriod;
	commandBuffer[5] = 1;		// [3:0] Phase 1 period
	commandBuffer[5] |= 15 << 4;// [7:4] Phase 2 period
	transaction_success &= writeCommands(6);

	commandBuffer[0] = TimingAndDrivingCommands::V_DeselectLevel;
	commandBuffer[1] = 4 << 4;	// [7:4] See Datasheet
	commandBuffer[2] = ScrollingCommands::DisableScroll;
	commandBuffer[3] = FundamentalCommands::EntireDisplayResumeToRam;
	commandBuffer[4] = FundamentalCommands::NormalDisplay;
	transaction_success &= writeCommands(5);

	commandBuffer[0] = FundamentalCommands::DisplayOn;
	transaction_success &= writeCommands(1);

	return transaction_success;
}

/**
 * @brief	MemoryMode::HORIZONTAL and MemoryMode::VERTICAL
 * 			have the best performance cause the whole buffer
 * 			is send in one transaction.
 */
template<class I2cMaster, uint8_t Height>
void
modm::Ssd1306<I2cMaster, Height>::initializeMemoryMode()
{
	commandBuffer[0] = AdressingCommands::MemoryMode;
	commandBuffer[1] = MemoryMode::HORIZONTAL;
	transaction_success &= writeCommands(2);

	// Default on Power-up - can be omitted
	commandBuffer[0] = AdressingCommands::ColumnAddress;
	commandBuffer[1] = 0;
	commandBuffer[2] = 127;
	commandBuffer[3] = AdressingCommands::PageAddress;
	commandBuffer[4] = 0;
	commandBuffer[5] = 7;
	transaction_success &= writeCommands(6);
}

// ----------------------------------------------------------------------------
template<class I2cMaster, uint8_t Height>
void
modm::Ssd1306<I2cMaster, Height>::startWriteDisplay()
{
	modm::this_fiber::poll([&]{ return this->transaction.configureDisplayWrite((uint8_t*)(&this->buffer), sizeof(this->buffer)); });
	modm::this_fiber::poll([&]{ return this->startTransaction(); });
}

template<class I2cMaster, uint8_t Height>
bool
modm::Ssd1306<I2cMaster, Height>::writeDisplay()
{
	startWriteDisplay();

	modm::this_fiber::poll([&]{ return not this->isTransactionRunning(); });

	return this->wasTransactionSuccessful();
}

template<class I2cMaster, uint8_t Height>
bool
modm::Ssd1306<I2cMaster, Height>::setOrientation(glcd::Orientation orientation)
{
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

	return writeCommands(2);
}

template<class I2cMaster, uint8_t Height>
bool
modm::Ssd1306<I2cMaster, Height>::configureScroll(uint8_t origin, uint8_t size,
												  ScrollDirection direction, ScrollStep steps)
{
	if (!disableScroll())
		return false;

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

	return writeCommands(7);
}

// ----------------------------------------------------------------------------
// MARK: write command
template<class I2cMaster, uint8_t Height>
bool
modm::Ssd1306<I2cMaster, Height>::writeCommands(std::size_t length)
{
	modm::this_fiber::poll([&]{ return this->transaction.configureWrite(commandBuffer, length); });
	return this->runTransaction();
}
