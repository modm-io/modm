/*
 * Copyright (c) 2020, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/architecture/interface/assert.h>

// ------------------------------------------------------------------------

extern void _exit(int);
void _exit(int status)
{
	modm_assert(false, "libc.exit",
			"The libc exit(status) function was called!", status);
}
