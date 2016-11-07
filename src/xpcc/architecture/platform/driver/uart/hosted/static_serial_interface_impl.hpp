/*
 * Copyright (c) 2013, Fabian Greif
 * Copyright (c) 2014, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_HOSTED_STATIC_SERIAL_INTERFACE_HPP
#	error "Do not include this file directly. Use 'static_serial_interfac.hpp'!"
#endif

#include "static_serial_interface.hpp"

template<int N>
xpcc::hosted::SerialInterface*
xpcc::hosted::StaticSerialInterface<N>::backend = 0;

template<int N>
template<uint32_t baudrate>
bool
xpcc::hosted::StaticSerialInterface<N>::initialize(SerialInterface& interface)
{
	backend = &interface;

	backend->setBaudRate(baudrate);
	return (backend->open());
}

template<int N>
void
xpcc::hosted::StaticSerialInterface<N>::writeBlocking(uint8_t data)
{
	backend->write(data);
}

template<int N>
void
xpcc::hosted::StaticSerialInterface<N>::writeBlocking(const uint8_t *data, std::size_t length)
{
	backend->writeBytes(data, length);
}

template<int N>
void
xpcc::hosted::StaticSerialInterface<N>::flushWriteBuffer()
{
	backend->flush();
}

template<int N>
bool
xpcc::hosted::StaticSerialInterface<N>::write(uint8_t data)
{
	backend->write(data);
}

template<int N>
std::size_t
xpcc::hosted::StaticSerialInterface<N>::write(const uint8_t *data, std::size_t length)
{
	backend->writeBytes(data, length);
}

template<int N>
bool
xpcc::hosted::StaticSerialInterface<N>::isWriteFinished()
{
	return true;
}

template<int N>
bool
xpcc::hosted::StaticSerialInterface<N>::read(uint8_t &data)
{
	char c;
	if (backend->read(c)) {
		data = static_cast<uint8_t>(c);
		return true;
	}

	return false;
}

template<int N>
std::size_t
xpcc::hosted::StaticSerialInterface<N>::read(uint8_t *data, std::size_t length)
{
	// Check how many bytes are available before reading to
	// avoid a blocking access.
	std::size_t available = backend->bytesAvailable();
	if (available < length) {
		length = available;
	}

	backend->readBytes(data, length);

	return length;
}

template<int N>
std::size_t
xpcc::hosted::StaticSerialInterface<N>::discardReceiveBuffer()
{
	return 0;
}

template<int N>
std::size_t
xpcc::hosted::StaticSerialInterface<N>::discardTransmitBuffer()
{
	return 0;
}
