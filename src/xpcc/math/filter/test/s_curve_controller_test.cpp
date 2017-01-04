/*
 * Copyright (c) 2009, Martin Rosekeit
 * Copyright (c) 2009-2011, Fabian Greif
 * Copyright (c) 2012, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/math/filter/s_curve_controller.hpp>

#include "s_curve_controller_test.hpp"

void
SCurveControllerTest::testConstructor()
{
	xpcc::SCurveController<int16_t>::Parameter parameter;
	
	xpcc::SCurveController<int16_t> controller(parameter);
	
	int16_t target = 200;
	int16_t input = 200;
	
	TEST_ASSERT_FALSE(controller.isTargetReached());
	
	controller.update(target - input, 0);
	
	TEST_ASSERT_EQUALS(controller.getValue(), 0);
	TEST_ASSERT_TRUE(controller.isTargetReached());
	
	// FIXME some useful tests are needed here
}
