// coding: utf-8
// ----------------------------------------------------------------------------
/* Copyright (c) 2011, Roboterclub Aachen e.V.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Roboterclub Aachen e.V. nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
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

#include <avr/interrupt.h>
#include <xpcc/architecture/platform/avr/atxmega.hpp>

#include "interconnect_link.hpp"
#include <xpcc_config.hpp>

#if defined(CPU_BOARD2_MASTER)

	// Atxmega128a1
#	define INTERCONNECT_ENABLED

#	undef  INTERCONNECT_USART_PORT
#	undef  INTERCONNECT_USART_ID
#	undef  INTERCONNECT_PIN_OUTPUT
#	undef  INTERCONNECT_PIN_INPUT

#	define INTERCONNECT_USART_PORT		C
#	define INTERCONNECT_USART_ID		0
#	define INTERCONNECT_PIN_OUTPUT		C,0
#	define INTERCONNECT_PIN_INPUT		C,1

#elif defined(CPU_BOARD2_SLAVE)

	// Atxmega32a4
#	define INTERCONNECT_ENABLED

#	undef  INTERCONNECT_USART_PORT
#	undef  INTERCONNECT_USART_ID
#	undef  INTERCONNECT_PIN_OUTPUT
#	undef  INTERCONNECT_PIN_INPUT

#	define INTERCONNECT_USART_PORT		D
#	define INTERCONNECT_USART_ID		1
#	define INTERCONNECT_PIN_OUTPUT		D,4
#	define INTERCONNECT_PIN_INPUT		D,5

#elif INTERCONNECT_USART_PORT != -1 || INTERCONNECT_USART_ID != -1 || \
	  INTERCONNECT_PIN_OUTPUT != -1 || INTERCONNECT_PIN_INPUT != -1

	// check that every needed define is there
#	if  INTERCONNECT_USART_PORT == -1 || INTERCONNECT_USART_ID == -1 || \
		INTERCONNECT_PIN_OUTPUT == -1 || INTERCONNECT_PIN_INPUT == -1
#		error "Interconnect link was not configured correctly! Please specify all necessary defines."
#	endif
#	define INTERCONNECT_ENABLED

#else
	// Disable Interconnect link
	#warning "Interconnect disabled"
#endif

#ifdef INTERCONNECT_ENABLED

// Helper macros
#define INTERCONNECT_PORT(x)				INTERCONNECT_PORT2(x)
#define INTERCONNECT_PORT2(port, id)		port

#define INTERCONNECT_USART		CONCAT(INTERCONNECT_USART_PORT, INTERCONNECT_USART_ID)	// e.g. D1

// Extra macros for _DATA and _STATUS because the compiler can optimize better
// if the registers are accessed directly and not over the base struct.
#define	USART_BASE			CONCAT(USART, INTERCONNECT_USART)				// e.g. USARTD1
#define USART_BASE_DATA		CONCAT3(USART, INTERCONNECT_USART, _DATA)		// e.g. USARTD1_DATA
#define USART_BASE_STATUS	CONCAT3(USART, INTERCONNECT_USART, _STATUS)	// e.g. USARTD1_STATUS

#define USART_DMA_RECEIVE_TRIGGER			CONCAT3(DMA_CH_TRIGSRC_USART, INTERCONNECT_USART, _RXC_gc)
#define USART_DMA_TRANSMIT_TRIGGER			CONCAT3(DMA_CH_TRIGSRC_USART, INTERCONNECT_USART, _DRE_gc)

#define USART_TRANSMISSION_COMPLETE_VECTOR	CONCAT3(USART, INTERCONNECT_USART, _TXC_vect)
#define USART_SLAVE_SELECT_INTERRUPT_VECTOR CONCAT3(PORT, INTERCONNECT_PORT(INTERCONNECT_PIN_INPUT), _INT0_vect)

// ----------------------------------------------------------------------------
// Used pins

#if INTERCONNECT_USART_ID == 0
	GPIO__INPUT3(UsartRx, INTERCONNECT_USART_PORT, 2);
	GPIO__OUTPUT3(UsartTx, INTERCONNECT_USART_PORT, 3);
#else
	GPIO__INPUT3(UsartRx, INTERCONNECT_USART_PORT, 6);
	GPIO__OUTPUT3(UsartTx, INTERCONNECT_USART_PORT, 7);
#endif

GPIO__INPUT2(UsartSS, INTERCONNECT_PIN_INPUT);
GPIO__OUTPUT2(UsartMS, INTERCONNECT_PIN_OUTPUT);

// ----------------------------------------------------------------------------
uint16_t xpcc::InterconnectLink::sizeReceiveBuffer;
uint16_t xpcc::InterconnectLink::sizeTransmitBuffer;

uint8_t *xpcc::InterconnectLink::receiveBufferUser;
uint8_t *xpcc::InterconnectLink::receiveBufferSystem;
uint8_t *xpcc::InterconnectLink::transmitBuffer;

volatile xpcc::InterconnectLink::ReceptionState xpcc::InterconnectLink::stateReception;
volatile xpcc::InterconnectLink::TransmissionState xpcc::InterconnectLink::stateTransmission;

bool xpcc::InterconnectLink::newDataReceived;

// ----------------------------------------------------------------------------
void
xpcc::InterconnectLink::initialize()
{
	UsartSS::configureInputSense(xpcc::atxmega::BOTHEDGES);
	UsartSS::configureInterrupt0(xpcc::atxmega::INTERRUPT_LEVEL_MEDIUM);
	
	UsartMS::setOutput();
	UsartMS::set();
	
	xpcc::InterconnectLink::stateReception = xpcc::InterconnectLink::RECEIVE_BUFFER_FREE;
	xpcc::InterconnectLink::stateTransmission = xpcc::InterconnectLink::TRANSMIT_BUFFER_FREE;
	xpcc::InterconnectLink::newDataReceived = false;
	
	configureUsart();
	
	// Enable DMA controller
	DMA.CTRL = DMA_ENABLE_bm | DMA_PRIMODE_CH0123_gc;
	
	configureDmaChannel0(); // transmit
	configureDmaChannel1(); // receive
}

// ----------------------------------------------------------------------------
void
xpcc::InterconnectLink::configureUsart()
{
	UsartRx::setInput();
	UsartTx::set();
	UsartTx::setOutput();
	
	uint16_t ubrr = 0; // TODO
	
	USART_BASE.BAUDCTRLB = static_cast<uint8_t>(ubrr >> 8);
	USART_BASE.BAUDCTRLA = static_cast<uint8_t>(ubrr & 0xff);
	
	// Enable transmit complete interrupt, receiver and transmitter
	// and set  the frame size to 8 bit
	USART_BASE.CTRLA = USART_TXCINTLVL_MED_gc;
	USART_BASE.CTRLC = USART_CHSIZE_8BIT_gc;
	USART_BASE.CTRLB = USART_RXEN_bm | USART_TXEN_bm;
}

void
xpcc::InterconnectLink::configureDmaChannel0()
{
	DMA.CH0.ADDRCTRL = DMA_CH_SRCRELOAD_TRANSACTION_gc  // source is reloaded with initial value
					| DMA_CH_SRCDIR_INC_gc			// mode of changing source
					| DMA_CH_DESTRELOAD_NONE_gc
					| DMA_CH_DESTDIR_FIXED_gc;
	
	DMA.CH0.TRIGSRC = USART_DMA_TRANSMIT_TRIGGER; // trigger source select
	
	DMA.CH0.DESTADDR0 = (((uint16_t) &USART_BASE_DATA) >> 0) & 0xff;
	DMA.CH0.DESTADDR1 = (((uint16_t) &USART_BASE_DATA) >> 8) & 0xff;
	DMA.CH0.DESTADDR2 = 0;
}

void
xpcc::InterconnectLink::configureDmaChannel1()
{
	// Enable error interrupt and disable transaction complete interrupt enable
	DMA.CH1.CTRLB =   DMA_CH_ERRINTLVL_MED_gc | DMA_CH_TRNINTLVL_OFF_gc;
	
	DMA.CH1.ADDRCTRL = DMA_CH_SRCRELOAD_NONE_gc		// source is reloaded with initial value
					| DMA_CH_SRCDIR_FIXED_gc		// mode of changing source
					| DMA_CH_DESTRELOAD_NONE_gc
					| DMA_CH_DESTDIR_INC_gc;
	DMA.CH1.TRIGSRC = USART_DMA_RECEIVE_TRIGGER; // trigger source select
	
	DMA.CH1.SRCADDR0 = (((uint16_t) &USART_BASE_DATA) >> 0) & 0xff;
	DMA.CH1.SRCADDR1 = (((uint16_t) &USART_BASE_DATA) >> 8) & 0xff;
	DMA.CH1.SRCADDR2 = 0;
}

// ----------------------------------------------------------------------------
void
xpcc::InterconnectLink::reactivateTransmission()
{
	// Clear the flags. maybe necessary if interrupt is enabled.
	DMA_CH0_CTRLB |= DMA_CH_TRNIF_bm | DMA_CH_ERRIF_bm;
	
	// enable Error Interrupt and Transaction Complete Interrupt
	DMA_CH0_CTRLB  =  DMA_CH_ERRINTLVL_MED_gc | DMA_CH_TRNINTLVL_MED_gc;

	DMA_CH0_SRCADDR0 = (((uint16_t) transmitBuffer) >> 0) & 0xff;
	DMA_CH0_SRCADDR1 = (((uint16_t) transmitBuffer) >> 8) & 0xff;
	DMA_CH0_SRCADDR2 = 0;
	
	DMA_CH0_TRFCNT = sizeTransmitBuffer;
	
	DMA_CH0_REPCNT = 1;
	
	DMA_CH0_CTRLA = DMA_CH_ENABLE_bm		// autocleared on transaction end
					| DMA_CH_REPEAT_bm		// autocleared at last repetition
					| DMA_CH_SINGLE_bm
					| DMA_CH_BURSTLEN_1BYTE_gc;
}

// ----------------------------------------------------------------------------
void
xpcc::InterconnectLink::reactivateReception()
{
	// flush unread data
	while (USART_BASE_STATUS & USART_RXCIF_bm) {
		char c = USART_BASE_DATA;
		c = 0;
	}
	DMA.CH1.TRFCNT = sizeReceiveBuffer;
	DMA.CH1.REPCNT = 1;
	
	DMA.CH1.DESTADDR0 = (((uint16_t) receiveBufferSystem) >> 0) & 0xff;
	DMA.CH1.DESTADDR1 = (((uint16_t) receiveBufferSystem) >> 8) & 0xff;
	DMA.CH1.DESTADDR2 = 0;
	
	DMA.CH1.CTRLA = DMA_CH_ENABLE_bm		// autocleared on transaction end
					| DMA_CH_REPEAT_bm		// autocleared at last repetition
					| DMA_CH_SINGLE_bm
					| DMA_CH_BURSTLEN_1BYTE_gc;
}

// ----------------------------------------------------------------------------
void
xpcc::InterconnectLink::makeTransaction()
{
	if (xpcc::InterconnectLink::stateTransmission == xpcc::InterconnectLink::TRANSMIT_BUFFER_FREE)
	{
		xpcc::InterconnectLink::stateTransmission = xpcc::InterconnectLink::TRANSMISSION_IN_PROGRESS;
		
		UsartMS::reset();
		xpcc::InterconnectLink::reactivateTransmission();
	}
}

// ----------------------------------------------------------------------------
static void
finishTransmit()
{
	UsartMS::set();
	xpcc::InterconnectLink::stateTransmission = xpcc::InterconnectLink::TRANSMIT_BUFFER_FREE;
}

// ----------------------------------------------------------------------------
ISR(USART_TRANSMISSION_COMPLETE_VECTOR)
{
	uint8_t c = DMA_CH0_CTRLA;
	if (c & DMA_CH_ENABLE_bm || c & DMA_CH_TRNIF_bm || c & DMA_CH_ERRIF_bm)
	{
	}
	else {
		// DMA disabled => transmission finished
		finishTransmit();
	}
}

ISR(DMA_CH1_vect)
{
	// clear the flag. necessary if interrupt is enabled.
	DMA_CH1_CTRLB |= DMA_CH_ERRIF_bm;
	xpcc::InterconnectLink::reactivateReception();
}

ISR(DMA_CH0_vect)
{
	// Clear the flags. maybe necessary if interrupt is enabled.
	DMA_CH0_CTRLB |= DMA_CH_TRNIF_bm | DMA_CH_ERRIF_bm;
	
	// TODO sth
//	reactivateReceiveCh1();
	if (USART_BASE_STATUS & USART_TXCIF_bm)
	{
		USART_BASE_STATUS &= USART_TXCIF_bm;
		finishTransmit();
	}
}

ISR(USART_SLAVE_SELECT_INTERRUPT_VECTOR)
{
	if (UsartSS::read())
	{
		// transaction complete
		if (DMA_CH1_CTRLA & DMA_CH_ENABLE_bm)
		{
			// errorcondition is trigered if dmacontroller has expected more bytes then received
			DMA_CH1_CTRLA = 0; // deactivate dma controller
			if (!(DMA_CH1_CTRLB & DMA_CH_ERRIF_bm)) { // else dma will trigger its ch1 interrupt
				xpcc::InterconnectLink::reactivateReception();
			}
		}
		else {
			// DMA is deactivated => transaction was completed
			// TODO: fill more checks here, e.g. checksum
			// ...
			if (true)
			{
				// check has succeed ...
				switch (xpcc::InterconnectLink::stateReception)
				{
					// ... switch receive buffers if possible ...
					case xpcc::InterconnectLink::RECEIVE_BUFFER_FREE:
						xpcc::InterconnectLink::switchReceiveBuffers();
						xpcc::InterconnectLink::newDataReceived = false;
						
						xpcc::InterconnectLink::reactivateReception();
						break;
					
					case xpcc::InterconnectLink::RECEIVE_BUFFER_ACCESSED:
						// no activate here
						// do while release
						xpcc::InterconnectLink::newDataReceived = true;
						break;
				}
			}
			else {
				xpcc::InterconnectLink::reactivateReception();
				xpcc::InterconnectLink::newDataReceived = false;
			}
		}
	}
	else {
		// transaction started
		// receiving part, with switching of the buffers
		// DMA controller has to be activated before.
		// if not, no data will be received this time
	}
}

#endif	// INTERCONNECT_ENABLED

