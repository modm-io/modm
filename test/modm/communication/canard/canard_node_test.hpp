/*
 * Copyright (c) 2026, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <unittest/testsuite.hpp>

/// @ingroup modm_test_test_communication_canard
class CanardNodeTest : public unittest::TestSuite
{
public:
	void
	setUp() override;

	void
	tearDown() override;

	void
	testSingleFrame();

	void
	testMultiFrame();

	void
	testMalloc();

	void
	testBackpressure();

	void
	testIgnoredFrames();

	void
	testKeepNodeId();

	void
	testLoopback();

	void
	testAnonymous();
};
