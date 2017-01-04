/*
 * Copyright (c) 2009-2010, 2014, Martin Rosekeit
 * Copyright (c) 2009, 2011, Fabian Greif
 * Copyright (c) 2012, 2014, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include "transmitter.hpp"
#include "header.hpp"

#include <modm/debug/logger.hpp>

// TODO: IMPORTANT! If the TIPC-Module is not loaded terminate with appropriate
// error message!!!!

// ----------------------------------------------------------------------------
xpcc::tipc::Transmitter::Transmitter( ) :
	domainId_( xpcc::tipc::Header::DOMAIN_ID_UNDEFINED )
{
}

// ----------------------------------------------------------------------------
xpcc::tipc::Transmitter::~Transmitter()
{
}

// ----------------------------------------------------------------------------
void
xpcc::tipc::Transmitter::setDomainId(unsigned int id)
{
	this->domainId_ = id;
}


// ----------------------------------------------------------------------------
void
xpcc::tipc::Transmitter::transmitRequest( uint8_t destination, const SmartPointer& payload )
{
	this->tipcTransmitterSocket_.transmitPayload(
			REQUEST_OFFSET + destination + TYPE_ID_OFFSET,
			0,
			payload.getPointer(),
			payload.getSize(),
			this->domainId_);
}

// ----------------------------------------------------------------------------
void
xpcc::tipc::Transmitter::transmitEvent( uint8_t event, const SmartPointer& payload )
{
	this->tipcTransmitterSocket_.transmitPayload(
			EVENT_OFFSET + event + TYPE_ID_OFFSET,
			0,
			payload.getPointer(),
			payload.getSize(),
			this->domainId_);
}

// ----------------------------------------------------------------------------
uint32_t
xpcc::tipc::Transmitter::getPortId()
{
	return this->tipcTransmitterSocket_.getPortId();
}
