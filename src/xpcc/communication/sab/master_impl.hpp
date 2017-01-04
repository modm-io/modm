/*
 * Copyright (c) 2011-2012, Fabian Greif
 * Copyright (c) 2012-2013, 2015, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_SAB_MASTER_HPP
#	error	"Don't include this file directly, use 'master.hpp' instead!"
#endif

// ----------------------------------------------------------------------------
template <typename Interface>
Interface xpcc::sab::Master<Interface>::interface;

template <typename Interface>
typename xpcc::sab::Master<Interface>::QueryStatus xpcc::sab::Master<Interface>::queryStatus;

template <typename Interface>
uint8_t xpcc::sab::Master<Interface>::expectedResponseLength;

template <typename Interface>
xpcc::ShortTimeout xpcc::sab::Master<Interface>::timer;

// ----------------------------------------------------------------------------
template <typename Interface>
void
xpcc::sab::Master<Interface>::initialize()
{
	queryStatus = ERROR_TIMEOUT;
}

// ----------------------------------------------------------------------------
template <typename Interface>
void
xpcc::sab::Master<Interface>::query(uint8_t slaveAddress, uint8_t command,
									const void *payload, uint8_t payloadLength, uint8_t responseLength)
{
	while (interface.isMessageAvailable()) {
		interface.dropMessage();
	}
	interface.sendMessage(slaveAddress, REQUEST, command, payload, payloadLength);
	
	queryStatus = IN_PROGRESS;
	expectedResponseLength = responseLength;
	
	timer.restart(timeout);
}

template <typename Interface> template <typename T>
void
xpcc::sab::Master<Interface>::query(uint8_t slaveAddress, uint8_t command,
		const T& payload, uint8_t responseLength)
{
	while (interface.isMessageAvailable()) {
		interface.dropMessage();
	}
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
	while (interface.isMessageAvailable()) {
		interface.dropMessage();
	}
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
const uint8_t *
xpcc::sab::Master<Interface>::getResponse()
{
	return reinterpret_cast<const uint8_t *>(interface.getPayload());
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
