/*
 * Copyright (c) 2021, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include "board.hpp"

// Default fuses for the AT90CAN128 bootloader
// see https://www.alvidi.de/data_sheets/pdf/bootloader/bootloader.pdf
modm_weak FUSES =
{
	0xFF, // .low
	0x98, // .high
	0xFF, // .extended
};
