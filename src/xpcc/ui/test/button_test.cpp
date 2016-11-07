/*
 * Copyright (c) 2009-2010, Martin Rosekeit
 * Copyright (c) 2009-2011, Fabian Greif
 * Copyright (c) 2012-2013, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include "../button.hpp"

#include "button_test.hpp"

struct TestPin
{
	static bool
	read()
	{
		
		return false;
	}
};

void
ButtonTest::testButton()
{
	xpcc::Button<TestPin> button;
	
	TEST_ASSERT_FALSE(button.isPressed());
	
	// TODO
	/*button.update();
	button.update();
	button.update();
	button.update();
	button.update();
	
	TEST_ASSERT_FALSE(button.isPressed());
	
	button.update();
	button.update();
	button.update();
	
	TEST_ASSERT_FALSE(button.isPressed());
	
	button.update();
	
	TEST_ASSERT_TRUE(button.isPressed());
	TEST_ASSERT_FALSE(button.isPressed());
	TEST_ASSERT_FALSE(button.isPressed());*/
}
