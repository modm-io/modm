/*
 * Copyright (c) 2009-2010, Fabian Greif
 * Copyright (c) 2009-2010, 2014, Martin Rosekeit
 * Copyright (c) 2012-2014, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include "connector.hpp"
#include <modm/debug/logger.hpp>

#undef  XPCC_LOG_LEVEL
#define XPCC_LOG_LEVEL xpcc::log::WARNING

// ----------------------------------------------------------------------------
xpcc::TipcConnector::TipcConnector() :
	receiver(this->transmitter.getPortId())
{
}

// ----------------------------------------------------------------------------
xpcc::TipcConnector::~TipcConnector()
{
}

// ----------------------------------------------------------------------------
void
xpcc::TipcConnector::setDomainId(unsigned int domainId)
{
	this->transmitter.setDomainId( domainId );
	this->receiver.setDomainId( domainId );
}

// ----------------------------------------------------------------------------
bool
xpcc::TipcConnector::isPacketAvailable() const
{
	return this->receiver.hasPacket();
}

// ----------------------------------------------------------------------------
const xpcc::Header&
xpcc::TipcConnector::getPacketHeader() const
{
	return *(xpcc::Header*) this->receiver.getPacket().getPointer();
}

// ----------------------------------------------------------------------------
const xpcc::SmartPointer
xpcc::TipcConnector::getPacketPayload() const
{
	SmartPointer payload( this->receiver.getPacket().getSize() - sizeof(xpcc::Header) );
	if( payload.getSize() > 0 ) {
		memcpy(
				payload.getPointer(),
				this->receiver.getPacket().getPointer() + sizeof(xpcc::Header),
				payload.getSize() );
	}
	return payload;
}

// ----------------------------------------------------------------------------
void
xpcc::TipcConnector::dropPacket()
{
	this->receiver.dropPacket();
}

// ----------------------------------------------------------------------------
void
xpcc::TipcConnector::sendPacket(const xpcc::Header &header, SmartPointer payload)
{
//	XPCC_LOG_DEBUG << XPCC_FILE_INFO
//			<< " payload size=" << payload.getSize()
//			<< " payload=" << payload
//			<< xpcc::flush;

	SmartPointer combinedPayload( sizeof(xpcc::Header) + payload.getSize() );

	memcpy(	combinedPayload.getPointer(), &header, sizeof(xpcc::Header) );

	// place payload behind the header
	if ( payload.getSize() > 0 ) {
		memcpy(	combinedPayload.getPointer() + sizeof(xpcc::Header),
				payload.getPointer(),
				payload.getSize());
	}

	if ( header.destination != 0 ) {
		// transmit a REQUENST
		this->transmitter.transmitRequest( header.destination, combinedPayload );;
	}
	else {
		// transmit an EVENT
		this->transmitter.transmitEvent( header.packetIdentifier, combinedPayload );
	}
}

// ----------------------------------------------------------------------------
void
xpcc::TipcConnector::update()
{
	// nothing to do, because TipcReceiver is using threads
}
