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
/*
 * WARNING: This file is generated automatically, do not edit!
 * Please modify the corresponding *.in file instead and rebuild this file.
 */
// ----------------------------------------------------------------------------

#include "uart2.hpp"
#include <xpcc/architecture/driver/atomic.hpp>

#include "../registers.h"
#include "uart_settings.h"

// ----------------------------------------------------------------------------
// Prototype for IRQ-Installation
extern void
installInterrupt(unsigned long source,
		unsigned long priority, void (* isrFunction)());

// ----------------------------------------------------------------------------
// InterruptID's for UART-Interrupts
#define UART_INT_RLS 	3		// Receive Line Status (is set on errors, see UxLSR[4:1])
#define UART_INT_RDA 	2		// Receive Data Available
#define UART_INT_CTI 	6		// Character Time-out indicator
#define UART_INT_THRE 	1		// THRE interrupt

// Bit Masks for Line Status Register
#define UART_TX_BUFFER_FREE 	0x20
#define UART_RX_HOLDS_CHAR 		0x01

// Bit Masks for Interrupt Information Register
#define UART_INT_PENDING_BM		0x01

// Bit definitions
#define	UART_FIFO_ENABLE				(1 << 0)
#define	UART_RX_FIFO_RESET				(1 << 1)
#define	UART_TX_FIFO_RESET				(1 << 2)

#define	UART_DLAB						(1 << 7)

#define	UART_ENABLE_RBR_INTERRUPT		(1 << 0)
#define	UART_ENABLE_THRE_INTERRUPT		(1 << 1)

// ----------------------------------------------------------------------------
namespace
{
	static xpcc::atomic::Queue<char, 128> receiveQueue;
}

// ----------------------------------------------------------------------------
// UART2 interrupt handler
static void
uart2_interrupt(void)
{
	uint32_t interruptInformation = U2IIR;
	if ((interruptInformation & UART_INT_PENDING_BM) == 0)
	{
		// At least one interrupt is pending => get causing interrupt
		uint32_t interruptCause = (interruptInformation & 0x0E) >> 1;
		
		switch (interruptCause)
		{
			// Receive Line Status, thrown on any kind of error (receive overrun (OE),
			// parity error (PE), framing error (FE) or break interrupt (BI)).
			// Error condition is in UxLSR[4:1].
			// The interrupt is cleared upon an UxLSR read.
			case UART_INT_RLS:
				(void) U2LSR;
				// TODO Error handling or reporting
				break;
			
			// The THRE is activated when the UARTn THR FIFO is empty
			case UART_INT_THRE:
				// TODO reactivate: UxTxEvent.propagate();
				break;
			
            // The RDA is activated when the UARTn Rx FIFO reaches the
			// trigger level defined in UnFCR[7:6] and is reset when the
			// UARTn Rx FIFO depth falls below the trigger level. When the
			// RDA interrupt goes active, the CPU can read a block of
			// data defined by the trigger level.
			case UART_INT_RDA:
				while (U2LSR & UART_RX_HOLDS_CHAR) {
					char c = U2RBR;
					if (!receiveQueue.push(c)) {
						// Error: Queue overflow
						// TODO Error handling or reporting
					}
				}
				break;
			
			// The CTI is set when the UARTnRx FIFO contains at least one
			// character and no UARTn Rx FIFO activity has occurred in 3.5
			// to 4.5 character times.
			// Any UARTn Rx FIFO activity (read or write of UARTn RSR) will
			// clear the interrupt. This interrupt is intended to flush the
			// UARTn RBR after a message has been received that is not a
			// multiple of the trigger level size
			case UART_INT_CTI:
				while (U2LSR & UART_RX_HOLDS_CHAR) {
					char c = U2RBR;
					if (!receiveQueue.push(c)) {
						// Error: Queue overflow
						// TODO Error handling or reporting
					}
				}
				break;
		}
	}
}

// ----------------------------------------------------------------------------
void
Uart2::initialize(uint8_t level)
{
	// Turn power on
	PCONP_p->peripheral.UART2 = 1;
	
	// Enable peripheral clock source
	PCLKSEL1_p->peripheral.UART2 = U2PCLK;
	
	// Baudrate
	// Set DLAB = 1 => access to DLL registers
	U2LCR = UART_DLAB | U2MODE;
	
	U2DLL = U2DLLVAL;
	U2DLM = U2DLMVAL;
	U2FDR = U2DIVMUL; // divaddval & mulval (Frac=0.22 s.b.)
	
	// Enable & clear FIFOs, set RX Trigger level
	U2FCR  = UART_FIFO_ENABLE | UART_RX_FIFO_RESET | UART_TX_FIFO_RESET |
			(level & 0x3) << 6;
	
	// Pins
	PINSEL0  |= (1 << 20);	// Set P0[10] to UART2 TXD
	PINSEL0  |= (1 << 22);	// Set P0[11] to UART2 RXD
	PINMODE0 |= (2 << 20);	// Set P0[10] to Neither pull-up or pull-down
	PINMODE0 |= (2 << 22);	// Set P0[11] to Neither pull-up or pull-down
	
	// Interrupts
	// clear DLAB bit => access to UxIER and UxRBR
	U2LCR &= ~UART_DLAB;
	
	installInterrupt(iUART2, 0, uart2_interrupt);
	
	// Enable RBR (Receive Data Available) interrupt
	U2IER = UART_ENABLE_RBR_INTERRUPT;
}

// ----------------------------------------------------------------------------
bool
Uart2::write(char c, bool blocking)
{
	while (!(U2LSR & UART_TX_BUFFER_FREE))
	{
		if (!blocking) {
			// return immediately if a non blocking mode is requested
			return false;
		}
		
		// else wait until there is space in the send buffer (16-byte)
	}
	
	U2THR = c;
	return true;
}

// ----------------------------------------------------------------------------
bool
Uart2::isCharacterAvailable()
{
	return !receiveQueue.isEmpty();
}

bool
Uart2::read(char &c, bool blocking)
{
	do
	{
		if (!receiveQueue.isEmpty())
		{
			c = receiveQueue.get();
			receiveQueue.pop();
			return true;
		}
	} while (blocking);
	
	return false;
}