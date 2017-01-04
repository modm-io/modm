/*
 * Copyright (c) 2009-2010, Martin Rosekeit
 * Copyright (c) 2009-2012, Fabian Greif
 * Copyright (c) 2012-2013, 2016, Niklas Hauser
 * Copyright (c) 2013, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef	XPCC_SAB__SLAVE_HPP
#define	XPCC_SAB__SLAVE_HPP

#include <cstddef>
#include <modm/architecture/driver/accessor/flash.hpp>

#include "interface.hpp"

namespace xpcc
{
	namespace sab
	{
		/**
		 * \internal
		 * \brief	Interface used to transmit data through a slave object
		 * 
		 * \ingroup	sab
		 */
		class Transmitter
		{
		public:
			virtual void
			send(bool acknowledge, const void *payload, std::size_t payloadLength) = 0;
		};
		
		/**
		 * \brief	Response object for an action call
		 * 
		 * \ingroup	sab
		 */
		class Response
		{
			template <typename I>
			friend class Slave;
			
		public:
			/**
			 * \brief	Signal an error condition
			 * 
			 * \param 	errorCode	Error code. Values below 0x20 are reserved
			 * 						for the system, feel free to use any other
			 * 						value for specific error conditions.
			 * 
			 * \see		xpcc::sab::Error
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
			xpcc_always_inline void
			send(const T& payload);
			
		protected:
			Response(Transmitter *parent);
			
			Response(const Response&);
			
			Response&
			operator = (const Response&);
			
			Transmitter *transmitter;
			bool triggered;				///< is used by abp::Slave to check that a response was send
		};
		
		/**
		 * \brief	Base-class for every object which should be used inside
		 * 			a callback.
		 * 
		 * Example:
		 * \code
		 * class Sensor : public xpcc::sab::Callable
		 * {
		 * public:
		 *     void
		 *     sendValue(xpcc::sab::Response& response)
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
		 * A complete example is available in the \c example/avr/sab folder.
		 * 
		 * \see		xpcc::sab::Slave
		 * \ingroup	sab
		 */
		struct Callable
		{
		};
		
		/**
		 * \brief	Possible Action
		 * 
		 * \see		SAB__ACTION()
		 * \ingroup	sab
		 */
		struct Action
		{
			typedef void (Callable::*Callback)(Response& response, const void *payload);
			
			inline void
			call(Response& response, const void *payload);
			
			Callable *object;
			Callback function;			//!< Method callActionback
			uint16_t payloadLength;		//!< Payload length in bytes
			uint8_t command;
		};
		
		/**
		 * \brief	SAB Slave
		 * 
		 * \code
		 * typedef xpcc::sab::Slave< xpcc::sab::Interface< xpcc::BufferedUart0 > > Slave;
		 * 
		 * FLASH_STORAGE(xpcc::sab::Action actionList[]) =
		 * {
		 *     SAB__ACTION(0x57, object, Object::method1,  0),
		 *     SAB__ACTION(0x03, object, Object::method2,  2),
		 * };
		 * 
		 * int
		 * main()
		 * {
		 *     // initialize the interface
		 *     Slave slave(0x02,
		 *             xpcc::accessor::asFlash(actionList),
		 *             sizeof(actionList) / sizeof(xpcc::sab::Action));
		 *     
		 *     while(1)
		 *     {
		 *         slave.update();
		 *     }
		 * }
		 * \endcode
		 * 
		 * A complete example is available in the \c example/sab folder.
		 * 
		 * \author	Fabian Greif
		 * \ingroup	sab
		 */
		template <typename Interface>
		class Slave : protected Transmitter
		{
		public:
			/**
			 * \brief	Initialize the slave
			 * 
			 * \param	address		Own address
			 * \param	list		List of all action callbacks, need to be
			 * 						stored in flash-memory
			 * \param	count		Number of entries in \a actionList
			 *
			 * \see		sab::xpcc::Action
			 * \see		SAB__ACTION()
			 */
			Slave(uint8_t address, xpcc::accessor::Flash<Action> list, uint8_t count);
			
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
			send(bool acknowledge, const void *payload, std::size_t payloadLength);
			
			uint8_t ownAddress;
			xpcc::accessor::Flash<Action> actionList;
			uint8_t actionCount;
			
			uint8_t currentCommand;
			Response response;
		};
	}
}

#ifdef __DOXYGEN__
	/**
	 * \brief	Define a sab::Action
	 * 
	 * Example:
	 * \code
	 * class Sensor : public xpcc::sab::Callable
	 * {
	 * public:
	 *     void
	 *     sendValue(xpcc::sab::Response& response)
	 *     {
	 *         response.send(this->value);
	 *     }
	 *     
	 *     void
	 *     doSomething(xpcc::sab::Response& response, const uint32_t* parameter)
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
	 * FLASH_STORAGE(xpcc::sab::Action actionList[]) =
	 * {
	 *     SAB__ACTION(0x57, sensor, Sensor::sendValue,   0),
	 *     SAB__ACTION(0x03, sensor, Sensor::doSomething, sizeof(uint32_t)),
	 * };
	 * \endcode
	 * 
	 * A complete example is available in the \c example/sab folder.
	 * 
	 * \param	command		Command byte
	 * \param	object		
	 * \param	function	Member function of object
	 * \param	length		Parameter size in bytes
	 * 
	 * \see		xpcc::sab::Action
	 * \ingroup	sab
	 */
	#define	SAB__ACTION(command, object, function, length)
#else
	#define	SAB__ACTION(command, object, function, length)		\
		{	static_cast<xpcc::sab::Callable *>(&object), \
			reinterpret_cast<xpcc::sab::Action::Callback>(&function), \
			length, \
			command }
#endif	// __DOXYGEN__

#include "slave_impl.hpp"

#endif	// XPCC_SAB__SLAVE_HPP
