// coding: utf-8
// ----------------------------------------------------------------------------
/* Copyright (c) 2012, Roboterclub Aachen e.V.
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
#define	XPCC_TOKEN_RING__NODE_HPP

#include <cstddef>
#include <xpcc/architecture/driver/accessor/flash.hpp>
#include <xpcc/workflow/timeout.hpp>
#include <stdint.h>

#include "interface.hpp"

namespace xpcc
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
		 * \see		RPR__LISTEN()
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
		 * \see		RPR__ERROR()
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
			 * \see		RPR__LISTEN()
			 * \see		RPR__ERROR()
			 */
			Node(xpcc::accessor::Flash<Listener> listenerCallbackList,
				 uint8_t listenerCallbackCount,
				 xpcc::accessor::Flash<Error> errorCallbackList,
				 uint8_t errorCallbackCount);
			
			/**
			 * \brief	Initialize the node without error handlers
			 */
			Node(xpcc::accessor::Flash<Listener> listenerCallbackList,
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
			
			xpcc::accessor::Flash<Listener> listenerCallbackList;
			uint8_t listenerCallbackCount;
			xpcc::accessor::Flash<Error> errorCallbackList;
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
	 * \see		xpcc::rpr::Listener
	 * \ingroup	rpr
	 */
	#define	RPR__LISTEN(type, source, command, object, function)
#else
	#define	RPR__LISTEN(type, source, command, object, function)	\
		{	type, \
			source, \
			command, \
			static_cast<xpcc::rpr::Callable *>(&object), \
			reinterpret_cast<xpcc::rpr::Listener::Callback>(&function) }
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
	 * \see		xpcc::rpr::ErrorHandler
	 * \ingroup	rpr
	 */
	#define	RPR__ERROR(address, command, object, function)
#else
	#define	RPR__ERROR(address, command, object, function)	\
		{	address, \
			command, \
			static_cast<xpcc::rpr::Callable *>(&object), \
			reinterpret_cast<xpcc::rpr::ErrorHandler::Callback>(&function) }
#endif	// __DOXYGEN__

#include "node_impl.hpp"

#endif	// XPCC_TOKEN_RING__NODE_HPP
