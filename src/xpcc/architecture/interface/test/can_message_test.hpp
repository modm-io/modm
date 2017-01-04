/*
 * Copyright (c) 2009, Martin Rosekeit
 * Copyright (c) 2009-2010, Fabian Greif
 * Copyright (c) 2012, 2015, Niklas Hauser
 * Copyright (c) 2016, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_UNITTEST_CAN_MESSAGE_HPP
#define XPCC_UNITTEST_CAN_MESSAGE_HPP

#include <unittest/testsuite.hpp>
#include <modm/architecture/interface/can_message.hpp>

// @author strogly-typed
class CanMessageTest : public unittest::TestSuite
{
public:
	void
	testEqualOperator();
};

#endif // XPCC_UNITTEST_CAN_MESSAGE_HPP
