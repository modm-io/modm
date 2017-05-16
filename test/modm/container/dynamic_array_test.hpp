/*
 * Copyright (c) 2009, Martin Rosekeit
 * Copyright (c) 2009-2010, Fabian Greif
 * Copyright (c) 2012, Niklas Hauser
 * Copyright (c) 2015, Kevin Läufer
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <unittest/testsuite.hpp>

class DynamicArrayTest : public unittest::TestSuite
{
public:
	void
	setUp();
	
	void
	testDefaultConstrutor();

	void
	testAllocationConstructor();

	void
	testSequenceConstructor();

	void
	testCopyConstructor();

	void
	testInitializerListConstructor();

	void
	testReserve();

	void
	testAppend();

	void
	testRemove();
	
	void
	testClear();
	
	void
	testRemoveAll();
	
	// iterators
	void
	testConstIterator();
	
	void
	testConstIteratorAccess();
	
	void
	testIterator();
	
	void
	testIteratorAccess();
	
	// TODO test decrement operator for iterators 
};
