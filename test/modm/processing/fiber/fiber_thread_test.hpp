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

/// @ingroup modm_test_test_architecture
class FiberThreadTest : public unittest::TestSuite
{
public:
	void
	testThreadLocalDestructor();

	void
	testYieldInsideCatch();

	void
	testSchedulersRunInParallel();

	void
	testMutexAcrossSchedulers();

	void
	testRecursiveMutexAcrossSchedulers();

	void
	testJoinAcrossSchedulers();

	void
	testBarrierAcrossSchedulers();

	void
	testSharedMutexAcrossSchedulers();

	void
	testSemaphoreAcrossSchedulers();

	void
	testConditionVariableAcrossSchedulers();

	void
	testLatchAcrossSchedulers();

	void
	testCallOnceAcrossSchedulers();

	void
	testStopTokenAcrossSchedulers();
};
