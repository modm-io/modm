/*
 * Copyright (c) 2009, Martin Rosekeit
 * Copyright (c) 2009, Thorsten Lajewski
 * Copyright (c) 2009-2010, Fabian Greif
 * Copyright (c) 2012, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/math/filter/s_curve_generator.hpp>

#include "s_curve_generator_test.hpp"

void
SCurveGeneratorTest::testConstructor()
{
	xpcc::SCurveGenerator<int16_t> generator;
	
	generator.setTarget(200);
	
	TEST_ASSERT_FALSE(generator.isTargetReached());
	
	generator.update();
	
	// FIXME some useful tests are needed here
}
