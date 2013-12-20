/*
 * Copyright (c) 2009, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
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
