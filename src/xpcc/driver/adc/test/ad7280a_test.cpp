/*
 * Copyright (c) 2009-2012, Fabian Greif
 * Copyright (c) 2009, Martin Rosekeit
 * Copyright (c) 2012, 2016, Sascha Schade
 * Copyright (c) 2012-2014, 2016, Niklas Hauser
 * Copyright (c) 2013, Kevin Laeufer
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/driver/adc/ad7280a.hpp>
#include <modm/platform/gpio/software_gpio.hpp>

#include "ad7280a_test.hpp"

// ----------------------------------------------------------------------------
#define ENABLE_MACRO_EXPORT
#include "spi_device.hpp"
#undef ENABLE_MACRO_EXPORT

test::SpiDevice device;

class Spi
{
public:
	static uint8_t
	write(uint8_t data)
	{
		return device.write(data);
	}
};

struct Cs
{
	static inline void
	set()
	{
		device.deselect();
	}

	static inline void
	reset()
	{
		device.select();
	}

	static void
	setOutput(bool)
	{
	}
};

typedef modm::Ad7280a<Spi, Cs, modm::GpioUnused, 1> Ad7280a;

// ----------------------------------------------------------------------------
void
Ad7280aTest::testCrcByte()
{
	TEST_ASSERT_EQUALS(Ad7280a::updateCrc(0x00),   0);
	TEST_ASSERT_EQUALS(Ad7280a::updateCrc(0x10), 174);
	TEST_ASSERT_EQUALS(Ad7280a::updateCrc(0x20), 115);
	TEST_ASSERT_EQUALS(Ad7280a::updateCrc(0x51), 103);
	TEST_ASSERT_EQUALS(Ad7280a::updateCrc(0xAB), 182);
	TEST_ASSERT_EQUALS(Ad7280a::updateCrc(0xEF), 236);
	TEST_ASSERT_EQUALS(Ad7280a::updateCrc(0xFF),  66);
}

// ----------------------------------------------------------------------------
void
Ad7280aTest::testCrcMessage()
{
	// Datasheet Example 1
	TEST_ASSERT_EQUALS(Ad7280a::calculateCrc(0x003430), 0x51);

	// Datasheet Example 2
	TEST_ASSERT_EQUALS(Ad7280a::calculateCrc(0x103430), 0x74);

	// Datasheet Example 3
	TEST_ASSERT_EQUALS(Ad7280a::calculateCrc(0x0070A1), 0x9A);

	// Datasheet Example 4
	TEST_ASSERT_EQUALS(Ad7280a::calculateCrc(0x205335), 0x46);
}

// ----------------------------------------------------------------------------
void
Ad7280aTest::testChainSetup()
{
	uint8_t arg1Rx[] = {0x01, 0xD2, 0xB4, 0x12};
	uint8_t arg1Tx[] = {0x00, 0x00, 0x00, 0x00};

	uint8_t arg2Rx[] = {0x01, 0xC2, 0xB6, 0xE2};
	uint8_t arg2Tx[] = {0x00, 0x00, 0x00, 0x00};

	uint8_t arg3Rx[] = {0x03, 0x87, 0x16, 0xCA};
	uint8_t arg3Tx[] = {0x00, 0x00, 0x00, 0x00};

	uint8_t arg4Rx[] = {0xF8, 0x00, 0x03, 0x0A};
	uint8_t arg4Tx[] = {0x11, 0xC2, 0x65, 0xDC};

	test::Transmission transmissionsInitialize[] = {
		test::Transmission(MODM_ARRAY_SIZE(arg1Rx), arg1Rx, arg1Tx),
		test::Transmission(MODM_ARRAY_SIZE(arg2Rx), arg2Rx, arg2Tx),
		test::Transmission(MODM_ARRAY_SIZE(arg3Rx), arg3Rx, arg3Tx),
		test::Transmission(MODM_ARRAY_SIZE(arg4Rx), arg4Rx, arg4Tx),
	};

	device.start(transmissionsInitialize, ARRAY_SIZE(transmissionsInitialize), __LINE__);

	TEST_ASSERT_TRUE(Ad7280a::chainSetup());

	device.finish();
}

// ----------------------------------------------------------------------------
void
Ad7280aTest::testSelftest()
{
	uint8_t arg1Rx[] = {0x01, 0xC2, 0xB6, 0xE2};
	uint8_t arg1Tx[] = {0x00, 0x00, 0x00, 0x00};

	uint8_t arg2Rx[] = {0x01, 0xB9, 0x11, 0x92};
	uint8_t arg2Tx[] = {0x00, 0x00, 0x00, 0x00};

	uint8_t arg3Rx[] = {0x03, 0x86, 0x17, 0xCA};
	uint8_t arg3Tx[] = {0x00, 0x00, 0x00, 0x00};

	uint8_t arg4Rx[] = {0xF8, 0x00, 0x03, 0x0A};
	uint8_t arg4Tx[] = {0x00, 0x1E, 0xA5, 0x90};

	test::Transmission transmissionsInitialize[] = {
		test::Transmission(MODM_ARRAY_SIZE(arg1Rx), arg1Rx, arg1Tx),
		test::Transmission(MODM_ARRAY_SIZE(arg2Rx), arg2Rx, arg2Tx),
		test::Transmission(MODM_ARRAY_SIZE(arg3Rx), arg3Rx, arg3Tx),

		// Read the self-test conversion result (value = 980)
		test::Transmission(MODM_ARRAY_SIZE(arg4Rx), arg4Rx, arg4Tx),
	};

	device.start(transmissionsInitialize, ARRAY_SIZE(transmissionsInitialize), __LINE__);

	TEST_ASSERT_TRUE(Ad7280a::performSelftest());

	device.finish();
}

// ----------------------------------------------------------------------------
void
Ad7280aTest::testSoftwareReset()
{
	uint8_t arg1Rx[] = {0x01, 0xD2, 0xB4, 0x12};
	uint8_t arg1Tx[] = {0x00, 0x00, 0x00, 0x00};

	uint8_t arg2Rx[] = {0x01, 0xC2, 0xB6, 0xE2};
	uint8_t arg2Tx[] = {0x00, 0x00, 0x00, 0x00};

	test::Transmission transmissionsInitialize[] = {
		test::Transmission(MODM_ARRAY_SIZE(arg1Rx), arg1Rx, arg1Tx),
		test::Transmission(MODM_ARRAY_SIZE(arg2Rx), arg2Rx, arg2Tx),
	};

	device.start(transmissionsInitialize, ARRAY_SIZE(transmissionsInitialize), __LINE__);

	Ad7280a::softwareReset();

	device.finish();
}

// ----------------------------------------------------------------------------
void
Ad7280aTest::testChannelRead()
{
	uint8_t arg1Rx[] = {0x01, 0xA6, 0x15, 0x1A};
	uint8_t arg1Tx[] = {0x00, 0x00, 0x00, 0x00};

	uint8_t arg2Rx[] = {0x01, 0xA1, 0x02, 0x0A};
	uint8_t arg2Tx[] = {0x00, 0x00, 0x00, 0x00};

	uint8_t arg3Rx[] = {0x03, 0x80, 0x61, 0xBA};
	uint8_t arg3Tx[] = {0x00, 0x00, 0x00, 0x00};

	uint8_t arg4Rx[] = {0xF8, 0x00, 0x03, 0x0A};
	uint8_t arg4Tx[] = {0x01, 0x91, 0x2D, 0x88};

	test::Transmission transmissionsInitialize[] = {
		test::Transmission(MODM_ARRAY_SIZE(arg1Rx), arg1Rx, arg1Tx),
		test::Transmission(MODM_ARRAY_SIZE(arg2Rx), arg2Rx, arg2Tx),
		test::Transmission(MODM_ARRAY_SIZE(arg3Rx), arg3Rx, arg3Tx),

		test::Transmission(MODM_ARRAY_SIZE(arg4Rx), arg4Rx, arg4Tx),
	};

	device.start(transmissionsInitialize, ARRAY_SIZE(transmissionsInitialize), __LINE__);

	uint16_t value = 0;
	TEST_ASSERT_TRUE(Ad7280a::readChannel(0, modm::ad7280a::CELL_VOLTAGE_4, &value));

	TEST_ASSERT_EQUALS(value, 549u);

	device.finish();
}

// ----------------------------------------------------------------------------
void
Ad7280aTest::testAllChannelRead()
{
	uint8_t arg1Rx[] = {0x03, 0x80, 0x11, 0xCA};
	uint8_t arg1Tx[] = {0x00, 0x00, 0x00, 0x00};

	uint8_t arg2Rx[] = {0x01, 0xB5, 0x14, 0xEA};
	uint8_t arg2Tx[] = {0x00, 0x00, 0x00, 0x00};

	uint8_t arg3Rx[] = {0xF8, 0x00, 0x03, 0x0A};
	uint8_t arg3Tx[] = {0x00, 0x03, 0x27, 0x24};

	uint8_t arg4Rx[] = {0xF8, 0x00, 0x03, 0x0A};
	uint8_t arg4Tx[] = {0x00, 0x86, 0x47, 0x34};

	uint8_t arg5Rx[] = {0xF8, 0x00, 0x03, 0x0A};
	uint8_t arg5Tx[] = {0x01, 0x09, 0x67, 0x84};

	uint8_t arg6Rx[] = {0xF8, 0x00, 0x03, 0x0A};
	uint8_t arg6Tx[] = {0x01, 0x8C, 0x87, 0x14};

	uint8_t arg7Rx[] = {0xF8, 0x00, 0x03, 0x0A};
	uint8_t arg7Tx[] = {0x02, 0x0F, 0xA5, 0xEC};

	uint8_t arg8Rx[] = {0xF8, 0x00, 0x03, 0x0A};
	uint8_t arg8Tx[] = {0x02, 0x92, 0xC6, 0x74};

	test::Transmission transmissionsInitialize[] = {
		test::Transmission(MODM_ARRAY_SIZE(arg1Rx), arg1Rx, arg1Tx),
		test::Transmission(MODM_ARRAY_SIZE(arg2Rx), arg2Rx, arg2Tx),

		test::Transmission(MODM_ARRAY_SIZE(arg3Rx), arg3Rx, arg3Tx),
		test::Transmission(MODM_ARRAY_SIZE(arg4Rx), arg4Rx, arg4Tx),
		test::Transmission(MODM_ARRAY_SIZE(arg5Rx), arg5Rx, arg5Tx),
		test::Transmission(MODM_ARRAY_SIZE(arg6Rx), arg6Rx, arg6Tx),
		test::Transmission(MODM_ARRAY_SIZE(arg7Rx), arg7Rx, arg7Tx),
		test::Transmission(MODM_ARRAY_SIZE(arg8Rx), arg8Rx, arg8Tx),
	};

	device.start(transmissionsInitialize, ARRAY_SIZE(transmissionsInitialize), __LINE__);

	uint16_t values[6];
	TEST_ASSERT_TRUE(Ad7280a::readAllChannels(values));

	TEST_ASSERT_EQUALS(values[0], 100u);
	TEST_ASSERT_EQUALS(values[1], 200u);
	TEST_ASSERT_EQUALS(values[2], 300u);
	TEST_ASSERT_EQUALS(values[3], 400u);
	TEST_ASSERT_EQUALS(values[4], 500u);
	TEST_ASSERT_EQUALS(values[5], 600u);

	device.finish();
}

// ----------------------------------------------------------------------------
void
Ad7280aTest::testBalancer()
{
	uint8_t arg1Rx[] = {0x02, 0x81, 0x83, 0xC2};
	uint8_t arg1Tx[] = {0x00, 0x00, 0x00, 0x00};

	test::Transmission transmissionsInitialize[] = {
		test::Transmission(MODM_ARRAY_SIZE(arg1Rx), arg1Rx, arg1Tx),
	};

	device.start(transmissionsInitialize, ARRAY_SIZE(transmissionsInitialize), __LINE__);

	Ad7280a::enableBalancer(0, modm::ad7280a::CB1 | modm::ad7280a::CB2);

	device.finish();
}
