/*
 * Copyright (c) 2009-2012, Fabian Greif
 * Copyright (c) 2009-2010, Martin Rosekeit
 * Copyright (c) 2012-2013, Niklas Hauser
 * Copyright (c) 2013, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef	MODM_SAB_INTERFACE_HPP
	#error	"Don't include this file directly, use 'interface.hpp' instead!"
#endif

#ifdef __AVR__
	#include <util/crc16.h>
#endif

/*#include <modm/debug/logger.hpp>

#undef MODM_LOG_LEVEL
#define MODM_LOG_LEVEL	modm::log::DEBUG*/

// ----------------------------------------------------------------------------
template <typename Device> typename modm::sab::Interface<Device>::State \
	modm::sab::Interface<Device>::state = SYNC;

template <typename Device> uint8_t modm::sab::Interface<Device>::buffer[maxPayloadLength + 3];
template <typename Device> uint8_t modm::sab::Interface<Device>::crc;
template <typename Device> uint8_t modm::sab::Interface<Device>::position;
template <typename Device> uint8_t modm::sab::Interface<Device>::length;
template <typename Device> uint8_t modm::sab::Interface<Device>::lengthOfReceivedMessage = 0;

// ----------------------------------------------------------------------------

template <typename Device>
void
modm::sab::Interface<Device>::initialize()
{
	//Device::setBaudrate(115200UL);
	state = SYNC;
}

// ----------------------------------------------------------------------------

template <typename Device>
void
modm::sab::Interface<Device>::sendMessage(uint8_t address, Flags flags, 
		uint8_t command,
		const void *payload, uint8_t payloadLength)
{
	uint8_t crcSend;
	
	Device::write(syncByte);
	Device::write(payloadLength);
	crcSend = crcUpdate(crcInitialValue, payloadLength);
	Device::write(address | flags);
	crcSend = crcUpdate(crcSend, address | flags);
	Device::write(command);
	crcSend = crcUpdate(crcSend, command);
	
	const uint8_t *ptr = static_cast<const uint8_t *>(payload);
	for (uint_fast8_t i = 0; i < payloadLength; ++i)
	{
		crcSend = crcUpdate(crcSend, *ptr);
		Device::write(*ptr);
		ptr++;
	}
	
	Device::write(crcSend);
}

template <typename Device> template <typename T>
void
modm::sab::Interface<Device>::sendMessage(uint8_t address, Flags flags,
		uint8_t command,
		const T& payload)
{
	sendMessage(address, flags,
			command,
			reinterpret_cast<const void *>(&payload), sizeof(T));
}

template <typename Device>
void
modm::sab::Interface<Device>::sendMessage(uint8_t address, Flags flags, uint8_t command)
{
	sendMessage(address, flags,
			command,
			0, 0);
}

// ----------------------------------------------------------------------------

template <typename Device>
bool
modm::sab::Interface<Device>::isMessageAvailable()
{
	return (lengthOfReceivedMessage != 0);
}

template <typename Device>
uint8_t
modm::sab::Interface<Device>::getAddress()
{
	return (buffer[0] & 0x3f);
}

template <typename Device>
uint8_t
modm::sab::Interface<Device>::getCommand()
{
	return buffer[1];
}

template <typename Device>
bool
modm::sab::Interface<Device>::isResponse()
{
	return (buffer[0] & 0x80) ? true : false;
}

template <typename Device>
bool
modm::sab::Interface<Device>::isAcknowledge()
{
	return (buffer[0] & 0x40) ? true : false;
}

template <typename Device>
const uint8_t*
modm::sab::Interface<Device>::getPayload()
{
	return &buffer[2];
}

template <typename Device>
uint8_t
modm::sab::Interface<Device>::getPayloadLength()
{
	return (lengthOfReceivedMessage - 3);
}

template <typename Device>
void
modm::sab::Interface<Device>::dropMessage()
{
	lengthOfReceivedMessage = 0;
}

// ----------------------------------------------------------------------------

template <typename Device>
void
modm::sab::Interface<Device>::update()
{
	uint8_t byte;
	while (Device::read(byte))
	{
		//MODM_LOG_DEBUG.printf("%02x ", byte);
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
					if (crc == 0) {
						lengthOfReceivedMessage = length;
						//MODM_LOG_DEBUG << "SAB received" << modm::endl;
					}
					else {
						//MODM_LOG_ERROR << "CRC error" << modm::endl;
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
