/*
 * Copyright (c) 2009, Martin Rosekeit
 * Copyright (c) 2009, Thorsten Lajewski
 * Copyright (c) 2009-2010, Fabian Greif
 * Copyright (c) 2012, Niklas Hauser
 * Copyright (c) 2012, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include "../internal/algorithm_base.hpp"
#include "algorithm_base_test.hpp"

void
AlgorithmBaseTest::testMin()
{
	TEST_ASSERT_EQUALS(std::min(1, 2), 1);
	TEST_ASSERT_EQUALS(std::min(2, 1), 1);
}

void
AlgorithmBaseTest::testMax()
{
	TEST_ASSERT_EQUALS(std::max(1, 2), 2);
	TEST_ASSERT_EQUALS(std::max(2, 1), 2);
}
