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

template <typename DEVICE> const uint8_t xpcc::apb::Interface<DEVICE>::syncByte = 0x54;
template <typename DEVICE> const uint8_t xpcc::apb::Interface<DEVICE>::crcInitialValue = 0x00;

template <typename DEVICE> typename xpcc::apb::Interface<DEVICE>::State \
	xpcc::apb::Interface<DEVICE>::state = SYNC;

template <typename DEVICE> uint8_t xpcc::apb::Interface<DEVICE>::buffer[32];
template <typename DEVICE> uint8_t xpcc::apb::Interface<DEVICE>::crc;
template <typename DEVICE> uint8_t xpcc::apb::Interface<DEVICE>::position;
template <typename DEVICE> uint8_t xpcc::apb::Interface<DEVICE>::length;
template <typename DEVICE> uint8_t xpcc::apb::Interface<DEVICE>::lengthOfReceivedMessage = 0;


// ----------------------------------------------------------------------------

template <typename DEVICE>
void
xpcc::apb::Interface<DEVICE>::initialize()
{
	DEVICE::setBaudrate(38400U);
	state = SYNC;
}

// ----------------------------------------------------------------------------

template <typename DEVICE>
void
xpcc::apb::Interface<DEVICE>::sendMessage(Header header, const uint8_t *data, uint8_t size)
{
	size &= 0x1f;
	
	DEVICE::write(syncByte);
	DEVICE::write((static_cast<uint8_t>(header) & 0xe0) | size);
	
	uint8_t crc = crcInitialValue;
	for (uint_fast8_t i = 0; i < size; ++i)
	{
		crc = crcUpdate(crc, *data);
		DEVICE::write(*data);
		data++;
	}
	
	DEVICE::write(crc);
}

// ----------------------------------------------------------------------------

template <typename DEVICE>
bool
xpcc::apb::Interface<DEVICE>::isMessageAvailable()
{
	return (lengthOfReceivedMessage > 0);
}

template <typename DEVICE>
uint8_t
xpcc::apb::Interface<DEVICE>::getLength()
{
	return lengthOfReceivedMessage;
}

template <typename DEVICE>
const uint8_t*
xpcc::apb::Interface<DEVICE>::getData()
{
	return buffer;
}

template <typename DEVICE>
void
xpcc::apb::Interface<DEVICE>::dropMessage()
{
	lengthOfReceivedMessage = 0;
}

// ----------------------------------------------------------------------------

template <typename DEVICE>
void
xpcc::apb::Interface<DEVICE>::update()
{
	char byte;
	while (DEVICE::read(byte))
	{
		switch (state)
		{
			case SYNC:
				if (byte == syncByte) {
					state = HEADER;
				}
				break;
			
			case HEADER:
				length = (byte & 0x1f) + 1;
				position = 0;
				crc = crcInitialValue;
				state = DATA;
				break;
			
			case DATA:
				buffer[position] = byte;
				crc = crcUpdate(crc, byte);
				
				position += 1;
				if (position >= length) {
					state = SYNC;
					if (crc == 0) {
						lengthOfReceivedMessage = length - 1;
					}
				}
				break;
			
			default:
				state = SYNC;
				break;
		}
	}
}

// ----------------------------------------------------------------------------

template <typename DEVICE>
uint8_t
xpcc::apb::Interface<DEVICE>::crcUpdate(uint8_t crc, uint8_t data)
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
