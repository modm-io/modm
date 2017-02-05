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

#include "stm32f469_discovery.hpp"
#include <modm/utils/bit_constants.hpp>

// ----------------------------------- SDRAM -----------------------------------
static void
sdram_configure_gpio(GPIO_TypeDef * const port, const uint16_t mask)
{
	// For all GPIOs:
	// - Pull-Up
	// - Fast Speed
	// - Alternate Function 12
	// => MODER = 0b10
	// => OTYPER = 0b0
	// => OSPEEDR = 0b11
	// => PUPDR = 0b01
	// => AFR = 0b1100

	// compute the 0b11 and 0b1111 spreads
	uint32_t portMask2 = 0;
	uint32_t portMask4[2] = {0,0};
	for (int ii = 0; ii < 16; ii++) {
		if (mask & (1 << ii)) {
			portMask2 |= (0b11 << (ii * 2));
			if (ii < 8) {
				portMask4[0] |= (0b1111 << (ii * 4));
			} else {
				portMask4[1] |= (0b1111 << ((ii - 8) * 4));
			}
		}
	}
	// mask to get the actual bit masks
	const uint32_t port01 = 0x55555555 & portMask2;
	const uint32_t port10 = 0xAAAAAAAA & portMask2;
	const uint32_t port1100L = 0xCCCCCCCC & portMask4[0];
	const uint32_t port1100H = 0xCCCCCCCC & portMask4[1];

	port->AFR[0]  = port1100L;
	port->AFR[1]  = port1100H;
	port->OSPEEDR = port10 | port01;
	port->PUPDR   = port01;
	port->MODER   = port10;
	port->OTYPER &= ~mask;

	// We lock the corresponding GPIO pins, so that users do not shoot
	// themselves in their feet, if they accidentally use SDRAM GPIOs.
	port->LCKR = 0x10000 | mask;
	port->LCKR = 0x00000 | mask;
	port->LCKR = 0x10000 | mask;
	(void) port->LCKR;
}

static bool
sdram_send_command(uint8_t mode, uint8_t auto_refresh = 1, uint16_t mode_register = 0)
{
	FMC_Bank5_6->SDCMR =	((mode_register << 9) & FMC_SDCMR_MRD) |
							(((auto_refresh - 1) << 5) & FMC_SDCMR_NRFS) |
							FMC_SDCMR_CTB1 | (mode & FMC_SDCMR_MODE);

	int t = 1024;
	for (; (FMC_Bank5_6->SDSR & FMC_SDSR_BUSY) and t; t--) {
		modm::delayMilliseconds(1);
	}

	return t;
}

void
board_initialize_sdram()
{
	using namespace modm;
	// Strongly inspired by the F469I-DISCO BSP from ST

	// Configure all pins
	// C0
	sdram_configure_gpio(GPIOC, Bit0);
	// D0, D1, D8, D9, D10, D14, D15
	sdram_configure_gpio(GPIOD, Bit0 | Bit1 | Bit8 | Bit9 | Bit10 | Bit14 | Bit15);
	// E0, E1, E7, E8, E9, E10, E11, E12, E13, E14, E15
	sdram_configure_gpio(GPIOE, Bit0 | Bit1 | Bit7 | Bit8 | Bit9 | Bit10 | Bit11 | Bit12 | Bit13 | Bit14 | Bit15);
	// F0, F1, F2, F3, F4, F5, F11, F12, F13, F14, F15
	sdram_configure_gpio(GPIOF, Bit0 | Bit1 | Bit2 | Bit3 | Bit4 | Bit5 | Bit11 | Bit12 | Bit13 | Bit14 | Bit15);
	// G0, G1, G4, G5, G8, G15
	sdram_configure_gpio(GPIOG, Bit0 | Bit1 | Bit4 | Bit5 | Bit8 | Bit15);
	// H2, H3, H8, H9, H10, H11, H12, H13, H14, H15
	sdram_configure_gpio(GPIOH, Bit2 | Bit3 | Bit8 | Bit9 | Bit10 | Bit11 | Bit12 | Bit13 | Bit14 | Bit15);
	// I0, I1, I2, I3, I4, I5, I6, I7, I9, I10
	sdram_configure_gpio(GPIOI, Bit0 | Bit1 | Bit2 | Bit3 | Bit4 | Bit5 | Bit6 | Bit7 | Bit9 | Bit10);

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
	modm::delayMilliseconds(1);
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
