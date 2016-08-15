// coding: utf-8
/* Copyright (c) 2016, Roboterclub Aachen e. V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_UNITTEST_CAN_MESSAGE_HPP
#define XPCC_UNITTEST_CAN_MESSAGE_HPP

#include <unittest/testsuite.hpp>
#include <xpcc/architecture/interface/can_message.hpp>

// @author strogly-typed
class CanMessageTest : public unittest::TestSuite
{
public:
	void
	testEqualOperator();
};

#endif // XPCC_UNITTEST_CAN_MESSAGE_HPP
