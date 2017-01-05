/*
 * Copyright (c) 2014-2015, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef	MODM_TOKEN_RING_NODE_HPP
#define	MODM_TOKEN_RING_NODE_HPP

#include <cstddef>
#include <modm/architecture/driver/accessor/flash.hpp>
#include <modm/processing/timer.hpp>
#include <stdint.h>

#include "interface.hpp"

namespace modm
{
	namespace rpr
	{
		/**
		 * \internal
		 * \brief	Interface used to transmit data through a slave object
		 * 
		 * \ingroup	rpr
		 */
		class Transmitter
		{
		public:
			virtual void
			unicastMessage(uint16_t destination,
						   uint8_t command,
						   const void *payload,
						   std::size_t payloadLength) = 0;
			
			virtual void
			multicastMessage(uint16_t destination,
							 uint8_t command,
							 const void *payload,
							 std::size_t payloadLength) = 0;
			
			virtual void
			broadcastMessage(uint8_t command,
							 const void *payload,
							 std::size_t payloadLength) = 0;
		};
		
		struct Callable
		{
		};
		
		/**
		 * \brief	Possible Listener
		 * 
		 * \see		RPR_LISTEN()
		 * \ingroup	rpr
		 */
		struct Listener
		{
			typedef void (Callable::*Callback)(Transmitter& node, Message *message);
			
			inline void
			call(Transmitter& node, Message *message);
			
			MessageType type;
			uint16_t source;
			uint8_t command;
			Callable *object;
			Callback function;			//!< Method callActionback
		};
		
		/**
		 * \brief	Possible Error
		 * 
		 * \see		RPR_ERROR()
		 * \ingroup	rpr
		 */
		struct Error
		{
			typedef void (Callable::*Callback)(Transmitter& node, ErrorMessage *error);
			
			inline void
			call(Transmitter& node, ErrorMessage *error);
			
			uint8_t command;			//!< Command of message
			Callable *object;
			Callback function;			//!< Method callActionback
		};
		
		/**
		 * \brief	Token Ring Node
		 * 
		 * \author	Niklas Hauser
		 * \ingroup	rpr
		 */
		template <typename Interface>
		class Node : protected Transmitter
		{
		public:
			/**
			 * \brief	Initialize the node
			 *
			 * \param	listenList	List of all listener callbacks, need to be
			 * 						stored in flash-memory
			 * \param	listenCount	Number of entries in \a listenList
			 * \param	errorHandlerList	List of all error handler callbacks,
			 *								need to be stored in flash-memory
			 * \param	errorHandlerCount	Number of entries in \a errorHandlerList
			 *
			 * \see		RPR_LISTEN()
			 * \see		RPR_ERROR()
			 */
			Node(modm::accessor::Flash<Listener> listenerCallbackList,
				 uint8_t listenerCallbackCount,
				 modm::accessor::Flash<Error> errorCallbackList,
				 uint8_t errorCallbackCount);
			
			/**
			 * \brief	Initialize the node without error handlers
			 */
			Node(modm::accessor::Flash<Listener> listenerCallbackList,
				 uint8_t listenerCallbackCount);
			
			
			inline void
			setAddress(uint16_t address, uint16_t groupAddress=0x7fff);
			
			/**
			 * \brief	Send a message to one node
			 *
			 * \param	destination		14bit destination address
			 * \param	command
			 * \param	payload
			 * \param	payloadLength
			 * \return \c true if no error occurred
			 */
			void
			unicastMessage(uint16_t destination,
						   uint8_t command,
						   const void *payload,
						   std::size_t payloadLength);
			
			void
			multicastMessage(uint16_t destination,
							 uint8_t command,
							 const void *payload,
							 std::size_t payloadLength);
			
			/**
			 * \brief	Start a new broadcast with a payload
			 *
			 * \param command
			 * \param payload
			 * \param payloadLength
			 * \return \c true if no error occurred
			 */
			void
			broadcastMessage(uint8_t command,
							 const void *payload,
							 std::size_t payloadLength);
			
			/**
			 * \brief	Receive and process messages
			 * 
			 * This method will decode the incoming messages and call the
			 * corresponding callback methods from the listener list. It must
			 * be called periodically, best in every main loop cycle.
			 */
			void
			update();
			
		protected:
			bool
			checkErrorCallbacks(ErrorMessage *errorMessage);
			
			bool
			checkListenerCallbacks(Message *message);
			
			modm::accessor::Flash<Listener> listenerCallbackList;
			uint8_t listenerCallbackCount;
			modm::accessor::Flash<Error> errorCallbackList;
			uint8_t errorCallbackCount;
		};
	}
}

#ifdef __DOXYGEN__
	/**
	 * \brief	Define a Listener
	 * 
	 * \param	type		Type of message (uni-, multi-, broadcast, funtional or any)
	 * \param	source		address of transmitter, use 0xffff for any
	 * \param	command		Command byte, use 0xff for any
	 * \param	object		object of class to be called
	 * \param	function	Member function of object
	 * 
	 * \see		modm::rpr::Listener
	 * \ingroup	rpr
	 */
	#define	RPR_LISTEN(type, source, command, object, function)
#else
	#define	RPR_LISTEN(type, source, command, object, function)	\
		{	type, \
			source, \
			command, \
			static_cast<modm::rpr::Callable *>(&object), \
			reinterpret_cast<modm::rpr::Listener::Callback>(&function) }
#endif	// __DOXYGEN__


#ifdef __DOXYGEN__
	/**
	 * \brief	Define a ErrorHandler
	 * 
	 * \param	address		Node address of message
	 * \param	command		Command of message
	 * \param	object		
	 * \param	function	Member function of object
	 * 
	 * \see		modm::rpr::ErrorHandler
	 * \ingroup	rpr
	 */
	#define	RPR_ERROR(address, command, object, function)
#else
	#define	RPR_ERROR(address, command, object, function)	\
		{	address, \
			command, \
			static_cast<modm::rpr::Callable *>(&object), \
			reinterpret_cast<modm::rpr::ErrorHandler::Callback>(&function) }
#endif	// __DOXYGEN__

#include "node_impl.hpp"

#endif	// MODM_TOKEN_RING_NODE_HPP
