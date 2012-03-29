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
};

typedef xpcc::Ad7280a<Spi, Cs> Ad7280a;

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

void
Ad7280aTest::testInitialize()
{
	test::Transmission transmissionsInitialize[] = {
		test::Transmission(4, RX_DATA(0x01,0xC2,0xB6,0xE2), TX_DATA(0,0,0,0)),
		test::Transmission(4, RX_DATA(0x03,0x87,0x16,0xCA), TX_DATA(0,0,0,0)),
		test::Transmission(4, RX_DATA(0xF8,0x00,0x03,0x0A), TX_DATA(0,0,0,0)),
	};
	
	device.start(transmissionsInitialize, ARRAY_SIZE(transmissionsInitialize));
	
	TEST_ASSERT_TRUE(Ad7280a::initialize());
	
	TEST_ASSERT_TRUE(device.isSuccessful());
	device.reportErrors();
}
