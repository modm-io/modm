// coding: utf-8
// ----------------------------------------------------------------------------
/* Copyright (c) 2009, Roboterclub Aachen e.V.
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

#include <avr/io.h>
#include <xpcc/architecture/platform/avr/atxmega.hpp>
#include <xpcc/architecture/driver/gpio.hpp>
#include "../dma.hpp"
#include "spi_e.hpp"
#include "xpcc_config.hpp"

#ifdef USARTE1

#if SPIE1_TX_DMA_CHANNEL >= 0
#	define DMA_TX	CONCAT(DmaChannel, SPIE1_TX_DMA_CHANNEL)
#
#	if SPIE1_RX_DMA_CHANNEL == SPIE1_TX_DMA_CHANNEL
#		error "You cannot use the same DMA channel for both sending and receiving."
#	endif
#
#	if SPIE1_RX_DMA_CHANNEL >= 0
#		define DMA_RX	CONCAT(DmaChannel, SPIE1_RX_DMA_CHANNEL)
#	endif
#endif

namespace
{
	GPIO__OUTPUT(XCK, E, 5);
	GPIO__INPUT(RXD, E, 6);
	GPIO__OUTPUT(TXD, E, 7);
	
	static uint8_t* transmitBuffer(0);
	static uint8_t* receiveBuffer(0);
	static uint16_t bufferLength(0);
	static uint8_t dummyBuffer(0xff);
	enum
	{
		BUFFER_TRANSMIT_INCR_bm = 0x01,
		BUFFER_RECEIVE_INCR_bm = 0x02,
		BUFFER_TRANSMIT_IS_NOT_ZERO_bm = 0x04,
		BUFFER_RECEIVE_IS_NOT_ZERO_bm = 0x08,
		BUFFER_IS_DUMMY_bm = 0x10,
		BUFFER_IS_BUSY_SYNC_bm = 0x20
	};
	static uint8_t status(0);
}

// ----------------------------------------------------------------------------
void
xpcc::atxmega::UartSpiMasterE1::initialize(uint32_t const bitrate)
{
	RXD::setInput(PULLUP);
	TXD::setOutput(xpcc::gpio::HIGH);
	XCK::setOutput(xpcc::gpio::LOW);
	
	uint16_t ubrr = F_CPU / (2 * bitrate) - 1;
	if (2*bitrate >= F_CPU) ubrr = 0;
	USARTE1_BAUDCTRLB = static_cast<uint8_t>(ubrr >> 8);
	USARTE1_BAUDCTRLA = static_cast<uint8_t>(ubrr);
	
	// enable SPI master mode 
	USARTE1_CTRLA = 0;
	USARTE1_CTRLC = USART_CMODE_MSPI_gc;
	USARTE1_CTRLB = USART_RXEN_bm | USART_TXEN_bm;
	
#ifdef DMA_TX
	DMA_TX::initialize(dma::BURST_1BYTE,
					   dma::TRIGGER_DMA_USARTE1_DRE,
					   dma::RELOAD_TRANSACTION);
	DMA_TX::setDestination(&USARTE1_DATA);
	DMA_TX::setSingleShotMode();
#endif
	
#ifdef DMA_RX
	DMA_RX::initialize(dma::BURST_1BYTE,
					   dma::TRIGGER_DMA_USARTE1_RXC,
					   dma::RELOAD_NONE,
					   dma::ADDRESS_FIXED,
					   dma::RELOAD_TRANSACTION);
	DMA_RX::setSource(&USARTE1_DATA);
	DMA_RX::setSingleShotMode();
#endif
}

// ----------------------------------------------------------------------------
bool
xpcc::atxmega::UartSpiMasterE1::setBuffer(uint16_t length, uint8_t* transmit, uint8_t* receive, BufferIncrease bufferIncrease)
{
	if (isBusy()) return false;
	
	transmitBuffer = transmit;
	receiveBuffer = receive ? receive : transmit;
	bufferLength = length;
	status &= ~(BUFFER_TRANSMIT_INCR_bm | BUFFER_RECEIVE_INCR_bm);
	status |= bufferIncrease;
	
#ifdef DMA_TX
	DMA_TX::setBlockTransferCount(bufferLength);
	if (transmit)
	{
		DMA_TX::setSourceAddressMode((bufferIncrease & BUFFER_INCR_TRANSMIT_DECR_RECEIVE)
									 ? dma::ADDRESS_INCREMENT : dma::ADDRESS_DECREMENT);
		DMA_TX::setSource(transmitBuffer);
		// tell everyone we have a non-zero buffer
		status |= BUFFER_TRANSMIT_IS_NOT_ZERO_bm;
	}
	else {
		// no buffer means we should send a dummy value
		DMA_TX::setSource(&dummyBuffer);
		// stop the address from increasing
		DMA_TX::setSourceAddressMode(dma::ADDRESS_FIXED);
		status &= ~BUFFER_TRANSMIT_IS_NOT_ZERO_bm;
	}
#endif
	
#ifdef DMA_RX
	if (receive)
	{
		DMA_RX::setBlockTransferCount(bufferLength);
		DMA_RX::setDestinationAddressMode((bufferIncrease & BUFFER_DECR_TRANSMIT_INCR_RECEIVE)
										  ? dma::ADDRESS_INCREMENT : dma::ADDRESS_DECREMENT);
		DMA_RX::setDestination(receiveBuffer);
		// tell everyone we have a non-zero buffer
		status |= BUFFER_RECEIVE_IS_NOT_ZERO_bm;
	}
	else {
		// we could write to a dummy buffer, but what use is that?
		status &= ~BUFFER_TRANSMIT_IS_NOT_ZERO_bm;
	}
#endif
	
	return true;
}

// ----------------------------------------------------------------------------
bool
xpcc::atxmega::UartSpiMasterE1::transfer(TransferOptions options)
{
	if (isBusy()) return false;
	
#if defined (DMA_TX) || defined (DMA_RX)
		
#	ifdef DMA_TX
	DMA_TX::getAndClearStatus();
	
	if (status & BUFFER_TRANSMIT_IS_NOT_ZERO_bm)
	{// if buffer is not intentionally set to dummy values
		if (!(options & TRANSFER_SEND_BUFFER_DISCARD_RECEIVE)) {
			// temporarily use the dummy buffer
			DMA_TX::setSourceAddressMode(dma::ADDRESS_FIXED);
			DMA_TX::setSource(&dummyBuffer);
			status |= BUFFER_IS_DUMMY_bm;
		}
		else if (status & BUFFER_IS_DUMMY_bm) {
			// we need to reset all the setting, since the last transfer was with
			// dummy values
			DMA_TX::setSourceAddressMode((status & BUFFER_TRANSMIT_INCR_bm) ? dma::ADDRESS_INCREMENT : dma::ADDRESS_DECREMENT);
			DMA_TX::setSource(transmitBuffer);
			status &= ~BUFFER_IS_DUMMY_bm;
		}
	}
	// Start the DMA transfer
	DMA_TX::enable();
#	endif
	
#	ifdef DMA_RX
	DMA_RX::getAndClearStatus();
	
	// if there is a buffer to write something to, start the DMA transfer
	if ((options & TRANSFER_SEND_DUMMY_SAVE_RECEIVE)
		&& (status & BUFFER_RECEIVE_IS_NOT_ZERO_bm)) {
		// empty the buffer
		(void) USARTE1_DATA;
		DMA_RX::enable();
	}
#	endif
	
#else
	uint8_t rx(0), tx(dummyBuffer);
	// send the buffer out, blocking
	status |= BUFFER_IS_BUSY_SYNC_bm;
	// check if we have to use a dummy buffer
	bool transmit = (options & TRANSFER_SEND_BUFFER_DISCARD_RECEIVE) & static_cast<bool>(transmitBuffer);
	bool receive = (options & TRANSFER_SEND_DUMMY_SAVE_RECEIVE) & static_cast<bool>(receiveBuffer);
	
	for(uint_fast16_t i=0; i < bufferLength; ++i)
	{
		if (transmit) {
			tx = transmitBuffer[(status & BUFFER_TRANSMIT_INCR_bm) ? i : bufferLength-1-i];
		}
		
		rx = write(tx);
		
		if (receive) {
			receiveBuffer[(status & BUFFER_RECEIVE_INCR_bm) ? i : bufferLength-1-i] = rx;
		}
	}
	
	status &= ~BUFFER_IS_BUSY_SYNC_bm;
#endif
	
	return true;
}


bool
xpcc::atxmega::UartSpiMasterE1::transferSync(TransferOptions options)
{
#if defined (DMA_TX) || defined (DMA_RX)
	if (transfer(options))
	{
		while (!isFinished())
			;
		return true;
	}
	return false;
#else
	return transfer(options);
#endif
}

bool
xpcc::atxmega::UartSpiMasterE1::isFinished()
{
#ifdef DMA_TX
	return (DMA_TX::getStatus() & (DMA_CH_ERRIF_bm | DMA_CH_TRNIF_bm)) && (USARTE1_STATUS & USART_TXCIF_bm);
#else
	return !(status & BUFFER_IS_BUSY_SYNC_bm);
#endif
}

bool
xpcc::atxmega::UartSpiMasterE1::isBusy()
{
#ifdef DMA_TX
	return (DMA_TX::getStatus() & (DMA_CH_CHPEND_bm | DMA_CH_CHBUSY_bm));
#else
	return (status & BUFFER_IS_BUSY_SYNC_bm);
#endif
}

// ----------------------------------------------------------------------------
uint8_t
xpcc::atxmega::UartSpiMasterE1::write(uint8_t data)
{
	// clear flags
	USARTE1_STATUS = USART_RXCIF_bm;
	
	// send byte
	USARTE1_DATA = data;
	
	// wait until the byte is transmitted
	while (!(USARTE1_STATUS & USART_RXCIF_bm))
		;
	
	return USARTE1_DATA;
}

#endif // USARTE1