// coding: utf-8
/* Copyright (c) 2016, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#include <stdlib.h>
#include <xpcc/debug/logger.hpp>
#include <xpcc/architecture/interface/assert.hpp>

using xpcc::AssertionHandler;
using xpcc::Abandonment;

#ifdef XPCC__OS_OSX
extern AssertionHandler __assertion_table_start __asm("section$start$__DATA$xpcc_assertion");
extern AssertionHandler __assertion_table_end __asm("section$end$__DATA$xpcc_assertion");
#else
extern AssertionHandler __assertion_table_start __asm("__start_xpcc_assertion");
extern AssertionHandler __assertion_table_end __asm("__stop_xpcc_assertion");
#endif

// Since we use the default linker script on hosted, the above linker section are
// only included if something is put into these sections. Therefore we are placing
// an empty assertion handler here, which does not influence assertion handling.

Abandonment
empty_assertion_handler(const char *, const char *, const char *, uintptr_t)
{
	return Abandonment::DontCare;
}
XPCC_ASSERTION_HANDLER(empty_assertion_handler);

extern "C"
{

void
xpcc_assert_fail(const char * identifier)
{
	// just forward this call
	xpcc_assert_fail_context(identifier, 0);
}

void xpcc_assert_fail_context(const char * identifier, uintptr_t context)
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
		xpcc_abandon(module, location, failure, context);
		exit(1);
	}
}

xpcc_weak
void xpcc_abandon(const char * module, const char * location, const char * failure, uintptr_t context)
{
	XPCC_LOG_ERROR.printf("Assertion '%s.%s.%s'", module, location, failure);
	if (context) { XPCC_LOG_ERROR.printf(" @ %p (%d)", (void *) context, context); }
	XPCC_LOG_ERROR.printf(" failed! Abandoning...\n");
}

}
