/*
 * Copyright (c) 2009, Martin Rosekeit
 * Copyright (c) 2009-2011, Fabian Greif
 * Copyright (c) 2012, Niklas Hauser
 * Copyright (c) 2012, 2016, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include "harness.hpp"

namespace unittest
{
	FLASH_STORAGE_STRING(stringEqual) = " == ";
	FLASH_STORAGE_STRING(stringDiffer) = " != ";
	FLASH_STORAGE_STRING(stringNotInRange) = " not in range ";
	FLASH_STORAGE_STRING(stringNotTrue) = "true == false\n";
}

bool
unittest::checkExpression(bool expr, unsigned int line)
{
	if (expr) {
		TEST_REPORTER_.reportPass();
		return true;
	} else {
		TEST_REPORTER_.reportFailure(line)
			<< modm::accessor::asFlash(unittest::stringNotTrue);
		return false;
	}
}

bool
unittest::checkEqual(const float& a, const float& b, unsigned int line)
{
	if (((a + TEST_FLOAT_EPISLON) >= b) and ((a - TEST_FLOAT_EPISLON) <= b))
	{
		TEST_REPORTER_.reportPass();
		return true;
	}
	else {
		TEST_REPORTER_.reportFailure(line)
			<< a << modm::accessor::asFlash(unittest::stringEqual) << b << '\n';
		return false;
	}
}
