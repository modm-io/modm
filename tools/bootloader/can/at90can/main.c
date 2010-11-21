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
 *
 * $Id$
 */
// ----------------------------------------------------------------------------
/**
 * \brief	CAN Bootloader
 * 
 * 
 * \version	$Id$
 * \author	Fabian Greif <fabian.greif@rwth-aachen.de>
 * \author	Adrian Weiler
 */
// ----------------------------------------------------------------------------

#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/boot.h>
#include <avr/eeprom.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"
#include "defaults.h"

#include "at90can.h"

#define	MESSAGE_NUMBER			message_number
#define	MESSAGE_DATA_COUNTER	message_data_counter
#define	MESSAGE_DATA_LENGH		message_data_length
#define	MESSAGE_DATA(x)			message_data[x]

// ----------------------------------------------------------------------------
// globale Variablen
static uint16_t flashpage = 0;
static uint8_t page_buffer_pos = 0;
static uint8_t page_buffer[SPM_PAGESIZE];

// -----------------------------------------------------------------------------
// Watchdog Timer als erstes im Programm deaktivieren
// siehe http://www.nongnu.org/avr-libc/user-manual/group__avr__watchdog.html

void
disable_watchdog(void) \
		__attribute__((naked)) \
		__attribute__((section(".init3")));

void
disable_watchdog(void)
{
	// Save MCUSR so that the main program can access if later
	GPIOR0 = MCUSR;
	MCUSR = 0;
	wdt_disable();
}

// ----------------------------------------------------------------------------
/**
 * \brief	starts the application program
 */
void
boot_jump_to_application(void)
{
	// relocate interrupt vectors
	uint8_t reg = MCUCR & ~((1 << IVCE) | (1 << IVSEL));
	
	MCUCR = reg | (1 << IVCE);
	MCUCR = reg;
	
	// reset SPI interface to power-up state
	SPCR = 0;
	SPSR = 0;
	
#if FLASHEND > 0xffff
	__asm__ __volatile__(
			"push __zero_reg__" "\n\t"
			"push __zero_reg__" "\n\t"
			"push __zero_reg__" "\n\t");
#else
	__asm__ __volatile__(
			"push __zero_reg__" "\n\t"
			"push __zero_reg__" "\n\t");
#endif
	
	// when the functions executes the 'ret' command to return to
	// its origin the AVR loads the return address from the stack. Because we
	// pushed null it instead jumps to address null which starts the main
	// application.
}

// ----------------------------------------------------------------------------
/**
 * \brief	write a complete page to the flash memorey
 * 
 * \param	page	page which should be written
 * \param	*buf	Pointer to the buffer with the data
 *
 * \see		avr-libc Documentation > Modules > Bootloader Support Utilities
 */
void
boot_program_page(uint16_t page, uint8_t *buf)
{
	uint32_t adr = page * SPM_PAGESIZE;
	
	boot_page_erase(adr);
	boot_spm_busy_wait();	  // Wait until the memory is erased.
	
	for (uint16_t i=0; i < SPM_PAGESIZE; i+=2)
	{
		// Set up little-endian word.
		uint16_t w = *buf++;
		w += (*buf++) << 8;
		
		boot_page_fill(adr + i, w);
	}
	
	boot_page_write(adr);		// Store buffer in flash page.
	boot_spm_busy_wait();		// Wait until the memory is written.
	
	// Reenable RWW-section again. We need this if we want to jump back
	// to the application after bootloading.
	boot_rww_enable();
}

// ----------------------------------------------------------------------------
int
main(void) __attribute__((OS_main));

int
main(void)
{
	enum { 
		IDLE,
		COLLECT_DATA,
		RECEIVED_PAGE
	} state = IDLE;
	uint8_t next_message_number = -1;
	
	// do some addition initialization
	BOOT_INIT;
	
	BOOT_LED_SET_OUTPUT;
	BOOT_LED_ON;
	
	// Relocate interrupt vectors to boot area
	MCUCR = (1 << IVCE);
	MCUCR = (1 << IVSEL);
	
	at90can_init();
	
	// start timer
	TCNT1 = TIMER_PRELOAD;
	TCCR1A = 0;
	TCCR1B = TIMER_PRESCALER;

	// clear overflow-flag
	TIMER_INTERRUPT_FLAG_REGISTER = (1 << TOV1);
	
	sei();

	while (1)
	{
		uint8_t command;
		uint16_t page;
		static uint8_t next_message_data_counter;
		
		// wait until we receive a new message
		while ((command = at90can_get_message()) == NO_MESSAGE)
		{
			if (TIMER_INTERRUPT_FLAG_REGISTER & (1 << TOV1))
			{
				BOOT_LED_OFF;
				
				// timeout => start application
				boot_jump_to_application();
			}
		}
		
		// stop timer
		TCCR1B = 0;
		
		// check if the message is a request, otherwise reject it
		if ((command & ~COMMAND_MASK) != REQUEST)
			continue;
		
		command &= COMMAND_MASK;
		
		// check message number
		next_message_number++;
		if (message_number != next_message_number)
		{
			// wrong message number => send NACK
			message_number = next_message_number;
			next_message_number--;
			at90can_send_message(command | WRONG_NUMBER_REPSONSE, 0);
			continue;
		}
		
		BOOT_LED_TOGGLE;
		
		// process command
		switch (command)
		{
		case IDENTIFY:
			// version and command of the bootloader
			message_data[0] = (BOOTLOADER_TYPE << 4) | (BOOTLOADER_VERSION & 0x0f);
			message_data[1] = PAGESIZE_IDENTIFIER;
			
			// number of writeable pages
			message_data[2] = HIGH_BYTE(RWW_PAGES);
			message_data[3] = LOW_BYTE(RWW_PAGES);
			
			at90can_send_message(IDENTIFY | SUCCESSFULL_RESPONSE, 4);
			break;
		
		// --------------------------------------------------------------------
		// set the current address in the page buffer
		case SET_ADDRESS:
			page = (message_data[0] << 8) | message_data[1];
			
			if (message_data_length == 4 && 
					message_data[2] < (SPM_PAGESIZE / 4) &&
					page < RWW_PAGES)
			{
				flashpage = page;
				page_buffer_pos = message_data[3];
				
				state = COLLECT_DATA;
				
				at90can_send_message(SET_ADDRESS | SUCCESSFULL_RESPONSE, 4);
			}
			else {
				goto error_response;
			}
			break;
		
		// --------------------------------------------------------------------
		// collect data
		case DATA:
			if (message_data_length != 4 ||
					page_buffer_pos >= (SPM_PAGESIZE / 4) ||
					state == IDLE) {
				state = IDLE;
				goto error_response;
			}
			
			// check if the message starts a new block
			if (message_data_counter & START_OF_MESSAGE_MASK)
			{
				message_data_counter &= ~START_OF_MESSAGE_MASK;		// clear flag
				next_message_data_counter = message_data_counter;
				state = COLLECT_DATA;
			}
			
			if (message_data_counter != next_message_data_counter) {
				state = IDLE;
				goto error_response;
			}
			next_message_data_counter--;
			
			// copy data
			memcpy(page_buffer + page_buffer_pos * 4, &message_data[0], 4);
			page_buffer_pos++;
			
			if (message_data_counter == 0)
			{
				if (page_buffer_pos == (SPM_PAGESIZE / 4))
				{
					message_data[0] = flashpage >> 8;
					message_data[1] = flashpage & 0xff;
					
					if (flashpage >= RWW_PAGES) {
						message_data_length = 2;
						goto error_response;
					}
					
					boot_program_page( flashpage, page_buffer );
					page_buffer_pos = 0;
					flashpage += 1;
					
					// send ACK
					at90can_send_message(DATA | SUCCESSFULL_RESPONSE, 2);
				}
				else {
					at90can_send_message(DATA | SUCCESSFULL_RESPONSE, 0);
				}
			}
			break;
		
		// --------------------------------------------------------------------
		// start the flashed application program
		case START_APP:
			at90can_send_message(START_APP | SUCCESSFULL_RESPONSE, 0);
			
			// wait for the mcp2515 to send the message
			_delay_ms(50);
			
			// start application
			BOOT_LED_OFF;
			boot_jump_to_application();
			break;
		
#if BOOTLOADER_TYPE > 0
		// --------------------------------------------------------------------
		case GET_FUSEBITS:
			message_data[0] = boot_lock_fuse_bits_get(GET_LOCK_BITS);
			message_data[1] = boot_lock_fuse_bits_get(GET_HIGH_FUSE_BITS);
			message_data[2] = boot_lock_fuse_bits_get(GET_LOW_FUSE_BITS);
			message_data[3] = boot_lock_fuse_bits_get(GET_EXTENDED_FUSE_BITS);
			
			at90can_send_message(GET_FUSEBITS | SUCCESSFULL_RESPONSE, 4);
			break;
		
		// --------------------------------------------------------------------
		case CHIP_ERASE:
			// erase complete flash except the bootloader region
			for (uint16_t i = 0; i < RWW_PAGES; i++ )
			{
				uint32_t adr = i * SPM_PAGESIZE;
				
				boot_page_erase( adr );
				boot_spm_busy_wait();
			}
			boot_rww_enable();
			
			at90can_send_message(CHIP_ERASE | SUCCESSFULL_RESPONSE, 0);
			break;
		
		// Lese 1..6 Byte aus dem EEprom
		case READ_EEPROM:
			if (message_length == 5 && MESSAGE_DATA[4] > 0 && MESSAGE_DATA[4] <= 6)
			{
				uint16_t ee_ptr = (MESSAGE_DATA[2] << 8) | MESSAGE_DATA[3];
				
				if (ee_ptr <= E2END) {
					message_length = MESSAGE_DATA[4] + 2;
					eeprom_read_block(MESSAGE_DATA + 2, (void *) ee_ptr, MESSAGE_DATA[4]);
					break;
				}
			}
			response = NACK;
			break;
		
		// schreibe 1..4 Byte ins EEprom
		case WRITE_EEPROM:
			if (message_length > 4) {
				uint16_t ee_ptr = (MESSAGE_DATA[2] << 8) | MESSAGE_DATA[3];
				
				if (ee_ptr <= E2END) {
					eeprom_write_block(MESSAGE_DATA + 4, (void *) ee_ptr, message_length - 4);
					response = ACK;
					break;
				}
			}
			response = NACK;
			break;
		
		// Lese 1..65556 Byte aus dem Flash. Bei mehr als 6 Zeichen
		// wird das Ergebniss auf mehrere Nachrichten verteilt.
		case READ_FLASH:
			if (message_length == 6)
			{
				uint16_t flash_ptr = (MESSAGE_DATA[2] << 8) | MESSAGE_DATA[3];
				
				if (flash_ptr <= FLASHEND)
				{
					uint16_t number = (MESSAGE_DATA[4] << 8) | MESSAGE_DATA[5];
					
					// Anzahl der zu senden Nachrichten bestimmen
					div_t r = div(number, 6);
					number = r.quot;
					if (r.rem > 0)
						number += 1;
					
					uint16_t counter = 0;
					for (uint16_t i=0;i<number;i++)
					{
						if (i == r.quot)
							// das letze Paket ist eventl. kuerzer
							message_length = r.rem;
						else
							message_length = 6;
						
						// FIXME
						//memcpy_P( MESSAGE_DATA + 2, (PGM_VOID_P) flash_ptr, message_length );
						flash_ptr += message_length;
						
						MESSAGE_DATA[1] = counter;
						counter = (counter + 1) & 0x3f;
						
						// Nachricht verschicken
						at90can_send_message(0);
					}
					
					response = ACK;
					break;
				}
			}
			response = NACK;
			break;
#endif
		
		error_response:
		default:
			at90can_send_message(command | ERROR_RESPONSE, message_data_length);
			break;
		}
	}
}
