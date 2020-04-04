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

#ifndef	MODM_INTERFACE_ATOMIC_LOCK_HPP
#define	MODM_INTERFACE_ATOMIC_LOCK_HPP

#ifdef __DOXYGEN__

namespace modm
{

namespace atomic
{

/**
 * Critical section
 *
 * Typical usage:
 * @code
 * function()
 * {
 *     // some code with interrupts enabled
 *
 *     {
 *         atomic::Lock lock;
 *
 *         // code which should be executed with disabled interrupts.
 *
 *         // with the end of this block the lock object is destroyed
 *         // and the interrupts are reenabled.
 *     }
 *
 *     // other code with interrupts enabled
 * }
 * @endcode
 * @ingroup	modm_architecture_atomic
 */
class Lock
{
public:
	modm_always_inline
	Lock();
};

/**
 * Opposite to atomic::Lock
 *
 * Use this class to create a block of code with interrupts enabled
 * inside a locked block.
 *
 * Most of the time you won't need this class. But on some rare
 * times it is useful. The modm::Scheduler is an example for that.
 * @ingroup	modm_architecture_atomic
 */
class Unlock
{
public:
	modm_always_inline
	Unlock();
};

}	// namespace atomic

}	// namespace modm

#else

// These functions may be implemented as header files, so we cannot rely on
// there being a link-able function and delegate this choice to the platform.
#include <modm/platform/core/atomic_lock_impl.hpp>

#endif

#endif	// MODM_INTERFACE_ATOMIC_LOCK_HPP
