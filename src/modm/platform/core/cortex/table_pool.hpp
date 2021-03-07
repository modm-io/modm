/*
 * Copyright (c) 2019, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef TABLE_POOL_HPP
#define TABLE_POOL_HPP

#include <cstdint>

namespace modm::platform
{

struct table_pool_t
{
	uint32_t traits;
	uint8_t *const start;
	uint8_t *const end;
} __attribute__((packed));

extern "C" const table_pool_t __table_heap_start[];
extern "C" const table_pool_t __table_heap_end[];

} //namespace
#endif