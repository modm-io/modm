/*
 * Copyright (c) 2014-2015, Niklas Hauser
 * Copyright (c) 2023, Raphael Lehmann
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_SSD1306_SPI_HPP
#error "Don't include this file directly, use 'ssd1306_spi.hpp' instead!"
#endif

namespace modm
{

template<class SpiMaster, class Cs, class Dc, uint8_t Height>
ResumableResult<bool>
Ssd1306Spi<SpiMaster, Cs, Dc, Height>::initialize(InitSequences initSeq)
{
	RF_BEGIN();

	this->attachConfigurationHandler([]() {
		SpiMaster::setDataMode(SpiMaster::DataMode::Mode0);
		SpiMaster::setDataOrder(SpiMaster::DataOrder::MsbFirst);
	});
	Cs::setOutput(modm::Gpio::High);
	Dc::setOutput();

	if (initSeq == InitSequences::Hp12832_02)
	{
		// Initialize sequence from HP12832-02-TSBG12P091-A-VER1.0.pdf page 14

		this->commandBuffer[0] = std::to_underlying(ssd1306::FundamentalCommands::DisplayOff);
		this->commandBuffer[1] = std::to_underlying(ssd1306::TimingAndDrivingCommands::DisplayClockDivideRatio);
		this->commandBuffer[2] = 0x80; // 0x90 or 0x80 (page 14)?
		this->commandBuffer[3] = std::to_underlying(ssd1306::HardwareConfigCommands::MultiplexRatio);
		this->commandBuffer[4] = 0x1F; // 0x3F or 0x1F (page 14)?
		this->commandBuffer[5] = std::to_underlying(ssd1306::HardwareConfigCommands::DisplayOffset);
		this->commandBuffer[6] = 0;
		this->commandBuffer[7] = std::to_underlying(ssd1306::AdressingCommands::MemoryMode);
		this->commandBuffer[8] = std::to_underlying(ssd1306::MemoryMode::HORIZONTAL);
		this->commandBuffer[9] = std::to_underlying(ssd1306::AdressingCommands::ColumnAddress);
		this->commandBuffer[10] = 0;
		this->commandBuffer[11] = 127;
		this->commandBuffer[12] = std::to_underlying(ssd1306::AdressingCommands::PageAddress);
		this->commandBuffer[13] = 0;
		this->commandBuffer[14] = (Height == 64) ? 7 : 3;
		this->commandBuffer[15] = std::to_underlying(ssd1306::HardwareConfigCommands::DisplayStartLine) | 0;
		this->commandBuffer[16] = std::to_underlying(ssd1306::HardwareConfigCommands::SegmentRemap0);
		this->commandBuffer[17] = std::to_underlying(ssd1306::HardwareConfigCommands::ComOutputScanDirectionIncrement);
		this->commandBuffer[18] = std::to_underlying(ssd1306::HardwareConfigCommands::ComPinsOrder);
		this->commandBuffer[19] = 0x02;
		this->commandBuffer[20] = std::to_underlying(ssd1306::FundamentalCommands::ContrastControl);
		this->commandBuffer[21] = 0x8F;
		this->commandBuffer[22] = std::to_underlying(ssd1306::TimingAndDrivingCommands::PreChargePeriod);
		this->commandBuffer[23] = 0x1F;
		this->commandBuffer[24] = std::to_underlying(ssd1306::TimingAndDrivingCommands::V_DeselectLevel);
		this->commandBuffer[25] = 0x30;
		this->commandBuffer[26] = std::to_underlying(ssd1306::FundamentalCommands::EntireDisplayResumeToRam);
		this->commandBuffer[27] = std::to_underlying(ssd1306::FundamentalCommands::NormalDisplay);
		this->commandBuffer[28] = std::to_underlying(ssd1306::TimingAndDrivingCommands::ChargePump);
		this->commandBuffer[29] = std::to_underlying(ssd1306::ChargePump::DISABLE);
		this->commandBuffer[30] = std::to_underlying(ssd1306::FundamentalCommands::DisplayOn);

		RF_RETURN_CALL(writeCommands(31));
	}
	else {
		RF_RETURN(false);
	}

	RF_END();
}

template<class SpiMaster, class Cs, class Dc, uint8_t Height>
ResumableResult<bool>
Ssd1306Spi<SpiMaster, Cs, Dc, Height>::writeDisplay()
{
	RF_BEGIN();

	RF_WAIT_UNTIL(this->acquireMaster());
	Cs::reset();

	Dc::set();

	RF_CALL(SpiMaster::transfer((uint8_t*)(&this->buffer), nullptr, sizeof(this->buffer)));

	if (this->releaseMaster()) {
		Cs::set();
	}

	RF_END_RETURN(true);
}

template<class SpiMaster, class Cs, class Dc, uint8_t Height>
ResumableResult<bool>
Ssd1306Spi<SpiMaster, Cs, Dc, Height>::writeCommands(std::size_t length)
{
	RF_BEGIN();

	RF_WAIT_UNTIL(this->acquireMaster());
	Cs::reset();

	Dc::reset();

	RF_CALL(SpiMaster::transfer(this->commandBuffer.data(), nullptr, length));

	if (this->releaseMaster()) {
		Cs::set();
	}

	RF_END_RETURN(true);
}

} // namespace modm
