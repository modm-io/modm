/*
 * Copyright (c) 2009, Martin Rosekeit
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

#include "../button_group.hpp"

#include "button_group_test.hpp"

void
ButtonGroupTest::testButtons()
{
	modm::ButtonGroup<> buttons(0);
	
	TEST_ASSERT_FALSE(buttons.isPressed(0x01));
	
	buttons.update(0xff);
	buttons.update(0xff);
	buttons.update(0xff);
	buttons.update(0xff);
	buttons.update(0xff);
	
	TEST_ASSERT_FALSE(buttons.isPressed(0x01));
	
	buttons.update(0x00);
	buttons.update(0x00);
	buttons.update(0x00);
	
	TEST_ASSERT_FALSE(buttons.isPressed(0x01));
	
	buttons.update(0x00);
	
	TEST_ASSERT_TRUE(buttons.isPressed(0x01));
	TEST_ASSERT_FALSE(buttons.isPressed(0x01));
	TEST_ASSERT_FALSE(buttons.isPressed(0x01));
	
	buttons.update(0xff);
	buttons.update(0xff);
	buttons.update(0xff);
	
	TEST_ASSERT_FALSE(buttons.isReleased(0x01));
	
	buttons.update(0xff);
	
	TEST_ASSERT_TRUE(buttons.isReleased(0x01));
	
	// TODO do some real testing
}
