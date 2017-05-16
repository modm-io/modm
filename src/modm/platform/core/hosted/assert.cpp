/*
 * Copyright (c) 2016, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <stdlib.h>
#include <modm/debug/logger.hpp>
#include <modm/architecture/interface/assert.hpp>

using modm::AssertionHandler;
using modm::Abandonment;

#ifdef MODM_OS_OSX
extern AssertionHandler __assertion_table_start __asm("section$start$__DATA$modm_assertion");
extern AssertionHandler __assertion_table_end __asm("section$end$__DATA$modm_assertion");
#else
extern AssertionHandler __assertion_table_start __asm("__start_modm_assertion");
extern AssertionHandler __assertion_table_end __asm("__stop_modm_assertion");
#endif

// Since we use the default linker script on hosted, the above linker section are
// only included if something is put into these sections. Therefore we are placing
// an empty assertion handler here, which does not influence assertion handling.

Abandonment
empty_assertion_handler(const char *, const char *, const char *, uintptr_t)
{
	return Abandonment::DontCare;
}
MODM_ASSERTION_HANDLER(empty_assertion_handler);

extern "C"
{

void
modm_assert_fail(const char * identifier)
{
	// just forward this call
	modm_assert_fail_context(identifier, 0);
}

void modm_assert_fail_context(const char * identifier, uintptr_t context)
{
	uint8_t state((uint8_t) Abandonment::DontCare);
	const char * module = identifier;
	const char * location = module + strlen(module) + 1;
	const char * failure = location + strlen(location) + 1;

	AssertionHandler * handler = &__assertion_table_start;
	for (; handler < &__assertion_table_end; handler++)
	{
		state |= (uint8_t) (*handler)(module, location, failure, context);
	}

	if (state == (uint8_t) Abandonment::DontCare or
		state & (uint8_t) Abandonment::Fail)
	{
		modm_abandon(module, location, failure, context);
		exit(1);
	}
}

modm_weak
void modm_abandon(const char * module, const char * location, const char * failure, uintptr_t context)
{
	MODM_LOG_ERROR.printf("Assertion '%s.%s.%s'", module, location, failure);
	if (context) { MODM_LOG_ERROR.printf(" @ %p (%d)", (void *) context, context); }
	MODM_LOG_ERROR.printf(" failed! Abandoning...\n");
}

}
