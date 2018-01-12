/*
 * Copyright (c) 2009, Martin Rosekeit
 * Copyright (c) 2009, Thorsten Lajewski
 * Copyright (c) 2009-2010, 2017-2018, Fabian Greif
 * Copyright (c) 2012, Sascha Schade
 * Copyright (c) 2012, 2017, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <stdc++/internal/algorithm_base.hpp>
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
