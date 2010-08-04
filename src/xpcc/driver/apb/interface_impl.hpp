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

#ifndef	XPCC_APB__INTERFACE_HPP
	#error	"Don't include this file directly, use 'interface.hpp' instead!"
#endif

#ifdef __AVR__
	#include <util/crc16.h>
#endif

// ----------------------------------------------------------------------------
template <typename Device> typename xpcc::apb::Interface<Device>::State \
	xpcc::apb::Interface<Device>::state = SYNC;

template <typename Device> uint8_t xpcc::apb::Interface<Device>::buffer[maxPayloadLength + 3];
template <typename Device> uint8_t xpcc::apb::Interface<Device>::crc;
template <typename Device> uint8_t xpcc::apb::Interface<Device>::position;
template <typename Device> uint8_t xpcc::apb::Interface<Device>::length;
template <typename Device> uint8_t xpcc::apb::Interface<Device>::lengthOfReceivedMessage = 0;

// ----------------------------------------------------------------------------

template <typename Device>
void
xpcc::apb::Interface<Device>::initialize()
{
	Device::setBaudrate(115200UL);
	state = SYNC;
}

// ----------------------------------------------------------------------------

template <typename Device>
void
xpcc::apb::Interface<Device>::sendMessage(uint8_t address, bool acknowledge, 
		uint8_t command,
		const void *payload, uint8_t payloadLength)
{
	uint8_t crc;
	if (acknowledge) {
		address |= ACK;
	}
	else {
		address |= NACK;
	};
	
	Device::write(syncByte);
	Device::write(payloadLength);
	crc = crcUpdate(crcInitialValue, payloadLength);
	Device::write(address);
	crc = crcUpdate(crc, address);
	Device::write(command);
	crc = crcUpdate(crc, command);
	
	const uint8_t *ptr = static_cast<const uint8_t *>(payload);
	for (uint_fast8_t i = 0; i < payloadLength; ++i)
	{
		crc = crcUpdate(crc, *ptr);
		Device::write(*ptr);
		ptr++;
	}
	
	Device::write(crc);
}

template <typename Device> template <typename T>
void
xpcc::apb::Interface<Device>::sendMessage(uint8_t address, bool acknowledge,
		uint8_t command,
		const T& payload)
{
	sendMessage(address, acknowledge,
			command,
			reinterpret_cast<const void *>(&payload), sizeof(T));
}

template <typename Device>
void
xpcc::apb::Interface<Device>::sendMessage(uint8_t address, bool acknowledge, uint8_t command)
{
	sendMessage(address, acknowledge,
			command,
			0, 0);
}

// ----------------------------------------------------------------------------

template <typename Device>
bool
xpcc::apb::Interface<Device>::isMessageAvailable()
{
	return (lengthOfReceivedMessage != 0);
}

template <typename Device>
uint8_t
xpcc::apb::Interface<Device>::getAddress()
{
	return (buffer[0] & 0x7f);
}

template <typename Device>
uint8_t
xpcc::apb::Interface<Device>::getCommand()
{
	return buffer[1];
}

template <typename Device>
bool
xpcc::apb::Interface<Device>::isAcknowledge()
{
	return (buffer[0] & 0x80) ? true : false;
}

template <typename Device>
const uint8_t*
xpcc::apb::Interface<Device>::getPayload()
{
	return &buffer[2];
}

template <typename Device>
uint8_t
xpcc::apb::Interface<Device>::getPayloadLength()
{
	return (lengthOfReceivedMessage - 3);
}

template <typename Device>
void
xpcc::apb::Interface<Device>::dropMessage()
{
	lengthOfReceivedMessage = 0;
}

// ----------------------------------------------------------------------------

template <typename Device>
void
xpcc::apb::Interface<Device>::update()
{
	char byte;
	while (Device::read(byte))
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
				buffer[position] = byte;
				crc = crcUpdate(crc, byte);
				
				position += 1;
				if (position >= length) {
					state = SYNC;
					if (crc == 0) {
						lengthOfReceivedMessage = length;
					}
				}
				break;
			
			default:
				state = SYNC;
				break;
		}
	}
}
