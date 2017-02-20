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

extern "C"
{

void xpcc_assert_fail(const char * identifier)
{
	uint8_t state((uint8_t) Abandonment::DontCare);
	const char * module = identifier;
	const char * location = module + strlen(module) + 1;
	const char * failure = location + strlen(location) + 1;

	AssertionHandler * handler = &__assertion_table_start;
	for (; handler < &__assertion_table_end; handler++)
	{
		state |= (uint8_t) (*handler)(module, location, failure);
	}

	if (state == (uint8_t) Abandonment::DontCare or
		state & (uint8_t) Abandonment::Fail)
	{
		xpcc_abandon(module, location, failure);
		exit(1);
	}
}

void xpcc_abandon(const char * module, const char * location, const char * failure) __attribute__((weak));
void xpcc_abandon(const char * module, const char * location, const char * failure)
{
	XPCC_LOG_ERROR << "Assertion '"
			<< module << "." << location << "." << failure
			<< "' failed! Abandoning." << xpcc::endl;
}

}
