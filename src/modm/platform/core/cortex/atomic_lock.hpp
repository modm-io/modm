/*
 * Copyright (c) 2009-2012, Fabian Greif
 * Copyright (c) 2010, Martin Rosekeit
 * Copyright (c) 2012, 2015, Sascha Schade
 * Copyright (c) 2012, 2015-2016, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef	MODM_PLATFORM_ATOMIC_LOCK_HPP
#define	MODM_PLATFORM_ATOMIC_LOCK_HPP

#include "../device.hpp"
#include <modm/architecture/utils.hpp>

namespace modm
{

namespace atomic
{

class Lock
{
public:
	modm_always_inline
	Lock() : cpsr(__get_PRIMASK())
	{
		__disable_irq();
	}

	modm_always_inline
	~Lock()
	{
		__set_PRIMASK(cpsr);
	}

private:
	uint32_t cpsr;
};

class Unlock
{
public:
	modm_always_inline
	Unlock() : cpsr(__get_PRIMASK())
	{
		__enable_irq();
	}

	modm_always_inline
	~Unlock()
	{
		__set_PRIMASK(cpsr);
	}

private:
	uint32_t cpsr;
};

}	// namespace atomic

}	// namespace modm

#endif	// MODM_PLATFORM_ATOMIC_LOCK_HPP
