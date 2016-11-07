/*
 * Copyright (c) 2010, Fabian Greif
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// Bootloader Settings

#define	BOOTLOADER_TYPE		0

#define	BOOT_LED			B,1

//#define	BOOT_INIT
//#define	BOOT_LED_SET_OUTPUT
//#define	BOOT_LED_ON
//#define	BOOT_LED_OFF
//#define	BOOT_LED_TOGGLE

// ----------------------------------------------------------------------------
// CAN Settings

#define	MCP2515_CS			B,4
#define	MCP2515_INT			B,2
