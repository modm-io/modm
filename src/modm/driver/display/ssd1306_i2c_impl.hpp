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

#ifndef MODM_SSD1306_I2C_HPP
#error "Don't include this file directly, use 'ssd1306_i2c.hpp' instead!"
#endif

template<class I2cMaster, uint8_t Height>
modm::Ssd1306I2c<I2cMaster, Height>::Ssd1306I2c(uint8_t address)
	: I2cDevice<I2cMaster, 3, ssd1306_i2c::Ssd1306_I2cWriteTransaction>(address)
{}

// ----------------------------------------------------------------------------
// MARK: - Tasks
template<class I2cMaster, uint8_t Height>
modm::ResumableResult<bool>
modm::Ssd1306I2c<I2cMaster, Height>::initialize()
{
	RF_BEGIN();
	transaction_success = true;

	this->commandBuffer[0] = std::to_underlying(ssd1306::FundamentalCommands::DisplayOff);
	this->commandBuffer[1] = std::to_underlying(ssd1306::TimingAndDrivingCommands::DisplayClockDivideRatio);
	this->commandBuffer[2] = 8 << 4;	// Frequency (influences scrolling speed too)
	this->commandBuffer[2] |= 0;		// Prescaler
	this->commandBuffer[3] = std::to_underlying(ssd1306::HardwareConfigCommands::MultiplexRatio);
	this->commandBuffer[4] = 63;		// Range 0-63
	this->commandBuffer[5] = std::to_underlying(ssd1306::HardwareConfigCommands::DisplayOffset);
	this->commandBuffer[6] = 0;		// Range 0-63
	transaction_success &= RF_CALL(writeCommands(7));

	RF_CALL(initializeMemoryMode());

	this->commandBuffer[0] = std::to_underlying(ssd1306::TimingAndDrivingCommands::ChargePump);
	this->commandBuffer[1] = std::to_underlying(ssd1306::ChargePump::V7_5);
	this->commandBuffer[2] = std::to_underlying(ssd1306::HardwareConfigCommands::SegmentRemap127);
	this->commandBuffer[3] = std::to_underlying(ssd1306::HardwareConfigCommands::ComOutputScanDirectionDecrement);
	this->commandBuffer[4] = std::to_underlying(ssd1306::HardwareConfigCommands::DisplayStartLine);
	this->commandBuffer[4] |= 0;		// Range 0-63
	transaction_success &= RF_CALL(writeCommands(5));

	this->commandBuffer[0] = std::to_underlying(ssd1306::HardwareConfigCommands::ComPinsOrder);
	this->commandBuffer[1] = Height == 64 ? 0x12 : 0x02;
	this->commandBuffer[2] = std::to_underlying(ssd1306::FundamentalCommands::ContrastControl);
	this->commandBuffer[3] = 0xCF;	// Strange non-linear beahaviour
	this->commandBuffer[4] = std::to_underlying(ssd1306::TimingAndDrivingCommands::PreChargePeriod);
	this->commandBuffer[5] = 1;		// [3:0] Phase 1 period
	this->commandBuffer[5] |= 15 << 4;// [7:4] Phase 2 period
	transaction_success &= RF_CALL(writeCommands(6));

	this->commandBuffer[0] = std::to_underlying(ssd1306::TimingAndDrivingCommands::V_DeselectLevel);
	this->commandBuffer[1] = 4 << 4;	// [7:4] See Datasheet
	this->commandBuffer[2] = std::to_underlying(ssd1306::ScrollingCommands::DisableScroll);
	this->commandBuffer[3] = std::to_underlying(ssd1306::FundamentalCommands::EntireDisplayResumeToRam);
	this->commandBuffer[4] = std::to_underlying(ssd1306::FundamentalCommands::NormalDisplay);
	transaction_success &= RF_CALL(writeCommands(5));

	this->commandBuffer[0] = std::to_underlying(ssd1306::FundamentalCommands::DisplayOn);
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
modm::Ssd1306I2c<I2cMaster, Height>::initializeMemoryMode()
{
	RF_BEGIN();
	this->commandBuffer[0] = std::to_underlying(ssd1306::AdressingCommands::MemoryMode);
	this->commandBuffer[1] = std::to_underlying(ssd1306::MemoryMode::HORIZONTAL);
	transaction_success &= RF_CALL(writeCommands(2));

	// Default on Power-up - can be omitted
	this->commandBuffer[0] = std::to_underlying(ssd1306::AdressingCommands::ColumnAddress);
	this->commandBuffer[1] = 0;
	this->commandBuffer[2] = 127;
	this->commandBuffer[3] = std::to_underlying(ssd1306::AdressingCommands::PageAddress);
	this->commandBuffer[4] = 0;
	this->commandBuffer[5] = 7;
	transaction_success &= RF_CALL(writeCommands(6));

	RF_END();
}

// ----------------------------------------------------------------------------
template<class I2cMaster, uint8_t Height>
modm::ResumableResult<void>
modm::Ssd1306I2c<I2cMaster, Height>::startWriteDisplay()
{
	RF_BEGIN();

	RF_WAIT_UNTIL(
		this->transaction.configureDisplayWrite(&this->buffer[0][0], sizeof(this->buffer)) and
		this->startTransaction());

	RF_END();
}

template<class I2cMaster, uint8_t Height>
modm::ResumableResult<bool>
modm::Ssd1306I2c<I2cMaster, Height>::writeDisplay()
{
	RF_BEGIN();

	RF_CALL(startWriteDisplay());

	RF_WAIT_WHILE(this->isTransactionRunning());

	RF_END_RETURN(this->wasTransactionSuccessful());
}

// ----------------------------------------------------------------------------
// MARK: write command
template<class I2cMaster, uint8_t Height>
modm::ResumableResult<bool>
modm::Ssd1306I2c<I2cMaster, Height>::writeCommands(std::size_t length)
{
	RF_BEGIN();

	RF_WAIT_UNTIL(this->startWrite(this->commandBuffer.data(), length));

	RF_WAIT_WHILE(this->isTransactionRunning());

	RF_END_RETURN(this->wasTransactionSuccessful());
}
