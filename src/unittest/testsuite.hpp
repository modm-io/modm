/*
 * Copyright (c) 2009-2010, 2012, Fabian Greif
 * Copyright (c) 2012, Sascha Schade
 * Copyright (c) 2012, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef	UNITTEST_TESTSUITE_HPP
#define	UNITTEST_TESTSUITE_HPP

#include "harness.hpp"

namespace unittest
{
	/**
	 * \brief	Base class for every test suite
	 * 
	 * \author	Fabian Greif
	 * \ingroup	unittest
	 */
	class TestSuite
	{
	public:
		virtual
		~TestSuite();
		
		virtual void
		setUp();
		
		virtual void
		tearDown();
	};
}

#endif	// UNITTEST_TESTSUITE_HPP
