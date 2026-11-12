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

#pragma once

#include <unittest/testsuite.hpp>

/// Runs two fibers with random yields between their steps for many seeds and
/// checks the invariants of the synchronization primitives after every step.
/// @ingroup modm_test_test_architecture
class FiberFuzzTest : public unittest::TestSuite
{
public:
	void
	testMutex();

	void
	testSharedMutex();

	void
	testCallOnce();

	void
	testBarrier();

	void
	testSemaphore();

	void
	testConditionVariable();
};
