/*
 * Copyright (c) 2022, Thomas Sommer
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef COLOR_TEST_HPP
#define COLOR_TEST_HPP

#include <unittest/testsuite.hpp>

/// @ingroup modm_test_test_ui
class ColorTest : public unittest::TestSuite
{
public:
	void
	testGray();

	void
	testRgb();

	void
	testHsv();

	void
	testConvertion_8bit();

	void
	testConvertion_16bit();

	void
	testRgbHsvPingPongConvertion_8bit();

	void
	testRgbHsvPingPongConvertion_16bit();

	void
	testConstevalOperators();
};

#endif	// COLOR_TEST_HPP
