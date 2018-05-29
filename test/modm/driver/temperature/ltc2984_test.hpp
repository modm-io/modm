/*
 * Copyright (c) 2017, Raphael Lehmann
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <unittest/testsuite.hpp>

class Ltc2984Test : public unittest::TestSuite
{
public:
	void
	testDataStatus();

	void
	testDataTemperature();

	void
	testSpi();

	void
	tearDown();
};
