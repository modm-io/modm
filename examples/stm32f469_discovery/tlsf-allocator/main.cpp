/*
 * Copyright (c) 2016-2017, Niklas Hauser
 * Copyright (c) 2017, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/board/board.hpp>
#include <modm/architecture/interface/memory.hpp>
#include <inttypes.h>

int main()
{
	Board::initialize();
	Board::Leds::setOutput();

	// 600 pointers to allocated memories
	uint8_t *d[600];
	int free_ii = 0;

	for (int ii=0; ii < 600; ii++)
	{
		d[ii] = nullptr;
		uint32_t size = rand() % (1024 * 100);

		d[ii] = new (modm::MemoryFastData) uint8_t[size];

		// print what size we requested and if it succeeded
		MODM_LOG_INFO.printf(" malloc(%3" PRId32 "kB) = ", size/1024);
		if (d[ii]) MODM_LOG_INFO << d[ii];
		else   MODM_LOG_INFO << "NO MEMORY ";

		if ((rand() % 100) >= 50) {
			// only some memory is returned to the system
			uint8_t *df;
			while ((df = d[free_ii++]) == nullptr and free_ii <= ii) ;
			MODM_LOG_INFO << "  ...freeing " << df;
			delete df;
		}
		MODM_LOG_INFO << modm::endl;
	}

	while (1)
	{
		Board::Leds::toggle();
		modm::delayMilliseconds(500);
	}
	return 0;
}
