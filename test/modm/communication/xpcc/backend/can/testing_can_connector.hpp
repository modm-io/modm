/*
 * Copyright (c) 2009, Martin Rosekeit
 * Copyright (c) 2009-2010, Fabian Greif
 * Copyright (c) 2012-2014, Niklas Hauser
 * Copyright (c) 2013, Kevin Laeufer
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef TESTING_CAN_CONNECTOR_HPP
#define TESTING_CAN_CONNECTOR_HPP

#include "../connector.hpp"
#include "fake_can_driver.hpp"

class TestingCanConnector : public xpcc::CanConnector<FakeCanDriver>
{
public:
	TestingCanConnector(FakeCanDriver *driver);

	// expose the internal variable for testing
	using xpcc::CanConnector<FakeCanDriver>::messageCounter;
};

#endif	// TESTING_CAN_CONNECTOR_HPP
