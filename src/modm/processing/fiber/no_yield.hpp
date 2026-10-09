/*
 * Copyright (c) 2023, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#pragma once

/// @cond
namespace modm::this_fiber
{

void inline
yield()
{
	__asm__ volatile ("" ::: "memory");
}

modm::fiber::id inline
get_id()
{
	return 0;
}

} // namespace modm::this_fiber
/// @endcond
