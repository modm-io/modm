// coding: utf-8
// ----------------------------------------------------------------------------
/* Copyright (c) 2012, Roboterclub Aachen e.V.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Roboterclub Aachen e.V. nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ROBOTERCLUB AACHEN E.V. ''AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL ROBOTERCLUB AACHEN E.V. BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
// ----------------------------------------------------------------------------

#include <xpcc/driver/adc/ad7280a.hpp>

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

typedef xpcc::Ad7280a<Spi, Cs, xpcc::gpio::Unused, 1> Ad7280a;

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
	test::Transmission transmissionsInitialize[] = {
		test::Transmission(4, RX_DATA(0x01,0xD2,0xB4,0x12), TX_DATA(0x00,0x00,0x00,0x00)),
		test::Transmission(4, RX_DATA(0x01,0xC2,0xB6,0xE2), TX_DATA(0x00,0x00,0x00,0x00)),
		test::Transmission(4, RX_DATA(0x03,0x87,0x16,0xCA), TX_DATA(0x00,0x00,0x00,0x00)),
		test::Transmission(4, RX_DATA(0xF8,0x00,0x03,0x0A), TX_DATA(0x11,0xC2,0x65,0xDC)),
	};
	
	device.start(transmissionsInitialize, ARRAY_SIZE(transmissionsInitialize), __LINE__);
	
	TEST_ASSERT_TRUE(Ad7280a::chainSetup());
	
	device.finish();
}

// ----------------------------------------------------------------------------
void
Ad7280aTest::testSelftest()
{
	test::Transmission transmissionsInitialize[] = {
		test::Transmission(4, RX_DATA(0x01,0xB8,0x10,0x92), TX_DATA(0x00,0x00,0x00,0x00)),
		test::Transmission(4, RX_DATA(0x01,0xC2,0xB6,0xE2), TX_DATA(0x00,0x00,0x00,0x00)),
		test::Transmission(4, RX_DATA(0x03,0x86,0x17,0xCA), TX_DATA(0x00,0x00,0x00,0x00)),
		test::Transmission(4, RX_DATA(0x03,0xA0,0x54,0x6A), TX_DATA(0x00,0x00,0x00,0x00)),
		test::Transmission(4, RX_DATA(0x03,0xA0,0x34,0x0A), TX_DATA(0x00,0x00,0x00,0x00)),
		
		// Read the self-test conversion result (value = 980)
		test::Transmission(4, RX_DATA(0xF8,0x00,0x03,0x0A), TX_DATA(0x00,0x1E,0xA5,0x90)),
	};
	
	device.start(transmissionsInitialize, ARRAY_SIZE(transmissionsInitialize), __LINE__);
	
	TEST_ASSERT_TRUE(Ad7280a::performSelftest());
	
	device.finish();
}

// ----------------------------------------------------------------------------
void
Ad7280aTest::testSoftwareReset()
{
	test::Transmission transmissionsInitialize[] = {
		test::Transmission(4, RX_DATA(0x01,0xD2,0xB4,0x12), TX_DATA(0x00,0x00,0x00,0x00)),
		test::Transmission(4, RX_DATA(0x01,0xC2,0xB6,0xE2), TX_DATA(0x00,0x00,0x00,0x00)),
	};
	
	device.start(transmissionsInitialize, ARRAY_SIZE(transmissionsInitialize), __LINE__);
	
	Ad7280a::softwareReset();
	
	device.finish();
}

// ----------------------------------------------------------------------------
void
Ad7280aTest::testChannelRead()
{
	test::Transmission transmissionsInitialize[] = {
		test::Transmission(4, RX_DATA(0x01,0xA6,0x15,0x1A), TX_DATA(0x00,0x00,0x00,0x00)),
		test::Transmission(4, RX_DATA(0x01,0xA1,0x02,0x0A), TX_DATA(0x00,0x00,0x00,0x00)),
		
		test::Transmission(4, RX_DATA(0xF8,0x00,0x03,0x0A), TX_DATA(0x01,0x91,0x2D,0x88)),
	};
	
	device.start(transmissionsInitialize, ARRAY_SIZE(transmissionsInitialize), __LINE__);
	
	uint16_t value;
	TEST_ASSERT_TRUE(Ad7280a::readChannel(0, xpcc::ad7280a::CELL_VOLTAGE_4, &value));
	
	TEST_ASSERT_EQUALS(value, 549);
	
	device.finish();
}

// ----------------------------------------------------------------------------
void
Ad7280aTest::testAllChannelRead()
{
	test::Transmission transmissionsInitialize[] = {
		test::Transmission(4, RX_DATA(0x03,0x80,0x11,0xCA), TX_DATA(0x00,0x00,0x00,0x00)),
		test::Transmission(4, RX_DATA(0x01,0xB4,0x15,0xEA), TX_DATA(0x00,0x00,0x00,0x00)),
		test::Transmission(4, RX_DATA(0x03,0xA0,0x54,0x6A), TX_DATA(0x00,0x00,0x00,0x00)),
		
		test::Transmission(4, RX_DATA(0xF8,0x00,0x03,0x0A), TX_DATA(0x00,0x03,0x27,0x24)),
		test::Transmission(4, RX_DATA(0xF8,0x00,0x03,0x0A), TX_DATA(0x00,0x86,0x47,0x34)),
		test::Transmission(4, RX_DATA(0xF8,0x00,0x03,0x0A), TX_DATA(0x01,0x09,0x67,0x84)),
		test::Transmission(4, RX_DATA(0xF8,0x00,0x03,0x0A), TX_DATA(0x01,0x8C,0x87,0x14)),
		test::Transmission(4, RX_DATA(0xF8,0x00,0x03,0x0A), TX_DATA(0x02,0x0F,0xA5,0xEC)),
		test::Transmission(4, RX_DATA(0xF8,0x00,0x03,0x0A), TX_DATA(0x02,0x92,0xC6,0x74)),
	};
	
	device.start(transmissionsInitialize, ARRAY_SIZE(transmissionsInitialize), __LINE__);
	
	uint16_t values[6];
	TEST_ASSERT_TRUE(Ad7280a::readAllChannels(values));
	
	TEST_ASSERT_EQUALS(values[0], 100);
	TEST_ASSERT_EQUALS(values[1], 200);
	TEST_ASSERT_EQUALS(values[2], 300);
	TEST_ASSERT_EQUALS(values[3], 400);
	TEST_ASSERT_EQUALS(values[4], 500);
	TEST_ASSERT_EQUALS(values[5], 600);
	
	device.finish();
}

// ----------------------------------------------------------------------------
void
Ad7280aTest::testBalancer()
{
	test::Transmission transmissionsInitialize[] = {
		test::Transmission(4, RX_DATA(0x02,0x81,0x83,0xC2), TX_DATA(0x00,0x00,0x00,0x00)),
	};
	
	device.start(transmissionsInitialize, ARRAY_SIZE(transmissionsInitialize), __LINE__);
	
	Ad7280a::enableBalancer(0, xpcc::ad7280a::CB1 | xpcc::ad7280a::CB2);
	
	device.finish();
}
