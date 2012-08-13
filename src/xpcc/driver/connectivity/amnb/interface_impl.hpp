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
 * 
 * $hello please remove me$
 */
// ----------------------------------------------------------------------------

#ifndef	XPCC_AMNB__INTERFACE_HPP
#	error	"Don't include this file directly, use 'interface.hpp' instead!"
#endif

#include <xpcc/architecture/driver/atomic/lock.hpp>

uint_fast16_t xpcc::amnb::Clock::time = 0;

xpcc::Timestamp
xpcc::amnb::Clock::now()
{
	uint_fast16_t tempTime;
	{
		atomic::Lock lock;
		tempTime = time;
	}
	
	return Timestamp(tempTime);
}

// ----------------------------------------------------------------------------
#ifdef __AVR__
#	include <util/crc16.h>
#endif

uint8_t
xpcc::amnb::crcUpdate(uint8_t crc, uint8_t data)
{
#ifdef __AVR__
	return _crc_ibutton_update(crc, data);
#else
	crc = crc ^ data;
	for (uint_fast8_t i = 0; i < 8; ++i)
	{
		if (crc & 0x01) {
			crc = (crc >> 1) ^ 0x8C;
		}
		else {
			crc >>= 1;
		}
	}
	return crc;
#endif
}

// ----------------------------------------------------------------------------
template <typename Device, uint8_t PROBABILITY, uint8_t TIMEOUT> typename xpcc::amnb::Interface<Device,PROBABILITY,TIMEOUT>::State \
	xpcc::amnb::Interface<Device,PROBABILITY,TIMEOUT>::state = SYNC;

template <typename Device, uint8_t PROBABILITY, uint8_t TIMEOUT> 
uint8_t xpcc::amnb::Interface<Device,PROBABILITY,TIMEOUT>::rx_buffer[maxPayloadLength + 3];

template <typename Device, uint8_t PROBABILITY, uint8_t TIMEOUT> 
uint8_t xpcc::amnb::Interface<Device,PROBABILITY,TIMEOUT>::tx_buffer[maxPayloadLength + 4];

template <typename Device, uint8_t PROBABILITY, uint8_t TIMEOUT>
uint8_t xpcc::amnb::Interface<Device,PROBABILITY,TIMEOUT>::crc;

template <typename Device, uint8_t PROBABILITY, uint8_t TIMEOUT>
uint8_t xpcc::amnb::Interface<Device,PROBABILITY,TIMEOUT>::position;

template <typename Device, uint8_t PROBABILITY, uint8_t TIMEOUT>
uint8_t xpcc::amnb::Interface<Device,PROBABILITY,TIMEOUT>::length;

template <typename Device, uint8_t PROBABILITY, uint8_t TIMEOUT>
uint8_t xpcc::amnb::Interface<Device,PROBABILITY,TIMEOUT>::lengthOfReceivedMessage = 0;

template <typename Device, uint8_t PROBABILITY, uint8_t TIMEOUT>
uint8_t xpcc::amnb::Interface<Device,PROBABILITY,TIMEOUT>::lengthOfTransmitMessage = 0;

template <typename Device, uint8_t PROBABILITY, uint8_t TIMEOUT>
bool xpcc::amnb::Interface<Device,PROBABILITY,TIMEOUT>::hasMessageToSend = false;

template <typename Device, uint8_t PROBABILITY, uint8_t TIMEOUT>
bool xpcc::amnb::Interface<Device,PROBABILITY,TIMEOUT>::rescheduleTransmit = false;

template <typename Device, uint8_t PROBABILITY, uint8_t TIMEOUT>
bool xpcc::amnb::Interface<Device,PROBABILITY,TIMEOUT>::transmitting = false;

template <typename Device, uint8_t PROBABILITY, uint8_t TIMEOUT>
xpcc::Timeout<> xpcc::amnb::Interface<Device,PROBABILITY,TIMEOUT>::resetTimer;

template <typename Device, uint8_t PROBABILITY, uint8_t TIMEOUT>
xpcc::Timeout<xpcc::amnb::Clock> xpcc::amnb::Interface<Device,PROBABILITY,TIMEOUT>::rescheduleTimer;

template <typename Device, uint8_t PROBABILITY, uint8_t TIMEOUT>
uint8_t xpcc::amnb::Interface<Device,PROBABILITY,TIMEOUT>::rescheduleTimeout;

template <typename Device, uint8_t PROBABILITY, uint8_t TIMEOUT>
bool xpcc::amnb::Interface<Device,PROBABILITY,TIMEOUT>::messageSent = false;

#if AMNB_TIMING_DEBUG
template <typename Device, uint8_t PROBABILITY, uint8_t TIMEOUT>
xpcc::Timestamp xpcc::amnb::Interface<Device,PROBABILITY,TIMEOUT>::latency;

template <typename Device, uint8_t PROBABILITY, uint8_t TIMEOUT>
uint8_t xpcc::amnb::Interface<Device,PROBABILITY,TIMEOUT>::collisions;
#endif

// ----------------------------------------------------------------------------

template <typename Device, uint8_t PROBABILITY, uint8_t TIMEOUT>
void
xpcc::amnb::Interface<Device,PROBABILITY,TIMEOUT>::initialize(int seed)
{
	srand(seed);
	rescheduleTimeout = static_cast<uint8_t>(rand());
	state = SYNC;
}

// ----------------------------------------------------------------------------

template <typename Device, uint8_t PROBABILITY, uint8_t TIMEOUT>
bool
xpcc::amnb::Interface<Device,PROBABILITY,TIMEOUT>::writeMessage()
{
	uint8_t check;
	transmitting = true;
	Device::resetErrorFlags();
	
	for (uint_fast8_t i=0; i < lengthOfTransmitMessage; ++i) {
		Device::write(tx_buffer[i]);
		
		// try and read the transmitted byte back but do not wait infinity
		uint16_t count(0);
		while (!Device::read(check) && (++count <= 1000)) ;
		
		// if the read timed out or framing error occurred or content mismatch
		if ((count > 1000) || Device::readErrorFlags() || (check != tx_buffer[i])) {
			// stop transmitting, signal the collision
			transmitting = false;
			rescheduleTransmit = true;
			Device::resetErrorFlags();
			// and wait for a random amount of time before sending again
			rescheduleTimer.restart(rescheduleTimeout % TIMEOUT);
#if AMNB_TIMING_DEBUG
			++collisions;
#endif
			return false;
		}
	}
	
#if AMNB_TIMING_DEBUG
	latency = xpcc::Clock::now() - latency;
#endif
	
	messageSent = true;
	hasMessageToSend = false;
	transmitting = false;
	return true;
}

template <typename Device, uint8_t PROBABILITY, uint8_t TIMEOUT>
bool
xpcc::amnb::Interface<Device,PROBABILITY,TIMEOUT>::sendMessage(uint8_t address, Flags flags, 
		uint8_t command,
		const void *payload, uint8_t payloadLength)
{
	 // dont overwrite the buffer when transmitting
	if (transmitting) return false;
	
	hasMessageToSend = false;
	messageSent = false;
	uint8_t crc;

#if AMNB_TIMING_DEBUG
	latency = xpcc::Clock::now();
#endif
	
	tx_buffer[0] = syncByte;
	tx_buffer[1] = payloadLength;
	tx_buffer[2] = address | flags;
	tx_buffer[3] = command;
	
	crc = crcUpdate(crcInitialValue, payloadLength);
	crc = crcUpdate(crc, address | flags);
	crc = crcUpdate(crc, command);
	
	const uint8_t *ptr = static_cast<const uint8_t *>(payload);
	uint_fast8_t i=0;
	for (; i < payloadLength; ++i)
	{
		crc = crcUpdate(crc, *ptr);
		tx_buffer[i+4] = *ptr;
		ptr++;
	}
	
	tx_buffer[i+4] = crc;
	
	lengthOfTransmitMessage = payloadLength + 5;
	hasMessageToSend = true;
	return true;
}

template <typename Device, uint8_t PROBABILITY, uint8_t TIMEOUT> template <typename T>
bool
xpcc::amnb::Interface<Device,PROBABILITY,TIMEOUT>::sendMessage(uint8_t address, Flags flags,
		uint8_t command,
		const T& payload)
{
	return sendMessage(address, flags,
				command,
				reinterpret_cast<const void *>(&payload), sizeof(T));
}

template <typename Device, uint8_t PROBABILITY, uint8_t TIMEOUT>
bool
xpcc::amnb::Interface<Device,PROBABILITY,TIMEOUT>::sendMessage(uint8_t address, Flags flags, uint8_t command)
{
	return sendMessage(address, flags,
				command,
				0, 0);
}

// ----------------------------------------------------------------------------

template <typename Device, uint8_t PROBABILITY, uint8_t TIMEOUT>
bool
xpcc::amnb::Interface<Device,PROBABILITY,TIMEOUT>::isMessageAvailable()
{
	return (lengthOfReceivedMessage != 0);
}

template <typename Device, uint8_t PROBABILITY, uint8_t TIMEOUT>
uint8_t
xpcc::amnb::Interface<Device,PROBABILITY,TIMEOUT>::getTransmittedAddress()
{
	return (tx_buffer[0] & 0x3f);
}

template <typename Device, uint8_t PROBABILITY, uint8_t TIMEOUT>
uint8_t
xpcc::amnb::Interface<Device,PROBABILITY,TIMEOUT>::getTransmittedCommand()
{
	return tx_buffer[1];
}

template <typename Device, uint8_t PROBABILITY, uint8_t TIMEOUT>
xpcc::amnb::Flags
xpcc::amnb::Interface<Device,PROBABILITY,TIMEOUT>::getTransmittedFlags()
{
	return static_cast<Flags>(tx_buffer[0] & 0xc0);
}

template <typename Device, uint8_t PROBABILITY, uint8_t TIMEOUT>
uint8_t
xpcc::amnb::Interface<Device,PROBABILITY,TIMEOUT>::getAddress()
{
	return (rx_buffer[0] & 0x3f);
}

template <typename Device, uint8_t PROBABILITY, uint8_t TIMEOUT>
uint8_t
xpcc::amnb::Interface<Device,PROBABILITY,TIMEOUT>::getCommand()
{
	return rx_buffer[1];
}

template <typename Device, uint8_t PROBABILITY, uint8_t TIMEOUT>
bool
xpcc::amnb::Interface<Device,PROBABILITY,TIMEOUT>::isResponse()
{
	return (rx_buffer[0] & 0x80);
}

template <typename Device, uint8_t PROBABILITY, uint8_t TIMEOUT>
bool
xpcc::amnb::Interface<Device,PROBABILITY,TIMEOUT>::isAcknowledge()
{
	return (rx_buffer[0] & 0x40);
}

template <typename Device, uint8_t PROBABILITY, uint8_t TIMEOUT>
bool
xpcc::amnb::Interface<Device,PROBABILITY,TIMEOUT>::isBusAvailable()
{
	return (state == SYNC) && !transmitting && !rescheduleTransmit;
}

template <typename Device, uint8_t PROBABILITY, uint8_t TIMEOUT>
bool
xpcc::amnb::Interface<Device,PROBABILITY,TIMEOUT>::messageTransmitted()
{
	return messageSent;
}

template <typename Device, uint8_t PROBABILITY, uint8_t TIMEOUT>
const uint8_t*
xpcc::amnb::Interface<Device,PROBABILITY,TIMEOUT>::getPayload()
{
	return rx_buffer+2;
}

template <typename Device, uint8_t PROBABILITY, uint8_t TIMEOUT>
uint8_t
xpcc::amnb::Interface<Device,PROBABILITY,TIMEOUT>::getPayloadLength()
{
	return (lengthOfReceivedMessage - 3);
}

template <typename Device, uint8_t PROBABILITY, uint8_t TIMEOUT>
void
xpcc::amnb::Interface<Device,PROBABILITY,TIMEOUT>::dropMessage()
{
	lengthOfReceivedMessage = 0;
}

// ----------------------------------------------------------------------------

template <typename Device, uint8_t PROBABILITY, uint8_t TIMEOUT>
void
xpcc::amnb::Interface<Device,PROBABILITY,TIMEOUT>::update()
{
	uint8_t byte;
	while (Device::read(byte))
	{
		if (Device::readErrorFlags())
		{
			// collision has been detected
			rescheduleTransmit = true;
			Device::resetErrorFlags();
			
			// erase the message in the buffer
			Device::flushReceiveBuffer();
			
			// and wait for a random amount of time before sending again
			rescheduleTimeout = static_cast<uint8_t>(rand());
			rescheduleTimer.restart(rescheduleTimeout % TIMEOUT);
			state = SYNC;
#if AMNB_TIMING_DEBUG
			++collisions;
#endif
			return;
		}
		
		switch (state)
		{
			case SYNC:
				if (byte == syncByte) state = LENGTH;
				break;
			
			case LENGTH:
				if (byte > maxPayloadLength) {
					state = SYNC;
				}
				else {
					length = byte + 3;		// +3 for header, command and crc byte
					position = 0;
					crc = crcUpdate(crcInitialValue, byte);
					state = DATA;
				}
				break;
			
			case DATA:
				rx_buffer[position++] = byte;
				crc = crcUpdate(crc, byte);
				
				if (position >= length)
				{
					if (crc == 0) lengthOfReceivedMessage = length;
					state = SYNC;
				}
				break;
			
			default:
				state = SYNC;
				break;
		}
		
		resetTimer.restart(resetTimeout);
	}
	if ((state != SYNC) && resetTimer.isExpired()) state = SYNC;
	
	// check if we have waited for a random amount of time
	if (rescheduleTransmit && rescheduleTimer.isExpired()) rescheduleTransmit = false;
	
	if (hasMessageToSend && !rescheduleTransmit && !transmitting && (state == SYNC)) {
		// if channel is free, send with probability P
		if (rescheduleTimeout < static_cast<uint8_t>(2.56f * PROBABILITY)) {
			writeMessage();
		}
		// otherwise reschedule
		else {
			rescheduleTransmit = true;
			rescheduleTimer.restart(rescheduleTimeout % TIMEOUT);
		}
		rescheduleTimeout = static_cast<uint8_t>(rand());
	}
}
