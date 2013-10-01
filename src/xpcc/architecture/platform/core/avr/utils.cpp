// coding: utf-8
/* Copyright (c) 2011, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#include <avr/pgmspace.h>

#include "utils.hpp"

uint8_t
xpcc::xmega::readCalibrationByte(uint8_t index)
{
	// Load the NVM Command register to read the calibration row.
	NVM_CMD = NVM_CMD_READ_CALIB_ROW_gc;

	uint8_t result = pgm_read_byte(index);

	// Clean up NVM Command register.
	NVM_CMD = NVM_CMD_NO_OPERATION_gc;

	return result;
}
