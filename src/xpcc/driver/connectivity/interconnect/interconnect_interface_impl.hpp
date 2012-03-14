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
 */
// ----------------------------------------------------------------------------


#ifndef	XPCC__INTERCONNECT_INTERFACE_HPP
	#error	"Don't include this file directly, use 'interconnect_interface.hpp' instead"
#endif

#include <xpcc/architecture/driver.hpp>
#include "interconnect_link.hpp"

// ----------------------------------------------------------------------------
template<typename T, typename R>
typename xpcc::InterconnectInterface<T, R>::Buffer xpcc::InterconnectInterface<T, R>::buffer;

// ----------------------------------------------------------------------------
template<typename T, typename R>
void
xpcc::InterconnectInterface<T, R>::initialize()
{
	InterconnectLink::sizeReceiveBuffer = sizeof(R);
	InterconnectLink::sizeTransmitBuffer = sizeof(T);
	
	InterconnectLink::transmitBuffer = (uint8_t *) &buffer.transmitBuffer;
	InterconnectLink::receiveBufferUser = (uint8_t *) &buffer.receiveBuffer0;
	InterconnectLink::receiveBufferSystem = (uint8_t *) &buffer.receiveBuffer1;
	
	InterconnectLink::initialize();
}

// ----------------------------------------------------------------------------
template<typename T, typename R>
const R*
xpcc::InterconnectInterface<T, R>::getReceiveBuffer()
{
	xpcc::atomic::Lock lock;
	InterconnectLink::stateReception = InterconnectLink::RECEIVE_BUFFER_ACCESSED;
	return reinterpret_cast<const R*>(InterconnectLink::receiveBufferUser);
}


template<typename T, typename R>
void 
xpcc::InterconnectInterface<T, R>::releaseReceiveBuffer()
{
	xpcc::atomic::Lock lock;
	InterconnectLink::stateReception = InterconnectLink::RECEIVE_BUFFER_FREE;
	
	if (InterconnectLink::newDataReceived)
	{
		InterconnectLink::switchReceiveBuffers();
		InterconnectLink::reactivateReception();
		InterconnectLink::newDataReceived = false;
	}
}

// ----------------------------------------------------------------------------
template<typename T, typename R>
T*
xpcc::InterconnectInterface<T, R>::getTransmitBuffer()
{
	while(true)
	{
		while (InterconnectLink::stateTransmission == InterconnectLink::TRANSMISSION_IN_PROGRESS) {
			// wait until the transmitter buffer is ready
		}
		
		xpcc::atomic::Lock lock;
		if (InterconnectLink::stateTransmission != InterconnectLink::TRANSMISSION_IN_PROGRESS) {
			InterconnectLink::stateTransmission = InterconnectLink::TRANSMIT_BUFFER_ACCESSED;
			return &buffer.transmitBuffer;
		}
	}
}

template<typename T, typename R>
const T*
xpcc::InterconnectInterface<T, R>::readTransmitBuffer()
{
	return &buffer.transmitBuffer;
}

template<typename T, typename R>
void 
xpcc::InterconnectInterface<T, R>::releaseTransmitBuffer()
{
	xpcc::atomic::Lock lock;
	if (InterconnectLink::stateTransmission != InterconnectLink::TRANSMISSION_IN_PROGRESS) {
		InterconnectLink::stateTransmission = InterconnectLink::TRANSMIT_BUFFER_FREE;
	}
}

// ----------------------------------------------------------------------------
template<typename T, typename R>
void 
xpcc::InterconnectInterface<T, R>::makeTransaction()
{
	// TODO check if it's ok, not to send. 
	// Do not call reactivateTransmitCh0 if transaction ongoing, usually
	// you would not call makeTransaction if transmission buffer is used.
	// But if makeTransaction is called from interrupt e.g. timer take care of
	// used transmission buffer.
	xpcc::atomic::Lock lock;
	InterconnectLink::makeTransaction();
}
