// coding: utf-8
// ----------------------------------------------------------------------------
/* Copyright (c) 2010, Roboterclub Aachen e.V.
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
 * $Id: interconnect_interface.hpp 607 2011-09-13 19:51:03Z dergraaf $
 */
// ----------------------------------------------------------------------------

#ifndef XPCC__INTERCONNECT_INTERFACE_HPP
#define XPCC__INTERCONNECT_INTERFACE_HPP

#include <xpcc/architecture/utils.hpp>
#include <xpcc/architecture/driver.hpp>

namespace xpcc
{
	/**
	 * \brief	Empty struct which can be used as a placeholder
	 */
	struct Empty
	{
	};
	
	/**
	 * \brief	Type safe interface for the interconnect link
	 * 
	 * The connected of the Transmit and Receive structs (T & R) are updated
	 * should be updated periodically. Simple sensor data can therefore directly
	 * stored within the structs.
	 * 
	 * More care needs to be taken when it comes to commands. The simplest and
	 * most reliable way is to transmit command with two counters. One controlled
	 * by the first node and one controlled by the second node.
	 * 
	 * When a command should be executed the counter is incremented by one.
	 * Parameters may also be stored in the transmit struct. The other node
	 * keeps a copy of the counter value and compares it to the received counter
	 * on every update.
	 * When the counter changes, it extecutes the corresponding command und
	 * stores the new counter value. This way it is made sure that the command
	 * is executed only once. To signal that the command is finished the
	 * counter in the other direction is used. The node sets the value of that
	 * counter to value of the counter received by the other node.
	 * 
	 * The transmitting node must now only check if the received counter equals
	 * the transmitted counter to know when the command was successfully executed.
	 * 
	 * <h2>Example</h2>
	 * 
	 * Definition of the two counters and the paramter field:
	 * \include interconnect_struct.cpp
	 * 
	 * Master:
	 * \include interconnect_master.cpp
	 * 
	 * Slave:
	 * \include interconnect_slave.cpp
	 * 
	 * <h2>FAQ:</h2>
	 * 
	 * You get an error message during linking saying something like:
	 * \code
	 * undefined reference to `xpcc::InterconnectLink::sizeReceiveBuffer'
	 * undefined reference to `xpcc::InterconnectLink::sizeTransmitBuffer'
	 * ...
	 * \endcode
	 * 
	 * Check that all necessary defines are set in the project file. This may
	 * either be CPU_BOARD2_MASTER/CPU_BOARD2_SLAVE when compiling for the
	 * CPU Board2 or the following:
	 * - INTERCONNECT_USART_PORT
	 * - INTERCONNECT_USART_ID
	 * - INTERCONNECT_PIN_OUTPUT
	 * - INTERCONNECT_PIN_INPUT
	 * 
	 * INTERCONNECT_USART_PORT and INTERCONNECT_USART_ID specifiy the used
	 * USART interface, INTERCONNECT_PIN_OUTPUT and INTERCONNECT_PIN_INPUT are
	 * two status pins which need to be connected crossover to the other
	 * microcontroller.
	 * 
	 * Example for the ATxmega128a1 (from the \c project.cfg file):
	 * \code
	 * [defines]
	 * INTERCONNECT_USART_PORT = C
	 * INTERCONNECT_USART_ID   = 0
	 * INTERCONNECT_PIN_OUTPUT = C,0
	 * INTERCONNECT_PIN_INPUT  = C,1
	 * \endcode
	 * 
	 * \tparam	T	Transmit Type
	 * \tparam	R	Receive Type
	 * 
	 * \ingroup	connectivity
	 * \author	Georgi
	 */
	template<typename T, typename R>
	class InterconnectInterface
	{
	public:
		static void
		initialize();
		
		static void
		makeTransaction();
		
		
		/**
		 * \brief	Access the receive buffer
		 * 
		 * \warning	You have to call releaseReceiveBuffer() afterwards to enable
		 * 			reception again. Otherwise the reception will be blocked!
		 * 
		 * \return	Pointer to the buffer
		 * \see		releaseReceiveBuffer()
		 */
		static const R*
		getReceiveBuffer();
		
		static void
		releaseReceiveBuffer();
		
		/**
		 * \brief	Access the transmit buffer without changing it.
		 * 
		 * No need to call releaseTransmitBuffer(), but it won't damage anything
		 * if you do.
		 */
		static const T*
		readTransmitBuffer();
		
		/**
		 * \brief	Access the transmit buffer
		 * 
		 * \warning	You have to call releaseTransmitBuffer() afterwards to enable
		 * 			transmission again. Otherwise the transmission will be blocked!
		 * 
		 * \return	Pointer to the buffer
		 * \see		releaseTransmitBuffer()
		 */
		static T*
		getTransmitBuffer();
		
		static void
		releaseTransmitBuffer();
		
	private:
		static struct Buffer
		{
			R receiveBuffer0;
			R receiveBuffer1;
			T transmitBuffer;
		} buffer;
	};
}

#include "interconnect_interface_impl.hpp"

#endif // XPCC__INTERCONNECT_INTERFACE_HPP
