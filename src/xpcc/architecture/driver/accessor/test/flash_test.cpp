/*
 * Copyright (c) 2009-2011, Fabian Greif
 * Copyright (c) 2009, Martin Rosekeit
 * Copyright (c) 2012, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/architecture/driver/accessor/flash.hpp>

#include "flash_test.hpp"

FLASH_STORAGE(int intValue) = 12345;

void
FlashTest::testIntegerAccess()
{
	xpcc::accessor::Flash<int> intPointer(&intValue);
	
	TEST_ASSERT_EQUALS(*intPointer, 12345);
}	

FLASH_STORAGE_STRING(stringValue) = "Hallo Welt!";

void
FlashTest::testStringAccess()
{
	char string[] = "Hallo Welt!";
	xpcc::accessor::Flash<char> stringPointer(stringValue);
	
	TEST_ASSERT_EQUALS_ARRAY(stringPointer, string, sizeof(string));
}
