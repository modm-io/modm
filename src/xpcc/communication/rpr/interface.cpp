/*

 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifdef __AVR__
#	include <util/crc16.h>
#endif

#include "interface.hpp"

uint16_t
modm::rpr::crcUpdate(uint16_t crc, uint8_t data)
{
#ifdef __AVR__
	return _crc16_update(crc, data);
#else
	crc ^= data;
	for (uint_fast8_t i = 0; i < 8; ++i)
	{
		if (crc & 1) {
			crc = (crc >> 1) ^ 0xA001;
		}
		else {
			crc = (crc >> 1);
		}
	}

	return crc;
#endif
}

