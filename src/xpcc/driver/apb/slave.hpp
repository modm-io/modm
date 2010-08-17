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

#ifndef	XPCC_APB__SLAVE_HPP
#define	XPCC_APB__SLAVE_HPP

#include <cstddef>
#include <xpcc/architecture/driver/accessor/flash.hpp>

#include "interface.hpp"

namespace xpcc
{
	namespace apb
	{
		/**
		 * \internal
		 * \brief	Interface used to transmit data through a slave object
		 * \ingroup	apb
		 */
		class Transmitter
		{
		public:
			virtual void
			send(bool acknowledge, const void *payload, uint8_t payloadLength) = 0;
		};
		
		/**
		 * \brief	Response object for an action call
		 * \ingroup	apb
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
			 * \see		xpcc::apb::Error
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
			
			bool triggered;
			Transmitter *transmitter;
		};
		
		/**
		 * \brief	Base-class for every object which should be used inside
		 * 			a callback.
		 * 
		 * Example:
		 * \code
		 * class Sensor : public xpcc::apb::Callable
		 * {
		 * public:
		 *     void
		 *     sendValue(xpcc::apb::Response& response)
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
		 * A complete example is available in the \c example/apb folder.
		 * 
		 * \see		xpcc::apb::Slave
		 * \ingroup	apb
		 */
		struct Callable
		{
		};
		
		/**
		 * \brief	Possible Action
		 * 
		 * Example:
		 * \code
		 * class Sensor : public xpcc::apb::Callable
		 * {
		 * public:
		 *     void
		 *     sendValue(xpcc::apb::Response& response)
		 *     {
		 *         response.send(this->value);
		 *     }
		 *     
		 *     void
		 *     doSomething(xpcc::apb::Response& response, const uint32_t* parameter)
		 *     {
		 *         // ... do something usefull ...
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
		 * FLASH(xpcc::apb::Action actionList[]) =
		 * {
		 *     APB__ACTION(0x57, sensor, Sensor::sendValue,   0),
		 *     APB__ACTION(0x03, sensor, Sensor::doSomething, sizeof(uint32_t)),
		 * };
		 * \endcode
		 * 
		 * A complete example is available in the \c example/apb folder.
		 * 
		 * \see		APB__ACTION()
		 * \ingroup	apb
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
		 * \brief	APB Slave
		 * 
		 * \code
		 * typedef xpcc::apb::Slave< xpcc::apb::Interface< xpcc::BufferedUart0 > > Slave;
		 * 
		 * FLASH(xpcc::apb::Action actionList[]) =
		 * {
		 *     APB__ACTION(0x57, object, Object::method1,  0),
		 *     APB__ACTION(0x03, object, Object::method2,  2),
		 * };
		 * 
		 * int
		 * main()
		 * {
		 *     // initialize the interface
		 *     Slave slave(0x02,
		 *             xpcc::accessor::asFlash(actionList),
		 *             sizeof(actionList) / sizeof(xpcc::apb::Action));
		 *     
		 *     while(1)
		 *     {
		 *         slave.update();
		 *     }
		 * }
		 * \endcode
		 * 
		 * A complete example is available in the \c example/apb folder.
		 * 
		 * \ingroup	apb
		 * \author	Fabian Greif
		 */
		template <typename Interface>
		class Slave : protected Transmitter
		{
		public:
			/**
			 * \brief	Initialize the slave
			 * 
			 * \param	address		Own address
			 * \param	actionList	List of all action callbacks, need to be
			 * 						stored in flash-memory
			 * \param	actionCount	Number of entries in \a actionList
			 *
			 * \see		apb::xpcc::Action
			 * \see		APB__ACTION()
			 */
			Slave(uint8_t address, xpcc::accessor::Flash<Action> actionList, uint8_t actionCount);
			
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
			
			uint8_t currentCommand;
			Response response;
		};
	}
}

#ifdef __DOXYGEN__
	/**
	 * \brief	Define a apb::Action
	 * 
	 * \param	command		Command byte
	 * \param	object		
	 * \param	function	Member function of object
	 * \param	length		Parameter size in bytes
	 * 
	 * \see		xpcc::apb::Action
	 * \ingroup	apb
	 */
	#define	APB__ACTION(command, object, function, length)
#else
	#define	APB__ACTION(command, object, function, length)		\
		{	command, \
			length, \
			static_cast<xpcc::apb::Callable *>(&object), \
			reinterpret_cast<xpcc::apb::Action::Callback>(&function) }
#endif	// __DOXYGEN__

#include "slave_impl.hpp"

#endif	// XPCC_APB__SLAVE_HPP
