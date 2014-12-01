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
class CoroutineTest : public unittest::TestSuite
{
public:
	// uses an empty protothread to test the basic methods of the class
	void
	testClassMethods();

	void
	testNesting();

	void
	testSpawn();

	void
	testComplexSpawn();

	void
	testCaseNumbers();

	/// Test if coroutines can return enum class values
	///
	/// This was not the case for the first iteration of
	/// coroutines, because the constant `0` cannot be
	/// implicitly casted to an enum class value.
	/// Thus this is a regression test.
	void
	testReturnEnumClass();
};
