/*

 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef	MODM_TOKEN_RING_NODE_HPP
#	error	"Don't include this file directly, use 'node.hpp' instead!"
#endif

// ----------------------------------------------------------------------------
inline void
modm::rpr::Listener::call(Transmitter& node, Message *message)
{
	// redirect call to the actual object
	(object->*function)(node, message);
}

inline void
modm::rpr::Error::call(Transmitter& node, ErrorMessage *error)
{
	// redirect call to the actual object
	(object->*function)(node, error);
}

// ----------------------------------------------------------------------------
template <typename Interface>
modm::rpr::Node<Interface>::Node(modm::accessor::Flash<Listener> listenerCallbackList,
								   uint8_t listenerCallbackCount,
								   modm::accessor::Flash<Error> errorCallbackList,
								   uint8_t errorCallbackCount) :
listenerCallbackList(listenerCallbackList), listenerCallbackCount(listenerCallbackCount),
errorCallbackList(errorCallbackList), errorCallbackCount(errorCallbackCount)
{
}

template <typename Interface>
modm::rpr::Node<Interface>::Node(modm::accessor::Flash<Listener> listenerCallbackList,
								   uint8_t listenerCallbackCount) :
listenerCallbackList(listenerCallbackList), listenerCallbackCount(listenerCallbackCount),
errorCallbackCount(0)
{
}

// ----------------------------------------------------------------------------
template <typename Interface>
void
modm::rpr::Node<Interface>::setAddress(uint16_t address, uint16_t groupAddress)
{
	Interface::initialize(address, groupAddress);
}

// ----------------------------------------------------------------------------
template <typename Interface>
void
modm::rpr::Node<Interface>::unicastMessage(uint16_t destination,
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
modm::rpr::Node<Interface>::multicastMessage(uint16_t destination,
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
modm::rpr::Node<Interface>::broadcastMessage(uint8_t command,
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
modm::rpr::Node<Interface>::update()
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
modm::rpr::Node<Interface>::checkErrorCallbacks(ErrorMessage *errorMessage)
{
	if (errorCallbackCount == 0)
		return false;
	
	modm::accessor::Flash<Error> list = errorCallbackList;
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
modm::rpr::Node<Interface>::checkListenerCallbacks(Message *message)
{
	if (listenerCallbackCount == 0)
		return false;
	
	modm::accessor::Flash<Listener> list = listenerCallbackList;
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
