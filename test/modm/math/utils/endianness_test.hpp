/*
 * Copyright (c) 2015, Sascha Schade
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
class EndiannessTest : public unittest::TestSuite
{
public:
	void
	testToBigEndian();

	void
	testToLittleEndian();

	void
	testFromBigEndian();

	void
	testFromLittleEndian();

	void
	testBigEndianStruct();

	void
	testLittleEndianStruct();

	void
	testBigEndianReinterpreted();

	void
	testLittleEndianReinterpreted();
};
