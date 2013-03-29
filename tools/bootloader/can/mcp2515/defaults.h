// coding: utf-8
// ----------------------------------------------------------------------------
/* Copyright (c) 2010, Roboterclub Aachen e.V.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *  * Neither the name of the Roboterclub Aachen e.V. nor the
 *    names of its contributors may be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ROBOTERCLUB AACHEN E.V. ''AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL ROBOTERCLUB AACHEN E.V. BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
// ----------------------------------------------------------------------------

#ifndef	DEFAULTS_H
#define	DEFAULTS_H

#include "config.h"
#include "mcp2515.h"

// ----------------------------------------------------------------------------
// Create pagesize identifier

#if	SPM_PAGESIZE == 32
	#define	PAGESIZE_IDENTIFIER		(0)
#elif SPM_PAGESIZE == 64
	#define	PAGESIZE_IDENTIFIER		(1)
#elif SPM_PAGESIZE == 128
	#define	PAGESIZE_IDENTIFIER		(2)
#elif SPM_PAGESIZE == 256
	#define	PAGESIZE_IDENTIFIER		(3)
#else
	#error	Strange value for SPM_PAGESIZE. Check the define!
#endif

// -----------------------------------------------------------------------------
// Check defines for the bootloader led

#ifdef BOOT_LED
	#ifndef	BOOT_LED_SET_OUTPUT	
		#define	BOOT_LED_SET_OUTPUT		SET_OUTPUT(BOOT_LED)
	#endif
	
	#ifndef	BOOT_LED_ON
		#define	BOOT_LED_ON				SET(BOOT_LED)
	#endif
	
	#ifndef	BOOT_LED_OFF
		#define	BOOT_LED_OFF			RESET(BOOT_LED)
	#endif
	
	#ifndef	BOOT_LED_TOGGLE	
		#define	BOOT_LED_TOGGLE			TOGGLE(BOOT_LED)
	#endif
#else
	#define	BOOT_LED_SET_OUTPUT			
	#define	BOOT_LED_ON
	#define	BOOT_LED_OFF
	#define	BOOT_LED_TOGGLE
	
	#warning	compiling bootloader without LED support
#endif

// -----------------------------------------------------------------------------
#ifndef	BOOT_INIT
	#define	BOOT_INIT
#endif

#ifndef	BOOTLOADER_TYPE
	#define	BOOTLOADER_TYPE		0
#endif

// -----------------------------------------------------------------------------
// Set current version of the bootloader

#define	BOOTLOADER_VERSION		2

// -----------------------------------------------------------------------------
// CAN settings

#ifndef	SPI_PRESCALER
	#define	SPI_PRESCALER		8
#endif

#ifndef	MCP2515_BITRATE
	#define	MCP2515_BITRATE		125
#endif

#ifndef	MCP2515_INTERRUPTS
	#define	MCP2515_INTERRUPTS	(1<<RX1IE)|(1<<RX0IE)
#endif

#ifndef	MCP2515_CLKOUT_PRESCALER
	#define	MCP2515_CLKOUT_PRESCALER	0
#endif

// ----------------------------------------------------------------------------
// Set a few AVR specific defines

#if defined(__AVR_ATmega8__)
	// $0C00
	#define	RWW_PAGES	96
	#define	RAMSTART	0x0060
	#define	SIG_FAMILY	0x93
	#define	SIG_DEVICE	0x07
	#define	TIMER_INTERRUPT_FLAG_REGISTER	TIFR
	
#elif defined(__AVR_ATmega88__)
	// $0C00
	#define	RWW_PAGES	96
	#define	RAMSTART	0x0100
	#define	SIG_FAMILY	0x93
	#define	SIG_DEVICE	0x0A
	#define	TIMER_INTERRUPT_FLAG_REGISTER	TIFR1
	
#elif defined(__AVR_ATmega168__)
	// $1C00
	#define	RWW_PAGES	120
	#define	RAMSTART	0x0100
	#define	SIG_FAMILY	0x94
	#define	SIG_DEVICE	0x06
	#define	TIMER_INTERRUPT_FLAG_REGISTER	TIFR1
	
#elif defined(__AVR_ATmega328P__)
	// $3800
	#define	RWW_PAGES	224
	#define	RAMSTART	0x0100
	#define	SIG_FAMILY	0x95
	#define	SIG_DEVICE	0x0f
	#define	TIMER_INTERRUPT_FLAG_REGISTER	TIFR1
	
#elif defined(__AVR_ATmega16__)	// TODO
	// $1C00
	#define	RWW_PAGES	120
	#define	RAMSTART	0x0060
	#define	SIG_FAMILY	0x94
	#define	SIG_DEVICE	0x03
	#define	TIMER_INTERRUPT_FLAG_REGISTER	TIFR
	
#elif defined(__AVR_ATmega32__)	// TODO
	// $3800
	#define	RWW_PAGES	224
	#define	RAMSTART	0x0060
	#define	SIG_FAMILY	0x95
	#define	SIG_DEVICE	0x02
	#define	TIMER_INTERRUPT_FLAG_REGISTER	TIFR
	
#elif defined(__AVR_ATmega644__)	// TODO
	// $7000
	#define	RWW_PAGES	224
	#define	RAMSTART	0x0100
	#define	SIG_FAMILY	0x96
	#define	SIG_DEVICE	0x09
	#define	TIMER_INTERRUPT_FLAG_REGISTER	TIFR1
	
#else
	#error	chosen AVR command is not supported yet!
#endif


#if 	defined(__AVR_ATmega8__) || \
		defined(__AVR_ATmega16__) || \
		defined(__AVR_ATmega32__)

	#define	IV_REG	GICR
#elif 	defined(__AVR_ATmega88__) || \
		defined(__AVR_ATmega168__) || \
		defined(__AVR_ATmega328P__) || \
		defined(__AVR_ATmega644__) || \
		defined(__AVR_AT90CAN32__) || \
		defined(__AVR_AT90CAN64__) || \
		defined(__AVR_AT90CAN128__)
	
	#define	IV_REG	MCUCR
#else
	#error	chosen AVR type is not supported yet!
#endif

// ----------------------------------------------------------------------------
// Select timer settings for the current crystal frequency (500ms)
// 
// TIMER_PRELOAD = 65536 - (0.5s * F_CPU) / 1024

#if F_CPU == 4000000UL
	#define	TIMER_PRESCALER		(1<<CS12)|(1<<CS10)		// Prescaler = 1024
	#define TIMER_PRELOAD		63583
#elif F_CPU == 7372800UL
	#define	TIMER_PRESCALER		(1<<CS12)|(1<<CS10)		// Prescaler = 1024
	#define	TIMER_PRELOAD		61936
#elif F_CPU == 8000000UL
	#define	TIMER_PRESCALER		(1<<CS12)|(1<<CS10)		// Prescaler = 1024
	#define	TIMER_PRELOAD		61629
#elif F_CPU == 14745600UL
	#define	TIMER_PRESCALER		(1<<CS12)|(1<<CS10)		// Prescaler = 1024
	#define	TIMER_PRELOAD		58336
#elif F_CPU == 16000000UL
	#define	TIMER_PRESCALER		(1<<CS12)|(1<<CS10)		// Prescaler = 1024
	#define	TIMER_PRELOAD		57724
#elif F_CPU == 20000000UL
	#define	TIMER_PRESCALER		(1<<CS12)|(1<<CS10)		// Prescaler = 1024
	#define	TIMER_PRELOAD		55770
#else
	#error	choosen F_CPU not supported yet!
#endif

// ----------------------------------------------------------------------------
// MCP2515 defaults

#if defined(__AVR_ATmega16__) || defined(__AVR_ATmega32__) || \
	defined(__AVR_ATmega644__)
	#define	P_MOSI	B,5
	#define	P_MISO	B,6
	#define	P_SCK	B,7
	
	#define	PORT_SPI	PORTB
	#define	DDR_SPI		DDRB
#elif defined(__AVR_ATmega8__)  || defined(__AVR_ATmega48__) || \
	  defined(__AVR_ATmega88__) || defined(__AVR_ATmega168__) || defined(__AVR_ATmega328P__)
	#define	P_MOSI	B,3
	#define	P_MISO	B,4
	#define	P_SCK	B,5
	
	#define	PORT_SPI	PORTB
	#define	DDR_SPI		DDRB
#else
	#error	the choosen AVR Type is not yet supported by this library.
#endif

#ifndef	MCP2515_CS
	#error	MCP2515_CS ist nicht definiert!
#endif

#ifdef	SPI_PRESCALER
	#if (SPI_PRESCALER == 2) || (SPI_PRESCALER == 8) || (SPI_PRESCALER == 32) || (SPI_PRESCALER == 64)
		#define	R_SPSR	(1<<SPI2X)
		#define SPI_PRESCALER_ 	(SPI_PRESCALER * 2)
	#else
		#define	R_SPSR	0
		#define	SPI_PRESCALER_	SPI_PRESCALER
	#endif
	
	#if (SPI_PRESCALER_ == 4)
		#define	R_SPCR	(0<<SPR1)|(0<<SPR0)
	#elif (SPI_PRESCALER_ == 16)
		#define	R_SPCR	(0<<SPR1)|(1<<SPR0)
	#elif (SPI_PRESCALER_ == 64)
		#define	R_SPCR	(1<<SPR1)|(0<<SPR0)
	#elif (SPI_PRESCALER_ == 128)
		#define	R_SPCR	(1<<SPR1)|(1<<SPR0)
	#else
		#error	 SPI_PRESCALER must be one of the values of 2^n with n = 2..7!
	#endif
#else
	#error	SPI_PRESCALER not defined!
#endif

#ifndef	MCP2515_CLKOUT_PRESCALER
	#error	MCP2515_CLKOUT_PRESCALER not defined!
#elif MCP2515_CLKOUT_PRESCALER == 0
	#define	CLKOUT_PRESCALER_	0x0
#elif MCP2515_CLKOUT_PRESCALER == 1
	#define	CLKOUT_PRESCALER_	0x4
#elif MCP2515_CLKOUT_PRESCALER == 2
	#define	CLKOUT_PRESCALER_	0x5
#elif MCP2515_CLKOUT_PRESCALER == 4
	#define	CLKOUT_PRESCALER_	0x6
#elif MCP2515_CLKOUT_PRESCALER == 8
	#define	CLKOUT_PRESCALER_	0x7
#else
	#error	invaild value of MCP2515_CLKOUT_PRESCALER
#endif

#ifndef	MCP2515_BITRATE
	#error	MCP2515_BITRATE not defined!
#else
	#if	MCP2515_BITRATE == 125
		// 125 kbps
		#define	R_CNF3	((1<<PHSEG21))
		#define	R_CNF2	((1<<BTLMODE)|(1<<PHSEG11))
		#define	R_CNF1	((1<<BRP2)|(1<<BRP1)|(1<<BRP0))
	#elif MCP2515_BITRATE == 250
		// 250 kbps
		#define	R_CNF3	((1<<PHSEG21))
		#define	R_CNF2	((1<<BTLMODE)|(1<<PHSEG11))
		#define	R_CNF1	((1<<BRP1)|(1<<BRP0))
	#elif MCP2515_BITRATE == 500
		// 500 kbps
		#define	R_CNF3	((1<<PHSEG21))
		#define	R_CNF2	((1<<BTLMODE)|(1<<PHSEG11))
		#define	R_CNF1	((1<<BRP0))
	#elif MCP2515_BITRATE == 1000
		// 1 Mbps
		#define	R_CNF3	((1<<PHSEG21))
		#define	R_CNF2	((1<<BTLMODE)|(1<<PHSEG11))
		#define	R_CNF1	(0)
	#else
		#error invalid value for MCP2515_BITRATE
	#endif
#endif

#endif	// DEFAULTS_H
