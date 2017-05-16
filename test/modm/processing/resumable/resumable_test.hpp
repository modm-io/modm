/*
 * Copyright (c) 2009, Martin Rosekeit
 * Copyright (c) 2009-2011, Fabian Greif
 * Copyright (c) 2012, 2014-2015, Niklas Hauser
 * Copyright (c) 2014, Kevin Läufer
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <unittest/testsuite.hpp>

// @author Niklas Hauser
class ResumableTest : public unittest::TestSuite
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

	/// Test if resumables can return enum class values
	///
	/// This was not the case for the first iteration of
	/// resumables, because the constant `0` cannot be
	/// implicitly casted to an enum class value.
	/// Thus this is a regression test.
	void
	testReturnEnumClass();

	void
	testReturnVoidClass();

	void
	testNonNestedResumables();
};
