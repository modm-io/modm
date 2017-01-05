/*
 * Copyright (c) 2009-2011, Fabian Greif
 * Copyright (c) 2010, Martin Rosekeit
 * Copyright (c) 2012, Sascha Schade
 * Copyright (c) 2012, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/math/geometry/angle.hpp>

#include "angle_test.hpp"

void
AngleTest::testNormalize()
{
	TEST_ASSERT_EQUALS_FLOAT(modm::Angle::normalize( 0.3 * M_PI),  0.3 * M_PI);
	TEST_ASSERT_EQUALS_FLOAT(modm::Angle::normalize(-0.6 * M_PI), -0.6 * M_PI);
	TEST_ASSERT_EQUALS_FLOAT(modm::Angle::normalize( 2.9 * M_PI),  0.9 * M_PI);
	TEST_ASSERT_EQUALS_FLOAT(modm::Angle::normalize(-2.9 * M_PI), -0.9 * M_PI);
	TEST_ASSERT_EQUALS_FLOAT(modm::Angle::normalize( 1.5 * M_PI), -0.5 * M_PI);
}

void
AngleTest::testReverse()
{
	TEST_ASSERT_EQUALS_FLOAT(modm::Angle::reverse(-0.5 * M_PI),  0.5 * M_PI);
	TEST_ASSERT_EQUALS_FLOAT(modm::Angle::reverse(-0.9 * M_PI),  0.1 * M_PI);
	TEST_ASSERT_EQUALS_FLOAT(modm::Angle::reverse( 0.7 * M_PI), -0.3 * M_PI);
}

void
AngleTest::testPerpendicularCw()
{
	TEST_ASSERT_EQUALS_FLOAT(modm::Angle::perpendicular(-0.5 * M_PI, true),  1.0 * M_PI);
	TEST_ASSERT_EQUALS_FLOAT(modm::Angle::perpendicular(-0.9 * M_PI, true),  0.6 * M_PI);
	TEST_ASSERT_EQUALS_FLOAT(modm::Angle::perpendicular( 0.7 * M_PI, true),  0.2 * M_PI);
	TEST_ASSERT_EQUALS_FLOAT(modm::Angle::perpendicular( 0.1 * M_PI, true), -0.4 * M_PI);
}

void
AngleTest::testPerpendicularCcw()
{
	TEST_ASSERT_EQUALS_FLOAT(modm::Angle::perpendicular(-0.5 * M_PI, false),  0.0 * M_PI);
	TEST_ASSERT_EQUALS_FLOAT(modm::Angle::perpendicular(-0.9 * M_PI, false), -0.4 * M_PI);
	TEST_ASSERT_EQUALS_FLOAT(modm::Angle::perpendicular( 0.7 * M_PI, false), -0.8 * M_PI);
	TEST_ASSERT_EQUALS_FLOAT(modm::Angle::perpendicular( 0.1 * M_PI, false),  0.6 * M_PI);
}
