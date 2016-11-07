/*
 * Copyright (c) 2009-2010, 2012, Fabian Greif
 * Copyright (c) 2010, Martin Rosekeit
 * Copyright (c) 2012, Georgi Grinshpun
 * Copyright (c) 2012-2014, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_IODEVICE_WRAPPER_HPP
#define XPCC_IODEVICE_WRAPPER_HPP

#include <stdint.h>

#include "iodevice.hpp"

namespace xpcc
{

/// The preferred behavior when the IODevice buffer is full
/// @ingroup	io
enum class
IOBuffer
{
	DiscardIfFull,
	BlockIfFull
};

/**
 * Wrapper to use any peripheral device that supports static
 * write() and read() as an IODevice.
 *
 * You have to decide what happens when the device buffer is full
 * and you cannot write to it at the moment.
 * There are two options:
 *  1. busy wait until the buffer is free, or
 *  2. discard the bytes that cannot be written.
 *
 * Option 1 has the advantage, that none of your data will be lost,
 * however, busy-waiting can take a long time and can mess up your
 * program timings.
 * There is also a **high risk of deadlock**, when writing to a
 * IODevice inside of an interrupt and then busy-waiting forever
 * because the IODevice requires interrupts itself to send out
 * the data.
 *
 * It is therefore highly recommended to use option 2, where surplus
 * data will be discarded.
 * You should increase the IODevice buffer size, if you experience
 * missing data from your connection.
 * This behavior is also deadlock safe when called from inside another
 * interrupt, and your program timing is minimally affected (essentially
 * only coping data into the buffer).
 *
 * There is no default template argument, so that you hopefully make
 * a concious decision and be aware of this behavior.
 *
 * Example:
 * @code
 * // configure a UART
 * Uart0 uart;
 *
 * // wrap it into an IODevice
 * xpcc::IODeviceWrapper<Uart0, xpcc::IOBuffer::DiscardIfFull> device;
 *
 * // use this device to print a message
 * device.write("Hello");
 *
 * // or create a IOStream and use the stream to print something
 * xpcc::IOStream stream(device);
 * stream << " World!";
 * @endcode
 *
 * @ingroup		io
 * @tparam		Device		Peripheral which should be wrapped
 * @tparam		behavior	preferred behavior when the Device buffer is full
 */
template< class Device, IOBuffer behavior >
class IODeviceWrapper : public IODevice
{
public:
	/**
	 * Constructor
	 *
	 * @param	device	configured object
	 */
	IODeviceWrapper(const Device& /*device*/)
	{
	}
	IODeviceWrapper()
	{
	}

	virtual void
	write(char c)
	{
		// this branch will be optimized away, since `behavior` is a template argument
		if (behavior == IOBuffer::DiscardIfFull)
		{
			Device::write(static_cast<uint8_t>(c));
		}
		else
		{
			while( !Device::write(static_cast<uint8_t>(c)) )
				;
		}
	}

	virtual void
	write(const char *s)
	{
		// this branch will be optimized away, since `behavior` is a template argument
		if (behavior == IOBuffer::DiscardIfFull)
		{
			while (*s)
			{
				Device::write(static_cast<uint8_t>(*s));
				s++;
			}
		}
		else
		{
			while (*s) {
				while( !Device::write(static_cast<uint8_t>(*s)) )
					;
				s++;
			}
		}
	}

	virtual void
	flush()
	{
	}

	virtual bool
	read(char& c)
	{
		return Device::read(reinterpret_cast<uint8_t&>(c));
	}
};

}

#endif // XPCC_IODEVICE_WRAPPER_HPP
