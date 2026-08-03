/*
 * Copyright (c) 2026, Kaelin Laundry
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef MODM_LWIP_HPP
#define MODM_LWIP_HPP

#include <lwip/init.h>
#include <lwip/timeouts.h>

namespace modm::lwip
{

/** RAII guard to ensure lwIP and callbacks it invokes never yield or are called
 * in an ISR. lwIP is not thread-safe. Backs lwIP's LWIP_ASSERT_CORE_LOCKED check. */
class LwIPSingleThreadGuard
{
public:
	LwIPSingleThreadGuard() { modm_lwip_single_thread_acquire(); }
	~LwIPSingleThreadGuard() { modm_lwip_single_thread_release(); }

	LwIPSingleThreadGuard(LwIPSingleThreadGuard const&) = delete;
	LwIPSingleThreadGuard& operator=(LwIPSingleThreadGuard const&) = delete;
	LwIPSingleThreadGuard(LwIPSingleThreadGuard&&) = delete;
	LwIPSingleThreadGuard& operator=(LwIPSingleThreadGuard&&) = delete;
};

namespace detail
{
inline bool initialized{false};
}

inline void
initialize()
{
	LwIPSingleThreadGuard guard;
	if (not detail::initialized) {
		lwip_init();
		detail::initialized = true;
	}
}

/// Process all lwIP timeouts that are currently due.
inline void
processTimeouts()
{
	LwIPSingleThreadGuard guard;
	if (detail::initialized)
		sys_check_timeouts();
}

} // namespace modm::lwip

#endif // MODM_LWIP_HPP
