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

#include <modm/architecture/interface/assert.hpp>
#include <stdlib.h>
#include <string.h>
#include <avr/pgmspace.h>

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
		const char * location = module + strlen_P(module) + 1;
		const char * failure = location + strlen_P(location) + 1;

		AssertionHandler * table_addr = &__assertion_table_start;
		for (; table_addr < &__assertion_table_end; table_addr++)
		{
			AssertionHandler handler = (AssertionHandler) pgm_read_word(table_addr);
			state |= (uint8_t) handler(module, location, failure);
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
