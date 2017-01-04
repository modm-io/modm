/*
 * Copyright (c) 2011-2013, 2015-2016, Niklas Hauser
 * Copyright (c) 2011, Fabian Greif
 * Copyright (c) 2012-2013, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef	XPCC_AMNB_NODE_HPP
#define	XPCC_AMNB_NODE_HPP

#include <cstddef>
#include <modm/architecture/driver/accessor/flash.hpp>
#include <modm/processing/timer.hpp>

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
			error(uint8_t errorCode = ERROR_GENERAL_ERROR);
			
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
			xpcc_always_inline void
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
		 * \see		AMNB_ACTION()
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
		 * \see		AMNB_LISTEN()
		 * \ingroup	amnb
		 */
		struct Listener
		{
			typedef void (Callable::*Callback)(const void *payload, const uint8_t length, const uint8_t sender);
			
			inline void
			call(const void *payload, const uint8_t length, const uint8_t sender);
			
			uint8_t address;			//!< Address of transmitting node
			uint8_t command;
			Callable *object;
			Callback function;			//!< Method callActionback
		};
		
		/**
		 * \brief	Possible Error
		 * 
		 * \see		AMNB_ERROR()
		 * \ingroup	amnb
		 */
		struct ErrorHandler
		{
			typedef void (Callable::*Callback)(Flags type, const uint8_t errorCode);
			
			inline void
			call(Flags type, const uint8_t errorCode);
			
			uint8_t address;			//!< Node address of message
			uint8_t command;			//!< Command of message
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
		 *     AMNB_ACTION(0x57, object, Object::method1,  0),
		 *     AMNB_ACTION(0x03, object, Object::method2,  2),
		 * };
		 * // optional
		 * FLASH_STORAGE(xpcc::amnb::Listener listenList[]) =
		 * {
		 *     AMNB_LISTEN(0x29, 0x46,	object, Object::method)
		 * };
		 * // also optional
		 * FLASH_STORAGE(xpcc::amnb::ErrorHandler errorHandlerList[]) =
		 * {
		 *     AMNB_LISTEN(0x29, 0x46,	object, Object::method)
		 * };
		 * 
		 * int
		 * main()
		 * {
		 *     // initialize the interface
		 *     Node node(0x02,
		 *               xpcc::accessor::asFlash(actionList),
		 *               sizeof(actionList) / sizeof(xpcc::amnb::Action),
		 *				 // optional
		 *               xpcc::accessor::asFlash(listenList),
		 *               sizeof(listenList) / sizeof(xpcc::amnb::Listener),
		 *               xpcc::accessor::asFlash(errorHandlerList),
		 *               sizeof(errorHandlerList) / sizeof(xpcc::amnb::ErrorHandler));
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
			 * \brief	Initialize the node
			 * 
			 * \param	address		Own address
			 * \param	actionList	List of all action callbacks, need to be
			 * 						stored in flash-memory
			 * \param	actionCount	Number of entries in \a actionList
			 * \param	listenList	List of all listener callbacks, need to be
			 * 						stored in flash-memory
			 * \param	listenCount	Number of entries in \a listenList
			 * \param	errorHandlerList	List of all error handler callbacks,
			 *								need to be stored in flash-memory
			 * \param	errorHandlerCount	Number of entries in \a errorHandlerList
			 *
			 * \see		AMNB_ACTION()
			 * \see		AMNB_LISTEN()
			 * \see		AMNB_ERROR()
			 */
			Node(uint8_t address, xpcc::accessor::Flash<Action> actionList, uint8_t actionCount,
				 xpcc::accessor::Flash<Listener> listenList, uint8_t listenCount,
				 xpcc::accessor::Flash<ErrorHandler> errorHandlerList, uint8_t errorHandlerCount);
			
			/**
			 * \brief	Initialize the node without error handlers
			 */
			Node(uint8_t address, xpcc::accessor::Flash<Action> actionList, uint8_t actionCount,
				 xpcc::accessor::Flash<Listener> listenList, uint8_t listenCount);
			
			/**
			 * \brief	Initialize the node without listeners or error handlers
			 */
			Node(uint8_t address, xpcc::accessor::Flash<Action> actionList, uint8_t actionCount);
			
			/**
			 * \brief	Start a new query with a payload
			 * 
			 * \param slaveAddress	
			 * \param command
			 * \param payload
			 * \param responseLength	Expected payload length of the response
			 * \return \c true if no error occurred
			 */
			template <typename T>
			bool
			query(uint8_t slaveAddress, uint8_t command,
				  const T& payload, uint8_t responseLength);
			
			template <typename T>
			bool
			query(uint8_t slaveAddress, uint8_t command,
				  const void *payload, uint8_t payloadLength, uint8_t responseLength);
			
			/**
			 * \brief	Start a new query without any payload
			 *
			 * \return \c true if no error occurred
			 */
			bool
			query(uint8_t slaveAddress, uint8_t command, uint8_t responseLength);
			
			/**
			 * \brief	Start a new broadcast with a payload
			 *
			 * \param command
			 * \param payload
			 * \return \c true if no error occurred
			 */
			template <typename T>
			bool
			broadcast(uint8_t command, const T& payload);
			
			/**
			 * \brief	Start a new broadcast with a payload
			 *
			 * \param command
			 * \param payload
			 * \param payloadLength
			 * \return \c true if no error occurred
			 */
			bool
			broadcast(uint8_t command, const void *payload, uint8_t payloadLength);
			
			/**
			 * \brief	Start a new broadcast without any payload
			 *
			 * \return \c true if no error occurred
			 */
			bool
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
			
			bool
			checkErrorHandlers(uint8_t address, uint8_t command, Flags type, uint8_t errorCode);
			
			
			uint8_t ownAddress;
			xpcc::accessor::Flash<Action> actionList;
			uint8_t actionCount;
			xpcc::accessor::Flash<Listener> listenList;
			uint8_t listenCount;
			xpcc::accessor::Flash<ErrorHandler> errorHandlerList;
			uint8_t errorHandlerCount;
			
			uint8_t currentCommand;
			Response response;
			
			enum QueryStatus
			{
				IN_PROGRESS,			///< Query in progress
				SUCCESS,				///< Response successfully received
				ERROR_RESPONSE = 0x40,	///< Error in the received message
				ERROR_TIMEOUT = 0x41,	///< No message received within the timeout window
				ERROR_PAYLOAD = 0x42,	///< Wrong payload size
			};
			
			QueryStatus queryStatus;
			uint8_t expectedAddress;
			uint8_t expectedResponseLength;
			xpcc::ShortTimeout timer;
			
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
	 *     AMNB_ACTION(0x57, sensor, Sensor::sendValue,   0),
	 *     AMNB_ACTION(0x03, sensor, Sensor::doSomething, sizeof(uint32_t)),
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
	#define	AMNB_ACTION(command, object, function, length)
#else
	#define	AMNB_ACTION(command, object, function, length)		\
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
	 * class ListenToNodes : public xpcc::amnb::Callable
	 * {
	 * public:
	 *     void
	 *     listenToCommand(uint8_t *payload, const uint8_t length, uint8_t sender)
	 *     {
	 *         // ... do something useful ...
	 *         
	 *     }
	 *
	 *     void
	 *     listenToCommandWithOutCaringForSender(uint8_t *payload, const uint8_t length)
	 *     {
	 *         // ... do something useful ...
	 *         
	 *     }
	 *     
	 *     // ...
	 * };
	 * 
	 * ListenToNodes listen;
	 * 
	 * FLASH_STORAGE(xpcc::amnb::Listener listenList[]) =
	 * {
	 *     AMNB_LISTEN(0x46, 0x03, listen, ListenToNodes::listenToCommand),
	 * };
	 * \endcode
	 * 
	 * A complete example is available in the \c example/amnb folder.
	 * 
	 * \param	address		Address of the transmitting node
	 * \param	command		Command byte
	 * \param	object		
	 * \param	function	Member function of object
	 * 
	 * \see		xpcc::amnb::Listener
	 * \ingroup	amnb
	 */
	#define	AMNB_LISTEN(address, command, object, function)
#else
	#define	AMNB_LISTEN(address, command, object, function)	\
		{	address, \
			command, \
			static_cast<xpcc::amnb::Callable *>(&object), \
			reinterpret_cast<xpcc::amnb::Listener::Callback>(&function) }
#endif	// __DOXYGEN__


#ifdef __DOXYGEN__
	/**
	 * \brief	Define a amnb::ErrorHandler
	 * 
	 * Example:
	 * \code
	 * class handleErrors : public xpcc::amnb::Callable
	 * {
	 * public:
	 *     void
	 *     errorForCommand(xpcc::amnb::Flags type, const uint8_t errorCode)
	 *     {
	 *         // ... do something useful with that information ...
	 *         
	 *     }
	 *     
	 *     // ...
	 * };
	 * 
	 * handleErrors errorhandler;
	 * 
	 * FLASH_STORAGE(xpcc::amnb::Listener listenList[]) =
	 * {
	 *     AMNB_LISTEN(0x37, 0x57, errorhandler, handleErrors::errorForCommand),
	 * };
	 * \endcode
	 * 
	 * A complete example is available in the \c example/amnb folder.
	 * 
	 * \param	address		Node address of message
	 * \param	command		Command of message
	 * \param	object		
	 * \param	function	Member function of object
	 * 
	 * \see		xpcc::amnb::ErrorHandler
	 * \ingroup	amnb
	 */
	#define	AMNB_ERROR(address, command, object, function)
#else
	#define	AMNB_ERROR(address, command, object, function)	\
		{	address, \
			command, \
			static_cast<xpcc::amnb::Callable *>(&object), \
			reinterpret_cast<xpcc::amnb::ErrorHandler::Callback>(&function) }
#endif	// __DOXYGEN__

#include "node_impl.hpp"

#endif	// XPCC_AMNB_NODE_HPP
