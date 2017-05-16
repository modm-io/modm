/*
 * Copyright (c) 2009-2012, Fabian Greif
 * Copyright (c) 2009-2010, Martin Rosekeit
 * Copyright (c) 2009, Georgi Grinshpun
 * Copyright (c) 2012-2014, 2016, Niklas Hauser
 * Copyright (c) 2016, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include "io_stream_test.hpp"

#include <modm/architecture/utils.hpp> // MODM_ARRAY_SIZE
#include <stdio.h>	// snprintf
#include <string.h>	// memset

// ----------------------------------------------------------------------------
// simple IODevice which stores all data in a memory buffer
// used for testing the output of an IOStream

class MemoryWriter : public modm::IODevice
{
public:
	MemoryWriter() :
		bytesWritten(0) {}

	/// Write a single char to the buffer.
	virtual void
	write(char c)
	{
		this->buffer[this->bytesWritten] = c;
		this->bytesWritten++;
	}

	using modm::IODevice::write;

	virtual void
	flush()
	{
		memset(this->buffer, 0, this->buffer_length);
		this->bytesWritten = 0;
	}

	/// Reading is not implemented
	virtual bool
	read(char& /*c*/)
	{
		return false;
	}

	/// Clear the buffer and reset counter.
	void
	clear()
	{
		memset(this->buffer, 0, this->buffer_length);
		this->bytesWritten = 0;
	}

	static constexpr std::size_t buffer_length = 100;
	char buffer[buffer_length];
	size_t bytesWritten;
};

// ----------------------------------------------------------------------------
static MemoryWriter device;

void
IoStreamTest::setUp()
{
	device.clear();
	stream = new modm::IOStream(device);
}

void
IoStreamTest::tearDown()
{
	delete stream;
}

// ----------------------------------------------------------------------------
void
IoStreamTest::testString()
{
	char string[] = "abc d ";

	(*stream) << string;

	TEST_ASSERT_EQUALS_ARRAY(string, device.buffer, 6);
	TEST_ASSERT_EQUALS(device.bytesWritten, 6U);
}

FLASH_STORAGE_STRING(flashString) = "abc";

void
IoStreamTest::testFlashString()
{
	char string[] = "abc";

	(*stream) << modm::accessor::asFlash(flashString);

	TEST_ASSERT_EQUALS_ARRAY(string, device.buffer, 3);
	TEST_ASSERT_EQUALS(device.bytesWritten, 3U);
}

// ----------------------------------------------------------------------------
void
IoStreamTest::testStreamUint8()
{
	char string[] = "244";

	(*stream) << static_cast<uint8_t>(244);

	TEST_ASSERT_EQUALS_ARRAY(string, device.buffer, 3);
	TEST_ASSERT_EQUALS(device.bytesWritten, 3U);
}

void
IoStreamTest::testStreamInt8()
{
	char string[] = "-123";

	(*stream) << static_cast<int8_t>(-123);

	TEST_ASSERT_EQUALS_ARRAY(string, device.buffer, 4);
	TEST_ASSERT_EQUALS(device.bytesWritten, 4U);
}

// ----------------------------------------------------------------------------
void
IoStreamTest::testStreamUint16()
{
	char string[] = "62412";

	(*stream) << static_cast<uint16_t>(62412);

	TEST_ASSERT_EQUALS_ARRAY(string, device.buffer, 5);
	TEST_ASSERT_EQUALS(device.bytesWritten, 5U);
}

void
IoStreamTest::testStreamUint16_2()
{
	char string[] = "65535";

	(*stream) << static_cast<uint16_t>(0xffff);

	TEST_ASSERT_EQUALS_ARRAY(string, device.buffer, 5);
	TEST_ASSERT_EQUALS(device.bytesWritten, 5U);
}

// ----------------------------------------------------------------------------
void
IoStreamTest::testStreamInt16()
{
	char string[] = "-12345";

	(*stream) << static_cast<int16_t>(-12345);

	TEST_ASSERT_EQUALS_ARRAY(string, device.buffer, 6);
	TEST_ASSERT_EQUALS(device.bytesWritten, 6U);
}

void
IoStreamTest::testStreamInt16_2()
{
	char string[] = "-32768";

	(*stream) << static_cast<int16_t>(-32768);

	TEST_ASSERT_EQUALS_ARRAY(string, device.buffer, 6);
	TEST_ASSERT_EQUALS(device.bytesWritten, 6U);
}

// ----------------------------------------------------------------------------
void
IoStreamTest::testStreamUint32()
{
	char string[] = "123";

	(*stream) << static_cast<uint32_t>(123);

	TEST_ASSERT_EQUALS_ARRAY(string, device.buffer, 3);
	TEST_ASSERT_EQUALS(device.bytesWritten, 3U);
}

void
IoStreamTest::testStreamUint32_2()
{
	char string[] = "4294967295";

	(*stream) << static_cast<uint32_t>(0xffffffff);

	TEST_ASSERT_EQUALS_ARRAY(string, device.buffer, 10);
	TEST_ASSERT_EQUALS(device.bytesWritten, 10U);
}

// ----------------------------------------------------------------------------
void
IoStreamTest::testStreamInt32()
{
	char string[] = "-12345678";

	(*stream) << static_cast<int32_t>(-12345678);

	TEST_ASSERT_EQUALS_ARRAY(string, device.buffer, 9);
	TEST_ASSERT_EQUALS(device.bytesWritten, 9U);
}

void
IoStreamTest::testStreamInt32_2()
{
	char string[] = "-2147483648";

	// avoid a warning about overflow
	(*stream) << static_cast<int32_t>(-2147483647 - 1);

	TEST_ASSERT_EQUALS_ARRAY(string, device.buffer, 11);
	TEST_ASSERT_EQUALS(device.bytesWritten, 11U);
}

void
IoStreamTest::testStreamInt32_3()
{
	char string[] = "0";

	(*stream) << static_cast<int32_t>(0);

	TEST_ASSERT_EQUALS_ARRAY(string, device.buffer, 1);
	TEST_ASSERT_EQUALS(device.bytesWritten, 1U);
}

// ----------------------------------------------------------------------------
void
IoStreamTest::testStreamUint64()
{
#ifndef __AVR__
	char string[] = "12345678901234";

	(*stream) << static_cast<uint64_t>(12345678901234ULL);

	TEST_ASSERT_EQUALS_ARRAY(string, device.buffer, 14);
	TEST_ASSERT_EQUALS(device.bytesWritten, 14U);
#endif
}

void
IoStreamTest::testStreamInt64()
{
#ifndef __AVR__
	char string[] = "-12345678901234";

	(*stream) << static_cast<int64_t>(-12345678901234LL);

	TEST_ASSERT_EQUALS_ARRAY(string, device.buffer, 15);
	TEST_ASSERT_EQUALS(device.bytesWritten, 15U);
#endif
}

// ----------------------------------------------------------------------------
void
IoStreamTest::testFloat()
{
	char string[] = "1.23000e+00";

	(*stream) << 1.23f;

	TEST_ASSERT_EQUALS_ARRAY(string, device.buffer, 11);
	TEST_ASSERT_EQUALS(device.bytesWritten, 11U);
}

void
IoStreamTest::testFloat2()
{
	char string[] = "4.57000e+02";

	(*stream) << 457.0f;

	TEST_ASSERT_EQUALS_ARRAY(string, device.buffer, 11);
	TEST_ASSERT_EQUALS(device.bytesWritten, 11U);
}

void
IoStreamTest::testFloat3()
{
	char string[] = "-5.12314e+07";

	(*stream) << -51231400.0f;

	TEST_ASSERT_EQUALS_ARRAY(string, device.buffer, 12);
	TEST_ASSERT_EQUALS(device.bytesWritten, 12U);
}

void
IoStreamTest::testFloat4()
{
	char string[] = "-7.23400e-04";

	(*stream) << -0.0007234f;

	TEST_ASSERT_EQUALS_ARRAY(string, device.buffer, 12);
	TEST_ASSERT_EQUALS(device.bytesWritten, 12U);
}

void
IoStreamTest::testBool1()
{
	char string[] = "true";

	bool boo = true;

	(*stream) << boo;

	TEST_ASSERT_EQUALS_ARRAY(string, device.buffer, 4);
	TEST_ASSERT_EQUALS(device.bytesWritten, 4U);
}

void
IoStreamTest::testBool2()
{
	char string[] = "false";

	bool boo = false;

	(*stream) << boo;

	TEST_ASSERT_EQUALS_ARRAY(string, device.buffer, 5);
	TEST_ASSERT_EQUALS(device.bytesWritten, 5U);
}

// ----------------------------------------------------------------------------
void
IoStreamTest::testHex1()
{
	char string[] = "10";

	char c = 0x10;

	(*stream) << modm::hex << c;

	TEST_ASSERT_EQUALS_ARRAY(string, device.buffer, 2);
	TEST_ASSERT_EQUALS(device.bytesWritten, 2U);
}

void
IoStreamTest::testHex2()
{
	char string[] = "48616C6C6F04";

	char s[] = "Hallo";

	(*stream) << modm::hex << s;

	TEST_ASSERT_EQUALS_ARRAY(string, device.buffer, 10);
	TEST_ASSERT_EQUALS(device.bytesWritten, 10U);
}

void
IoStreamTest::testHex3()
{
	char string[] = "0123";

	int16_t i = 0x123;

	(*stream) << modm::hex << i;

	TEST_ASSERT_EQUALS_ARRAY(string, device.buffer, 4);
	TEST_ASSERT_EQUALS(device.bytesWritten, 4U);
}

void
IoStreamTest::testHex4()
{
	char string[] = "12345645";

	int32_t i = 0x12345645;

	(*stream) << modm::hex << i;

	TEST_ASSERT_EQUALS_ARRAY(string, device.buffer, 8);
	TEST_ASSERT_EQUALS(device.bytesWritten, 8U);
}

void
IoStreamTest::testHex5()
{
	char string[] = "0100";

	bool boo = true;

	(*stream) << modm::hex << boo;

	boo = false;
	(*stream) << modm::hex << boo;

	TEST_ASSERT_EQUALS_ARRAY(string, device.buffer, 4);
	TEST_ASSERT_EQUALS(device.bytesWritten, 4U);
}

// ----------------------------------------------------------------------------
void
IoStreamTest::testBin1()
{
	char string[] = "00010010";

	char c = 0x12;

	(*stream) << modm::bin << c;

	TEST_ASSERT_EQUALS_ARRAY(string, device.buffer, 8);
	TEST_ASSERT_EQUALS(device.bytesWritten, 8U);
}

void
IoStreamTest::testBin2()
{
	char string[] = "0100100001100001011011000110110001101111";

	char s[] = "Hallo";

	(*stream) << modm::bin << s;

	TEST_ASSERT_EQUALS_ARRAY(string, device.buffer, 40);
	TEST_ASSERT_EQUALS(device.bytesWritten, 40U);
}

void
IoStreamTest::testBin3()
{
	char string[] = "0000000100100011";

	int16_t i = 0x0123;

	(*stream) << modm::bin << i;

	TEST_ASSERT_EQUALS_ARRAY(string, device.buffer, 16);
	TEST_ASSERT_EQUALS(device.bytesWritten, 16U);
}

void
IoStreamTest::testBin4()
{
	char string[] = "00010010001101000101011001000101";

	int32_t i = 0x12345645;

	(*stream) << modm::bin << i;

	TEST_ASSERT_EQUALS_ARRAY(string, device.buffer, 32);
	TEST_ASSERT_EQUALS(device.bytesWritten, 32U);
}

void
IoStreamTest::testBin5()
{
	char string[] = "0000000100000000";

	bool boo = true;

	(*stream) << modm::bin << boo;

	boo = false;
	(*stream) << modm::bin << boo;

	TEST_ASSERT_EQUALS_ARRAY(string, device.buffer, 16);
	TEST_ASSERT_EQUALS(device.bytesWritten, 16U);
}

void
IoStreamTest::testPrintf1()
{
	// Test unformatted printing
	(*stream).printf("Lala");

	TEST_ASSERT_EQUALS_ARRAY("Lala", device.buffer, 4);
	TEST_ASSERT_EQUALS(device.bytesWritten, 4U);
}

void
IoStreamTest::testPrintf2()
{
	// Compare modm's formatter with glibc's formatter

	float ff_testvector[] = {
		123.456789
		-42.9994,
		-42.9995,
		-42.9996,
		-42.9997,
		-42.9998,
		-42.9999,
		-42.99999,
		-0.002345,
		+0.0067890
	};

	for (std::size_t ii = 0; ii < MODM_ARRAY_SIZE(ff_testvector); ++ii)
	{
		float ff = ff_testvector[ii];

		for (uint_fast8_t width = 1; width < 9; ++width)
		{
			for (uint_fast8_t width_frac = 1; width_frac < 5; ++width_frac)
			{
				char fmt_str[10];
				sprintf(fmt_str, "%%%d.%df", width, width_frac);

				char glibc[device.buffer_length];

				size_t len = snprintf(glibc, device.buffer_length, fmt_str, ff);

				(*stream).printf(fmt_str, ff);

				TEST_ASSERT_EQUALS_ARRAY(glibc, device.buffer, len);
				TEST_ASSERT_EQUALS(device.bytesWritten, len);
				(*stream).flush();
			}
		}
	}
}

int myFunc1(void) { return -1; };
int myFunc2(void) { return -1; };

void
IoStreamTest::testFp()
{
	int (*myFuncPtr1)(void);
	int (*myFuncPtr2)(void);
	int (*myFuncPtr3)(void);

	myFuncPtr1 = &myFunc1;
	myFuncPtr2 = &myFunc2;
	myFuncPtr3 = myFuncPtr1;

	TEST_ASSERT_EQUALS(myFuncPtr1, myFuncPtr1);
	TEST_ASSERT_EQUALS(myFuncPtr1, myFuncPtr3);
	TEST_ASSERT_DIFFERS(myFuncPtr1, myFuncPtr2);
}

void
IoStreamTest::testPointer()
{
#if MODM_SIZEOF_POINTER == 2
	char string[] = "0x0123";
	void * p = (void *) 0x0123;
	const size_t bytesWritten = 6;
#elif MODM_SIZEOF_POINTER == 4
	char string[] = "0x01234567";
	void * p = (void *) 0x01234567;
	const size_t bytesWritten = 10;
#else
	char string[] = "0x0123456789ABCDEF";
	void * p = (void *) 0x0123456789ABCDEF;
	const size_t bytesWritten = 18;
#endif

	(*stream).printf("%p", p);

	TEST_ASSERT_EQUALS_ARRAY(string, device.buffer, bytesWritten);
	TEST_ASSERT_EQUALS(device.bytesWritten, bytesWritten);
}
