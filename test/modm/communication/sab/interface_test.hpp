/*
 * Copyright (c) 2009, Martin Rosekeit
 * Copyright (c) 2009-2011, Fabian Greif
 * Copyright (c) 2012-2013, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <unittest/testsuite.hpp>

/// @ingroup modm_test_test_communication_sab
class InterfaceTest : public unittest::TestSuite
{
public:
	virtual void
	setUp();

	void
	testSendRequest();

	void
	testSendAck();

	void
	testSendNack();

	void
	testReceive();

	void
	testReceiveAck();

	void
	testReceiveNack();
};
