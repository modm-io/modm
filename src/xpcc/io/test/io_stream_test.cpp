// coding: utf-8
/* Copyright (c) 2009, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#include "io_stream_test.hpp"

// ----------------------------------------------------------------------------
// simple IODevice which stores all data in a memory buffer
// used for testing the output of an IOStream

class MemoryWriter : public xpcc::IODevice
{
public:
	MemoryWriter() :
		bytesWritten(0) {}

	virtual void
	write(char c)
	{
		this->buffer[this->bytesWritten] = c;
		this->bytesWritten++;
	}

	using xpcc::IODevice::write;

	virtual void
	flush()
	{
		// TODO
		write('\b');
	}

	virtual bool
	read(char& /*c*/)
	{
		return false;
	}

	void
	clear()
	{
		this->bytesWritten = 0;
	}

	char buffer[100];
	int bytesWritten;
};

// ----------------------------------------------------------------------------
static MemoryWriter device;

void
IoStreamTest::setUp()
{
	device.clear();
	stream = new xpcc::IOStream(device);
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
	TEST_ASSERT_EQUALS(device.bytesWritten, 6);
}

FLASH_STORAGE_STRING(flashString) = "abc";

void
IoStreamTest::testFlashString()
{
	char string[] = "abc";

	(*stream) << xpcc::accessor::asFlash(flashString);

	TEST_ASSERT_EQUALS_ARRAY(string, device.buffer, 3);
	TEST_ASSERT_EQUALS(device.bytesWritten, 3);
}

// ----------------------------------------------------------------------------
void
IoStreamTest::testStreamUint8()
{
	char string[] = "244";

	(*stream) << static_cast<uint8_t>(244);

	TEST_ASSERT_EQUALS_ARRAY(string, device.buffer, 3);
	TEST_ASSERT_EQUALS(device.bytesWritten, 3);
}

void
IoStreamTest::testStreamInt8()
{
	char string[] = "-123";

	(*stream) << static_cast<int8_t>(-123);

	TEST_ASSERT_EQUALS_ARRAY(string, device.buffer, 4);
	TEST_ASSERT_EQUALS(device.bytesWritten, 4);
}

// ----------------------------------------------------------------------------
void
IoStreamTest::testStreamUint16()
{
	char string[] = "62412";

	(*stream) << static_cast<uint16_t>(62412);

	TEST_ASSERT_EQUALS_ARRAY(string, device.buffer, 5);
	TEST_ASSERT_EQUALS(device.bytesWritten, 5);
}

void
IoStreamTest::testStreamUint16_2()
{
	char string[] = "65535";

	(*stream) << static_cast<uint16_t>(0xffff);

	TEST_ASSERT_EQUALS_ARRAY(string, device.buffer, 5);
	TEST_ASSERT_EQUALS(device.bytesWritten, 5);
}

// ----------------------------------------------------------------------------
void
IoStreamTest::testStreamInt16()
{
	char string[] = "-12345";

	(*stream) << static_cast<int16_t>(-12345);

	TEST_ASSERT_EQUALS_ARRAY(string, device.buffer, 6);
	TEST_ASSERT_EQUALS(device.bytesWritten, 6);
}

void
IoStreamTest::testStreamInt16_2()
{
	char string[] = "-32768";

	(*stream) << static_cast<int16_t>(-32768);

	TEST_ASSERT_EQUALS_ARRAY(string, device.buffer, 6);
	TEST_ASSERT_EQUALS(device.bytesWritten, 6);
}

// ----------------------------------------------------------------------------
void
IoStreamTest::testStreamUint32()
{
	char string[] = "123";

	(*stream) << static_cast<uint32_t>(123);

	TEST_ASSERT_EQUALS_ARRAY(string, device.buffer, 3);
	TEST_ASSERT_EQUALS(device.bytesWritten, 3);
}

void
IoStreamTest::testStreamUint32_2()
{
	char string[] = "4294967295";

	(*stream) << static_cast<uint32_t>(0xffffffff);

	TEST_ASSERT_EQUALS_ARRAY(string, device.buffer, 10);
	TEST_ASSERT_EQUALS(device.bytesWritten, 10);
}

// ----------------------------------------------------------------------------
void
IoStreamTest::testStreamInt32()
{
	char string[] = "-12345678";

	(*stream) << static_cast<int32_t>(-12345678);

	TEST_ASSERT_EQUALS_ARRAY(string, device.buffer, 9);
	TEST_ASSERT_EQUALS(device.bytesWritten, 9);
}

void
IoStreamTest::testStreamInt32_2()
{
	char string[] = "-2147483648";

	// avoid a warning about overflow
	(*stream) << static_cast<int32_t>(-2147483647 - 1);

	TEST_ASSERT_EQUALS_ARRAY(string, device.buffer, 11);
	TEST_ASSERT_EQUALS(device.bytesWritten, 11);
}

void
IoStreamTest::testStreamInt32_3()
{
	char string[] = "0";

	(*stream) << static_cast<int32_t>(0);

	TEST_ASSERT_EQUALS_ARRAY(string, device.buffer, 1);
	TEST_ASSERT_EQUALS(device.bytesWritten, 1);
}

// ----------------------------------------------------------------------------
void
IoStreamTest::testStreamUint64()
{
#ifndef __AVR__
	char string[] = "12345678901234";

	(*stream) << static_cast<uint64_t>(12345678901234ULL);

	TEST_ASSERT_EQUALS_ARRAY(string, device.buffer, 14);
	TEST_ASSERT_EQUALS(device.bytesWritten, 14);
#endif
}

void
IoStreamTest::testStreamInt64()
{
#ifndef __AVR__
	char string[] = "-12345678901234";

	(*stream) << static_cast<int64_t>(-12345678901234LL);

	TEST_ASSERT_EQUALS_ARRAY(string, device.buffer, 15);
	TEST_ASSERT_EQUALS(device.bytesWritten, 15);
#endif
}

// ----------------------------------------------------------------------------
void
IoStreamTest::testFloat()
{
	char string[] = "1.23000e+00";

	(*stream) << 1.23f;

	TEST_ASSERT_EQUALS_ARRAY(string, device.buffer, 11);
	TEST_ASSERT_EQUALS(device.bytesWritten, 11);
}

void
IoStreamTest::testFloat2()
{
	char string[] = "4.57000e+02";

	(*stream) << 457.0f;

	TEST_ASSERT_EQUALS_ARRAY(string, device.buffer, 11);
	TEST_ASSERT_EQUALS(device.bytesWritten, 11);
}

void
IoStreamTest::testFloat3()
{
	char string[] = "-5.12314e+07";

	(*stream) << -51231400.0f;

	TEST_ASSERT_EQUALS_ARRAY(string, device.buffer, 12);
	TEST_ASSERT_EQUALS(device.bytesWritten, 12);
}

void
IoStreamTest::testFloat4()
{
	char string[] = "-7.23400e-04";

	(*stream) << -0.0007234f;

	TEST_ASSERT_EQUALS_ARRAY(string, device.buffer, 12);
	TEST_ASSERT_EQUALS(device.bytesWritten, 12);
}

void
IoStreamTest::testBool1()
{
	char string[] = "true";

	bool boo = true;

	(*stream) << boo;

	TEST_ASSERT_EQUALS_ARRAY(string, device.buffer, 4);
	TEST_ASSERT_EQUALS(device.bytesWritten, 4);
}

void
IoStreamTest::testBool2()
{
	char string[] = "false";

	bool boo = false;

	(*stream) << boo;

	TEST_ASSERT_EQUALS_ARRAY(string, device.buffer, 5);
	TEST_ASSERT_EQUALS(device.bytesWritten, 5);
}

// ----------------------------------------------------------------------------
void
IoStreamTest::testHex1()
{
	char string[] = "10";

	char c = 0x10;

	(*stream) << xpcc::hex << c;

	TEST_ASSERT_EQUALS_ARRAY(string, device.buffer, 2);
	TEST_ASSERT_EQUALS(device.bytesWritten, 2);
}

void
IoStreamTest::testHex2()
{
	char string[] = "48616C6C6F04";

	char s[] = "Hallo";

	(*stream) << xpcc::hex << s;

	TEST_ASSERT_EQUALS_ARRAY(string, device.buffer, 10);
	TEST_ASSERT_EQUALS(device.bytesWritten, 10);
}

void
IoStreamTest::testHex3()
{
	char string[] = "0123";

	int16_t i = 0x123;

	(*stream) << xpcc::hex << i;

	TEST_ASSERT_EQUALS_ARRAY(string, device.buffer, 4);
	TEST_ASSERT_EQUALS(device.bytesWritten, 4);
}

void
IoStreamTest::testHex4()
{
	char string[] = "12345645";

	int32_t i = 0x12345645;

	(*stream) << xpcc::hex << i;

	TEST_ASSERT_EQUALS_ARRAY(string, device.buffer, 8);
	TEST_ASSERT_EQUALS(device.bytesWritten, 8);
}

void
IoStreamTest::testHex5()
{
	char string[] = "0100";

	bool boo = true;

	(*stream) << xpcc::hex << boo;

	boo = false;
	(*stream) << xpcc::hex << boo;

	TEST_ASSERT_EQUALS_ARRAY(string, device.buffer, 4);
	TEST_ASSERT_EQUALS(device.bytesWritten, 4);
}

// ----------------------------------------------------------------------------
void
IoStreamTest::testBin1()
{
	char string[] = "00010010";

	char c = 0x12;

	(*stream) << xpcc::bin << c;

	TEST_ASSERT_EQUALS_ARRAY(string, device.buffer, 8);
	TEST_ASSERT_EQUALS(device.bytesWritten, 8);
}

void
IoStreamTest::testBin2()
{
	char string[] = "0100100001100001011011000110110001101111";

	char s[] = "Hallo";

	(*stream) << xpcc::bin << s;

	TEST_ASSERT_EQUALS_ARRAY(string, device.buffer, 40);
	TEST_ASSERT_EQUALS(device.bytesWritten, 40);
}

void
IoStreamTest::testBin3()
{
	char string[] = "0000000100100011";

	int16_t i = 0x0123;

	(*stream) << xpcc::bin << i;

	TEST_ASSERT_EQUALS_ARRAY(string, device.buffer, 16);
	TEST_ASSERT_EQUALS(device.bytesWritten, 16);
}

void
IoStreamTest::testBin4()
{
	char string[] = "00010010001101000101011001000101";

	int32_t i = 0x12345645;

	(*stream) << xpcc::bin << i;

	TEST_ASSERT_EQUALS_ARRAY(string, device.buffer, 32);
	TEST_ASSERT_EQUALS(device.bytesWritten, 32);
}

void
IoStreamTest::testBin5()
{
	char string[] = "0000000100000000";

	bool boo = true;

	(*stream) << xpcc::bin << boo;

	boo = false;
	(*stream) << xpcc::bin << boo;

	TEST_ASSERT_EQUALS_ARRAY(string, device.buffer, 16);
	TEST_ASSERT_EQUALS(device.bytesWritten, 16);
}
