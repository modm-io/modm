/*
 * Copyright (c) 2009, Martin Rosekeit
 * Copyright (c) 2009-2010, Fabian Greif
 * Copyright (c) 2012-2013, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef CAN_CONNECTOR_BASE_TEST_HPP
#define CAN_CONNECTOR_BASE_TEST_HPP

#include <unittest/testsuite.hpp>

/// @ingroup modm_test_test_communication_xpcc
class CanConnectorBaseTest : public unittest::TestSuite
{
public:
	void
	testConversionToIdentifier();

	void
	testConversionToHeader();
};

#endif // CAN_CONNECTOR_BASE_TEST_HPP
