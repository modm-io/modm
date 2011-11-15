// coding: utf-8
// ----------------------------------------------------------------------------
/* Copyright (c) 2011, Roboterclub Aachen e.V.
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

#ifndef XPCC_SAB__MASTER_HPP
	#error	"Don't include this file directly, use 'master.hpp' instead!"
#endif

// ----------------------------------------------------------------------------
template <typename Interface>
Interface xpcc::sab::Master<Interface>::interface;

template <typename Interface>
typename xpcc::sab::Master<Interface>::QueryStatus xpcc::sab::Master<Interface>::queryStatus;

template <typename Interface>
uint8_t xpcc::sab::Master<Interface>::expectedResponseLength;

template <typename Interface>
xpcc::Timeout<> xpcc::sab::Master<Interface>::timer;

// ----------------------------------------------------------------------------
template <typename Interface>
void
xpcc::sab::Master<Interface>::initialize()
{
	queryStatus = ERROR_TIMEOUT;
}

// ----------------------------------------------------------------------------
template <typename Interface> template <typename T>
void
xpcc::sab::Master<Interface>::query(uint8_t slaveAddress, uint8_t command,
		const T& payload, uint8_t responseLength)
{
	interface.dropMessage();
	interface.sendMessage(slaveAddress, REQUEST, command, payload);
	
	queryStatus = IN_PROGRESS;
	expectedResponseLength = responseLength;
	
	timer.restart(timeout);
}

template <typename Interface>
void
xpcc::sab::Master<Interface>::query(uint8_t slaveAddress, uint8_t command,
		uint8_t responseLength)
{
	interface.dropMessage();
	interface.sendMessage(slaveAddress, REQUEST, command, 0, 0);
	
	queryStatus = IN_PROGRESS;
	expectedResponseLength = responseLength;
	
	timer.restart(timeout);
}

// ----------------------------------------------------------------------------
template <typename Interface>
bool
xpcc::sab::Master<Interface>::isQueryCompleted()
{
	return (queryStatus != IN_PROGRESS);
}

// ----------------------------------------------------------------------------
template <typename Interface>
bool
xpcc::sab::Master<Interface>::isSuccess()
{
	return (queryStatus == SUCCESS);
}

// ----------------------------------------------------------------------------
template <typename Interface>
uint8_t
xpcc::sab::Master<Interface>::getErrorCode()
{
	if (queryStatus == ERROR_RESPONSE) {
		// Error code is in the first payload byte
		return interface.getPayload()[0];
	}
	else {
		return queryStatus;
	}
}

// ----------------------------------------------------------------------------
template <typename Interface> template <typename T>
const T *
xpcc::sab::Master<Interface>::getResponse()
{
	return reinterpret_cast<const T *>(interface.getPayload());
}

template <typename Interface>
const void *
xpcc::sab::Master<Interface>::getResponse()
{
	return reinterpret_cast<const void *>(interface.getPayload());
}

// ----------------------------------------------------------------------------
template <typename Interface>
void
xpcc::sab::Master<Interface>::update()
{
	interface.update();
	
	if (queryStatus != IN_PROGRESS) {
		return;
	}
	
	if (interface.isMessageAvailable())
	{
		if (!interface.isResponse())
		{
			// with CAN transceivers as bus drivers every message send is
			// also received as a new message => drop every message which is
			// not a response
			interface.dropMessage();
		}
		else if (interface.isAcknowledge())
		{
			if (interface.getPayloadLength() == expectedResponseLength) {
				queryStatus = SUCCESS;
			}
			else {
				queryStatus = ERROR_PAYLOAD;
			}
		}
		else {
			queryStatus = ERROR_RESPONSE;
		}
	}
	else if (timer.isExpired())
	{
		queryStatus = ERROR_TIMEOUT;
	}
}
