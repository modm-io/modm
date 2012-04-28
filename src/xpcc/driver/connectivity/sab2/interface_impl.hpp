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

#ifndef	XPCC_SAB2__INTERFACE_HPP
	#error	"Don't include this file directly, use 'interface.hpp' instead!"
#endif

#include "constants.hpp"

//#include <xpcc/debug/logger.hpp>
//#undef XPCC_LOG_LEVEL
//#define XPCC_LOG_LEVEL	xpcc::log::DEBUG

// ----------------------------------------------------------------------------
template <typename Device, std::size_t N> uint8_t xpcc::sab2::Interface<Device, N>::buffer[N + 4];
template <typename Device, std::size_t N> uint16_t xpcc::sab2::Interface<Device, N>::crc = crcInitialValue;

template <typename Device, std::size_t N> typename xpcc::sab2::Interface<Device, N>::Size \
	xpcc::sab2::Interface<Device, N>::length;
template <typename Device, std::size_t N> typename xpcc::sab2::Interface<Device, N>::Size \
	xpcc::sab2::Interface<Device, N>::lengthOfReceivedMessage = 0;

template <typename Device, std::size_t N> bool xpcc::sab2::Interface<Device, N>::nextEscaped = false;

// ----------------------------------------------------------------------------

template <typename Device, std::size_t N>
void
xpcc::sab2::Interface<Device, N>::initialize()
{
}

// ----------------------------------------------------------------------------

template <typename Device, std::size_t N>
void
xpcc::sab2::Interface<Device, N>::sendMessage(uint8_t address, Flags flags, 
		uint8_t command,
		const void *payload, Size payloadLength)
{
	uint16_t crcSend = crcInitialValue;
	
	Device::write(frameBounderyByte);
	
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
	
	writeByteEscaped(crcSend & 0xff);
	writeByteEscaped(crcSend >> 8);
	
	Device::write(frameBounderyByte);
}

template <typename Device, std::size_t N> template <typename T>
void
xpcc::sab2::Interface<Device, N>::sendMessage(uint8_t address, Flags flags,
		uint8_t command,
		const T& payload)
{
	sendMessage(address, flags,
			command,
			reinterpret_cast<const void *>(&payload), sizeof(T));
}

template <typename Device, std::size_t N>
void
xpcc::sab2::Interface<Device, N>::sendMessage(uint8_t address, Flags flags, uint8_t command)
{
	sendMessage(address, flags,
			command,
			0, 0);
}

// ----------------------------------------------------------------------------

template <typename Device, std::size_t N>
bool
xpcc::sab2::Interface<Device, N>::isMessageAvailable()
{
	return (lengthOfReceivedMessage != 0);
}

template <typename Device, std::size_t N>
uint8_t
xpcc::sab2::Interface<Device, N>::getAddress()
{
	return (buffer[0] & 0x3f);
}

template <typename Device, std::size_t N>
uint8_t
xpcc::sab2::Interface<Device, N>::getCommand()
{
	return buffer[1];
}

template <typename Device, std::size_t N>
bool
xpcc::sab2::Interface<Device, N>::isResponse()
{
	return (buffer[0] & 0x80) ? true : false;
}

template <typename Device, std::size_t N>
bool
xpcc::sab2::Interface<Device, N>::isAcknowledge()
{
	return (buffer[0] & 0x40) ? true : false;
}

template <typename Device, std::size_t N>
const uint8_t*
xpcc::sab2::Interface<Device, N>::getPayload()
{
	return &buffer[2];
}

template <typename Device, std::size_t N>
typename xpcc::sab2::Interface<Device, N>::Size
xpcc::sab2::Interface<Device, N>::getPayloadLength()
{
	return (lengthOfReceivedMessage - 4);
}

template <typename Device, std::size_t N>
void
xpcc::sab2::Interface<Device, N>::dropMessage()
{
	lengthOfReceivedMessage = 0;
}

// ----------------------------------------------------------------------------
template <typename Device, std::size_t N>
void
xpcc::sab2::Interface<Device, N>::update()
{
	uint8_t data;
	while ((lengthOfReceivedMessage == 0) && Device::read(data))
	{
		//XPCC_LOG_DEBUG.printf("%02x ", data);
		
		if (data == frameBounderyByte) {
			if (nextEscaped) {
				//XPCC_LOG_ERROR << "framing error" << xpcc::endl;
			}
			else {
				if (length >= 4) {
					if (crc == 0) {
						lengthOfReceivedMessage = length;
					}
					else {
						//XPCC_LOG_ERROR.printf("crc=%04x\n", crc);
					}
				}
			}
			
			crc = crcInitialValue;
			length = 0;
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
			
			if (length >= (N+4)) {
				// Error message to long
				length = 0;
				//XPCC_LOG_ERROR << "message to long" << xpcc::endl;
			}
			else {
				buffer[length] = data;
				length += 1;
				
				crc = crcUpdate(crc, data);
			}
		}
	}
}

// ----------------------------------------------------------------------------
template <typename Device, std::size_t N>
void
xpcc::sab2::Interface<Device, N>::writeByteEscaped(uint8_t data)
{
	if (data == frameBounderyByte || data == controlEscapeByte) {
		Device::write(controlEscapeByte);
		Device::write(data ^ 0x20);		// toggle bit 5
	}
	else {
		Device::write(data);
	}
}
