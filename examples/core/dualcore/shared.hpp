/*
 * Copyright (c) 2026, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#pragma once

#include <modm/architecture/interface/atomic_queue.hpp>
#include <atomic>

// Both projects include this header, the Cortex-M7 defines it in SRAM4
struct Shared
{
	// One queue per direction with exactly one producer and one consumer
	modm::atomic::Queue<uint32_t, 16> toM4;
	modm::atomic::Queue<uint32_t, 16> toM7;
	// Loads and stores are atomic between both cores
	std::atomic<bool> button{false};
	// Read-modify-write is not atomic between both cores, use dualcore::Lock
	uint32_t counter{0};
};
