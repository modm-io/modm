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

/// Tests the fiber synchronization primitives from outside of a fiber, which
/// must work with and without the fiber scheduler.
/// @ingroup modm_test_test_architecture_fiber
class FiberSyncTest : public unittest::TestSuite
{
public:
	void
	testPolling();

	void
	testMutex();

	void
	testSemaphore();

	void
	testLatch();

	void
	testConditionVariable();
};
