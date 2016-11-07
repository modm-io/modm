/*
 * Copyright (c) 2009, Martin Rosekeit
 * Copyright (c) 2009-2011, Fabian Greif
 * Copyright (c) 2012, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <unittest/testsuite.hpp>

class MatrixTest : public unittest::TestSuite
{
public:
	void
	testConstruction();
	
	void
	testAssign();
	
	void
	testIdentityMatrix();
	
	void
	testZeroMatrix();
	
	void
	testSubMatrix();
	
	void
	testAccess();
	
	void
	testCompare();
	
	void
	testArithmetics();
	
	void
	testMatrixMultiplicationInPlace();
	
	void
	testMatrixMultiplication();
	
	void
	testTranspose();
	
	void
	testReplace();
	
	void
	testAddRowColumn();
	
	void
	testRemoveRowColumn();
	
	void
	testDeterminant();
};
