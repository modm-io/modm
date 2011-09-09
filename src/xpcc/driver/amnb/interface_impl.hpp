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
 * $Id$
 */
// ----------------------------------------------------------------------------

#ifndef	XPCC_AMNB__INTERFACE_HPP
	#error	"Don't include this file directly, use 'interface.hpp' instead!"
#endif

#ifdef __AVR__
	#include <util/crc16.h>
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
template <typename Device> typename xpcc::amnb::Interface<Device>::State \
	xpcc::amnb::Interface<Device>::state = SYNC;

template <typename Device> 
uint8_t xpcc::amnb::Interface<Device>::rx_buffer[maxPayloadLength + 3];

template <typename Device> 
uint8_t xpcc::amnb::Interface<Device>::tx_buffer[maxPayloadLength + 4];

template <typename Device>
uint8_t xpcc::amnb::Interface<Device>::crc;

template <typename Device>
uint8_t xpcc::amnb::Interface<Device>::position;

template <typename Device>
uint8_t xpcc::amnb::Interface<Device>::length;

template <typename Device>
uint8_t xpcc::amnb::Interface<Device>::lengthOfReceivedMessage = 0;

template <typename Device>
uint8_t xpcc::amnb::Interface<Device>::lengthOfSendingMessage = 0;

template <typename Device>
bool xpcc::amnb::Interface<Device>::hasMessageToSend = false;

template <typename Device>
bool xpcc::amnb::Interface<Device>::transmitting = false;

template <typename Device>
xpcc::Timeout<> xpcc::amnb::Interface<Device>::timer;

// ----------------------------------------------------------------------------

template <typename Device>
void
xpcc::amnb::Interface<Device>::initialize()
{
	Device::setBaudrate(115200UL);
	state = SYNC;
}

// ----------------------------------------------------------------------------

template <typename Device>
bool
xpcc::amnb::Interface<Device>::writeMessage()
{
	char check;
	transmitting = true;
	
	// detect collisions only on the first two bytes
	Device::write(syncByte);
	while (!Device::read(check)) ;
	if (check != syncByte) {
		transmitting = false;
		return false;
	}
	Device::write(tx_buffer[0]);
	while (!Device::read(check)) ;
	if (check != tx_buffer[0]) {
		transmitting = false;
		return false;
	}
	
	for (uint8_t i=1; i < lengthOfSendingMessage; ++i) {
		Device::write(tx_buffer[i]);
	}
	
	hasMessageToSend = false;
	transmitting = false;
	return true;
}

template <typename Device>
void
xpcc::amnb::Interface<Device>::sendMessage(uint8_t address, Flags flags, 
		uint8_t command,
		const void *payload, uint8_t payloadLength)
{
	if (transmitting) return; // dont overwrite the buffer when transmitting
	hasMessageToSend = false;
	uint8_t crc;
	
	tx_buffer[0] = payloadLength;
	tx_buffer[1] = address | flags;
	tx_buffer[2] = command;
	
	crc = crcUpdate(crcInitialValue, payloadLength);
	crc = crcUpdate(crc, address | flags);
	crc = crcUpdate(crc, command);
	
	const uint8_t *ptr = static_cast<const uint8_t *>(payload);
	uint8_t i=0;
	for (i = 0; i < payloadLength; ++i)
	{
		crc = crcUpdate(crc, *ptr);
		tx_buffer[i+3] = *ptr;
		ptr++;
	}
	
	tx_buffer[i+3] = crc;
	
	lengthOfSendingMessage = payloadLength + 4;
	hasMessageToSend = true;
}

template <typename Device> template <typename T>
void
xpcc::amnb::Interface<Device>::sendMessage(uint8_t address, Flags flags,
		uint8_t command,
		const T& payload)
{
	sendMessage(address, flags,
				command,
				reinterpret_cast<const void *>(&payload), sizeof(T));
}

template <typename Device>
void
xpcc::amnb::Interface<Device>::sendMessage(uint8_t address, Flags flags, uint8_t command)
{
	sendMessage(address, flags,
				command,
				0, 0);
}

// ----------------------------------------------------------------------------

template <typename Device>
bool
xpcc::amnb::Interface<Device>::isMessageAvailable()
{
	return (lengthOfReceivedMessage != 0);
}

template <typename Device>
uint8_t
xpcc::amnb::Interface<Device>::getAddress()
{
	return (rx_buffer[0] & 0x3f);
}

template <typename Device>
uint8_t
xpcc::amnb::Interface<Device>::getCommand()
{
	return rx_buffer[1];
}

template <typename Device>
bool
xpcc::amnb::Interface<Device>::isResponse()
{
	return (rx_buffer[0] & 0x80) ? true : false;
}

template <typename Device>
bool
xpcc::amnb::Interface<Device>::isAcknowledge()
{
	return (rx_buffer[0] & 0x40) ? true : false;
}

template <typename Device>
bool
xpcc::amnb::Interface<Device>::isBusAvailable()
{
	return (state == SYNC) && !transmitting;
}

template <typename Device>
const uint8_t*
xpcc::amnb::Interface<Device>::getPayload()
{
	return &rx_buffer[2];
}

template <typename Device>
uint8_t
xpcc::amnb::Interface<Device>::getPayloadLength()
{
	return (lengthOfReceivedMessage - 3);
}

template <typename Device>
void
xpcc::amnb::Interface<Device>::dropMessage()
{
	lengthOfReceivedMessage = 0;
}

// ----------------------------------------------------------------------------

template <typename Device>
void
xpcc::amnb::Interface<Device>::update()
{
	char byte;
	while (!transmitting && Device::read(byte))
	{
		switch (state)
		{
			case SYNC:
				if (byte == syncByte) {
					state = LENGTH;
				}
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
				rx_buffer[position] = byte;
				crc = crcUpdate(crc, byte);
				
				position += 1;
				if (position >= length) {
					if (crc == 0) {
						lengthOfReceivedMessage = length;
					}
					state = SYNC;
				}
				break;
			
			default:
				state = SYNC;
				break;
		}
		timer.restart(timeout);
	}
	if (timer.isExpired())
	{
		state = SYNC;
	}
	
	if (hasMessageToSend && isBusAvailable()) writeMessage();
}
