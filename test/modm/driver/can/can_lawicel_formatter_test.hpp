/*
 * Copyright (c) 2009, Martin Rosekeit
 * Copyright (c) 2009-2011, Fabian Greif
 * Copyright (c) 2010, Thorsten Lajewski
 * Copyright (c) 2012, Sascha Schade
 * Copyright (c) 2012-2015, Niklas Hauser
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

class CanLawicelFormatterTest : public unittest::TestSuite
{
public:
	void
	testIdentifierToStringExtended();

	void
	testIdentifierToStringStandard();

	void
	testMessageToStringStandard();

	void
	testMessageToStringExtended();

	void
	testStringToMessage();

	/**
	 * \brief	Test if this library can decode their own messages
	 *
	 * string -> message -> string.
	 */
	void
	testRoundtripMessage();

	/**
	 * \brief	Test if this library can decode their own messages
	 *
	 * message -> string -> message.
	 */
	void
	testRoudtripString();

	// check if invalid input is rejected as expected
	void
	testInvalidInput();
};
