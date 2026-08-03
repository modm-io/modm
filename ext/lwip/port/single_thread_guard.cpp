/*
 * Copyright (c) 2026, Kaelin Laundry
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <modm/architecture/interface/assert.hpp>
#include <modm/architecture/interface/interrupt.hpp>

#include <atomic>

namespace
{

// lwIP calls are synchronous and must not yield. Entry tracking catches
// accidental re-entry added to these call paths later.
std::atomic_bool entered{false};

} // namespace

extern "C" void
modm_lwip_single_thread_acquire()
{
	modm_assert(not modm::isInterruptContext(), "lwip.context",
			"lwIP cannot be entered from an interrupt");
	modm_assert(not entered.exchange(true), "lwip.owner", "lwIP core already locked");
}

extern "C" void
modm_lwip_single_thread_release()
{
	modm_assert(not modm::isInterruptContext(), "lwip.context",
			"lwIP cannot be released from an interrupt");
	modm_assert(entered.exchange(false), "lwip.owner", "lwIP core not locked");
}

extern "C" void
modm_lwip_assert_core_locked()
{
	modm_assert(not modm::isInterruptContext(), "lwip.context",
			"lwIP cannot be entered from an interrupt");
	modm_assert(entered.load(), "lwip.owner", "lwIP core not locked");
}
