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

/// @cond
namespace modm
{

namespace atomic
{

class Lock
{
public:
	Lock() {}
};

class Unlock
{
public:
	Unlock() {}
};

}	// namespace atomic

}	// namespace modm

/// @endcond
#endif	// MODM_PLATFORM_ATOMIC_LOCK_HPP
