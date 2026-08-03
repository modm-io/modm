/*
 * Copyright (c) 2026, Kaelin Laundry
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <modm/architecture/detect.hpp>
#include <modm/architecture/interface/assert.hpp>
#ifdef MODM_CPU_CORTEX_M
#include <modm/platform/device.hpp>
#endif

namespace
{

// lwIP calls are synchronous and must not yield. Entry tracking catches
// accidental re-entry added to these call paths later.
bool entered{false};

// TODO: is there a good place to put this in modm::architecture or modm::platform::core?
bool
isForeground()
{
#ifdef MODM_CPU_CORTEX_M
	return __get_IPSR() == 0;
#else
	return true;
#endif
}

} // namespace

extern "C" void
modm_lwip_single_thread_acquire()
{
	modm_assert(isForeground(), "lwip.context",
			"lwIP cannot be entered from an interrupt");
	modm_assert(not entered, "lwip.owner", "lwIP core already locked");
	entered = true;
}

extern "C" void
modm_lwip_single_thread_release()
{
	modm_assert(isForeground(), "lwip.context",
			"lwIP cannot be released from an interrupt");
	modm_assert(entered, "lwip.owner", "lwIP core not locked");
	entered = false;
}

extern "C" void
modm_lwip_assert_core_locked()
{
	modm_assert(isForeground(), "lwip.context",
			"lwIP cannot be entered from an interrupt");
	modm_assert(entered, "lwip.owner", "lwIP core not locked");
}
