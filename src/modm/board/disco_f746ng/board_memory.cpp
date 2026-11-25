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
// MT48LC4M32B2 with 4 banks of 4096 rows x 256 columns on SDNE0, of which only
// the lower 16 data bits are connected (UM1907 Section 6.13)
static void
sdram_command(uint32_t mode, uint32_t refreshes = 1, uint32_t mode_register = 0)
{
	FMC_Bank5_6->SDCMR = (mode_register << FMC_SDCMR_MRD_Pos) | ((refreshes - 1) << FMC_SDCMR_NRFS_Pos) |
						 FMC_SDCMR_CTB1 | mode;
	for (int t = 1024; (FMC_Bank5_6->SDSR & FMC_SDSR_BUSY) and t; t--) modm::delay_us(10);
}

static void
board_initialize_sdram()
{
	using Pins = GpioSet<
		// D0-D15
		GpioD14, GpioD15, GpioD0, GpioD1, GpioE7, GpioE8, GpioE9, GpioE10,
		GpioE11, GpioE12, GpioE13, GpioE14, GpioE15, GpioD8, GpioD9, GpioD10,
		// A0-A11, BA0, BA1
		GpioF0, GpioF1, GpioF2, GpioF3, GpioF4, GpioF5, GpioF12, GpioF13, GpioF14,
		GpioF15, GpioG0, GpioG1, GpioG4, GpioG5,
		// SDNE0, SDCKE0, SDCLK, SDNWE, SDNRAS, SDNCAS, NBL0, NBL1
		GpioH3, GpioC3, GpioG8, GpioH5, GpioF11, GpioG15, GpioE0, GpioE1>;
	Pins::configure(Gpio::OutputType::PushPull, Gpio::OutputSpeed::VeryHigh);
	Pins::setAlternateFunction(12);

	Rcc::enable<Peripheral::Fmc>();
	// SDCLK = HCLK/2 = 108MHz, read burst, CAS latency 2, 4 banks, 16-bit, 12-bit rows, 8-bit columns
	FMC_Bank5_6->SDCR[0] = FMC_SDCR1_SDCLK_1 | FMC_SDCR1_RBURST | FMC_SDCR1_CAS_1 | FMC_SDCR1_NB |
						   FMC_SDCR1_MWID_0 | FMC_SDCR1_NR_0;
	// TRCD=2, TRP=2, TWR=2, TRC=7, TRAS=4, TXSR=7, TMRD=2 cycles
	FMC_Bank5_6->SDTR[0] = 0x01116361;

	sdram_command(1); // clock enable
	modm::delay_ms(1);
	sdram_command(2); // precharge all
	sdram_command(3, 8); // auto refresh
	sdram_command(4, 1, 0x220); // load mode: burst length 1, CAS latency 2, single write burst
	// 64ms / 4096 rows * 108MHz - 20, a little faster for margin
	FMC_Bank5_6->SDRTR = 1539 << FMC_SDRTR_COUNT_Pos;

	// The default memory map makes 0xC0000000 device memory, which does not allow
	// unaligned accesses. Make the SDRAM normal memory, but not cacheable, so that the
	// CPU, DMA2D and LTDC do not need cache maintenance.
	MPU->RNR = 0;
	MPU->RBAR = 0xC000'0000;
	MPU->RASR = MPU_RASR_XN_Msk | (0b011 << MPU_RASR_AP_Pos) | (0b001 << MPU_RASR_TEX_Pos) |
				MPU_RASR_S_Msk | ((23 - 1) << MPU_RASR_SIZE_Pos) | MPU_RASR_ENABLE_Msk; // 8MiB
	MPU->CTRL = MPU_CTRL_PRIVDEFENA_Msk | MPU_CTRL_ENABLE_Msk;
	__DSB();
	__ISB();
}

static void
modm_board_init()
{
	// initialize system clock and external SDRAM before accessing external memories
	SystemClock::enable();
	board_initialize_sdram();
}

MODM_HARDWARE_INIT(modm_board_init);
