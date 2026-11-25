/*
 * Copyright (c) 2015-2017, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include "board.hpp"

// ----------------------------------- SDRAM -----------------------------------
static bool
sdram_send_command(uint8_t mode, uint8_t auto_refresh = 1, uint16_t mode_register = 0)
{
	FMC_Bank5_6->SDCMR =	((mode_register << 9) & FMC_SDCMR_MRD) |
							(((auto_refresh - 1) << 5) & FMC_SDCMR_NRFS) |
							FMC_SDCMR_CTB1 | (mode & FMC_SDCMR_MODE);

	int t = 1024;
	for (; (FMC_Bank5_6->SDSR & FMC_SDSR_BUSY) and t; t--) {
		modm::delay_ms(1);
	}

	return t;
}

void
board_initialize_sdram()
{
	using namespace modm;
	// Strongly inspired by the F469I-DISCO BSP from ST

	// Configure all pins as fast push-pull outputs with pull-up on alternate function 12
	using Pins = GpioSet<
		// D0-D15
		GpioD14, GpioD15, GpioD0, GpioD1, GpioE7, GpioE8, GpioE9, GpioE10,
		GpioE11, GpioE12, GpioE13, GpioE14, GpioE15, GpioD8, GpioD9, GpioD10,
		// D16-D31
		GpioH8, GpioH9, GpioH10, GpioH11, GpioH12, GpioH13, GpioH14, GpioH15,
		GpioI0, GpioI1, GpioI2, GpioI3, GpioI6, GpioI7, GpioI9, GpioI10,
		// A0-A11, BA0, BA1
		GpioF0, GpioF1, GpioF2, GpioF3, GpioF4, GpioF5, GpioF12, GpioF13, GpioF14,
		GpioF15, GpioG0, GpioG1, GpioG4, GpioG5,
		// SDNE0, SDCKE0, SDCLK, SDNWE, SDNRAS, SDNCAS, NBL0-NBL3
		GpioH3, GpioH2, GpioG8, GpioC0, GpioF11, GpioG15, GpioE0, GpioE1, GpioI4, GpioI5>;
	Pins::configure(modm::platform::Gpio::OutputType::PushPull, modm::platform::Gpio::OutputSpeed::VeryHigh);
	Pins::configure(modm::platform::Gpio::InputType::PullUp);
	Pins::setAlternateFunction(12);
	// Lock the pins, so that they cannot be reconfigured by accident
	Pins::lock();

	// Configure the SDRAM
	// Enable FMC clock
	RCC->AHB3ENR |= RCC_AHB3ENR_FMCEN;
	(void) RCC->AHB3ENR;

	/* SDRAM Control register (only bank 1 is used):
	 * - RPIPE: 0b00 (0 cycle delay)
	 * - RBURST: 0b1 (enabled)
	 * - SDCLK: 0b10 (2x HCLK periods)
	 * - WP: 0x0 (disabled)
	 * - CAS: 0x11 (3 cycles)
	 * - NB: 0b1 (4 internal banks)
	 * - MWID: 0b10 (32bit data bus width)
	 * - NR: 0b01 (12 bit)
	 * - NC: 0b00 (8 bit)
	 */
	FMC_Bank5_6->SDCR[0] =  FMC_SDCR1_RBURST | FMC_SDCR1_SDCLK_1 | FMC_SDCR1_NB | FMC_SDCR1_NR_0 |
							FMC_SDCR1_MWID_1 | FMC_SDCR1_CAS_1 | FMC_SDCR1_CAS_0;

	/* SDRAM Timing register (for 90 MHz as SD clock frequency)
	 * - TRCD: 0b0001 (2 cycles)
	 * - TRP:  0b0001 (2 cycles)
	 * - TWR:  0b0001 (2 cycles)
	 * - TRC:  0b0110 (7 cycles)
	 * - TRAS: 0b0011 (4 cycles)
	 * - TXSR: 0b0110 (7 cycles)
	 * - TMRD: 0b0001 (2 cycles)
	 */
	FMC_Bank5_6->SDTR[0] = 0x01116361;

	// FMC_SDRAM_CMD_CLK_ENABLE
	sdram_send_command(1);
	modm::delay_ms(1);
	// FMC_SDRAM_CMD_PALL
	sdram_send_command(2);
	// FMC_SDRAM_CMD_AUTOREFRESH_MODE
	sdram_send_command(3, 8);
	/* FMC_SDRAM_CMD_LOAD_MODE:
	 * - SDRAM_MODEREG_BURST_LENGTH_1
	 * - SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL
	 * - SDRAM_MODEREG_CAS_LATENCY_3
	 * - SDRAM_MODEREG_OPERATING_MODE_STANDARD
	 * - SDRAM_MODEREG_WRITEBURST_MODE_SINGLE
	 */
	sdram_send_command(4, 1, 0x230);

	// Set refresh rate:
	FMC_Bank5_6->SDRTR = (0x0569 << 1);

	// Zero the entire SDRAM
	memset((void*)0xC0000000, 0, 16*1024*1024);
}
