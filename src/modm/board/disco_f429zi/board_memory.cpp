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

using namespace Board;

// ----------------------------------- SDRAM -----------------------------------
// IS42S16400J: 4 banks of 4096 rows x 256 columns x 16-bit on SDNE1 (UM1670 Section 7.10)
static void
sdram_command(uint32_t mode, uint32_t refreshes = 1, uint32_t mode_register = 0)
{
	FMC_Bank5_6->SDCMR = (mode_register << FMC_SDCMR_MRD_Pos) | ((refreshes - 1) << FMC_SDCMR_NRFS_Pos) |
						 FMC_SDCMR_CTB2 | mode;
	for (int t = 1024; (FMC_Bank5_6->SDSR & FMC_SDSR_BUSY) and t; t--) modm::delay_us(10);
}

static void
board_initialize_sdram()
{
	using namespace fmc;
	using Pins = GpioSet<Sdcke1, Sdclk, Sdncas, Sdnras, Sdnwe, Cs, Ldqm, Udqm, Ba0, Ba1,
						 A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11,
						 D0, D1, D2, D3, D4, D5, D6, D7, D8, D9, D10, D11, D12, D13, D14, D15>;
	Pins::configure(Gpio::OutputType::PushPull, Gpio::OutputSpeed::VeryHigh);
	Pins::setAlternateFunction(12);

	Rcc::enable<Peripheral::Fmc>();
	// SDCLK = HCLK/2 = 84MHz, read pipe delay 1, CAS latency 3, 4 banks, 16-bit, 12-bit rows, 8-bit columns.
	// The clock and read settings are only in the register of the first bank.
	FMC_Bank5_6->SDCR[0] = FMC_SDCR1_SDCLK_1 | FMC_SDCR1_RPIPE_0;
	FMC_Bank5_6->SDCR[1] = FMC_SDCR1_CAS_0 | FMC_SDCR1_CAS_1 | FMC_SDCR1_NB |
						   FMC_SDCR1_MWID_0 | FMC_SDCR1_NR_0;
	// TRCD=2, TRP=2, TWR=2, TRC=7, TRAS=4, TXSR=7, TMRD=2 cycles.
	// TRC and TRP are only in the register of the first bank.
	FMC_Bank5_6->SDTR[0] = 0x01116361;
	FMC_Bank5_6->SDTR[1] = 0x01116361;

	sdram_command(1); // clock enable
	modm::delay_ms(1);
	sdram_command(2); // precharge all
	sdram_command(3, 4); // auto refresh
	sdram_command(4, 1, 0x230); // load mode: burst length 1, CAS latency 3, single write burst
	// 64ms / 4096 rows * 84MHz - 20
	FMC_Bank5_6->SDRTR = 1292 << FMC_SDRTR_COUNT_Pos;
}

static void
modm_board_init()
{
	// initialize system clock and external SDRAM before accessing external memories
	SystemClock::enable();
	board_initialize_sdram();
}

MODM_HARDWARE_INIT(modm_board_init);
