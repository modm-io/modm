/*
 * Copyright (c) 2009-2010, Martin Rosekeit
 * Copyright (c) 2009, 2011, Fabian Greif
 * Copyright (c) 2012, 2014, 2017, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include "receiver_socket.hpp"

#include <sys/socket.h>
#include <linux/tipc.h>
#include <errno.h>
#include <cstring>
#include <unistd.h>

#include <memory>

#include <iostream>

#include <modm/debug/logger.hpp>

#undef  MODM_LOG_LEVEL
#define MODM_LOG_LEVEL modm::log::WARNING

// ----------------------------------------------------------------------------
xpcc::tipc::ReceiverSocket::ReceiverSocket() :
	socketDescriptor_ ( socket (AF_TIPC, SOCK_RDM,0) ) // Create the socket
{
}
// ----------------------------------------------------------------------------
xpcc::tipc::ReceiverSocket::~ReceiverSocket()
{
	// Close the socket
	close(this->socketDescriptor_);
}
// ----------------------------------------------------------------------------
void
xpcc::tipc::ReceiverSocket::registerOnPacket(	unsigned int typeId,
												unsigned int lowerInstance,
												unsigned int upperInstance)
{

	int result = 0;
	sockaddr_tipc fromAddress;

	fromAddress.family				=	AF_TIPC;
	fromAddress.addrtype			=	TIPC_ADDR_NAMESEQ;
	fromAddress.addr.nameseq.type	=	typeId;
	fromAddress.addr.nameseq.lower	=	lowerInstance;
	fromAddress.addr.nameseq.upper	=	upperInstance;
	fromAddress.scope				=	TIPC_CLUSTER_SCOPE;	// Scope of puplisching is cluster (node < cluster < zone)

	MODM_LOG_INFO << MODM_FILE_INFO << "(typeId, lowerBound, upperBound) = (" << typeId << ", " << lowerInstance << ", " << upperInstance << ")" << modm::flush;

	// Binding means registering to a specific packet
	result = 	bind (	this->socketDescriptor_,
						(struct sockaddr*)&fromAddress,
						sizeof(sockaddr_tipc) );

	// If there was an error binding the socket throw an exception because this case
	// cannot be handled here.
	if (0 != result) {
		MODM_LOG_ERROR << MODM_FILE_INFO << "Port {" << typeId << ", " << lowerInstance << ", " << upperInstance << "}  could not be created." << modm::flush;
		// TODO: Throw an exception!!
	}
}
// ----------------------------------------------------------------------------
// This method gets the header of the current TIPC data in the queue.
// It returns the true if a header was available - otherwise false.
bool
xpcc::tipc::ReceiverSocket::receiveHeader(
		uint32_t& transmitterPort,
		Header& tipcHeader )
{
	sockaddr_tipc fromAddress;
	socklen_t addressLength = sizeof( struct sockaddr_tipc );
	Header localTipcHeader;
  	int result = 0;

	// First receive the tipc-header
	result = recvfrom(
			this->socketDescriptor_,
			&localTipcHeader,
			sizeof(Header),
			MSG_PEEK | MSG_DONTWAIT,
	        (sockaddr*) &fromAddress,
	        &addressLength);

	if( result > 0) {
		// Make a copy of the received data
		transmitterPort = fromAddress.addr.id.ref;
		tipcHeader = localTipcHeader;

		return true;
	}
	else if ( errno == EWOULDBLOCK ) {
//		MODM_LOG_DEBUG << __FILE__ << __FUNCTION__ << " no data in buffer" << modm::flush;
		// no data in the buffer
	}
	else if ( errno == 9 ) {
		modm::log::error
				<< MODM_FILE_INFO
				<< "Bad file descriptor"
				<< modm::flush;
	}
	else {
		modm::log::error
				<< MODM_FILE_INFO
				<< "Sorry: unknown Error while receiving data. errno=" << errno
				<< modm::flush;
		// TODO: Error handling??!!
	}

	return false;
}
// -------------------------------------------------------------------------------------------------------
// This method gets payload form the TIPC socket form the given length
// without deleting it. It returns true if the payload could be received
// correctly from the TIPC socket - otherwise false.
bool
xpcc::tipc::ReceiverSocket::receivePayload(uint8_t* payloadPointer, size_t payloadLength)
{
	int result = 0;

	// Allocate memory for the whole packet inclusive header

	std::unique_ptr<uint8_t[]> packetPointer ( new uint8_t[ sizeof(Header) + payloadLength ] );

	result = recv(	this->socketDescriptor_,
					packetPointer.get(),
					sizeof(Header) + payloadLength,
					MSG_PEEK | MSG_DONTWAIT);	// Do not delete data from TIPC and do not wait for data

	if( result > 0 ) {

		// Copy the payload-part of the packet to it's destination!
		memcpy(payloadPointer, packetPointer.get()+sizeof(Header), payloadLength);

/*		MODM_LOG_DEBUG << __FILE__ << __FUNCTION__;
		for(unsigned int i=0; i<payloadLength; i++) {
			MODM_LOG_DEBUG << " " << (int)(*(payloadPointer+i));
		}
		MODM_LOG_DEBUG << modm::flush;
*/
		return true;
	}
	else if ( errno == EWOULDBLOCK ) {
		modm::log::debug
				<< MODM_FILE_INFO
				<< "no data in buffer"
				<< modm::flush;
		// no data in the buffer
	}
	else if ( errno == 9 ) {
		modm::log::error
				<< MODM_FILE_INFO
				<< "Bad file descriptor"
				<< modm::flush;
	}
	else {
		modm::log::error
				<< MODM_FILE_INFO
				<< modm::flush;
		// TODO: Error handling??!!
	}

	return false;
}
// ----------------------------------------------------------------------------
// This method removes the current packet from the TIPC message queue.
// If the method was successful (if a message could be removed) the
// method returns true - otherwise false.
bool
xpcc::tipc::ReceiverSocket::popPayload()
{
 	int result = 0;

	char c;	// TODO: Better way?
	result = recv(	this->socketDescriptor_,
					&c,		// We need no space for data
					1,		// We read no data
					MSG_DONTWAIT);	// Delete data from TIPC but do not wait for data

	if( result > 0 ) {
		return true;
	}
	else if ( errno == EWOULDBLOCK ) {
		modm::log::debug
				<< MODM_FILE_INFO
				<< "no data in buffer"
				<< modm::flush;
		// no data in the buffer
	}
	else if ( errno == 9 ) {
		modm::log::error
				<< MODM_FILE_INFO
				<< " Bad file descriptor"
				<< modm::flush;
	}
	else {
		modm::log::error
				<< MODM_FILE_INFO
				<< modm::flush;
		// TODO: Error handling??!!
	}

	return false;
}
// ----------------------------------------------------------------------------
