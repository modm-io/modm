/*
 * Copyright (c) 2009, Martin Rosekeit
 * Copyright (c) 2009-2010, Fabian Greif
 * Copyright (c) 2012, 2015, Niklas Hauser
 * Copyright (c) 2016, Kevin Läufer
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <unittest/testsuite.hpp>

class PlatformGpioTest : public unittest::TestSuite
{
public:
	virtual void
	setUp() override;

	void
	testEnabled();

	void
	testSetOutput();

	void
	testSetInput();

	void
	testOutput();

	void
	testInput();

	void
	testAnalog();

	void
	testConnect();

	void
	testLock();
};
