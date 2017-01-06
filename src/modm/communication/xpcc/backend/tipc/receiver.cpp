/*
 * Copyright (c) 2009-2010, 2014, Martin Rosekeit
 * Copyright (c) 2009-2011, Fabian Greif
 * Copyright (c) 2012, 2014, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include "receiver.hpp"
#include "header.hpp"

#include <boost/bind.hpp>

#include <modm/debug/logger.hpp>

#undef  MODM_LOG_LEVEL
#define MODM_LOG_LEVEL modm::log::WARNING

// ----------------------------------------------------------------------------
xpcc::tipc::Receiver::Receiver(
		uint32_t ignoreTipcPortId) :
	tipcReceiverSocket_(),
	ignoreTipcPortId_(ignoreTipcPortId),
	domainId_( tipc::Header::DOMAIN_ID_UNDEFINED ),
	packetQueue_(),
	receiverThread_(),
	receiverSocketLock_(),
	packetQueueLock_(),
	isAlive_(true)
{
	// The start of the thread has to be placed _after_ the initialization of isAlive_
	this->receiverThread_.reset(new Thread(boost::bind(&Receiver::runReceiver, this)));
}

// ----------------------------------------------------------------------------
xpcc::tipc::Receiver::~Receiver()
{
	this->isAlive_ = false;
	this->receiverThread_->join();
}

// ----------------------------------------------------------------------------
void
xpcc::tipc::Receiver::setDomainId( unsigned int domainId )
{
	this->domainId_ = domainId;
}

// ----------------------------------------------------------------------------
void
xpcc::tipc::Receiver::dropPacket()
{
	// Set the mutex guard for the packetQueue
	MutexGuard packetQueueGuard(this->packetQueueLock_);

	this->packetQueue_.pop();
}

// ----------------------------------------------------------------------------
bool
xpcc::tipc::Receiver::hasPacket() const
{
	// Set the mutex guard for the packetQueue
	MutexGuard packetQueueGuard(this->packetQueueLock_);

	return !this->packetQueue_.empty();
}

// ----------------------------------------------------------------------------
bool
xpcc::tipc::Receiver::isAlive()
{
	return this->isAlive_;
}

// ----------------------------------------------------------------------------
void*
xpcc::tipc::Receiver::runReceiver()
{
	while (this->isAlive())
	{
		this->update();
		usleep(1000);
	}

	MODM_LOG_INFO << MODM_FILE_INFO << "Thread terminates." << modm::flush;
	return 0;
}

// ----------------------------------------------------------------------------
// This method is private and is called from the runReceiver again and again!
void
xpcc::tipc::Receiver::update()
{
	xpcc::tipc::Header tipcHeader;
	uint32_t tipcPortId;
	// Set the mutex guard for the receiver socket
	MutexGuard receiverSocketGuard( this->receiverSocketLock_ );

	// Get the TIPC header (typeId and instanceRange) - call by reference
	while( this->tipcReceiverSocket_.receiveHeader( tipcPortId, tipcHeader ) )
	{
		// ignore messages, that are send by the port, that shoud be ignored
		if 		(tipcPortId != this->ignoreTipcPortId_ &&
				(this->domainId_==Header::DOMAIN_ID_UNDEFINED || this->domainId_==tipcHeader.domainId) )
		{
			MODM_LOG_DEBUG << MODM_FILE_INFO << "Header available." << modm::flush;

			// Try to allocate memory for the packet
			Payload payload ( tipcHeader.size );

			// Get the payload by passing a void pointer by reference and the length
			// of the payload to be read from the socket.
			this->tipcReceiverSocket_.receivePayload(
					payload.getPointer(),
					tipcHeader.size);

			// Set the mutex guard for the packetQueue
			MutexGuard packetQueueGuard( this->packetQueueLock_);

			// add the packet to the queue
			this->packetQueue_.push( payload );
		}
		// Clean the TIPC socket! ( That means removing the current data from the queue)
		this->tipcReceiverSocket_.popPayload();
	}
}
// ----------------------------------------------------------------------------
const modm::SmartPointer&
xpcc::tipc::Receiver::getPacket() const
{
	// Set the mutex guard for the packetQueue
	MutexGuard packetQueueGuard( this->packetQueueLock_);

	if (!this->packetQueue_.empty()) {
		return this->packetQueue_.front();
	}
	else {
		// No packet was available
		MODM_LOG_ERROR << MODM_FILE_INFO << "Empty packet queue." << modm::flush;
		throw "xpcc::tipc::Receiver::frontPayload() : Empty packet queue.";
	}
}

// ----------------------------------------------------------------------------
void
xpcc::tipc::Receiver::addEventId(uint8_t id)
{
	// Set the mutex guard for the receiver socket
	MutexGuard receiverSocketGuard(this->receiverSocketLock_);

	// TODO: Logging on which packet one is registered..

	// Ranges dürfen sich nicht überschneiden. Eine Range gilt fürs gesamte TIPC,
	// daher ist es nicht möglich in die InstanceId auch die Komponenten ID
	// mit einzubeziehen

	this->tipcReceiverSocket_.registerOnPacket(	EVENT_OFFSET + id + TYPE_ID_OFFSET,
												0x00,
												0x00);
}

// ----------------------------------------------------------------------------
void
xpcc::tipc::Receiver::addReceiverId(uint8_t id)
{
	// Set the mutex guard for the receiver socket
	MutexGuard receiverSocketGuard(this->receiverSocketLock_);

	// TODO: Logging on which packet one is registered..

	// Ranges dürfen sich nicht überschneiden. Eine Range gilt fürs gesamte TIPC,
	// daher ist es nicht möglich in die InstanceId auch die Komponenten ID
	// mit einzubeziehen

	this->tipcReceiverSocket_.registerOnPacket(	REQUEST_OFFSET + id + TYPE_ID_OFFSET,
												0x00,
												0x00);
}
