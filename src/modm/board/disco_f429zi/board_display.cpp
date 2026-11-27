/*
 * Copyright (c) 2026, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include "board.hpp"
#include <modm/architecture/interface/fiber.hpp>

using namespace Board;
using namespace std::chrono_literals;

// ------------------------------------ LCD ------------------------------------
static void
ili9341(uint8_t command, std::initializer_list<uint8_t> parameters = {})
{
	lcd::Csx::reset();
	lcd::WrxDcx::reset();
	l3g::SpiMaster::transfer(command);
	lcd::WrxDcx::set();
	for (uint8_t parameter : parameters) l3g::SpiMaster::transfer(parameter);
	lcd::Csx::set();
}

void
Board::initializeDisplay()
{
	using namespace lcd;
	using Af14 = GpioSet<R2, R4, R5, R7, G2, G4, G5, G6, G7, B2, B3, B5, B6, B7, Hsync, Vsync, Enable, DotClk>;
	using Af9 = GpioSet<R3, R6, G3, B4>;
	Af14::configure(Gpio::OutputType::PushPull, Gpio::OutputSpeed::VeryHigh);
	Af14::setAlternateFunction(14);
	Af9::configure(Gpio::OutputType::PushPull, Gpio::OutputSpeed::VeryHigh);
	Af9::setAlternateFunction(9);

	// The ILI9341 is configured over SPI5, which it shares with the L3GD20
	Csx::setOutput(modm::Gpio::High);
	WrxDcx::setOutput(modm::Gpio::High);
	l3g::Cs::setOutput(modm::Gpio::High);
	l3g::SpiMaster::connect<l3g::Sck::Sck, l3g::Mosi::Mosi, l3g::Miso::Miso>();
	l3g::SpiMaster::initialize<SystemClock, 5.25_MHz>();
	l3g::SpiMaster::setDataMode(l3g::SpiMaster::DataMode::Mode3);

	// Initialization of the ILI9341 for the RGB interface from the STM32F429I-DISCO BSP
	ili9341(0xCA, {0xC3, 0x08, 0x50});
	ili9341(0xCF, {0x00, 0xC1, 0x30});						// Power control B
	ili9341(0xED, {0x64, 0x03, 0x12, 0x81});				// Power on sequence
	ili9341(0xE8, {0x85, 0x00, 0x78});						// Driver timing control A
	ili9341(0xCB, {0x39, 0x2C, 0x00, 0x34, 0x02});			// Power control A
	ili9341(0xF7, {0x20});									// Pump ratio control
	ili9341(0xEA, {0x00, 0x00});							// Driver timing control B
	ili9341(0xB1, {0x00, 0x1B});							// Frame rate control
	ili9341(0xB6, {0x0A, 0xA2});							// Display function control
	ili9341(0xC0, {0x10});									// Power control 1
	ili9341(0xC1, {0x10});									// Power control 2
	ili9341(0xC5, {0x45, 0x15});							// VCOM control 1
	ili9341(0xC7, {0x90});									// VCOM control 2
	ili9341(0x36, {0xC8});									// Memory access control
	ili9341(0xF2, {0x00});									// 3 gamma disable
	ili9341(0xB0, {0xC2});									// RGB interface signal control
	ili9341(0xB6, {0x0A, 0xA7, 0x27, 0x04});				// Display function control
	ili9341(0x2A, {0x00, 0x00, 0x00, 0xEF});				// Column address
	ili9341(0x2B, {0x00, 0x00, 0x01, 0x3F});				// Page address
	ili9341(0xF6, {0x01, 0x00, 0x06});						// Interface control: RGB interface
	ili9341(0x2C);
	modm::this_fiber::sleep_for(200ms);
	ili9341(0x26, {0x01});									// Gamma curve
	ili9341(0xE0, {0x0F, 0x29, 0x24, 0x0C, 0x0E, 0x09, 0x4E, 0x78,
				   0x3C, 0x09, 0x13, 0x05, 0x17, 0x11, 0x00});	// Positive gamma
	ili9341(0xE1, {0x00, 0x16, 0x1B, 0x04, 0x11, 0x07, 0x31, 0x33,
				   0x42, 0x05, 0x0C, 0x0A, 0x28, 0x2F, 0x0F});	// Negative gamma
	ili9341(0x11);											// Sleep out
	modm::this_fiber::sleep_for(200ms);
	ili9341(0x29);											// Display on
	ili9341(0x2C);

	// LCD clock: 1MHz * 192 / 4 / 8 = 6MHz, which is a 65Hz refresh rate with the timing below
	RCC->PLLSAICFGR = (192 << RCC_PLLSAICFGR_PLLSAIN_Pos) | (7 << RCC_PLLSAICFGR_PLLSAIQ_Pos) |
					  (4 << RCC_PLLSAICFGR_PLLSAIR_Pos);
	RCC->DCKCFGR = (RCC->DCKCFGR & ~RCC_DCKCFGR_PLLSAIDIVR) | RCC_DCKCFGR_PLLSAIDIVR_1;
	RCC->CR |= RCC_CR_PLLSAION;
	modm::this_fiber::poll_for(10ms, []{ return RCC->CR & RCC_CR_PLLSAIRDY; });

	// HSYNC 10, HBP 20, 240 pixels, HFP 10, VSYNC 2, VBP 2, 320 lines, VFP 4, all signals active low
	Rcc::enable<Peripheral::Ltdc>();
	LTDC->SSCR = (9 << 16) | 1;
	LTDC->BPCR = (29 << 16) | 3;
	LTDC->AWCR = (269 << 16) | 323;
	LTDC->TWCR = (279 << 16) | 327;
	LTDC->BCCR = 0;
	LTDC->GCR = LTDC_GCR_LTDCEN;

	LTDC_Layer1->WHPCR = (269 << 16) | 30;
	LTDC_Layer1->WVPCR = (323 << 16) | 4;
	LTDC_Layer1->PFCR = 2; // RGB565
	LTDC_Layer1->CACR = 255;
	LTDC_Layer1->DCCR = 0;
	LTDC_Layer1->BFCR = (4 << 8) | 5;
	LTDC_Layer1->CFBLR = ((240 * 2) << 16) | (240 * 2 + 3);
	LTDC_Layer1->CFBLNR = 320;
}

void
Board::setDisplayBuffer(void *buffer)
{
	LTDC_Layer1->CFBAR = uint32_t(buffer);
	LTDC_Layer1->CR = LTDC_LxCR_LEN;
	LTDC->SRCR = LTDC_SRCR_VBR;
}

void *
Board::getDisplayBuffer()
{
	return reinterpret_cast<void *>(LTDC_Layer1->CFBAR);
}
