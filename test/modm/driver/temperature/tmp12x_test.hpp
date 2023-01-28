/*
 * Copyright (c) 2020, Christopher Durand
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <unittest/testsuite.hpp>
#include <modm/driver/temperature/tmp12x.hpp>

/// @ingroup modm_test_test_driver
class Tmp12xTest : public unittest::TestSuite
{
public:
	void
	testTmp123TemperatureData();

	void
	testTmp125TemperatureData();
};
