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

#ifndef	XPCC_AMNB__NODE_HPP
#error	"Don't include this file directly, use 'node.hpp' instead!"
#endif

// ----------------------------------------------------------------------------
template <typename T>
void
xpcc::amnb::Response::send(const T& payload)
{
	triggered = true;
	transmitter->send(true, reinterpret_cast<const void *>(&payload), sizeof(T));
}

xpcc::amnb::Response::Response(Transmitter *parent) :
transmitter(parent), triggered(false)
{
}

void
xpcc::amnb::Response::error(uint8_t errorCode)
{
	triggered = true;
	
	uint8_t error = errorCode;
	transmitter->send(false, &error, 1);
}

void
xpcc::amnb::Response::send()
{
	triggered = true;
	transmitter->send(true, 0, 0);
}

void
xpcc::amnb::Response::send(const void *payload, std::size_t length)
{
	triggered = true;
	transmitter->send(true, payload, length);
}

// ----------------------------------------------------------------------------
inline void
xpcc::amnb::Action::call(Response& response, const void *payload)
{
	// redirect call to the actual object
	(object->*function)(response, payload);
}

// ----------------------------------------------------------------------------
inline void
xpcc::amnb::Listener::call(const void *payload, const uint8_t length)
{
	// redirect call to the actual object
	(object->*function)(payload, length);
}

// ----------------------------------------------------------------------------
inline void
xpcc::amnb::ErrorHandler::call(Flags type, const uint8_t errorCode)
{
	// redirect call to the actual object
	(object->*function)(type, errorCode);
}

// Disable warnings for Visual Studio about using 'this' in a base member
// initializer list.
// In this case though it is totally safe so it is ok to disable this warning.
#ifdef XPCC__COMPILER_MSVC
#	pragma warning(disable:4355)
#endif

// ----------------------------------------------------------------------------
template <typename Interface>
xpcc::amnb::Node<Interface>::Node(uint8_t address,
								  xpcc::accessor::Flash<Action> actionList,
								  uint8_t actionCount,
								  xpcc::accessor::Flash<Listener> listenList,
								  uint8_t listenCount,
								  xpcc::accessor::Flash<ErrorHandler> errorHandlerList,
								  uint8_t errorHandlerCount) :
ownAddress(address),
actionList(actionList), actionCount(actionCount),
listenList(listenList), listenCount(listenCount),
errorHandlerList(errorHandlerList), errorHandlerCount(errorHandlerCount),
response(this)
{
	Interface::initialize(address);
	queryStatus = ERROR_TIMEOUT;
}

template <typename Interface>
xpcc::amnb::Node<Interface>::Node(uint8_t address,
								  xpcc::accessor::Flash<Action> actionList,
								  uint8_t actionCount,
								  xpcc::accessor::Flash<Listener> listenList,
								  uint8_t listenCount) :
ownAddress(address),
actionList(actionList), actionCount(actionCount),
listenList(listenList), listenCount(listenCount),
errorHandlerCount(0),
response(this)
{
	Interface::initialize(address);
	queryStatus = ERROR_TIMEOUT;
}

template <typename Interface>
xpcc::amnb::Node<Interface>::Node(uint8_t address,
								  xpcc::accessor::Flash<Action> actionList,
								  uint8_t actionCount) :
ownAddress(address),
actionList(actionList), actionCount(actionCount),
listenCount(0),
errorHandlerCount(0),
response(this)
{
	Interface::initialize(address);
	queryStatus = ERROR_TIMEOUT;
}

// ----------------------------------------------------------------------------
template <typename Interface> template <typename T>
bool
xpcc::amnb::Node<Interface>::query(uint8_t slaveAddress, uint8_t command,
								   const T& payload, uint8_t responseLength)
{
	if (queryStatus == IN_PROGRESS) {
		checkErrorHandlers(slaveAddress, command, REQUEST, ERROR__QUERY_IN_PROGRESS);
		return false;
	}
	
	bool noError(true);
	if (!Interface::messageTransmitted()) {
		checkErrorHandlers(Interface::getTransmittedAddress(),
						   Interface::getTransmittedCommand(),
						   Interface::getTransmittedFlags(),
						   ERROR__MESSAGE_OVERWRITTEN);
		noError = false;
	}
	if (!Interface::sendMessage(slaveAddress, REQUEST, command, payload)) {
		checkErrorHandlers(slaveAddress, command, REQUEST, ERROR__TRANSMITTER_BUSY);
		noError = false;
	}
	
	queryStatus = IN_PROGRESS;
	expectedResponseLength = responseLength;
	expectedAddress = slaveAddress;
	
	timer.restart(timeout);
	return noError;
}

template <typename Interface>
bool
xpcc::amnb::Node<Interface>::query(uint8_t slaveAddress, uint8_t command,
								   uint8_t responseLength)
{
	if (queryStatus == IN_PROGRESS) {
		checkErrorHandlers(slaveAddress, command, REQUEST, ERROR__QUERY_IN_PROGRESS);
		return false;
	}
	
	bool noError(true);
	if (!Interface::messageTransmitted()) {
		checkErrorHandlers(Interface::getTransmittedAddress(),
						   Interface::getTransmittedCommand(),
						   Interface::getTransmittedFlags(),
						   ERROR__MESSAGE_OVERWRITTEN);
		noError = false;
	}
	if (!Interface::sendMessage(slaveAddress, REQUEST, command, 0, 0)) {
		checkErrorHandlers(slaveAddress, command, REQUEST, ERROR__TRANSMITTER_BUSY);
		noError = false;
	}
	
	queryStatus = IN_PROGRESS;
	expectedResponseLength = responseLength;
	expectedAddress = slaveAddress;
	
	timer.restart(timeout);
	return noError;
}

// ----------------------------------------------------------------------------
template <typename Interface> template <typename T>
bool
xpcc::amnb::Node<Interface>::broadcast(uint8_t command, const T& payload)
{
	bool noError(true);
	if (!Interface::messageTransmitted()) {
		checkErrorHandlers(Interface::getTransmittedAddress(),
						   Interface::getTransmittedCommand(),
						   Interface::getTransmittedFlags(),
						   ERROR__MESSAGE_OVERWRITTEN);
		noError = false;
		
	}
	if (!Interface::sendMessage(this->ownAddress, BROADCAST, command, payload)) {
		checkErrorHandlers(this->ownAddress, command, BROADCAST, ERROR__TRANSMITTER_BUSY);
		noError = false;
	}
	return noError;
}

template <typename Interface>
bool
xpcc::amnb::Node<Interface>::broadcast(uint8_t command, const void *payload, uint8_t payloadLength)
{
	bool noError(true);
	if (!Interface::messageTransmitted()) {
		checkErrorHandlers(Interface::getTransmittedAddress(),
						   Interface::getTransmittedCommand(),
						   Interface::getTransmittedFlags(),
						   ERROR__MESSAGE_OVERWRITTEN);
		noError = false;
	}
	if (!Interface::sendMessage(this->ownAddress, BROADCAST, command, payload, payloadLength)) {
		checkErrorHandlers(this->ownAddress, command, BROADCAST, ERROR__TRANSMITTER_BUSY);
		noError = false;
	}
	return noError;
}

template <typename Interface>
bool
xpcc::amnb::Node<Interface>::broadcast(uint8_t command)
{
	bool noError(true);
	if (!Interface::messageTransmitted()) {
		checkErrorHandlers(Interface::getTransmittedAddress(),
						   Interface::getTransmittedCommand(),
						   Interface::getTransmittedFlags(),
						   ERROR__MESSAGE_OVERWRITTEN);
		noError = false;
	}
	if (!Interface::sendMessage(this->ownAddress, BROADCAST, command, 0,0)) {
		checkErrorHandlers(this->ownAddress, command, BROADCAST, ERROR__TRANSMITTER_BUSY);
		noError = false;
	}
	return noError;
}

// ----------------------------------------------------------------------------
template <typename Interface>
bool
xpcc::amnb::Node<Interface>::isQueryCompleted()
{
	return (queryStatus != IN_PROGRESS);
}

// ----------------------------------------------------------------------------
template <typename Interface>
bool
xpcc::amnb::Node<Interface>::isSuccess()
{
	return (queryStatus == SUCCESS);
}

// ----------------------------------------------------------------------------
template <typename Interface>
uint8_t
xpcc::amnb::Node<Interface>::getErrorCode()
{
	if (queryStatus == ERROR_RESPONSE) {
		// Error code is in the first payload byte
		return Interface::getPayload()[0];
	}
	else {
		return queryStatus;
	}
}

// ----------------------------------------------------------------------------
template <typename Interface> template <typename T>
const T *
xpcc::amnb::Node<Interface>::getResponse()
{
	return reinterpret_cast<const T *>(Interface::getPayload());
}

template <typename Interface>
const void *
xpcc::amnb::Node<Interface>::getResponse()
{
	return reinterpret_cast<const void *>(Interface::getPayload());
}

// ----------------------------------------------------------------------------
template <typename Interface>
void
xpcc::amnb::Node<Interface>::update()
{	
	Interface::update();
	
	if (Interface::isMessageAvailable())
	{
		uint8_t messageAddress = Interface::getAddress();
		uint8_t messageCommand = Interface::getCommand();
		bool checkListeners = false;
		// --------------------------------------------------------------------
		if (Interface::isResponse())
		{
			// my request answer
			if ((queryStatus == IN_PROGRESS) && (messageAddress == expectedAddress))
			{
				if (Interface::isAcknowledge())
				{
					if (Interface::getPayloadLength() == expectedResponseLength) {
						queryStatus = SUCCESS;
						
						checkListeners = true;
					}
					else {
						queryStatus = ERROR_PAYLOAD;
						checkErrorHandlers(messageAddress, messageCommand, ACK, ERROR__QUERY_WRONG_PAYLOAD_LENGTH);
					}
				}
				else {
					queryStatus = ERROR_RESPONSE;
					checkErrorHandlers(messageAddress, messageCommand, NACK, ERROR__QUERY_ERROR_CODE);
				}
			}
			// other request answer
			else {
				checkListeners = true;
			}
		}
		// --------------------------------------------------------------------
		else {
			// Requests
			if (Interface::isAcknowledge())
			{
				if (messageAddress == this->ownAddress)
				{
					this->response.triggered = false;
					this->currentCommand = messageCommand;
					
					xpcc::accessor::Flash<Action> list = actionList;
					for (uint8_t i = 0; i < actionCount; ++i, ++list)
					{
						Action action(*list);
						if (this->currentCommand == action.command)
						{
							if (Interface::getPayloadLength() == action.payloadLength)
							{
								// execute callback function
								action.call(this->response, Interface::getPayload());
								
								if (!this->response.triggered) {
									this->response.error(ERROR__ACTION_NO_RESPONSE);
									checkErrorHandlers(messageAddress, messageCommand, NACK, ERROR__ACTION_NO_RESPONSE);
								}
							}
							else {
								this->response.error(ERROR__ACTION_WRONG_PAYLOAD_LENGTH);
								checkErrorHandlers(messageAddress, messageCommand, NACK, ERROR__ACTION_WRONG_PAYLOAD_LENGTH);
							}
							break;
						}
					}
				}
			}
			// Broadcasts
			else {
				checkListeners = true;
			}
			
			if (!this->response.triggered) {
				this->response.error(ERROR__ACTION_NO_ACTION);
				checkErrorHandlers(messageAddress, messageCommand, NACK, ERROR__ACTION_NO_ACTION);
			}
		}
		// --------------------------------------------------------------------
		
		if (checkListeners && (listenCount > 0))
		{	// check if we want to listen to it
			xpcc::accessor::Flash<Listener> list = listenList;
			for (uint8_t i = 0; i < listenCount; ++i, ++list)
			{
				Listener listen(*list);
				if ((messageAddress == listen.address) && (messageCommand == listen.command))
				{
					// execute callback function
					listen.call(Interface::getPayload(), Interface::getPayloadLength());
					break;
				}
			}
		}
		// finished with the message, drop it.
		Interface::dropMessage();
	}
	// my request timeout
	else if (timer.isExpired() && (queryStatus == IN_PROGRESS))
	{
		queryStatus = ERROR_TIMEOUT;
		checkErrorHandlers(Interface::getTransmittedAddress(), Interface::getTransmittedCommand(), REQUEST, ERROR__QUERY_TIMEOUT);
	}
}


template <typename Interface>
bool
xpcc::amnb::Node<Interface>::checkErrorHandlers(uint8_t address, uint8_t command, Flags type, uint8_t errorCode)
{
	if (errorHandlerCount == 0) return false;
	
	xpcc::accessor::Flash<ErrorHandler> list = errorHandlerList;
	for (uint8_t i = 0; i < errorHandlerCount; ++i, ++list)
	{
		ErrorHandler errorHandler(*list);
		if ((address == errorHandler.address) && (command == errorHandler.command))
		{
			// execute callback function
			errorHandler.call(type, errorCode);
			return true;
		}
	}
	return false;
}

// ----------------------------------------------------------------------------
template <typename Interface>
void
xpcc::amnb::Node<Interface>::send(bool acknowledge,
								  const void *payload, uint8_t payloadLength)
{
	Flags flags;
	if (acknowledge) {
		flags = xpcc::amnb::ACK;
	}
	else {
		flags = xpcc::amnb::NACK;
	}
	
	Interface::sendMessage(this->ownAddress, flags, this->currentCommand,
						   payload, payloadLength);
}
