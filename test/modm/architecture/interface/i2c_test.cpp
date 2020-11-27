/*
 * Copyright (c) 2020, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include "i2c_test.hpp"

#include <modm/architecture/interface/i2c.hpp>
#include <modm-test/mock/iodevice.hpp>

// ----------------------------------------------------------------------------
static modm_test::platform::IODevice device;

void
I2cTest::setUp()
{
	device.clear();
	stream = new modm::IOStream(device);
}

void
I2cTest::tearDown()
{
	delete stream;
}

// ----------------------------------------------------------------------------


void
I2cTest::testStreamOperators()
{
	(*stream) << modm::I2c::DetachCause();
	TEST_ASSERT_EQUALS_ARRAY("NormalStop", device.buffer, 10);

	device.clear();
	(*stream) << modm::I2c::Operation();
	TEST_ASSERT_EQUALS_ARRAY("Stop", device.buffer, 4);

	device.clear();
	(*stream) << modm::I2c::OperationAfterStart();
	TEST_ASSERT_EQUALS_ARRAY("Stop", device.buffer, 4);

	device.clear();
	(*stream) << modm::I2c::OperationAfterRead();
	TEST_ASSERT_EQUALS_ARRAY("Stop", device.buffer, 4);

	device.clear();
	(*stream) << modm::I2c::OperationAfterWrite();
	TEST_ASSERT_EQUALS_ARRAY("Stop", device.buffer, 4);
}
