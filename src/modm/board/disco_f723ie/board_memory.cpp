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

// The 512kB PSRAM (IS66WV51216, 55ns) is on FMC NE1 (UM2140 Section 5.11).
// The data and address pins are shared with the LCD on NE2.
static void
board_initialize_psram()
{
	using Pins = GpioSet<
		// D0-D15
		GpioD14, GpioD15, GpioD0, GpioD1, GpioE7, GpioE8, GpioE9, GpioE10,
		GpioE11, GpioE12, GpioE13, GpioE14, GpioE15, GpioD8, GpioD9, GpioD10,
		// A0-A17
		GpioF0, GpioF1, GpioF2, GpioF3, GpioF4, GpioF5, GpioF12, GpioF13, GpioF14,
		GpioF15, GpioG0, GpioG1, GpioG2, GpioG3, GpioG4, GpioG5, GpioD11, GpioD12,
		// NOE, NWE, NE1, NBL0, NBL1
		GpioD4, GpioD5, GpioD7, GpioE0, GpioE1>;
	Pins::configure(Gpio::OutputType::PushPull, Gpio::OutputSpeed::VeryHigh);
	Pins::setAlternateFunction(12);

	Rcc::enable<Peripheral::Fmc>();
	// One HCLK cycle is 4.6ns at 216MHz.
	// Mode A, 16-bit, write enable, tRC = tWC >= 55ns with some margin
	FMC_Bank1->BTCR[0] = FMC_BCR1_MWID_0 | FMC_BCR1_WREN | FMC_BCR1_MBKEN;
	FMC_Bank1->BTCR[1] = (2 << FMC_BTR1_BUSTURN_Pos) | (10 << FMC_BTR1_DATAST_Pos) |
						 (5 << FMC_BTR1_ADDSET_Pos);
}

static void
modm_board_init()
{
	// initialize system clock and PSRAM before accessing external memories
	Board::SystemClock::enable();
	board_initialize_psram();
}

MODM_HARDWARE_INIT(modm_board_init);
