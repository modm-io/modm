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

#include <iostream>

#include "tipc_receiver.h"
#include "../universal_channel_identifier.h"
#include "../universal_component_identifier.h"
#include "../icc_header_definition.h"


#include <modules/backplane/logging/logger_message_forwarder.hpp>
SUB_LOGGER_CREATE(logger, Logger::WARNING, "icc::tipc::receiver");

// -------------------------------------------------------------------------------------------------------
rca::tipc::Receiver::Receiver(	icc::TChannelId 	channelId,
								icc::TComponentId	componentId) :
	channelId_		( channelId ),
	componentId_	( componentId ),
	isAlive_		( true )
{
	// The start of the thread has to be placed _after_ the initilisation of isAlive_
	this->receiverThread_.reset( new TThread( boost::bind(&Receiver::runReceiver, this) ) );
}
 // -------------------------------------------------------------------------
rca::tipc::Receiver::~Receiver()
{
	this->isAlive_ = false;
	this->receiverThread_->join();
}
// -------------------------------------------------------------------------
bool
rca::tipc::Receiver::frontHeader( icc::THeader & header )
{
	// Set the mutex guard for the packetQueue
	TMutexGuard packetQueueGuard( this->packetQueueLock_);

	if( !this->packetQueue_.empty() ) {
		TPacketQueueSummary packetQueueSummary = this->packetQueue_.front();
		header = packetQueueSummary.iccHeader;

		return true;
	}
	else {
		return false;
	}
}
// -------------------------------------------------------------------------
void
rca::tipc::Receiver::popFront()
{
	// Set the mutex guard for the packetQueue
	TMutexGuard packetQueueGuard( this->packetQueueLock_);

	this->packetQueue_.pop();
}
// -------------------------------------------------------------------------
bool
rca::tipc::Receiver::hasPacket()
{
	// Set the mutex guard for the packetQueue
	TMutexGuard packetQueueGuard( this->packetQueueLock_ );

	return !this->packetQueue_.empty();
}
// -------------------------------------------------------------------------
bool rca::tipc::Receiver::isAlive() {
	return this->isAlive_;
}
// -------------------------------------------------------------------------
void*
rca::tipc::Receiver::runReceiver()
{
	while( this->isAlive() ) {
		//	SUB_LOGGER_LOG(logger, Logger::DEBUG, "runReceiver")
		//		<< "calling update from: " << thisPointer->componentId_;
		this->update();
		usleep(10000);
	}

	SUB_LOGGER_LOG(logger, Logger::INFO, "runReceiver")
			<< "of:" << this->componentId_
			<< " has terminated.";
	return 0;
}
// -------------------------------------------------------------------------
// This method is private and is called from the runReceiver again and again!
void
rca::tipc::Receiver::update()
{
	THeader tipcHeader;
	// Set the mutex guard for the receiver socket
	TMutexGuard receiverSocketGuard( this->receiverSocketLock_ );


	SUB_LOGGER_LOG(logger, Logger::DEBUG, "update")
		<< this->componentId_;

	// Get the TIPC header (typeId and instanceRange) - call by reference
	while( this->tipcReceiverSocket_.receiveHeader( tipcHeader ) ) {
		SUB_LOGGER_LOG(logger, Logger::DEBUG, "update")
			<< this->componentId_ << " headerAvailable ";

		// Try to allocate memory for the packet
		TSharedArr tipcPayloadPtr ( new char[tipcHeader.payloadLength] );

		bool payloadAvailabe	=	false;

		// Get the payload by passing a void pointer by reference and the length
		// of the payload to be read from the socket.
		payloadAvailabe	= this->tipcReceiverSocket_.receivePayload(	tipcPayloadPtr.get(),
																	tipcHeader.payloadLength);

		if( payloadAvailabe ) {
			// Place the payload together with it's header in a list
			TPacketQueueSummary packetQueueSummary;
			packetQueueSummary.iccHeader		=	*((icc::THeader*) tipcPayloadPtr.get()); // the header stand on the beginning of the packet

			// Receiver != Transmitter AND
			// Receiver listen to the reight channel AND
			// Message was send to Receiver or was send to all components
			if( ( packetQueueSummary.iccHeader.transmitterChannelId		!= this->channelId_			||
				  packetQueueSummary.iccHeader.transmitterComponentId 	!= this->componentId_   	)	&&
				( packetQueueSummary.iccHeader.receiverChannelId 		== this->channelId_     	) 	&&
			    ( packetQueueSummary.iccHeader.receiverComponentId		== icc::ALL_COMPONENTS_ID 	||
			      packetQueueSummary.iccHeader.receiverComponentId  	== this->componentId_   	) 	) {

				packetQueueSummary.iccPayloadPtr.reset( new char[packetQueueSummary.iccHeader.size] );
				memcpy(	packetQueueSummary.iccPayloadPtr.get(),
						tipcPayloadPtr.get() + sizeof(icc::THeader),
						packetQueueSummary.iccHeader.size);

				// Set the mutex guard for the packetQueue
				TMutexGuard packetQueueGuard( this->packetQueueLock_);

				// Add the packet to the queue
				this->packetQueue_.push( packetQueueSummary );
			}
		}

		// Clean the TIPC socket! ( That means removing the current data from the queue)
		this->tipcReceiverSocket_.popPayload();
	}
}
// -------------------------------------------------------------------------
bool
rca::tipc::Receiver::frontPayload( void * payload, size_t size )
{
	// Set the mutex guard for the packetQueue
	TMutexGuard packetQueueGuard( this->packetQueueLock_);

	if( !this->packetQueue_.empty() ) {
		TPacketQueueSummary packetQueueSummary = this->packetQueue_.front();

		if ( size != packetQueueSummary.iccHeader.size ) {
			SUB_LOGGER_LOG(logger, Logger::ERROR, "frontPayload")
				<< "ERROR: ICC-Receiver: size of given payload type is different from received payload!!."
				<< "\n       given_size=" << size << " received_size=" << packetQueueSummary.iccHeader.size
				<< "\n       payload_id=" << std::hex << "0x" << packetQueueSummary.iccHeader.payloadId
				<< "\n       transmitter_id=" << std::hex << "0x" << packetQueueSummary.iccHeader.transmitterComponentId
				<< "\n       receiver_id=" << std::hex << "0x" << packetQueueSummary.iccHeader.receiverComponentId
				<< "\n       serviceSpecifier=" << packetQueueSummary.iccHeader.serviceSpecifier
				<< " - error=" << packetQueueSummary.iccHeader.error
				<< std::dec;

			throw rca::icc::WrongPayloadSizeException();
		}

		// Make a copy of the payload (is located after the icc header)!
		memcpy(	payload,
				packetQueueSummary.iccPayloadPtr.get(),
				packetQueueSummary.iccHeader.size);

		// TODO: Error handling?!

		return true;
	}
	else {
		// No packet was available
		return false;
	}
}
// -------------------------------------------------------------------------
void
rca::tipc::Receiver::addPacketListener(	icc::TChannelId 	channelId,
										icc::TPayloadId 	payloadId)
{
	unsigned int lowerInstancId = 0;
	unsigned int upperInstancId = 0;

	// Set the mutex guard for the receiver socket
	TMutexGuard receiverSocketGuard( this->receiverSocketLock_ );

	// TODO: Logging on which packet one is registered..

	// Register on a packet type specified by an instance. The instanceId
	// contains the channelId. The component Id is not used on this level
	// because it is not possible, that the ranges are intercept with each other.
	// So it is not possible to say from 1 to 10 and from 2 to 5.


	/* Ranges dürfen sich nicht überschneiden. Eine Range gilt fürs gesamte TIPC,
	 * daher ist es nicht möglich in die InstanceId auch die Komponenten ID
	 * mit einzubeziehen
	 */

	// Get packets on ONE channels and the given component(s)
	lowerInstancId = channelId;
	upperInstancId = channelId;
	this->tipcReceiverSocket_.registerOnPacket(	payloadId + TYPE_ID_OFFSET,
												lowerInstancId,
												upperInstancId);

}
// -------------------------------------------------------------------------
