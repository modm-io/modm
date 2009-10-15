// coding: utf-8
// ----------------------------------------------------------------------------
/* Copyright (c) 2009, Roboterclub Aachen e.V.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
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

#include <boost/bind.hpp>

#include "tipc_receiver.h"
#include "header.hpp"

#include "../../../debug/logger/logger.hpp"

// -------------------------------------------------------------------------------------------------------
xpcc::tipc::Receiver::Receiver() :
	tipcReceiverSocket_	(),
	packetQueue_		(),
	receiverThread_		(),
	receiverSocketLock_	(),
	packetQueueLock_	(),
	isAlive_			( true )
{
	// The start of the thread has to be placed _after_ the initialization of isAlive_
	this->receiverThread_.reset( new Thread( boost::bind(&Receiver::runReceiver, this) ) );
}
 // -------------------------------------------------------------------------
xpcc::tipc::Receiver::~Receiver()
{
	this->isAlive_ = false;
	this->receiverThread_->join();
}
// -------------------------------------------------------------------------
const ::xpcc::Header&
xpcc::tipc::Receiver::frontHeader( )
{
	// Set the mutex guard for the packetQueue
	MutexGuard packetQueueGuard( this->packetQueueLock_);

	return this->packetQueue_.front().header;
}
// -------------------------------------------------------------------------
void
xpcc::tipc::Receiver::popFront()
{
	// Set the mutex guard for the packetQueue
	MutexGuard packetQueueGuard( this->packetQueueLock_);

	this->packetQueue_.pop();
}
// -------------------------------------------------------------------------
bool
xpcc::tipc::Receiver::hasPacket() const
{
	// Set the mutex guard for the packetQueue
	MutexGuard packetQueueGuard( this->packetQueueLock_ );

	return !this->packetQueue_.empty();
}
// -------------------------------------------------------------------------
bool
xpcc::tipc::Receiver::isAlive() {
	return this->isAlive_;
}
// -------------------------------------------------------------------------
void*
xpcc::tipc::Receiver::runReceiver()
{
	while( this->isAlive() ) {
		//	xpcc::log::debug
		//		<< __CLASS__ << "::" << __FUNCTION__
		//		<< xpcc::flush;
		this->update();
		usleep(1000);
	}

	xpcc::log::info
			<< __FUNCTION__
			<< " has terminated."
			<< xpcc::flush;
	return 0;
}
// -------------------------------------------------------------------------
// This method is private and is called from the runReceiver again and again!
void
xpcc::tipc::Receiver::update()
{
	xpcc::tipc::Header tipcHeader;
	// Set the mutex guard for the receiver socket
	MutexGuard receiverSocketGuard( this->receiverSocketLock_ );


//	xpcc::log::debug
//			<< __FUNCTION__
//			<< xpcc::flush;

	// Get the TIPC header (typeId and instanceRange) - call by reference
	while( this->tipcReceiverSocket_.receiveHeader( tipcHeader ) ) {
		xpcc::log::debug
			<< __FUNCTION__
			<< " headerAvailable "
			<< xpcc::flush;

		// Try to allocate memory for the packet
		SharedArr tipcPayloadPtr ( new char[tipcHeader.size] );

		// Get the payload by passing a void pointer by reference and the length
		// of the payload to be read from the socket.
		this->tipcReceiverSocket_.receivePayload(
				tipcPayloadPtr.get(),
				tipcHeader.size);

		// Place the payload together with it's header in a list
		PacketQueueSummary packetQueueSummary;
		packetQueueSummary.header		=	*((xpcc::Header*) tipcPayloadPtr.get()); // the header stand on the beginning of the packet

		packetQueueSummary.payloadPtr.reset( new char[tipcHeader.size - sizeof(xpcc::Header)] );
		memcpy(	packetQueueSummary.payloadPtr.get(),
				tipcPayloadPtr.get() + sizeof(xpcc::Header),
				tipcHeader.size - sizeof(xpcc::Header));

		// Set the mutex guard for the packetQueue
		MutexGuard packetQueueGuard( this->packetQueueLock_);

		// Add the packet to the queue
		this->packetQueue_.push( packetQueueSummary );

		// Clean the TIPC socket! ( That means removing the current data from the queue)
		this->tipcReceiverSocket_.popPayload();
	}
}
// -------------------------------------------------------------------------
const uint8_t *
xpcc::tipc::Receiver::frontPayload()
{
	// Set the mutex guard for the packetQueue
	MutexGuard packetQueueGuard( this->packetQueueLock_);

	if( !this->packetQueue_.empty() ) {
		return (uint8_t*) this->packetQueue_.front().payloadPtr.get();
	}
	else {
		// No packet was available
		return 0;
	}
}
// -------------------------------------------------------------------------
void
xpcc::tipc::Receiver::addEventId(uint8_t id)
{
	// Set the mutex guard for the receiver socket
	MutexGuard receiverSocketGuard( this->receiverSocketLock_ );

	// TODO: Logging on which packet one is registered..

	// Ranges dürfen sich nicht überschneiden. Eine Range gilt fürs gesamte TIPC,
	// daher ist es nicht möglich in die InstanceId auch die Komponenten ID
	// mit einzubeziehen

	this->tipcReceiverSocket_.registerOnPacket(	EVENT_OFFSET + id + TYPE_ID_OFFSET,
												0x00,
												0x00);
}
// -------------------------------------------------------------------------
void
xpcc::tipc::Receiver::addReceiverId(uint8_t id)
{
	// Set the mutex guard for the receiver socket
	MutexGuard receiverSocketGuard( this->receiverSocketLock_ );

	// TODO: Logging on which packet one is registered..

	// Ranges dürfen sich nicht überschneiden. Eine Range gilt fürs gesamte TIPC,
	// daher ist es nicht möglich in die InstanceId auch die Komponenten ID
	// mit einzubeziehen

	this->tipcReceiverSocket_.registerOnPacket(	REQUEST_OFFSET + id + TYPE_ID_OFFSET,
												0x00,
												0x00);
}
// -------------------------------------------------------------------------
xpcc::tipc::Receiver::PacketQueueSummary::PacketQueueSummary() :
	header(),
	payloadPtr()
{

}

// -----------------------------------------------------------------------------

xpcc::tipc::Receiver::PacketQueueSummary::PacketQueueSummary(
		xpcc::Header header,
		SharedArr ptr) :
	header( header ),
	payloadPtr( ptr )
{

}

// -----------------------------------------------------------------------------

