/*
 * Copyright (c) 2009, Georgi Grinshpun
 * Copyright (c) 2009-2011, Fabian Greif
 * Copyright (c) 2010, Martin Rosekeit
 * Copyright (c) 2011-2014, 2017, Niklas Hauser
 * Copyright (c) 2013, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <avr/pgmspace.h>

#include "utils.hpp"

uint8_t
modm::platform::readCalibrationByte(uint8_t index)
{
	// Load the NVM Command register to read the calibration row.
	NVM_CMD = NVM_CMD_READ_CALIB_ROW_gc;

	uint8_t result = pgm_read_byte(index);

	// Clean up NVM Command register.
	NVM_CMD = NVM_CMD_NO_OPERATION_gc;

	return result;
}
