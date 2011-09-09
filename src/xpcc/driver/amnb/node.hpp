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
#define	XPCC_AMNB__NODE_HPP

#include <cstddef>
#include <xpcc/architecture/driver/accessor/flash.hpp>
#include <xpcc/workflow/timeout.hpp>

#include "interface.hpp"

namespace xpcc
{
	namespace amnb
	{
		/**
		 * \internal
		 * \brief	Interface used to transmit data through a slave object
		 * 
		 * \ingroup	amnb
		 */
		class Transmitter
		{
		public:
			virtual void
			send(bool acknowledge, const void *payload, uint8_t payloadLength) = 0;
		};
		
		/**
		 * \brief	Response object for an action call
		 * 
		 * \ingroup	amnb
		 */
		class Response
		{
			template <typename I>
			friend class Node;
			
		public:
			/**
			 * \brief	Signal an error condition
			 * 
			 * \param 	errorCode	Error code. Values below 0x20 are reserved
			 * 						for the system, feel free to use any other
			 * 						value for specific error conditions.
			 * 
			 * \see		xpcc::amnb::Error
			 */
			void
			error(uint8_t errorCode = ERROR__GENERAL_ERROR);
			
			/**
			 * \brief	Send a response without any data
			 */
			void
			send();
			
			/**
			 * \brief	Send a response with an attached payload
			 * 
			 * \param	payload		Pointer to the payload
			 * \param	length		Number of bytes
			 */
			void
			send(const void *payload, std::size_t length);
			
			/**
			 * \brief	Send a response with an attached payload
			 */
			template <typename T>
			ALWAYS_INLINE void
			send(const T& payload);
			
		protected:
			Response(Transmitter *parent);
			
			Response(const Response&);
			
			Response&
			operator = (const Response&);
			
			Transmitter *transmitter;
			bool triggered;				///< is used by amnp::Node to check that a response was send
		};
		
		/**
		 * \brief	Base-class for every object which should be used inside
		 * 			a callback.
		 * 
		 * Example:
		 * \code
		 * class Sensor : public xpcc::amnb::Callable
		 * {
		 * public:
		 *     void
		 *     sendValue(xpcc::amnb::Response& response)
		 *     {
		 *         response.send(this->value);
		 *     }
		 *     
		 *     // ...
		 *     
		 * private:
		 *     int8_t value;
		 * };
		 * \endcode
		 * 
		 * A complete example is available in the \c example/amnb folder.
		 * 
		 * \see		xpcc::amnb::Node
		 * \ingroup	amnb
		 */
		struct Callable
		{
		};
		
		/**
		 * \brief	Possible Action
		 * 
		 * \see		AMNB__ACTION()
		 * \ingroup	amnb
		 */
		struct Action
		{
			typedef void (Callable::*Callback)(Response& response, const void *payload);
			
			inline void
			call(Response& response, const void *payload);
			
			uint8_t command;
			uint8_t payloadLength;		//!< Payload length in bytes
			Callable *object;
			Callback function;			//!< Method callActionback
		};
		
		/**
		 * \brief	Possible Listener
		 * 
		 * \see		AMNB__LISTEN()
		 * \ingroup	amnb
		 */
		struct Listener
		{
			typedef void (Callable::*Callback)(const void *payload);
			
			inline void
			call(const void *payload);
			
			uint8_t address;			//!< Address of transmitting chip
			uint8_t command;
			Callable *object;
			Callback function;			//!< Method callActionback
		};
		
		/**
		 * \brief	AMNB Node
		 * 
		 * \code
		 * typedef xpcc::amnb::Node< xpcc::amnb::Interface< xpcc::BufferedUart0 > > Node;
		 * 
		 * FLASH_STORAGE(xpcc::amnb::Action actionList[]) =
		 * {
		 *     AMNB__ACTION(0x57, object, Object::method1,  0),
		 *     AMNB__ACTION(0x03, object, Object::method2,  2),
		 * };
		 * 
		 * FLASH_STORAGE(xpcc::amnb::Listener listenList[]) =
		 * {
		 *     AMNB__LISTEN(0x29, 0x46,	object, Object::method)
		 * };
		 * 
		 * int
		 * main()
		 * {
		 *     // initialize the interface
		 *     Node node(0x02,
		 *               xpcc::accessor::asFlash(actionList),
		 *               sizeof(actionList) / sizeof(xpcc::amnb::Action),
		 *               xpcc::accessor::asFlash(listenList),
		 *               sizeof(listenList) / sizeof(xpcc::amnb::Listener));
		 *     
		 *     while(1)
		 *     {
		 *         node.update();
		 *     }
		 * }
		 * \endcode
		 * 
		 * A complete example is available in the \c example/amnb folder.
		 * 
		 * \author	Fabian Greif, Niklas Hauser
		 * \ingroup	amnb
		 */
		template <typename Interface>
		class Node : protected Transmitter
		{
		public:
			/**
			 * \brief	Initialize the slave
			 * 
			 * \param	address		Own address
			 * \param	actionList	List of all action callbacks, need to be
			 * 						stored in flash-memory
			 * \param	actionCount	Number of entries in \a actionList
			 * \param	listenList	List of all listener callbacks, need to be
			 * 						stored in flash-memory
			 * \param	listenCount	Number of entries in \a listenList
			 *
			 * \see		amnb::xpcc::Action
			 * \see		AMNB__ACTION()
			 */
			Node(uint8_t address, xpcc::accessor::Flash<Action> actionList, uint8_t actionCount,
				 xpcc::accessor::Flash<Listener> listenList, uint8_t listenCount);
			
			/**
			 * \brief	Start a new query with a payload
			 * 
			 * \param slaveAddress	
			 * \param command		
			 * \param payload		
			 * \param responseLength	Expected payload length of the response
			 */
			template <typename T>
			void
			query(uint8_t slaveAddress, uint8_t command,
				  const T& payload, uint8_t responseLength);
			
			/**
			 * \brief	Start a new query without any payload
			 * 
			 */
			void
			query(uint8_t slaveAddress, uint8_t command, uint8_t responseLength);
			
			/**
			 * \brief	Start a new broadcast with a payload
			 * 
			 * \param command		
			 * \param payload
			 */
			template <typename T>
			void
			broadcast(uint8_t command, const T& payload);
			
			/**
			 * \brief	Start a new broadcast with a payload
			 * 
			 * \param command
			 * \param payload
			 * \param payloadLength
			 */
			void
			broadcast(uint8_t command, const void *payload, uint8_t payloadLength);
			
			/**
			 * \brief	Start a new query without any payload
			 * 
			 */
			void
			broadcast(uint8_t command);
			
			bool
			isQueryCompleted();
			
			/**
			 * \brief	Check if the last query could be performed successful
			 * 
			 * Only valid if isQueryCompleted() returns \c true.
			 * 
			 * \return	\c true if the query was successful. Use getResponse() to
			 * 			access the result.
			 */
			bool
			isSuccess();
			
			/**
			 * \brief	Check error code
			 * 
			 * Only valid if isQueryCompleted() returns \c true while
			 * isSuccess() returns \c false.
			 * 
			 * \return	Error code
			 * \see		xpcc::amnb::Error
			 */
			uint8_t
			getErrorCode();
			
			
			template <typename T>
			inline const T *
			getResponse();
			
			inline const void *
			getResponse();
			
			/**
			 * \brief	Receive and process messages
			 * 
			 * This method will decode the incoming messages and call the
			 * corresponding callback methods from the action list. It must
			 * be called periodically, best in every main loop cycle.
			 */
			void
			update();
			
		protected:
			void
			send(bool acknowledge, const void *payload, uint8_t payloadLength);
			
			uint8_t ownAddress;
			xpcc::accessor::Flash<Action> actionList;
			uint8_t actionCount;
			xpcc::accessor::Flash<Listener> listenList;
			uint8_t listenCount;
			
			uint8_t currentCommand;
			Response response;
			
			enum QueryStatus
			{
				IN_PROGRESS,			///< Query in progress
				SUCCESS,				///< Response sucessfully received
				ERROR_RESPONSE = 0x40,	///< Error in the received message
				ERROR_TIMEOUT = 0x41,	///< No message received within the timeout window
				ERROR_PAYLOAD = 0x42,	///< Wrong payload size
			};
			
			QueryStatus queryStatus;
			uint8_t expectedAddress;
			uint8_t expectedResponseLength;
			xpcc::Timeout<> timer;
			
			static const uint8_t timeout = 10;		///< timeout value in milliseconds
		};
	}
}

#ifdef __DOXYGEN__
	/**
	 * \brief	Define a amnb::Action
	 * 
	 * Example:
	 * \code
	 * class Sensor : public xpcc::amnb::Callable
	 * {
	 * public:
	 *     void
	 *     sendValue(xpcc::amnb::Response& response)
	 *     {
	 *         response.send(this->value);
	 *     }
	 *     
	 *     void
	 *     doSomething(xpcc::amnb::Response& response, const uint32_t* parameter)
	 *     {
	 *         // ... do something useful ...
	 *         
	 *         response.send();
	 *     }
	 *     
	 *     // ...
	 *     
	 * private:
	 *     int8_t value;
	 * };
	 * 
	 * Sensor sensor;
	 * 
	 * FLASH_STORAGE(xpcc::amnb::Action actionList[]) =
	 * {
	 *     AMNB__ACTION(0x57, sensor, Sensor::sendValue,   0),
	 *     AMNB__ACTION(0x03, sensor, Sensor::doSomething, sizeof(uint32_t)),
	 * };
	 * \endcode
	 * 
	 * A complete example is available in the \c example/amnb folder.
	 * 
	 * \param	command		Command byte
	 * \param	object		
	 * \param	function	Member function of object
	 * \param	length		Parameter size in bytes
	 * 
	 * \see		xpcc::amnb::Action
	 * \ingroup	amnb
	 */
	#define	AMNB__ACTION(command, object, function, length)
#else
	#define	AMNB__ACTION(command, object, function, length)		\
		{	command, \
			length, \
			static_cast<xpcc::amnb::Callable *>(&object), \
			reinterpret_cast<xpcc::amnb::Action::Callback>(&function) }
#endif	// __DOXYGEN__



#ifdef __DOXYGEN__
	/**
	 * \brief	Define a amnb::Listener
	 * 
	 * Example:
	 * \code
	 * class Sensor : public xpcc::amnb::Callable
	 * {
	 * public:
	 *     void
	 *     doSomething(const uint32_t* parameter)
	 *     {
	 *         // ... do something useful ...
	 *         
	 *     }
	 *     
	 *     // ...
	 *     
	 * private:
	 *     int8_t value;
	 * };
	 * 
	 * Sensor sensor;
	 * 
	 * FLASH_STORAGE(xpcc::amnb::Listener listenList[]) =
	 * {
	 *     AMNB__LISTEN(0x37, 0x57, sensor, Sensor::sendValue),
	 *     AMNB__LISTEN(0x46, 0x03, sensor, Sensor::doSomething),
	 * };
	 * \endcode
	 * 
	 * A complete example is available in the \c example/amnb folder.
	 * 
	 * \param	address		Address of the transmitting chip
	 * \param	command		Command byte
	 * \param	object		
	 * \param	function	Member function of object
	 * 
	 * \see		xpcc::amnb::Action
	 * \ingroup	amnb
	 */
	#define	AMNB__LISTEN(address, command, object, function)
#else
	#define	AMNB__LISTEN(address, command, object, function)	\
		{	address, \
			command, \
			static_cast<xpcc::amnb::Callable *>(&object), \
			reinterpret_cast<xpcc::amnb::Listener::Callback>(&function) }
#endif	// __DOXYGEN__

#include "node_impl.hpp"

#endif	// XPCC_AMNB__NODE_HPP
