/*
 * Copyright (c) 2009, Martin Rosekeit
 * Copyright (c) 2009-2010, 2018, Fabian Greif
 * Copyright (c) 2012-2014, 2017, Niklas Hauser
 * Copyright (c) 2013, Kevin Läufer
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

#include <modm/communication/xpcc/backend/can/connector.hpp>

#include <modm-test/mock/can_driver.hpp>

/// @ingroup modm_test_test_communication
class TestingCanConnector : public xpcc::CanConnector<modm_test::platform::CanDriver>
{
public:
	TestingCanConnector(modm_test::platform::CanDriver *driver);

	// expose the internal variable for testing
	using xpcc::CanConnector<modm_test::platform::CanDriver>::messageCounter;
};

#endif	// TESTING_CAN_CONNECTOR_HPP
