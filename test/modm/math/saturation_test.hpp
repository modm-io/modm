/*
 * Copyright (c) 2009, Martin Rosekeit
 * Copyright (c) 2009-2010, Fabian Greif
 * Copyright (c) 2012, Niklas Hauser
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
class SaturationTest : public unittest::TestSuite
{
public:
	void
	testSaturated_int8_t();

	void
	testSaturated_uint8_t();

	void
	testSaturated_int16_t();

	void
	testSaturated_uint16_t();

	void
	testSaturated_uint8_t_ref();
};
