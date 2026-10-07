/*
 * Copyright (c) 2022, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/board.hpp>

int
main()
{
	Board::initialize();

	printf("Hello World!\n");
	printf("Hello %s!\n", "String Formatting");
	float value = 5.012423523124f;
	printf("Float %f\n", (double) value);

	uint8_t counter{0};
	while (true)
	{
		modm::delay(1s);
		counter++;
		printf("Counter %3d\n", counter);
	}
	return 0;
}
