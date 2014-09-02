// coding: utf-8
/* Copyright (c) 2014, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#include <unittest/testsuite.hpp>

// @author Niklas Hauser
class NestedProtothreadTest : public unittest::TestSuite
{
public:
	// uses an empty protothread to test the basic methods of the class
	void
	testClassMethods();

	void
	testNesting();

	void
	testSpawn();
};
