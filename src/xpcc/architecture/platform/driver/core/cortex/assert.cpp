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

#include <xpcc/architecture/interface/assert.hpp>

extern "C" void exit(int);

using xpcc::AssertionHandler;
using xpcc::Abandonment;

extern AssertionHandler __assertion_table_start;
extern AssertionHandler __assertion_table_end;

extern "C"
{

void xpcc_assert_evaluate(bool condition, const char * identifier)
{
	if (!condition)
	{
		uint8_t state(uint8_t(Abandonment::DontCare));
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
}

void xpcc_abandon(const char *, const char *, const char *) __attribute__((weak));
void xpcc_abandon(const char *, const char *, const char *) {}

}
