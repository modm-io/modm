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

// The LCD is on FMC NE2, which shares the data and address pins with the PSRAM on
// NE1, so they are already configured (UM2140 Section 5.14)
void
Board::initializeDisplay()
{
	GpioG9::configure(Gpio::OutputType::PushPull, Gpio::OutputSpeed::VeryHigh);
	GpioG9::setAlternateFunction(12); // NE2
	// Extended mode with slow reads (RDX low >= 355ns) and fast writes (tWC >= 66ns)
	FMC_Bank1->BTCR[2] = FMC_BCR2_MWID_0 | FMC_BCR2_WREN | FMC_BCR2_EXTMOD | FMC_BCR2_MBKEN;
	FMC_Bank1->BTCR[3] = (1 << FMC_BTR1_BUSTURN_Pos) | (80 << FMC_BTR1_DATAST_Pos) |
						 (15 << FMC_BTR1_ADDSET_Pos);
	FMC_Bank1E->BWTR[2] = (1 << FMC_BWTR1_BUSTURN_Pos) | (10 << FMC_BWTR1_DATAST_Pos) |
						  (5 << FMC_BWTR1_ADDSET_Pos);

	// The default memory map makes the FMC cacheable normal memory, which may buffer,
	// merge and reorder the LCD accesses. Make the LCD region strongly ordered instead.
	MPU->RNR = 0;
	MPU->RBAR = 0x6400'0000;
	MPU->RASR = MPU_RASR_XN_Msk | (0b011 << MPU_RASR_AP_Pos) | (0b000 << MPU_RASR_TEX_Pos) |
				((26 - 1) << MPU_RASR_SIZE_Pos) | MPU_RASR_ENABLE_Msk; // 64MiB
	MPU->CTRL = MPU_CTRL_PRIVDEFENA_Msk | MPU_CTRL_ENABLE_Msk;
	__DSB();
	__ISB();

	lcd::Backlight::setOutput(modm::Gpio::Low);
	lcd::TearingEffect::setInput();
	auto &display = getDisplay();
	display.initialize();
	display.setBackgroundColor(modm::color::html::Black);
	display.clear();
	lcd::Backlight::set();
}

Board::Display&
Board::getDisplay()
{
	static Display display;
	return display;
}
