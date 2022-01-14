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

#include <unittest/testsuite.hpp>

/// @ingroup modm_test_test_math
class Vector5Test : public unittest::TestSuite
{
public:
	void
	testConstructor();

	// void
	// testAssign();

	// void
	// testCompare();

	// void
	// testRawDataAccess();

	void
	testOperators();

	void
	testLength();
};
