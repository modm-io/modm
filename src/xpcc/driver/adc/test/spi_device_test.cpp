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

#include "spi_device_test.hpp"

#define ENABLE_MACRO_EXPORT
#include "spi_device.hpp"
#undef ENABLE_MACRO_EXPORT

// ----------------------------------------------------------------------------
void
SpiDeviceTest::testInitialState()
{
	test::SpiDevice device;
	
	TEST_ASSERT_TRUE(device.isSuccessful());
}

void
SpiDeviceTest::testEmptyTransmission()
{
	test::SpiDevice device;
	
	device.start(0, 0);
	
	TEST_ASSERT_TRUE(device.isSuccessful());
}

void
SpiDeviceTest::testSingleTransmission()
{
	test::Transmission transmissions[] = {
		test::Transmission(4, RX_DATA(1,2,3,4), TX_DATA(4,3,2,1)),
	};
	
	test::SpiDevice device;
	device.start(transmissions, ARRAY_SIZE(transmissions));
	
	TEST_ASSERT_FALSE(device.isSuccessful());
	
	device.select();
	TEST_ASSERT_EQUALS(device.write(1), 4);
	TEST_ASSERT_EQUALS(device.write(2), 3);
	TEST_ASSERT_EQUALS(device.write(3), 2);
	TEST_ASSERT_EQUALS(device.write(4), 1);
	device.deselect();
	
	TEST_ASSERT_TRUE(device.isSuccessful());
	
	device.select();
	device.deselect();
	
	TEST_ASSERT_FALSE(device.isSuccessful());
	
	// Restart transmission
	device.start(transmissions, ARRAY_SIZE(transmissions));
	device.select();
	TEST_ASSERT_EQUALS(device.write(1), 4);
	TEST_ASSERT_EQUALS(device.write(2), 3);
	TEST_ASSERT_EQUALS(device.write(3), 2);
	TEST_ASSERT_EQUALS(device.write(4), 1);
	TEST_ASSERT_EQUALS(device.write(5), 0xff);		// one byte to much
	device.deselect();
	
	TEST_ASSERT_FALSE(device.isSuccessful());
}

void
SpiDeviceTest::testMultipleTransmissions()
{
	test::Transmission transmissions[] = {
		test::Transmission(4, RX_DATA(1,2,3,4), TX_DATA(4,3,2,1)),
		test::Transmission(2, RX_DATA(5, 6), TX_DATA(100, 101)),
	};
	
	test::SpiDevice device;
	device.start(transmissions, ARRAY_SIZE(transmissions));
	
	TEST_ASSERT_FALSE(device.isSuccessful());
	
	device.select();
	TEST_ASSERT_EQUALS(device.write(1), 4);
	TEST_ASSERT_EQUALS(device.write(2), 3);
	TEST_ASSERT_EQUALS(device.write(3), 2);
	TEST_ASSERT_EQUALS(device.write(4), 1);
	device.deselect();
	
	device.select();
	TEST_ASSERT_EQUALS(device.write(5), 100);
	TEST_ASSERT_EQUALS(device.write(6), 101);
	device.deselect();
	
	TEST_ASSERT_TRUE(device.isSuccessful());
}
