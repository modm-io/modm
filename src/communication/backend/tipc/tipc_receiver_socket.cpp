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
#include <iostream>
#include <stdlib.h>
#include <sys/poll.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <linux/tipc.h>
#include <errno.h>
#include <boost/scoped_array.hpp>
#include <string.h>

#include "tipc_receiver_socket.h"

#include <modules/backplane/logging/logger_message_forwarder.hpp>
SUB_LOGGER_CREATE(logger, Logger::WARNING, "icc::tipc::receiver_socket");

// -------------------------------------------------------------------------------------------------------
rca::tipc::ReceiverSocket::ReceiverSocket() :
	socketDescriptor_ ( socket (AF_TIPC, SOCK_RDM,0) ) // Create the socket
{
}
// -------------------------------------------------------------------------------------------------------
rca::tipc::ReceiverSocket::~ReceiverSocket() 
{
	// Close the socket
	close(this->socketDescriptor_);
}
// -------------------------------------------------------------------------------------------------------
void 
rca::tipc::ReceiverSocket::registerOnPacket(	unsigned int typeId,
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

	SUB_LOGGER_LOG(logger, Logger::INFO, "registerOnPacket")
		<< "(typeId, lowerBound, upperBound) = (" << typeId << ", " << lowerInstance << ", " << upperInstance << ")";

	// Binding means registering to a specific packet
	result = 	bind (	this->socketDescriptor_, 
						(struct sockaddr*)&fromAddress,
						sizeof(sockaddr_tipc) );

	// If there was an error binding the socket throw an exception because this case
	// cannot be handled here.
	if (0 != result) {		
		SUB_LOGGER_LOG(logger, Logger::ERROR, "registerOnPacket")
			<< "Port {" << typeId << ", " << lowerInstance << ", " << upperInstance << "}  could not be created.";
		// TODO: Throw an exception!!
	}
}
// -----------------------------------------------------------------------------
// This method gets the header of the current TIPC data in the queue. 
// It returns the true if a header was available - otherwise false.
bool 
rca::tipc::ReceiverSocket::receiveHeader( THeader & tipcHeader ) 
{	
//	sockaddr_tipc fromAddress;
//	socklen_t addressLength = sizeof( struct sockaddr_tipc );
	THeader localTipcHeader;
  	int result = 0;
	
	// First receive the tipc-header
	result = recv(	this->socketDescriptor_,
					&localTipcHeader,
					sizeof(THeader),
					MSG_PEEK | MSG_DONTWAIT);	// Do not delete data from TIPC and do not wait for data
	
	if( result > 0) {
		// Make a copy of the received data
		tipcHeader = localTipcHeader;
		return true;
	}
	else if ( errno == EWOULDBLOCK ) {
		SUB_LOGGER_LOG(logger, Logger::DEBUG, "receiveHeader")
			<< "no data in buffer";
		// no data in the buffer
	}
	else if ( errno == 9 ) {
		SUB_LOGGER_LOG(logger, Logger::ERROR, "receiveHeader")
			<< "Bad file descriptor";
	}
	else {
		SUB_LOGGER_LOG(logger, Logger::ERROR, "receiveHeader")
			<< "Sorry: unknown Error while receiving data. errno=" << errno;
		// TODO: Error handling??!!
	}

	return false;
}
// -------------------------------------------------------------------------------------------------------
// This method gets payload form the TIPC socket form the given length
// without deleting it. It returns true if the payload could be received
// correctly from the TIPC socket - otherwise false.
bool 
rca::tipc::ReceiverSocket::receivePayload(char* payloadPointer, size_t payloadLength) 
{		
	int result = 0;

	// Allocate memory for the whole packet inclusive header			
	boost::scoped_array<char> packetPointer ( new char[ sizeof(THeader) + payloadLength ] );

	result = recv(	this->socketDescriptor_,
					packetPointer.get(),
					sizeof(THeader) + payloadLength,
					MSG_PEEK | MSG_DONTWAIT);	// Do not delete data from TIPC and do not wait for data
											
	if( result > 0 ) {
		
		// Copy the payload-part of the packet to it's destination!
		memcpy(payloadPointer, packetPointer.get()+sizeof(THeader), payloadLength);
		
		return true;
	}
	else if ( errno == EWOULDBLOCK ) {
		SUB_LOGGER_LOG(logger, Logger::DEBUG, "receivePayload")
			<< "no data in buffer";
		// no data in the buffer
	}
	else if ( errno == 9 ) {
		SUB_LOGGER_LOG(logger, Logger::ERROR, "receiveHeader")
			<< "Bad file descriptor";
	}
	else {
		SUB_LOGGER_LOG(logger, Logger::ERROR, "receivePayload");
		// TODO: Error handling??!!
	}
	
	return false;
}		
// -------------------------------------------------------------------------------------------------------
// This method removes the current packet from the TIPC message queue.
// If the method was successful (if a message could be removed) the
// method returns true - otherwise false.
bool 
rca::tipc::ReceiverSocket::popPayload() 
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
		SUB_LOGGER_LOG(logger, Logger::DEBUG, "popPayload")
			<< "no data in buffer";
		// no data in the buffer
	}
	else if ( errno == 9 ) {
		SUB_LOGGER_LOG(logger, Logger::ERROR, "receiveHeader")
			<< "Bad file descriptor";
	}
	else {
		SUB_LOGGER_LOG(logger, Logger::ERROR, "popPayload");
		// TODO: Error handling??!!
	}
	
	return false;
}
// -------------------------------------------------------------------------------------------------------
