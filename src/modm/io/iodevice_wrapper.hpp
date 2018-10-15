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

#endif // MODM_IODEVICE_WRAPPER_HPP
