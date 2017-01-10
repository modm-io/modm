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

SECTIONS
{
	.modm_assertion : AT( ADDR(.text) + SIZEOF(.text) + SIZEOF(.data))
	{
		. = ALIGN(2);
		__assertion_table_start = . + SIZEOF(.data);

		KEEP(*(.assertion))

		__assertion_table_end = . + SIZEOF(.data);
	}
}
INSERT AFTER .data
