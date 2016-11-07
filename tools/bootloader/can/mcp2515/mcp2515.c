/*
 * Copyright (c) 2010, Fabian Greif
 * Copyright (c) 2012, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <avr/pgmspace.h>
#include <util/delay.h>

#include "utils.h"
#include "defaults.h"

// ----------------------------------------------------------------------------
// Prototypes for the functions defined in assembler
uint8_t
spi_putc(uint8_t data);

void
mcp2515_write_register(uint8_t adress, uint8_t data);

// ----------------------------------------------------------------------------
static uint8_t PROGMEM mcp2515_register_map[45] = {
	0xff, 0xc0, 0x00, 0x00,				// Filter 0
	0xff, 0xc0, 0x00, 0x00,				// Filter 1
	0xff, 0xc0, 0x00, 0x00,				// Filter 2
	0,									// BFPCTRL
	0,									// TXRTSCTRL
	0,									// CANSTAT (read-only)
	(1<<REQOP2) | CLKOUT_PRESCALER_,	// CANCTRL
	0xff, 0xc0, 0x00, 0x00,				// Filter 3
	0xff, 0xc0, 0x00, 0x00,				// Filter 4
	0xff, 0xc0, 0x00, 0x00,				// Filter 5
	0,									// TEC (read-only)
	0,									// REC (read-only)
	0,									// CANSTAT (read-only)
	(1<<REQOP2) | CLKOUT_PRESCALER_,	// CANCTRL
	0xff, 0xc0, 0x00, 0x00,				// Mask 0
	0xff, 0xc0, 0x00, 0x00,				// Mask 1
	R_CNF3,
	R_CNF2,
	R_CNF1,
	MCP2515_INTERRUPTS,
	0							// clear interrupt flags
};

// ----------------------------------------------------------------------------
// abgespeckte Variante der "normalen" MCP2515 Initialisierung

void mcp2515_init(void)	__attribute__((naked)) \
						__attribute__((section(".init3")));
void mcp2515_init(void)
{
	// Aktivieren der Pins fuer das SPI Interface
	PORT_SPI &=  ~((1 << PIN_NUM(P_SCK)) | (1 << PIN_NUM(P_MOSI)));
	DDR_SPI |= (1 << PIN_NUM(P_SCK)) | (1 << PIN_NUM(P_MOSI));
	
	SET(MCP2515_CS);
	SET_OUTPUT(MCP2515_CS);
	
	// Aktivieren des SPI Master Interfaces
	SPCR = (1 << SPE) | (1 << MSTR) | R_SPCR;
	SPSR = R_SPSR;
	
	_delay_us(1);
	
	// MCP2515 per Software Reset zuruecksetzten,
	// danach ist er automatisch im Konfigurations Modus
	RESET(MCP2515_CS);
	spi_putc(SPI_RESET);
	SET(MCP2515_CS);
	
	// ein bisschen warten bis der MCP2515 sich neu gestartet hat
	_delay_ms(0.1);
	
	// Filter usw. setzen
	RESET(MCP2515_CS);
	spi_putc(SPI_WRITE);
	spi_putc(RXF0SIDH);
	for (uint8_t i = 0; i < sizeof(mcp2515_register_map); i++) {
		spi_putc(pgm_read_byte(&mcp2515_register_map[i]));
	}
	SET(MCP2515_CS);
	
	// nur Standard IDs, Message Rollover nach Puffer 1
	mcp2515_write_register(RXB0CTRL, (0 << RXM1) | (1 << RXM0) | (1 << BUKT));
	mcp2515_write_register(RXB1CTRL, (0 << RXM1) | (1 << RXM0));
	
	// MCP2515 zurueck in den normalen Modus versetzten
	mcp2515_write_register(CANCTRL, CLKOUT_PRESCALER_);
}
