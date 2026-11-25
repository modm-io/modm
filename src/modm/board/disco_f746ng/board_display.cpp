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
void
Board::initializeDisplay()
{
	// R0-R7, G0-G7, B0-B7 except B4, CLK, DE, HSYNC, VSYNC (UM1907 Table 13)
	using Af14 = GpioSet<GpioI15, GpioJ0, GpioJ1, GpioJ2, GpioJ3, GpioJ4, GpioJ5, GpioJ6,
			GpioJ7, GpioJ8, GpioJ9, GpioJ10, GpioJ11, GpioK0, GpioK1, GpioK2,
			GpioE4, GpioJ13, GpioJ14, GpioJ15, GpioK4, GpioK5, GpioK6,
			GpioI14, GpioK7, GpioI10, GpioI9>;
	Af14::configure(Gpio::OutputType::PushPull, Gpio::OutputSpeed::VeryHigh);
	Af14::setAlternateFunction(14);
	GpioG12::configure(Gpio::OutputType::PushPull, Gpio::OutputSpeed::VeryHigh);
	GpioG12::setAlternateFunction(9); // B4

	// LCD clock: 1MHz * 192 / 5 / 4 = 9.6MHz, which is a 59Hz refresh rate with the timing below
	RCC->PLLSAICFGR = (192 << RCC_PLLSAICFGR_PLLSAIN_Pos) | (7 << RCC_PLLSAICFGR_PLLSAIQ_Pos) |
					  (5 << RCC_PLLSAICFGR_PLLSAIR_Pos);
	RCC->DCKCFGR1 = (RCC->DCKCFGR1 & ~RCC_DCKCFGR1_PLLSAIDIVR) | RCC_DCKCFGR1_PLLSAIDIVR_0;
	RCC->CR |= RCC_CR_PLLSAION;
	modm::this_fiber::poll_for(10ms, []{ return RCC->CR & RCC_CR_PLLSAIRDY; });

	// RK043FN48H: HSYNC 41, HBP 13, 480 pixels, HFP 32, VSYNC 10, VBP 2, 272 lines, VFP 2,
	// all signals active low
	Rcc::enable<Peripheral::Ltdc>();
	LTDC->SSCR = (40 << 16) | 9;
	LTDC->BPCR = (53 << 16) | 11;
	LTDC->AWCR = (533 << 16) | 283;
	LTDC->TWCR = (565 << 16) | 285;
	LTDC->BCCR = 0;
	LTDC->GCR = LTDC_GCR_LTDCEN;

	LTDC_Layer1->WHPCR = (533 << 16) | 54;
	LTDC_Layer1->WVPCR = (283 << 16) | 12;
	LTDC_Layer1->PFCR = 2; // RGB565
	LTDC_Layer1->CACR = 255;
	LTDC_Layer1->DCCR = 0;
	LTDC_Layer1->BFCR = (4 << 8) | 5;
	LTDC_Layer1->CFBLR = ((lcd::Width * 2) << 16) | (lcd::Width * 2 + 3);
	LTDC_Layer1->CFBLNR = lcd::Height;

	lcd::Display::setOutput(modm::Gpio::High);
	lcd::Backlight::setOutput(modm::Gpio::High);
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
