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
 */
// ----------------------------------------------------------------------------

#ifndef	XPCC_TOKEN_RING__NODE_HPP
#	error	"Don't include this file directly, use 'node.hpp' instead!"
#endif

// ----------------------------------------------------------------------------
inline void
xpcc::rpr::Listener::call(Transmitter& node, Message *message)
{
	// redirect call to the actual object
	(object->*function)(node, message);
}

inline void
xpcc::rpr::Error::call(Transmitter& node, ErrorMessage *error)
{
	// redirect call to the actual object
	(object->*function)(node, error);
}

// ----------------------------------------------------------------------------
template <typename Interface>
xpcc::rpr::Node<Interface>::Node(xpcc::accessor::Flash<Listener> listenerCallbackList,
								   uint8_t listenerCallbackCount,
								   xpcc::accessor::Flash<Error> errorCallbackList,
								   uint8_t errorCallbackCount) :
listenerCallbackList(listenerCallbackList), listenerCallbackCount(listenerCallbackCount),
errorCallbackList(errorCallbackList), errorCallbackCount(errorCallbackCount)
{
}

template <typename Interface>
xpcc::rpr::Node<Interface>::Node(xpcc::accessor::Flash<Listener> listenerCallbackList,
								   uint8_t listenerCallbackCount) :
listenerCallbackList(listenerCallbackList), listenerCallbackCount(listenerCallbackCount),
errorCallbackCount(0)
{
}

// ----------------------------------------------------------------------------
template <typename Interface>
void
xpcc::rpr::Node<Interface>::setAddress(uint16_t address, uint16_t groupAddress)
{
	Interface::initialize(address, groupAddress);
}

// ----------------------------------------------------------------------------
template <typename Interface>
void
xpcc::rpr::Node<Interface>::unicastMessage(uint16_t destination,
											 uint8_t command,
											 const void *payload,
											 std::size_t payloadLength)
{
	Interface::sendMessage(destination,
						   MESSAGE_TYPE_UNICAST,
						   command,
						   payload,
						   payloadLength);
}

template <typename Interface>
void
xpcc::rpr::Node<Interface>::multicastMessage(uint16_t destination,
											   uint8_t command,
											   const void *payload,
											   std::size_t payloadLength)
{
	Interface::sendMessage(destination,
						   MESSAGE_TYPE_MULTICAST,
						   command,
						   payload,
						   payloadLength);
}

template <typename Interface>
void
xpcc::rpr::Node<Interface>::broadcastMessage(uint8_t command,
											   const void *payload,
											   std::size_t payloadLength)
{
	Interface::sendMessage(0,
						   MESSAGE_TYPE_BROADCAST,
						   command,
						   payload,
						   payloadLength);
}

// ----------------------------------------------------------------------------
template <typename Interface>
void
xpcc::rpr::Node<Interface>::update()
{	
	Interface::update();
	
	Message *rx;
	while ( (rx = Interface::getReceivedMessage()) )
	{
		checkListenerCallbacks(rx);
		Interface::dropReceivedMessage();
	}
}

// ----------------------------------------------------------------------------
template <typename Interface>
bool
xpcc::rpr::Node<Interface>::checkErrorCallbacks(ErrorMessage *errorMessage)
{
	if (errorCallbackCount == 0)
		return false;
	
	xpcc::accessor::Flash<Error> list = errorCallbackList;
	for (uint_fast8_t i = 0; i < errorCallbackCount; ++i, ++list)
	{
		Error handler(*list);
//		if ((address == errorHandler.address) && (command == errorHandler.command))
		{
			// execute callback function
			handler.call(*this, errorMessage);
			return true;
		}
	}
	return false;
}

template <typename Interface>
bool
xpcc::rpr::Node<Interface>::checkListenerCallbacks(Message *message)
{
	if (listenerCallbackCount == 0)
		return false;
	
	xpcc::accessor::Flash<Listener> list = listenerCallbackList;
	for (uint_fast8_t i = 0; i < listenerCallbackCount; ++i, ++list)
	{
		Listener handler(*list);
		
		if (handler.type == MESSAGE_TYPE_ANY || handler.type == message->type)
		{
			if (handler.source == ADDRESS_ANY || handler.source == message->source)
			{
				if (handler.command == COMMAND_ANY || handler.command == message->command)
				{
					// execute callback function
					handler.call(*this, message);
					return true;
				}
			}
		}
	}
	return false;
}
