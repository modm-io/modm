/*
 * Copyright (c) 2023, Christopher Durand
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include "gpio_port_test.hpp"

#include <modm/platform.hpp>

using namespace modm::platform;

using Port = SoftwareGpioPort<
	GpioA23,
	GpioC9,
	GpioD20,
	GpioD21
>;

using Port2 = SoftwareGpioPort<
	GpioA23,
	GpioInverted<GpioC9>,
	GpioInverted<GpioD20>,
	GpioD21
>;

void
GpioPortTest::setUp()
{
	Port::setOutput(false);
}

void
GpioPortTest::testPortRead()
{
	Port::setOutput(false);
	TEST_ASSERT_EQUALS(Port::isSet(), 0b0000);
	TEST_ASSERT_EQUALS(Port::read(), 0b0000);

	GpioA23::set();
	TEST_ASSERT_EQUALS(Port::isSet(), 0b1000);
	TEST_ASSERT_EQUALS(Port::read(), 0b1000);

	GpioD20::set();
	TEST_ASSERT_EQUALS(Port::isSet(), 0b1010);
	TEST_ASSERT_EQUALS(Port::read(), 0b1010);

	GpioA23::reset();
	GpioD20::reset();
	GpioC9::set();
	TEST_ASSERT_EQUALS(Port::isSet(), 0b0100);
	TEST_ASSERT_EQUALS(Port::read(), 0b0100);

	GpioD21::set();
	TEST_ASSERT_EQUALS(Port::isSet(), 0b0101);
	TEST_ASSERT_EQUALS(Port::read(), 0b0101);

	Port::reset();
	TEST_ASSERT_EQUALS(Port::isSet(), 0b0000);
	TEST_ASSERT_EQUALS(Port::read(), 0b0000);
}

void
GpioPortTest::testPortWrite()
{
	Port::write(0b1001);
	TEST_ASSERT_EQUALS(Port::isSet(), 0b1001);
	TEST_ASSERT_EQUALS(Port::read(), 0b1001);

	Port::write(0b0110);
	TEST_ASSERT_EQUALS(Port::isSet(), 0b0110);
	TEST_ASSERT_EQUALS(Port::read(), 0b0110);

	Port::write(0b0010);
	TEST_ASSERT_EQUALS(Port::isSet(), 0b0010);
	TEST_ASSERT_EQUALS(Port::read(), 0b0010);

	Port::write(0b0000);
	TEST_ASSERT_EQUALS(Port::isSet(), 0b0000);
	TEST_ASSERT_EQUALS(Port::read(), 0b0000);

	Port::set();
	TEST_ASSERT_EQUALS(Port::isSet(), 0b1111);
	TEST_ASSERT_EQUALS(Port::read(), 0b1111);

	Port::reset();
	TEST_ASSERT_EQUALS(Port::isSet(), 0b0000);
	TEST_ASSERT_EQUALS(Port::read(), 0b0000);
}

void
GpioPortTest::testPortInverted()
{
	// inverted read
	Port::write(0b1111);
	TEST_ASSERT_EQUALS(Port2::isSet(), 0b1001);
	TEST_ASSERT_EQUALS(Port2::read(), 0b1001);

	Port::write(0b1101);
	TEST_ASSERT_EQUALS(Port2::isSet(), 0b1011);
	TEST_ASSERT_EQUALS(Port2::read(), 0b1011);

	Port::write(0b1001);
	TEST_ASSERT_EQUALS(Port2::isSet(), 0b1111);
	TEST_ASSERT_EQUALS(Port2::read(), 0b1111);

	Port::write(0b0000);
	TEST_ASSERT_EQUALS(Port2::isSet(), 0b0110);
	TEST_ASSERT_EQUALS(Port2::read(), 0b0110);

	// inverted write
	Port2::write(0b0000);
	TEST_ASSERT_EQUALS(Port::isSet(), 0b0110);
	TEST_ASSERT_EQUALS(Port::read(), 0b0110);

	Port2::write(0b0100);
	TEST_ASSERT_EQUALS(Port::isSet(), 0b0010);
	TEST_ASSERT_EQUALS(Port::read(), 0b0010);

	Port2::write(0b0110);
	TEST_ASSERT_EQUALS(Port::isSet(), 0b0000);
	TEST_ASSERT_EQUALS(Port::read(), 0b0000);

	Port2::write(0b1111);
	TEST_ASSERT_EQUALS(Port::isSet(), 0b1001);
	TEST_ASSERT_EQUALS(Port::read(), 0b1001);
}
