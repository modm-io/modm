/*
 * Copyright (c) 2009, Martin Rosekeit
 * Copyright (c) 2009-2010, Fabian Greif
 * Copyright (c) 2012, 2016-2017, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_INTERFACE_ACCESSOR_HPP
#define MODM_INTERFACE_ACCESSOR_HPP

#include <modm/architecture/interface/accessor_ram.hpp>
#include <modm/architecture/interface/accessor_flash.hpp>

namespace modm
{

namespace accessor
{

/**
 * Add volatile modifier to T
 * @ingroup modm_architecture_accessor
 */
template<typename T>
modm_always_inline volatile T&
asVolatile(T& value)
{
	return (volatile T&) value;
}

}	// namespace accessor

}	// namespace modm

#endif // MODM_INTERFACE_ACCESSOR_HPP
