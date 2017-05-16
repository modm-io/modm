/*
 * Copyright (c) 2009, Martin Rosekeit
 * Copyright (c) 2009-2010, 2013, Fabian Greif
 * Copyright (c) 2012, Niklas Hauser
 * Copyright (c) 2015, Kevin Läufer
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <unittest/testsuite.hpp>

class Polygon2DTest : public unittest::TestSuite
{
public:
	void
	testConstructor();

	void
	testInitializerListConstructor();

	void
	testAppendAndAccess();
	
	void
	testShiftOperator();
	
	void
	testIntersectionPolygon();
	
	void
	testIntersectionCircle();
	
	void
	testIntersectionLineSegment();
	
	void
	testIntersectionPointsLineSegment();
	
	void
	testPointContainedCW();
	
	void
	testPointContainedCCW();
};
