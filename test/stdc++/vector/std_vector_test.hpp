/*
 * Copyright (c) 2018, Christopher Durand
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <unittest/testsuite.hpp>

/// @ingroup modm_test_test_stdc++
class StdVectorTest : public unittest::TestSuite
{
public:
	void
	testInsertion();

	void
	testIteration();

	void
	testEmplace();

	void
	testRemove();
};
