/*
 * Copyright (c) 2021, Thomas Sommer
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
	testRgbCopyConstructors();

	void
	testHsvCopyConstructors();

	void
	testBrightnessCopyConstructors();

	void
	testConvertion_8bit();

	// TODO 16bit convertion not yet working
	// see hsv_impl.hpp and rgb_impl.hpp
	// void
	// testConvertion_16bit();

	void
	testRgbHsvPingPongConvertion_8bit();

	// TODO 16bit convertion not yet working
	// see hsv_impl.hpp and rgb_impl.hpp
	// void
	// testRgbHsvPingPongConvertion_16bit();
};

#endif	// COLOR_TEST_HPP
