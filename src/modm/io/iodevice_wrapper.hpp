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

#ifndef MODM_IODEVICE_WRAPPER_HPP
#define MODM_IODEVICE_WRAPPER_HPP

#include <stdint.h>

#include "iodevice.hpp"

namespace modm
{

/// The preferred behavior when the IODevice buffer is full
/// @ingroup	modm_io
enum class
IOBuffer
{
	DiscardIfFull,
	BlockIfFull
};

/**
 * @ingroup		modm_io
 * @tparam		Device		Peripheral which should be wrapped
 * @tparam		behavior	preferred behavior when the Device buffer is full
 */
template< class Device, IOBuffer behavior >
class IODeviceWrapper : public IODevice
{
public:
	IODeviceWrapper() = default;
	using IODevice::write;

	void
	write(char c) override
	{
		bool written;
		do
		{
			written = Device::write(uint8_t(c));
		}
		while(behavior == IOBuffer::BlockIfFull and not written);
	}

	void
	flush() override
	{
		Device::flushWriteBuffer();
	}

	bool
	read(char& c) override
	{
		return Device::read(reinterpret_cast<uint8_t&>(c));
	}
};

/// @ingroup modm_io
template< class Device, IOBuffer behavior >
class IODeviceObjectWrapper : public IODevice
{
	Device &device;
public:
	IODeviceObjectWrapper(Device& device) : device{device} {}
	using IODevice::write;

	void
	write(char c) override
	{
		bool written;
		do
		{
			written = device.write(uint8_t(c));
		}
		while(behavior == IOBuffer::BlockIfFull and not written);
	}

	void
	flush() override
	{
		device.flushWriteBuffer();
	}

	bool
	read(char& c) override
	{
		return device.read(reinterpret_cast<uint8_t&>(c));
	}
};

}

#endif // MODM_IODEVICE_WRAPPER_HPP
