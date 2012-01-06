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
 * $Id: interface_impl.hpp 683 2012-01-03 01:50:57Z dergraaf $
 */
// ----------------------------------------------------------------------------

#ifndef	XPCC_SAB2__INTERFACE_HPP
	#error	"Don't include this file directly, use 'interface.hpp' instead!"
#endif

#ifdef __AVR__
	#include <util/crc16.h>
#endif

#include "constants.hpp"
//#include <xpcc/debug/logger.hpp>

//#undef XPCC_LOG_LEVEL
//#define XPCC_LOG_LEVEL	xpcc::log::DEBUG

// ----------------------------------------------------------------------------
template <typename Device> typename xpcc::sab2::Interface<Device>::State \
	xpcc::sab2::Interface<Device>::state = SYNC;

template <typename Device> uint8_t xpcc::sab2::Interface<Device>::buffer[maxPayloadLength + 3];
template <typename Device> uint8_t xpcc::sab2::Interface<Device>::crc;
template <typename Device> uint8_t xpcc::sab2::Interface<Device>::position;
template <typename Device> uint8_t xpcc::sab2::Interface<Device>::length;
template <typename Device> uint8_t xpcc::sab2::Interface<Device>::lengthOfReceivedMessage = 0;
template <typename Device> bool xpcc::sab2::Interface<Device>::nextEscaped = false;

// ----------------------------------------------------------------------------

template <typename Device>
void
xpcc::sab2::Interface<Device>::initialize()
{
	state = SYNC;
}

// ----------------------------------------------------------------------------

template <typename Device>
void
xpcc::sab2::Interface<Device>::sendMessage(uint8_t address, Flags flags, 
		uint8_t command,
		const void *payload, uint8_t payloadLength)
{
	uint8_t crcSend;
	
	Device::write(frameBounderyByte);
	writeByteEscaped(payloadLength);
	crcSend = crcUpdate(crcInitialValue, payloadLength);
	writeByteEscaped(address | flags);
	crcSend = crcUpdate(crcSend, address | flags);
	writeByteEscaped(command);
	crcSend = crcUpdate(crcSend, command);
	
	const uint8_t *ptr = static_cast<const uint8_t *>(payload);
	for (uint_fast8_t i = 0; i < payloadLength; ++i)
	{
		crcSend = crcUpdate(crcSend, *ptr);
		writeByteEscaped(*ptr);
		ptr++;
	}
	
	writeByteEscaped(crcSend);
	Device::write(frameBounderyByte);
	
	//XPCC_LOG_DEBUG << "send a=" << address << " c=" << command << " l=" << payloadLength << xpcc::endl;
}

template <typename Device> template <typename T>
void
xpcc::sab2::Interface<Device>::sendMessage(uint8_t address, Flags flags,
		uint8_t command,
		const T& payload)
{
	sendMessage(address, flags,
			command,
			reinterpret_cast<const void *>(&payload), sizeof(T));
}

template <typename Device>
void
xpcc::sab2::Interface<Device>::sendMessage(uint8_t address, Flags flags, uint8_t command)
{
	sendMessage(address, flags,
			command,
			0, 0);
}

// ----------------------------------------------------------------------------

template <typename Device>
bool
xpcc::sab2::Interface<Device>::isMessageAvailable()
{
	return (lengthOfReceivedMessage != 0);
}

template <typename Device>
uint8_t
xpcc::sab2::Interface<Device>::getAddress()
{
	return (buffer[0] & 0x3f);
}

template <typename Device>
uint8_t
xpcc::sab2::Interface<Device>::getCommand()
{
	return buffer[1];
}

template <typename Device>
bool
xpcc::sab2::Interface<Device>::isResponse()
{
	return (buffer[0] & 0x80) ? true : false;
}

template <typename Device>
bool
xpcc::sab2::Interface<Device>::isAcknowledge()
{
	return (buffer[0] & 0x40) ? true : false;
}

template <typename Device>
const uint8_t*
xpcc::sab2::Interface<Device>::getPayload()
{
	return &buffer[2];
}

template <typename Device>
uint8_t
xpcc::sab2::Interface<Device>::getPayloadLength()
{
	return (lengthOfReceivedMessage - 3);
}

template <typename Device>
void
xpcc::sab2::Interface<Device>::dropMessage()
{
	lengthOfReceivedMessage = 0;
}

// ----------------------------------------------------------------------------
template <typename Device>
void
xpcc::sab2::Interface<Device>::update()
{
	uint8_t data;
	while (Device::read(data))
	{
		//XPCC_LOG_DEBUG.printf("%02x ", data);
		
		if (data == frameBounderyByte) {
			if ((state != SYNC && state != LENGTH) || nextEscaped) {
				//XPCC_LOG_ERROR << "framing error" << xpcc::endl;
			}
			state = LENGTH;
			nextEscaped = false;
		}
		else if (data == controlEscapeByte) {
			nextEscaped = true;
			continue;
		}
		else
		{
			if (nextEscaped) {
				nextEscaped = false;
				data = data ^ 0x20;	// toggle bit 5
			}
			
			switch (state)
			{
				case LENGTH:
					if (data > maxPayloadLength) {
						state = SYNC;
					}
					else {
						length = data + 3;		// +3 for header, command and crc byte
						position = 0;
						crc = crcUpdate(crcInitialValue, data);
						state = DATA;
					}
					break;
				
				case DATA:
					buffer[position] = data;
					crc = crcUpdate(crc, data);
					
					position += 1;
					if (position >= length) {
						if (crc == 0) {
							lengthOfReceivedMessage = length;
							//XPCC_LOG_DEBUG << "SAB received" << xpcc::endl;
						}
						else {
							//XPCC_LOG_ERROR << "CRC error" << xpcc::endl;
						}
						state = SYNC;
					}
					break;
				
				default:
					state = SYNC;
					break;
			}
		}
	}
}

// ----------------------------------------------------------------------------
template <typename Device>
void
xpcc::sab2::Interface<Device>::writeByteEscaped(uint8_t data)
{
	if (data == frameBounderyByte || data == controlEscapeByte) {
		Device::write(controlEscapeByte);
		Device::write(data ^ 0x20);		// toggle bit 5
	}
	else {
		Device::write(data);
	}
}
