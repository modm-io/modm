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
 * $Id$
 */
// ----------------------------------------------------------------------------

#ifndef XPCC__INTERCONNECT_LINK_HPP
#define XPCC__INTERCONNECT_LINK_HPP

#include <xpcc/architecture/utils.hpp>
#include <xpcc/architecture/driver.hpp>

namespace xpcc
{
	/**
	 * \brief	Fast data exchange between two microcontrollers via USART.
	 * 
	 * The class can be configured with the \c project.cfg file. Example:
	 * \code
	 * [defines]
	 * INTERCONNECT_USART_PORT	 = D
	 * INTERCONNECT_USART_ID	 = 1
	 * INTERCONNECT_PIN_OUTPUT	 = D,5
	 * INTERCONNECT_PIN_INPUT	 = D,4
	 * \endcode
	 * 
	 * Or you can use the predefined values for the CPU Board by defining
	 * CPU_BOARD2_MASTER = 1 or CPU_BOARD2_SLAVE = 1. 
	 * 
	 * Resources used:
	 * - DMA Channel 0 is used to transmit Data
	 * - DMA Channel 1 is used to receive Data
	 * - INT0 of the Port connected to the input pin (see INTERCONNECT_PIN_INPUT) is
	 * 	 used to check when data was received.
	 * 
	 * \warning	Medium level interrupts and interrupts in general must be enabled!
	 * 
	 * \author	Georgi
	 * \ingroup	driver
	 */
	struct InterconnectLink
	{
	public:
		static void
		initialize();
		
		static inline void
		switchReceiveBuffers()
		{
			uint8_t *r = receiveBufferUser;
			receiveBufferUser = receiveBufferSystem;
			receiveBufferSystem = r;
		}
		
		static void
		reactivateTransmission();
		
		static void
		reactivateReception();
		
		static void
		makeTransaction();
		
	public:
		enum ReceptionState
		{
			RECEIVE_BUFFER_FREE,		///< Idle
			RECEIVE_BUFFER_ACCESSED
		};
		
		enum TransmissionState
		{
			TRANSMIT_BUFFER_FREE,		///< Idle
			TRANSMIT_BUFFER_ACCESSED,
			TRANSMISSION_IN_PROGRESS
		};
		
	public:
		static uint16_t sizeReceiveBuffer;
		static uint16_t sizeTransmitBuffer;
		
		static uint8_t *receiveBufferUser;
		static uint8_t *receiveBufferSystem;
		static uint8_t *transmitBuffer;
		
		static volatile ReceptionState stateReception;
		static volatile TransmissionState stateTransmission;
		
		static bool newDataReceived;
		
	private:
		static void
		configureUsart();
		
		/// Used to transmit data
		static void
		configureDmaChannel0();
		
		/// Used to receive data
		static void
		configureDmaChannel1();
	};
}

#endif // XPCC__INTERCONNECT_LINK_HPP
