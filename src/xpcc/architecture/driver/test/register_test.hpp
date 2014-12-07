// coding: utf-8
/* Copyright (c) 2014, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_UNITTEST_REGISTER_HPP
#define XPCC_UNITTEST_REGISTER_HPP

#include <unittest/testsuite.hpp>
#include <xpcc/architecture/peripheral/register.hpp>

// @author Niklas Hauser
class RegisterTest : public unittest::TestSuite
{
public:
	void
	testOperators();
};


#endif // XPCC_UNITTEST_REGISTER_HPP
